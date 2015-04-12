#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED
#include <glm/glm.hpp>
#include <string>


class Object {
public:
  virtual void intersect() = 0;
}



class Triangle : public Object {
public:
  void intersect();
private:
  glm::vec3 p0,p1,p2;
}

class Sphere : public Object {
public:
  void intersect();
private:
  glm::vec3 center;
  float radius;
}

class Cylinder : public Object {
public:
  void intersect();
private:
  glm::vec3 center;
  float radius, height;
}

class Torus : public Object {
public:
  void intersect();
private:
  glm::vec3 center;
  float radius, tube_radius;
}

class ObjModel : public Object {
public:
  void intersect();
private:
  std::string filename;
}

#endif