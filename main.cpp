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

int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window *gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;
bool gQuit = false;
GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gIndexBufferObject = 0;
GLuint gGraphicsPipelineShaderProgram = 0;
float g_Uoffset = -2.0f;
float g_URotate = 0.0f;
float g_UScale = 0.5f;
// Create a single global camera
Camera gCamera;

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

#define GLCheck(x)                                                             \
  GLClearAllErrors();                                                          \
  x;                                                                           \
  GLCheckErrorStatus(#x, __LINE__);

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

void initializeProgram() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL2 not initialized" << std::endl;
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  gGraphicsApplicationWindow = SDL_CreateWindow(
      "OPENGL WINDOW", 0, 0, gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);
  if (gGraphicsApplicationWindow == nullptr) {
    std::cout << "SDL_WINDOW_OPENGL not initialized" << std::endl;
    exit(1);
  }

  gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
  if (gOpenGLContext == nullptr) {
    std::cout << "OpenGL Context not initialized" << std::endl;
    exit(1);
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    std::cout << "GLAD not initialized" << glGetString(GL_VENDOR) << std::endl;
  }

  getOpenGLVersion();
}

void vertexSpecification() {
  const std::vector<GLfloat> vertexData{
      -0.5f, -0.5f, 0.0f, // vertex 1
      1.0f,  0.0f,  0.0f, // color of vertex 1
      0.5f,  -0.5f, 0.0f, // vertex 2
      0.0f,  1.0f,  0.0f, // color of vertex 2
      -0.5f, 0.5f,  0.0f, // vertex 3
      0.0f,  0.0f,  1.0f, // color of vertex 3
      0.5f,  0.5f,  0.0f, // vertex 4
      0.0f,  1.0f,  0.0f, // color of vertex 4
  };

  const std::vector<GLuint> indexBufferData{2, 0, 1, 3, 2, 1};
  glGenVertexArrays(1, &gVertexArrayObject);
  glBindVertexArray(gVertexArrayObject);
  glGenBuffers(1, &gVertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat),
               vertexData.data(), GL_STATIC_DRAW);

  // index buffer object
  glGenBuffers(1, &gIndexBufferObject);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size() * sizeof(GLuint),
               indexBufferData.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6,
                        (void *)0);

  // Color Information
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6,
                        (GLvoid *)(sizeof(GL_FLOAT) * 3));

  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
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
  gGraphicsPipelineShaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void input() {

  static int mouseX = gScreenWidth / 2, mouseY = gScreenHeight / 2;
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      std::cout << "Exiting" << std::endl;
      gQuit = true;
    } else if (e.type == SDL_MOUSEMOTION) {
      mouseX += e.motion.xrel;
      mouseY += e.motion.yrel;
      gCamera.MouseLook(mouseX, mouseY);
    }
  }

  g_URotate -= 0.01f;
  // std::cout << "g_URotate: " << g_URotate << std::endl;
  g_URotate += 0.01f;
  // std::cout << "g_URotate: " << g_URotate << std::endl;
  // TODO : Use some other key to move object
  // g_Uoffset += 0.001f;
  // std::cout << "g_Uoffset: " << g_Uoffset << std::endl;
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float speed = 0.0005f;
  if (state[SDL_SCANCODE_UP]) {
    gCamera.MoveForward(speed);
  }
  if (state[SDL_SCANCODE_DOWN]) {
    gCamera.MoveBackward(speed);
    // g_Uoffset -= 0.001f;
    // std::cout << "g_Uoffset: " << g_Uoffset << std::endl;
  }
  if (state[SDL_SCANCODE_LEFT]) {
    gCamera.MoveLeft(speed);
  }
  if (state[SDL_SCANCODE_RIGHT]) {
    gCamera.MoveRight(speed);
  }
}

void preDraw() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glViewport(0, 0, gScreenWidth, gScreenHeight);
  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glUseProgram(gGraphicsPipelineShaderProgram);

  g_URotate -= 0.01f;
  // std::cout << "g_URotate: " << g_URotate << std::endl;

  // Order of transformations matter,
  // try changing for different effects
  // keep input matrix as identity
  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, g_Uoffset));

  model =
      glm::rotate(model, glm::radians(g_URotate), glm::vec3(0.0f, 1.0f, 0.0f));

  model = glm::scale(model, glm::vec3(g_UScale, g_UScale, g_UScale));

  glm::mat4 view = gCamera.GetViewMatrix();

  GLint u_ViewLocation =
      glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ViewMatrix");

  if (u_ViewLocation >= 0) {
    glUniformMatrix4fv(u_ViewLocation, 1, GL_FALSE, &view[0][0]);
  } else {
    std::cout
        << "couldn't find location of u_ViewMatrix, maybe be spelling mistake\n"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  // Projection Matrix (in perspective)
  glm::mat4 perspective = glm::perspective(
      glm::radians(45.0f), (float)gScreenWidth / (float)gScreenHeight, 0.1f,
      100.0f);

  GLint u_ModelMatrixLocation =
      glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix");

  if (u_ModelMatrixLocation >= 0) {
    glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  } else {
    std::cout << "couldn't find location of u_ModelMatrix\n" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Retrieve our location of our perspective matrix uniform
  GLint u_ProjectionLocation =
      glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Projection");

  if (u_ProjectionLocation >= 0) {
    glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);
  } else {
    std::cout << "couldn't find location of u_Projection\n" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void draw() {
  glBindVertexArray(gVertexArrayObject);
  GLCheck(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}

void mainLoop() {
  SDL_WarpMouseInWindow(gGraphicsApplicationWindow, gScreenWidth / 2,
                        gScreenHeight / 2);
  SDL_SetRelativeMouseMode(SDL_TRUE);

  while (!gQuit) {
    input();
    preDraw();
    draw();
    SDL_GL_SwapWindow(gGraphicsApplicationWindow);
  }
}
void cleanUp() {
  SDL_DestroyWindow(gGraphicsApplicationWindow);
  SDL_Quit();
}

int main() {
  initializeProgram();
  vertexSpecification();
  createGraphicsPipeline();
  mainLoop();
  cleanUp();
  return 0;
}