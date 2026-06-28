#include "Camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>
#include <cstdlib>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

struct App
{
  int mScreenWidth = 640;
  int mScreenHeight = 480;
  SDL_Window *mGraphicsApplicationWindow = nullptr;
  SDL_GLContext mOpenGLContext = nullptr;
  bool mQuit = false;
  GLuint mGraphicsPipelineShaderProgram = 0;
  // Create a single global camera
  Camera mCamera;
};

struct Transform
{
  float x, y, z;
};

struct Mesh3D
{
  GLuint mVertexArrayObject = 0;
  GLuint mVertexBufferObject = 0;
  GLuint mIndexBufferObject = 0;
  GLuint mPipeline = 0;
  Transform mTransform;
  float m_URotate = 0.0f;
  float m_UScale = 0.5f;
};

App gApp;
Mesh3D gMesh1;
Mesh3D gMesh2;

// #define GLCheck(x)    \
//   GLClearAllErrors(); \
//   x;                  \
//   GLCheckErrorStatus(#x, __LINE__);

// Returns the location of uniform variable based on it's name
int findUniformLocation(GLuint pipeline, const GLchar *name)
{
  GLint location =
      glGetUniformLocation(pipeline, name);

  if (location < 0)
  {
    std::cerr << "couldn't find location of" << name << "maybe spelling error"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return location;
}

// Setup vertex data per mesh
void meshCreate(Mesh3D *mesh)
{
  const std::vector<GLfloat> vertexData{
      -0.5f,
      -0.5f,
      0.0f, // vertex 1
      1.0f,
      0.0f,
      0.0f, // color of vertex 1
      0.5f,
      -0.5f,
      0.0f, // vertex 2
      0.0f,
      1.0f,
      0.0f, // color of vertex 2
      -0.5f,
      0.5f,
      0.0f, // vertex 3
      0.0f,
      0.0f,
      1.0f, // color of vertex 3
      0.5f,
      0.5f,
      0.0f, // vertex 4
      0.0f,
      1.0f,
      0.0f, // color of vertex 4
  };

  const std::vector<GLuint> indexBufferData{2, 0, 1, 3, 2, 1};
  glGenVertexArrays(1, &mesh->mVertexArrayObject);
  glBindVertexArray(mesh->mVertexArrayObject);
  glGenBuffers(1, &mesh->mVertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat),
               vertexData.data(), GL_STATIC_DRAW);

  // index buffer object
  glGenBuffers(1, &mesh->mIndexBufferObject);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mIndexBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size() * sizeof(GLuint),
               indexBufferData.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 6, (void *)0);

  // Color Information
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6,
                        (GLvoid *)(sizeof(GLfloat) * 3));

  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

void meshDelete(Mesh3D *mesh)
{
  glDeleteBuffers(1, &mesh->mVertexBufferObject);
  glDeleteVertexArrays(1, &mesh->mVertexArrayObject);
}

/*
Needs to set pipeline before we draw
*/
void meshSetPipeline(Mesh3D *mesh, GLuint pipeline)
{
  mesh->mPipeline = pipeline;
}

/**
 * Note : We per mesh choose the graphics pipeline we'll use
 * Generally not efficient to change state(pipelines) frequently,
 * we're doing this for flexibilty
 */

void drawMesh(Mesh3D *mesh)
{
  if (mesh == nullptr)
  {
    return;
  }

  glUseProgram(mesh->mPipeline);

  mesh->m_URotate -= 1.0f;
  // std::cout << "g_URotate: " << g_URotate << std::endl;

  // Order of transformations matter,
  // try changing for different effects
  // keep input matrix as identity
  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), glm::vec3(mesh->mTransform.x, mesh->mTransform.y, mesh->mTransform.z));

  model =
      glm::rotate(model, glm::radians(mesh->m_URotate), glm::vec3(0.0f, 1.0f, 0.0f));

  model = glm::scale(model, glm::vec3(mesh->m_UScale, mesh->m_UScale, mesh->m_UScale));

  // note we oftem combine view and model matrix to send in one uniform
  glm::mat4 view = gApp.mCamera.GetViewMatrix();

  GLint u_ModelMatrixLocation =
      findUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_ModelMatrix");
  glUniformMatrix4fv(u_ModelMatrixLocation, 1, false, &model[0][0]);

  GLint u_ViewLocation =
      findUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_ViewMatrix");
  glUniformMatrix4fv(u_ViewLocation, 1, false, &view[0][0]);

  // Projection Matrix (in perspective)
  glm::mat4 perspective = glm::perspective(
      glm::radians(45.0f), (float)gApp.mScreenWidth / (float)gApp.mScreenHeight, 0.1f,
      100.0f);

  // Retrieve our location of our perspective matrix uniform
  GLint u_ProjectionLocation =
      glGetUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_Projection");

  if (u_ProjectionLocation >= 0)
  {
    glUniformMatrix4fv(u_ProjectionLocation, 1, false, &perspective[0][0]);
  }
  else
  {
    std::cout << "couldn't find location of u_Projection\n"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  // setup which graphic pipeline we'll use
  glUseProgram(mesh->mPipeline);
  glBindVertexArray(mesh->mVertexArrayObject);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glUseProgram(0);
}

static void GLClearAllErrors()
{
  while (glGetError() != GL_NO_ERROR)
  {
  }
}

static bool GLCheckErrorStatus(const char *function, int line)
{
  while (GLenum error = glGetError())
  {
    // Check the hexcode of error number on
    // https://wikis.khronos.org/opengl/OpenGL_Error
    std::cout << "OpenGL Error:" << error << "\tLine:" << line
              << "\tFunction:" << function << std::endl;
    return true;
  }
  return false;
}

std::string loadShaderAsString(const std::string &filename)
{
  std::string result = "";
  std::string line = "";
  std::ifstream myFile(filename.c_str());

  if (myFile.is_open())
  {
    while (std::getline(myFile, line))
    {
      result += line + '\n';
    }
    myFile.close();
  }
  return result;
}

void getOpenGLVersion()
{
  std::cout << "OpenGL" << glGetString(GL_VENDOR) << std::endl;
  std::cout << "OpenGL" << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL" << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
}

void initializeProgram(App *app)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cout << "SDL2 not initialized" << std::endl;
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  gApp.mGraphicsApplicationWindow = SDL_CreateWindow(
      "OPENGL WINDOW", 0, 0, gApp.mScreenWidth, gApp.mScreenHeight, SDL_WINDOW_OPENGL);
  if (gApp.mGraphicsApplicationWindow == nullptr)
  {
    std::cout << "SDL_WINDOW_OPENGL not initialized" << std::endl;
    exit(1);
  }

  gApp.mOpenGLContext = SDL_GL_CreateContext(gApp.mGraphicsApplicationWindow);
  if (gApp.mOpenGLContext == nullptr)
  {
    std::cout << "OpenGL Context not initialized" << std::endl;
    exit(1);
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
  {
    std::cout << "GLAD not initialized" << glGetString(GL_VENDOR) << std::endl;
  }

  getOpenGLVersion();
}

GLuint compileShader(GLuint type, const std::string &source)
{
  GLuint shaderObject;
  if (type == GL_VERTEX_SHADER)
  {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  }
  else if (type == GL_FRAGMENT_SHADER)
  {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char *src = source.c_str();
  glShaderSource(shaderObject, 1, &src, nullptr);
  glCompileShader(shaderObject);
  return shaderObject;
}

GLuint createShaderProgram(const std::string &vertexShaderSource,
                           const std::string &fragmentShaderSource)
{
  GLuint programObject = glCreateProgram();
  GLuint myVertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint myFragmentShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  glAttachShader(programObject, myVertexShader);
  glAttachShader(programObject, myFragmentShader);

  glLinkProgram(programObject);

  glValidateProgram(programObject);

  return programObject;
}

void createGraphicsPipeline()
{
  // path should be according to exe's location
  std::string vertexShaderSource = loadShaderAsString("../shaders/vert.glsl");
  std::string fragmentShaderSource = loadShaderAsString("../shaders/frag.glsl");
  gApp.mGraphicsPipelineShaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void input()
{

  static int mouseX = gApp.mScreenWidth / 2, mouseY = gApp.mScreenHeight / 2;
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0)
  {
    if (e.type == SDL_QUIT)
    {
      std::cout << "Exiting" << std::endl;
      gApp.mQuit = true;
    }
    else if (e.type == SDL_MOUSEMOTION)
    {
      mouseX += e.motion.xrel;
      mouseY += e.motion.yrel;
      gApp.mCamera.MouseLook(mouseX, mouseY);
    }
  }

  // TODO : Use some other key to move object
  // g_Uoffset += 0.001f;
  // std::cout << "g_Uoffset: " << g_Uoffset << std::endl;
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float speed = 0.05f;
  if (state[SDL_SCANCODE_UP])
  {
    gApp.mCamera.MoveForward(speed);
  }

  if (state[SDL_SCANCODE_DOWN])
  {
    gApp.mCamera.MoveBackward(speed);
    // g_Uoffset -= 0.001f;
    // std::cout << "g_Uoffset: " << g_Uoffset << std::endl;
  }
  if (state[SDL_SCANCODE_LEFT])
  {
    gApp.mCamera.MoveLeft(speed);
  }
  if (state[SDL_SCANCODE_RIGHT])
  {
    gApp.mCamera.MoveRight(speed);
  }
}

void mainLoop()
{
  SDL_WarpMouseInWindow(gApp.mGraphicsApplicationWindow, gApp.mScreenWidth / 2,
                        gApp.mScreenHeight / 2);
  SDL_SetRelativeMouseMode(SDL_TRUE);

  while (!gApp.mQuit)
  {
    input();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gApp.mScreenWidth, gApp.mScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    drawMesh(&gMesh1);

    drawMesh(&gMesh2);
    // meshCreate();
    SDL_GL_SwapWindow(gApp.mGraphicsApplicationWindow);
  }
}
void cleanUp()
{
  SDL_DestroyWindow(gApp.mGraphicsApplicationWindow);
  gApp.mGraphicsApplicationWindow = nullptr;
  meshDelete(&gMesh1);
  meshDelete(&gMesh2);
  glDeleteProgram(gApp.mGraphicsPipelineShaderProgram);
  SDL_Quit();
}

int main()
{
  initializeProgram(&gApp);
  meshCreate(&gMesh1);
  gMesh1.mTransform.x = 0.0f;
  gMesh1.mTransform.y = 0.0f;
  gMesh1.mTransform.z = -2.0f;
  meshCreate(&gMesh2);
  gMesh2.mTransform.x = 0.0f;
  gMesh2.mTransform.y = 0.0f;
  gMesh2.mTransform.z = -4.0f;
  createGraphicsPipeline();

  // For each our meshes set them to a pipeline
  meshSetPipeline(&gMesh1, gApp.mGraphicsPipelineShaderProgram);
  meshSetPipeline(&gMesh2, gApp.mGraphicsPipelineShaderProgram);
  mainLoop();
  cleanUp();
  return 0;
}