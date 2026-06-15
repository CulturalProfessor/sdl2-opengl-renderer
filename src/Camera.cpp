#include "Camera.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera() {
  // Assume we are at origin
  mEye = glm::vec3(0.0f, 0.0f, 0.0f);
  // Assume we are looking out into the world
  // Note: This is along '-z',because otherwise
  // we'd be looking behind us.
  mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
  // Assume we start on a perfect plane
  mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(mEye, mEye+mViewDirection, mUpVector);
}

void Camera::MouseLook(int mouseX, int mouseY) {
  std::cout << "Mouse :" << mouseX << "," << mouseY << std::endl;
  glm::vec2 currentMouse = glm::vec2(mouseX, mouseY);

  static bool firstLook = true;
  if (firstLook) {
    mOldMousePosition = currentMouse;
    firstLook = false;
  }

  glm::vec2 mouseDelta = mOldMousePosition - currentMouse;

  mViewDirection =
      glm::rotate(mViewDirection, glm::radians(mouseDelta.x), mUpVector);

  mOldMousePosition = currentMouse;
};

void Camera::MoveForward(float speed) {
  // Simple not yet correct
  mEye += (mViewDirection * speed);
};
void Camera::MoveBackward(float speed) { mEye -= (mViewDirection * speed); };
void Camera::MoveLeft(float speed) {};
void Camera::MoveRight(float speed) {};