#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "glm/glm.hpp"

class Camera {
public:
  Camera();
  ~Camera();
  void setPosition(float x, float y, float z);
  void setPosition(glm::vec3 pos);
  glm::vec3 getPosition();
private:
  glm::vec3 position;
  glm::vec3 orientation;
  float fov_x, fov_y;
};


#endif