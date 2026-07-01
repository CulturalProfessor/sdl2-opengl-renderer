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
#include <stb_image.h>
#include <string>
#include <vector>

struct App {
  int mScreenWidth = 1080;
  int mScreenHeight = 800;
  SDL_Window *mGraphicsApplicationWindow = nullptr;
  SDL_GLContext mOpenGLContext = nullptr;
  bool mQuit = false;
  GLuint mGraphicsPipelineShaderProgram = 0;
  // Create a single global camera
  Camera mCamera;
};

struct Transform {
  glm::mat4 mModelMatrix{glm::mat4(1.0f)};
};

struct Mesh3D {
  GLuint mVertexArrayObject = 0;
  GLuint mVertexBufferObject = 0;
  GLuint mIndexBufferObject = 0;
  GLuint mPipeline = 0;
  GLuint mTexture;
  Transform mTransform;
};

App gApp;
Mesh3D gMesh1;
Mesh3D gMesh2;

// #define GLCheck(x)    \
//   GLClearAllErrors(); \
//   x;                  \
//   GLCheckErrorStatus(#x, __LINE__);

GLuint generateTexture(const char *imagePath) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options
  // (on the currently bound texture object)
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels, format = 1;

  unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

  if (nrChannels == 1) {
    format = GL_RED;
  } else if (nrChannels == 3) {
    format = GL_RGB;
  } else if (nrChannels == 4) {
    format = GL_RGBA;
  }

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return texture;
}

// Returns the location of uniform variable based on it's name
int findUniformLocation(GLuint pipeline, const GLchar *name) {
  GLint location = glGetUniformLocation(pipeline, name);

  if (location < 0) {
    std::cerr << "couldn't find location of" << name << "maybe spelling error"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return location;
}

// Setup vertex data per mesh
void meshCreate(Mesh3D *mesh) {
  const std::vector<GLfloat> vertexData{
      // pos (x,y,z)      color (r,g,b)       uv (u,v) (texture coordinates)
      // ---- Front face (dark red) ----
      -0.5f, -0.5f, 0.5f, 0.75f, 0.22f, 0.17f, 0.0f, 0.0f, // vertex 1
      0.5f, -0.5f, 0.5f, 0.75f, 0.22f, 0.17f, 1.0f, 0.0f,  // vertex 2
      0.5f, 0.5f, 0.5f, 0.75f, 0.22f, 0.17f, 1.0f, 1.0f,   // vertex 3
      -0.5f, 0.5f, 0.5f, 0.75f, 0.22f, 0.17f, 0.0f, 1.0f,  // vertex 4

      // ---- Back face (burnt orange) ----
      0.5f, -0.5f, -0.5f, 0.83f, 0.33f, 0.00f, 0.0f, 0.0f,  // vertex 5
      -0.5f, -0.5f, -0.5f, 0.83f, 0.33f, 0.00f, 1.0f, 0.0f, // vertex 6
      -0.5f, 0.5f, -0.5f, 0.83f, 0.33f, 0.00f, 1.0f, 1.0f,  // vertex 7
      0.5f, 0.5f, -0.5f, 0.83f, 0.33f, 0.00f, 0.0f, 1.0f,   // vertex 8

      // ---- Right face (dark gold) ----
      0.5f, -0.5f, 0.5f, 0.72f, 0.58f, 0.04f, 0.0f, 0.0f,  // vertex 9
      0.5f, -0.5f, -0.5f, 0.72f, 0.58f, 0.04f, 1.0f, 0.0f, // vertex 10
      0.5f, 0.5f, -0.5f, 0.72f, 0.58f, 0.04f, 1.0f, 1.0f,  // vertex 11
      0.5f, 0.5f, 0.5f, 0.72f, 0.58f, 0.04f, 0.0f, 1.0f,   // vertex 12

      // ---- Left face (forest green) ----
      -0.5f, -0.5f, -0.5f, 0.12f, 0.52f, 0.29f, 0.0f, 0.0f, // vertex 13
      -0.5f, -0.5f, 0.5f, 0.12f, 0.52f, 0.29f, 1.0f, 0.0f,  // vertex 14
      -0.5f, 0.5f, 0.5f, 0.12f, 0.52f, 0.29f, 1.0f, 1.0f,   // vertex 15
      -0.5f, 0.5f, -0.5f, 0.12f, 0.52f, 0.29f, 0.0f, 1.0f,  // vertex 16

      // ---- Top face (deep blue) ----
      -0.5f, 0.5f, 0.5f, 0.12f, 0.38f, 0.55f, 0.0f, 0.0f,  // vertex 17
      0.5f, 0.5f, 0.5f, 0.12f, 0.38f, 0.55f, 1.0f, 0.0f,   // vertex 18
      0.5f, 0.5f, -0.5f, 0.12f, 0.38f, 0.55f, 1.0f, 1.0f,  // vertex 19
      -0.5f, 0.5f, -0.5f, 0.12f, 0.38f, 0.55f, 0.0f, 1.0f, // vertex 20

      // ---- Bottom face (dark purple) ----
      -0.5f, -0.5f, -0.5f, 0.42f, 0.20f, 0.51f, 0.0f, 0.0f, // vertex 21
      0.5f, -0.5f, -0.5f, 0.42f, 0.20f, 0.51f, 1.0f, 0.0f,  // vertex 22
      0.5f, -0.5f, 0.5f, 0.42f, 0.20f, 0.51f, 1.0f, 1.0f,   // vertex 23
      -0.5f, -0.5f, 0.5f, 0.42f, 0.20f, 0.51f, 0.0f, 1.0f,  // vertex 24
  };

  // winding order, take counter clockwise of each face as
  // seen in front of that face
  const std::vector<GLuint> indexBufferData{
      0,  1,  2,  0,  2,  3,  // front
      4,  5,  6,  4,  6,  7,  // back
      8,  9,  10, 8,  10, 11, // right
      12, 13, 14, 12, 14, 15, // left
      16, 17, 18, 16, 18, 19, // top
      20, 21, 22, 20, 22, 23, // bottom
  };
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
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 8, (void *)0);

  // Color Information
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8,
                        (GLvoid *)(sizeof(GLfloat) * 3));

  // Texture information
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));

  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

void meshDelete(Mesh3D *mesh) {
  glDeleteBuffers(1, &mesh->mVertexBufferObject);
  glDeleteVertexArrays(1, &mesh->mVertexArrayObject);
}

/*
Needs to set pipeline before we draw
*/
void meshSetPipeline(Mesh3D *mesh, GLuint pipeline) {
  mesh->mPipeline = pipeline;
}

/**
 * Note : We per mesh choose the graphics pipeline we'll use
 * Generally not efficient to change state(pipelines) frequently,
 * we're doing this for flexibilty
 */

void drawMesh(Mesh3D *mesh) {
  if (mesh == nullptr) {
    return;
  }

  glUseProgram(mesh->mPipeline);

  // note we oftem combine view and model matrix to send in one uniform
  glm::mat4 view = gApp.mCamera.GetViewMatrix();

  GLint u_ModelMatrixLocation =
      findUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_ModelMatrix");
  glUniformMatrix4fv(u_ModelMatrixLocation, 1, false,
                     &mesh->mTransform.mModelMatrix[0][0]);

  GLint u_ViewLocation =
      findUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_ViewMatrix");
  glUniformMatrix4fv(u_ViewLocation, 1, false, &view[0][0]);

  // Projection Matrix (in perspective)
  glm::mat4 perspective = gApp.mCamera.GetProjectionMatrix();

  // Retrieve our location of our perspective matrix uniform
  GLint u_ProjectionLocation =
      findUniformLocation(gApp.mGraphicsPipelineShaderProgram, "u_Projection");
  glUniformMatrix4fv(u_ProjectionLocation, 1, false, &perspective[0][0]);

  // setup which graphic pipeline we'll use
  glUseProgram(mesh->mPipeline);
  glBindVertexArray(mesh->mVertexArrayObject);
  glBindTexture(GL_TEXTURE_2D, mesh->mTexture);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glUseProgram(0);
}

// translates a mesh, updates it's model matrix
void meshTranslate(Mesh3D *mesh, float x, float y, float z) {
  mesh->mTransform.mModelMatrix =
      glm::translate(mesh->mTransform.mModelMatrix, glm::vec3(x, y, z));
}

// rotates a mesh about a axis (a vector)
void meshRotate(Mesh3D *mesh, float yAngle, glm::vec3 axis) {
  mesh->mTransform.mModelMatrix =
      glm::rotate(mesh->mTransform.mModelMatrix, glm::radians(yAngle), axis);
}

// scales a mesh in non-uniform way
void meshScale(Mesh3D *mesh, float x, float y, float z) {
  mesh->mTransform.mModelMatrix =
      glm::scale(mesh->mTransform.mModelMatrix, glm::vec3(x, y, z));
}

static void GLClearAllErrors() {
  while (glGetError() != GL_NO_ERROR) {
  }
}

static bool GLCheckErrorStatus(const char *function, int line) {
  while (GLenum error = glGetError()) {
    // Check the hexcode of error number on
    // https://wikis.khronos.org/opengl/OpenGL_Error
    std::cout << "OpenGL Error:" << error << "\tLine:" << line
              << "\tFunction:" << function << std::endl;
    return true;
  }
  return false;
}

std::string loadShaderAsString(const std::string &filename) {
  std::string result = "";
  std::string line = "";
  std::ifstream myFile(filename.c_str());

  if (myFile.is_open()) {
    while (std::getline(myFile, line)) {
      result += line + '\n';
    }
    myFile.close();
  }
  return result;
}

void getOpenGLVersion() {
  std::cout << "OpenGL" << glGetString(GL_VENDOR) << std::endl;
  std::cout << "OpenGL" << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL" << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
}

void initializeProgram(App *app) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL2 not initialized" << std::endl;
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  gApp.mGraphicsApplicationWindow =
      SDL_CreateWindow("OPENGL WINDOW", 0, 0, gApp.mScreenWidth,
                       gApp.mScreenHeight, SDL_WINDOW_OPENGL);
  if (gApp.mGraphicsApplicationWindow == nullptr) {
    std::cout << "SDL_WINDOW_OPENGL not initialized" << std::endl;
    exit(1);
  }

  gApp.mOpenGLContext = SDL_GL_CreateContext(gApp.mGraphicsApplicationWindow);
  if (gApp.mOpenGLContext == nullptr) {
    std::cout << "OpenGL Context not initialized" << std::endl;
    exit(1);
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    std::cout << "GLAD not initialized" << glGetString(GL_VENDOR) << std::endl;
  }

  getOpenGLVersion();
}

GLuint compileShader(GLuint type, const std::string &source) {
  GLuint shaderObject;
  if (type == GL_VERTEX_SHADER) {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  } else if (type == GL_FRAGMENT_SHADER) {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char *src = source.c_str();
  glShaderSource(shaderObject, 1, &src, nullptr);
  glCompileShader(shaderObject);
  return shaderObject;
}

GLuint createShaderProgram(const std::string &vertexShaderSource,
                           const std::string &fragmentShaderSource) {
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

void createGraphicsPipeline() {
  // path should be according to exe's location
  std::string vertexShaderSource = loadShaderAsString("../shaders/vert.glsl");
  std::string fragmentShaderSource = loadShaderAsString("../shaders/frag.glsl");
  gApp.mGraphicsPipelineShaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void input() {

  static int mouseX = gApp.mScreenWidth / 2, mouseY = gApp.mScreenHeight / 2;
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      std::cout << "Exiting" << std::endl;
      gApp.mQuit = true;
    } else if (e.type == SDL_MOUSEMOTION) {
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
  if (state[SDL_SCANCODE_UP]) {
    gApp.mCamera.MoveForward(speed);
  }

  if (state[SDL_SCANCODE_DOWN]) {
    gApp.mCamera.MoveBackward(speed);
    // g_Uoffset -= 0.001f;
    // std::cout << "g_Uoffset: " << g_Uoffset << std::endl;
  }
  if (state[SDL_SCANCODE_LEFT]) {
    gApp.mCamera.MoveLeft(speed);
  }
  if (state[SDL_SCANCODE_RIGHT]) {
    gApp.mCamera.MoveRight(speed);
  }
}

void mainLoop() {
  SDL_WarpMouseInWindow(gApp.mGraphicsApplicationWindow, gApp.mScreenWidth / 2,
                        gApp.mScreenHeight / 2);
  SDL_SetRelativeMouseMode(SDL_TRUE);

  while (!gApp.mQuit) {
    input();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, gApp.mScreenWidth, gApp.mScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    static float rotate = 0.5f;

    meshRotate(&gMesh1, rotate, glm::vec3(1.0f, 0.0f, 0.0f));
    meshRotate(&gMesh2, -rotate, glm::vec3(0.0f, 1.0f, 0.0f));
    drawMesh(&gMesh1);

    drawMesh(&gMesh2);
    // meshCreate();
    SDL_GL_SwapWindow(gApp.mGraphicsApplicationWindow);
  }
}
void cleanUp() {
  SDL_DestroyWindow(gApp.mGraphicsApplicationWindow);
  gApp.mGraphicsApplicationWindow = nullptr;
  meshDelete(&gMesh1);
  meshDelete(&gMesh2);
  glDeleteProgram(gApp.mGraphicsPipelineShaderProgram);
  SDL_Quit();
}

int main() {
  initializeProgram(&gApp);
  gApp.mCamera.SetProjectionMatrix(
      glm::radians(45.0f), (float)gApp.mScreenWidth / (float)gApp.mScreenHeight,
      0.1f, 100.0f);

  gMesh1.mTexture = generateTexture("../assets/brick.jpg");
  gMesh2.mTexture = generateTexture("../assets/wall.jpg");

  meshCreate(&gMesh1);
  meshCreate(&gMesh2);

  // Order of transformations matter,
  // try changing for different effects
  // keep input matrix as identity
  meshTranslate(&gMesh1, 0.0f, 0.75f, -4.0f);
  meshScale(&gMesh1, 1.0f, 1.0f, 1.0f);

  meshTranslate(&gMesh2, 0.0f, -0.75f, -4.0f);
  meshScale(&gMesh2, 1.0f, 1.0f, 1.0f);
  createGraphicsPipeline();

  // For each our meshes set them to a pipeline
  meshSetPipeline(&gMesh1, gApp.mGraphicsPipelineShaderProgram);
  meshSetPipeline(&gMesh2, gApp.mGraphicsPipelineShaderProgram);
  mainLoop();
  cleanUp();
  return 0;
}