#include "camera.h"


glm::vec3 Camera::getPosition() {
  return position;
}

void Camera::setPosition(glm::vec3 pos) {
  position = pos;
}

void Camera::setPosition(float x,float y,float z) {
  position = glm::vec3(x,y,z);
}