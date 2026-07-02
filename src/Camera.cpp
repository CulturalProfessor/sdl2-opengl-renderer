#include "Camera.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera() {
  // Assume we are little away from origin
  mEye = glm::vec3(0.0f, 0.0f, 2.0f);
  // Assume we are looking out into the world
  // Note: This is along '-z',because otherwise
  // we'd be looking behind us.
  mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
  // Assume we start on a perfect plane
  mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(mEye, mEye + mViewDirection, mUpVector);
}

void Camera::SetProjectionMatrix(float fovy, float aspect, float near,
                                 float far) {
  mProjectionMatrix = glm::perspective(fovy, aspect, near, far);
}

glm::mat4 Camera::GetProjectionMatrix() const { return mProjectionMatrix; }

void Camera::MouseLook(int mouseX, int mouseY) {
  // std::cout << "Mouse :" << mouseX << "," << mouseY << std::endl;
  glm::vec2 currentMouse = glm::vec2(mouseX, mouseY);

  static bool firstLook = true;
  if (firstLook) {
    mOldMousePosition = currentMouse;
    firstLook = false;
  }

  float sensitivity = 0.25f;
  // pixels shift gets factored by sensitivity which is later converted to angle
  glm::vec2 mouseDelta = (mOldMousePosition - currentMouse) * sensitivity;

  // yaw (right<->left)
  mViewDirection =
      glm::rotate(mViewDirection, glm::radians(mouseDelta.x), mUpVector);

  // perpendicular to mUpVector (+y-axis) and mViewDirection
  //  ( our eye direction)
  glm::vec3 rightVector = glm::normalize(glm::cross(mViewDirection, mUpVector));

  // the pitch (up<->down) which is going to be set if
  // within limits (not parallel mUpVector)
  glm::vec3 pitchedDirection =
      glm::rotate(mViewDirection, glm::radians(mouseDelta.y), rightVector);

  // verify if pitch is not parallel to mUpVector
  // (leaving around ~8 degree margin) then set pitch
  if (glm::abs(glm::dot(glm::normalize(pitchedDirection), mUpVector)) < 0.99f) {
    mViewDirection = pitchedDirection;
  }

  mOldMousePosition = currentMouse;
};

void Camera::MoveForward(float speed) {
  // Simple not yet correct
  mEye += (mViewDirection * speed);
};
void Camera::MoveBackward(float speed) { mEye -= (mViewDirection * speed); };
void Camera::MoveLeft(float speed) {
  glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
  mEye -= rightVector * speed;
};
void Camera::MoveRight(float speed) {
  glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
  mEye += rightVector * speed;
};

void Camera::MoveUp(float speed) { mEye += mUpVector * speed; };
void Camera::MoveDown(float speed) { mEye -= mUpVector * speed; };