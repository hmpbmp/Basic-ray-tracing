#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "glm/glm.hpp"

class Camera {
public:
  Camera();
  glm::vec3 getPosition();
  void setPosition ( float x, float y, float z );
  void setPosition ( glm::vec3 &pos );

  glm::vec3 getOrientation();
  void setOrientation ( glm::vec3 &or );
  void setOrientation ( float x, float y, float z );

  float getFOVX();
  float getFOVY();
  void setFOV ( float x, float y );

  glm::vec3 getDirection();
  void setDirection ( glm::vec3 &dir );

  glm::vec3 getUp();
  glm::vec3 getLookAt();

private:
  glm::vec3 position;
  glm::vec3 orientation;
  glm::vec3 up, lookat, dir;
  glm::vec3 direction;
  float fov_x, fov_y;
};


#endif