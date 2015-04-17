#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "glm/glm.hpp"

class Camera {
public:
  Camera();
  void setPosition ( float x, float y, float z );
  void setPosition ( glm::vec3 pos );
  void setOrientation ( glm::vec3 or );
  void setOrientation ( float x, float y, float z );
  void setFOV ( float x, float y );
  glm::vec3 getPosition();
  void calculateViewingCoordinates();
private:
  glm::vec3 position;
  glm::vec3 orientation;
  glm::vec3 up, lookat;
  glm::vec3 w,u,v;
  float fov_x, fov_y;
};


#endif