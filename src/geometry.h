#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ray.h"

struct Intersection {
  float param;
  glm::vec3 point;
};


class Object {
public:
  virtual std::vector<Intersection> intersect ( Ray ray ) = 0 {};
};


class Triangle : public Object {
public:
  virtual std::vector<Intersection> intersect ( Ray ray );
private:
  glm::vec3 p0,p1,p2;
};

class Sphere : public Object {
public:
  Sphere ( float r ) : radius ( r ) {};
  virtual std::vector<Intersection> intersect ( Ray ray );
private:
  float radius;
};


/*
class Cylinder : public Object {
public:
  std::vector<Intersection> intersect ( Ray ray );
private:
  float radius, height;
};

class Torus : public Object {
public:
  std::vector<Intersection> intersect ( Ray ray );
private:
  float radius, tube_radius;
};

class ObjModel : public Object {
public:
  std::vector<Intersection> intersect ( Ray ray );
private:
  std::string filename;
};
  */

#endif