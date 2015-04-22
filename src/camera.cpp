#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#define PI_F 3.14159265359f

Camera::Camera() {
  up = glm::vec3 ( 0.0, 0.0, 1.0 );
  lookat = glm::vec3 ( 0.0,0.0,0.0 );
}

glm::vec3 Camera::getPosition() {
  return position;
}

void Camera::setPosition ( glm::vec3 &pos ) {
  position = pos;
  dir = glm::normalize(lookat - pos);
}

void Camera::setPosition ( float x,float y,float z ) {
  setPosition(glm::vec3 ( x,y,z ));
}

glm::vec3 Camera::getOrientation() {
  return orientation;
}

void Camera::setOrientation ( glm::vec3 &or ) {
  setOrientation ( or.x, or.y, or.z );
}

void Camera::setOrientation ( float h, float p, float r ) {
  orientation = glm::vec3 ( h, p, r );
  float dist = glm::length(lookat - position);
  glm::mat4x4 trans1 ( 1.0f );
  trans1 = glm::rotate(trans1, r / 180 * PI_F, dir);
  glm::vec4 tr_vec = trans1 * glm::vec4(up, 0.0f);
  up = glm::normalize(glm::vec3(tr_vec.x,tr_vec.y,tr_vec.z));
  
  glm::mat4x4 trans2 ( 1.0f );
  glm::vec3 right = glm::normalize(glm::cross(dir,up));
  trans2 = glm::rotate(trans2, p / 180 * PI_F, right);
  tr_vec = trans2 * glm::vec4(dir, 0.0f);
  dir = glm::normalize(glm::vec3(tr_vec.x,tr_vec.y,tr_vec.z));
  up = glm::normalize(glm::cross(right, dir));

  glm::mat4x4 trans3 ( 1.0f );
  trans3 = glm::rotate(trans3, h / 180 * PI_F, up);
  tr_vec = trans3 * glm::vec4(dir, 0.0f);
  dir = glm::normalize(glm::vec3(tr_vec.x,tr_vec.y,tr_vec.z));
  lookat = position + dir * dist;
}

float Camera::getFOVX() {
  return fov_x;
}

float Camera::getFOVY() {
  return fov_y;
}

glm::vec3 Camera::getDirection() {
  return direction;
}

void Camera::setDirection ( glm::vec3 &dir ) {
  direction = dir;
}


void Camera::setFOV ( float x, float y ) {
  fov_x = x;
  fov_y = y;
}

glm::vec3 Camera::getUp() {
  return up;
}

glm::vec3 Camera::getLookAt() {
  return lookat;
}