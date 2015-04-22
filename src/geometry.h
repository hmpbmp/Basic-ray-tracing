#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ray.h"

struct Intersection {
  float param;
  float dist;
  glm::vec3 point;
  glm::vec3 normal;

};

struct DistanceComparator {
  bool operator() ( const Intersection &a, const Intersection &b ) const {
    return ( a.dist < b.dist );
  }
};

struct NormalComparator {
  bool operator() ( const Intersection &a, const Intersection &b ) const {
    return ( a.normal.y < b.normal.y );
  }
};




class Object {
public:
  virtual Intersection intersect ( Ray &ray ) = 0 {};
};


class Triangle : public Object {
public:
  Triangle ( glm::vec3 first, glm::vec3 second, glm::vec3 third ) :p0 ( first ),p1 ( second ),p2 ( third ) {};
  virtual Intersection intersect ( Ray &ray );
private:
  glm::vec3 p0,p1,p2;
};


class Sphere : public Object {
public:
  Sphere ( float r ) : radius ( r ) {};
  virtual Intersection intersect ( Ray &ray );
private:
  float radius;
};


class Cylinder : public Object {
public:
  Cylinder ( float r, float h ) : radius ( r ), height ( h ) {};
  Intersection intersect ( Ray &ray );
private:
  float radius, height;
};
/*
class Cone : public Object {
public:
 Cone ( float r, float h ) : radius ( r ), height ( h ) {};
 Intersection intersect ( Ray &ray );
private:
 float radius, height;
}; */

class Torus : public Object {
public:
  Torus ( float rad, float tube_rad ) : radius ( rad ), tube_radius ( tube_rad ) {};
  virtual Intersection intersect ( Ray &ray );
private:
  float radius, tube_radius;
};

/*
class ObjModel : public Object {
public:
  std::vector<Intersection> intersect ( Ray ray );
private:
  std::string filename;
};
  */

#endif