#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

class Ray {
public:
  glm::vec3 getOrigin() {
    return origin;
  }
  glm::vec3 getDirection() {
    return direction;
  }
  void setOrigin ( glm::vec3 vec ) {
    origin = vec;
  }
  void setDirection ( glm::vec3 vec ) {
    direction = vec;
  }

private:
  glm::vec3 origin;
  glm::vec3 direction;
};

#endif