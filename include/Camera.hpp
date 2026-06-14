#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/glm.hpp"

class Camera {

public:
  Camera();
  // The main view matrix we'll create and return
  glm::mat4 GetViewMatrix() const;

  void MoveForward(float speed);
  void MoveBackward(float speed);
  void MoveLeft(float speed);
  void MoveRight(float speed);

private:
  glm::vec3 mEye;
  glm::vec3 mViewDirection;
  glm::vec3 mUpVector;
};

#endif