#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ray.h"

struct Intersection {
  float dist;
  glm::vec3 point;
  glm::vec3 normal;
  std::vector<float> distances;

};


struct DistanceComparator {
  bool operator() ( const Intersection &a, const Intersection &b ) const {
    return ( a.dist < b.dist );
  }
};

struct NormalComparator {
  bool operator() ( const Intersection &a, const Intersection &b ) const {
    return ( a.normal.z < b.normal.z );
  }
};




class Object {
public:
  virtual Intersection intersect ( Ray &ray ) = 0 {};
protected:
  bool intersectAABB ( Ray &ray );
  void detmin ( glm::vec3 &min, glm::vec3 &v);
  void detmax ( glm::vec3 &max, glm::vec3 &v);
  glm::vec3 min, max;
};


class Plane: public Object {
public :
  Plane ( glm::vec3 normal ) :norm ( normal ) {};
  virtual Intersection intersect ( Ray &ray );
private:
  glm::vec3 norm;
  bool intersectAABB ( Ray & ray ) {
    return ( glm::dot ( ray.getDirection(), norm ) > FLT_EPSILON );
  }
};

class Triangle : public Object {
public:
  Triangle ( glm::vec3 first, glm::vec3 second, glm::vec3 third ) :p0 ( first ),p1 ( second ),p2 ( third ) {
    min.x = glm::min ( glm::min ( p0.x, p1.x ), p2.x );
    min.y = glm::min ( glm::min ( p0.y, p1.y ), p2.y );
    min.z = glm::min ( glm::min ( p0.z, p1.z ), p2.z );

    max.x = glm::max ( glm::max ( p0.x, p1.x ), p2.x );
    max.y = glm::max ( glm::max ( p0.y, p1.y ), p2.y );
    max.z = glm::max ( glm::max ( p0.z, p1.z ), p2.z );
  };
  virtual Intersection intersect ( Ray &ray );
private:
  glm::vec3 p0,p1,p2;

};


class Sphere : public Object {
public:
  Sphere ( float r ) : radius ( r ) {
    min = glm::vec3 ( -r,-r,-r );
    max = glm::vec3 ( r,r,r );
  };
  virtual Intersection intersect ( Ray &ray );
private:
  float radius;
};


class Cylinder : public Object {
public:
  Cylinder ( float r, float h ) : radius ( r ), height ( h ) {
    min = glm::vec3 ( -r, -r, -height /2 );
    max = glm::vec3 ( r, r, height / 2 );
  };
  virtual Intersection intersect ( Ray &ray );
private:
  float radius, height;
};

class Cone : public Object {
public:
  Cone ( float R, float r, float h ) : small_radius ( r ), big_radius ( R ), height ( h ) {
    zmin = h * r / ( R - r );
    zmax = zmin + h;
    tg = r / zmin;
    min = glm::vec3 ( -R, -R , zmin );
    max = glm::vec3 ( R, R , zmax );
  };
  virtual Intersection intersect ( Ray &ray );
private:
  float small_radius, big_radius, height;
  float tg;
  float zmin, zmax;
};

class Torus : public Object {
public:
  Torus ( float rad, float tube_rad ) : radius ( rad ), tube_radius ( tube_rad ) {
    min = glm::vec3 ( - ( rad + tube_rad ), - ( rad + tube_rad ), -tube_rad );
    max = glm::vec3 (  ( rad + tube_rad ),  ( rad + tube_rad ), tube_rad );
  };
  virtual Intersection intersect ( Ray &ray );
private:
  float radius, tube_radius;
};


class ObjModel : public Object {
public:
  ObjModel ( std::string filename );
  virtual Intersection intersect ( Ray &ray );
private:
  std::string name;
  std::vector<Triangle> triangles;
};

#endif