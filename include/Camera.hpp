#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/glm.hpp"

class Camera {

public:
  Camera();

  void SetProjectionMatrix(float fovy, float aspect, float near, float far);
  glm::mat4 GetProjectionMatrix() const;
  // The main view matrix we'll create and return
  glm::mat4 GetViewMatrix() const;
  glm::vec3 GetEyePosition() const;
  glm::vec3 GetViewDirection() const;

  void MouseLook(int mouseX, int mouseY);
  void MoveForward(float speed);
  void MoveBackward(float speed);
  void MoveLeft(float speed);
  void MoveRight(float speed);
  void MoveUp(float speed);
  void MoveDown(float speed);

private:
  glm::mat4 mProjectionMatrix;
  glm::vec3 mEye;
  glm::vec3 mViewDirection;
  glm::vec3 mUpVector;
  glm::vec2 mOldMousePosition;
};

#endif