#include "Camera.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

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
  return glm::lookAt(mEye, mViewDirection, mUpVector);
}

void Camera::MoveForward(float speed) {
  // Simple not yet correct
  mEye.z -= speed;
};
void Camera::MoveBackward(float speed) { mEye.z += speed; };
void Camera::MoveLeft(float speed) {};
void Camera::MoveRight(float speed) {};