#include "geometry.h"

//Muller-Trumbore
std::vector<Intersection> Triangle::intersect ( Ray ray ) {
  std::vector<Intersection> res;
  glm::vec3 edge1, edge2;
  glm::vec3 P, Q, T;
  glm::vec3 dir = ray.getDirection();
  glm::vec3 orig = ray.getOrigin();
  float det, inv_det, u, v;
  float t;
  edge1  = p1 - p0;
  edge2  = p2 - p0;
  P   = glm::cross ( dir, edge2 );
  det = glm::dot ( edge1, P );
  if ( det > -FLT_EPSILON && det < FLT_EPSILON ) {
    return res;
  }

  inv_det = 1.f / det;
  T = orig - p0;
  u = glm::dot ( T, P ) * inv_det;
  if ( u < 0.f || u > 1.f ) {
    return res;
  }

  Q = glm::cross ( T, edge1 );
  v = glm::dot ( dir, Q ) * inv_det;
  if ( v < 0.f || u + v  > 1.f ) {
    return res;
  }

  t = glm::dot ( edge2, Q ) * inv_det;

  if ( t > FLT_EPSILON ) {
    Intersection inter;
    inter.param = t;
    inter.point = orig + t * dir;
  }
  return res;
}



std::vector <Intersection> Sphere::intersect ( Ray ray ) {
  std::vector<Intersection> res;
  glm::vec3 dir = ray.getDirection();
  glm::vec3 orig = ray.getOrigin();
  float a = glm::dot ( dir,dir );
  float b = glm::dot ( dir, orig );
  float c = glm::dot ( orig, orig ) - radius * radius;

  float rad = b * b - a * c;
  if ( rad < 0 ) {
    return res;
  }

  float x1 = ( -b - sqrtf ( rad ) ) / a;
  float x2 = ( -b + sqrtf ( rad ) ) / a;

  if ( x1 > FLT_EPSILON ) {
    Intersection inter;
    inter.param = x1;
    inter.point = orig + x1 * dir;
    res.push_back ( inter );
  }

  if ( x2 > FLT_EPSILON ) {
    Intersection inter;
    inter.param = x2;
    inter.point = orig + x2 * dir;
    res.push_back ( inter );
  }

  return res;
}




