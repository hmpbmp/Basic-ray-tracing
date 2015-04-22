#include "geometry.h"
#include <algorithm>
#include <poly34.h>

//Muller-Trumbore
Intersection Triangle::intersect ( Ray &ray ) {
  Intersection inter;
  inter.param = -1;
  glm::vec3 edge1, edge2;
  glm::vec3 P, Q, T;
  glm::vec3 rayDirection = ray.getDirection();
  glm::vec3 rayOrigin = ray.getOrigin();
  float det, inv_det, u, v;
  float t;
  edge1  = p1 - p0;
  edge2  = p2 - p0;
  P   = glm::cross ( rayDirection, edge2 );
  det = glm::dot ( edge1, P );
  if ( det > -FLT_EPSILON && det < FLT_EPSILON ) {
    return inter;
  }

  inv_det = 1.f / det;
  T = rayOrigin - p0;
  u = glm::dot ( T, P ) * inv_det;
  if ( u < 0.f || u > 1.f ) {
    return inter;
  }

  Q = glm::cross ( T, edge1 );
  v = glm::dot ( rayDirection, Q ) * inv_det;
  if ( v < 0.f || u + v  > 1.f ) {
    return inter;
  }

  t = glm::dot ( edge2, Q ) * inv_det;

  if ( t > FLT_EPSILON ) {
    inter.param = t;
    inter.point = rayOrigin + t * rayDirection;
    inter.normal = glm::normalize ( glm::cross ( edge1,edge2 ) );
  }
  return inter;
}



Intersection Sphere::intersect ( Ray &ray ) {
  Intersection inter;
  inter.param = -1;
  glm::vec3 rayDirection = ray.getDirection();
  glm::vec3 rayOrigin = ray.getOrigin();
  float a = glm::dot ( rayDirection,rayDirection );
  float b = glm::dot ( rayDirection, rayOrigin );
  float c = glm::dot ( rayOrigin, rayOrigin ) - radius * radius;

  float rad = b * b - a * c;
  if ( rad < FLT_EPSILON ) {
    return inter;
  }

  float x1 = ( -b - sqrtf ( rad ) ) / a;
  float x2 = ( -b + sqrtf ( rad ) ) / a;

  if ( x1 > FLT_EPSILON ) {
    inter.param = x1;
    inter.point = rayOrigin + x1 * rayDirection;
    inter.normal = glm::normalize ( ( rayOrigin + x1 * rayDirection ) / radius );
    inter.dist = glm::distance ( inter.point, rayOrigin );
    return inter;
  }
  if ( x2 > FLT_EPSILON ) {
    Intersection inter;
    inter.param = x2;
    inter.point = rayOrigin + x2 * rayDirection;
    inter.normal = glm::normalize ( ( rayOrigin + x2 * rayDirection ) / radius );
    inter.dist = glm::distance ( inter.point, rayOrigin );
    return inter;
  }
  return inter;
}



Intersection Torus::intersect ( Ray &ray ) {
  Intersection inter;
  inter.param = -1;
  glm::vec3 rayOrigin = ray.getOrigin();
  glm::vec3 rayDirection = ray.getDirection();

  float e = glm::dot ( rayOrigin, rayOrigin ) - radius * radius - tube_radius * tube_radius;
  float f = glm::dot ( rayOrigin, rayDirection );
  float g = 4.0f * radius * radius;

  float A = 4.0f  * f;
  float B = 2.0f * e + 4.0f * f * f + g * rayDirection.z * rayDirection.z;
  float C = 4.0f * f * e + 2.0f * g * rayOrigin.z * rayDirection.z;
  float D = e * e - g * ( tube_radius * tube_radius - rayOrigin.z * rayOrigin.z );


  double roots[4];
  int r_num = SolveP4 ( roots, A, B, C, D );
  if ( r_num == 0 ) {
    return inter;
  }

  double min = FLT_MAX;
  for ( int i = 0; i < r_num; i++ ) {
    if ( roots[i] < min && roots[i] > FLT_EPSILON ) {
      min = roots[i];
    }
  }
  if ( min == FLT_MAX ) {
    return inter;
  }

  inter.param = ( float ) min;
  inter.point = rayOrigin + inter.param * rayDirection;
  inter.dist = glm::distance ( inter.point, rayOrigin );

  float a = radius / sqrt ( inter.point.x * inter.point.x + inter.point.y * inter.point.y );
  inter.normal.x = ( 1 - a ) * inter.point.x;
  inter.normal.y = ( 1 - a ) * inter.point.y;
  inter.normal.z = inter.point.z;
  inter.normal = glm::normalize ( inter.normal );
  return inter;

}

Intersection Cylinder::intersect ( Ray &ray ) {
  Intersection inter;
  inter.param = -1;
  glm::vec3 rayOrigin = ray.getOrigin();
  glm::vec3 rayDirection = ray.getDirection();


  float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y;
  float b = rayDirection.x * rayOrigin.x + rayDirection.y * rayOrigin.y;
  float c = rayOrigin.x * rayOrigin.x + rayOrigin.y * rayOrigin.y - radius * radius;

  float rad = b * b - a * c;
  if ( rad < FLT_EPSILON ) {
    return inter;
  }

  float x1 = ( -b - sqrtf ( rad ) ) / a;
  float x2 = ( -b + sqrtf ( rad ) ) / a;

  if ( ( x1 > FLT_EPSILON ) && ( ( rayOrigin + x1 * rayDirection ).z > FLT_EPSILON - height / 2 ) && ( ( rayOrigin + x1 * rayDirection ).z < FLT_EPSILON + height / 2 ) ) {
    inter.param = x1;
    inter.point = rayOrigin + x1 * rayDirection;
    inter.normal.x = inter.point.x / radius;
    inter.normal.y = inter.point.y / radius;
    inter.normal.z = 0.0f;
    inter.dist = glm::distance ( inter.point, rayOrigin );
    return inter;
  }
  if ( ( x2 > FLT_EPSILON ) && ( ( rayOrigin + x2 * rayDirection ).z > FLT_EPSILON - height / 2 ) && ( ( rayOrigin + x2 * rayDirection ).z < FLT_EPSILON + height / 2 ) ) {
    Intersection inter;
    inter.param = x2;
    inter.point = rayOrigin + x2 * rayDirection;
    inter.normal.x = inter.point.x / radius;
    inter.normal.y = inter.point.y / radius;
    inter.normal.z = 0.0f;
    inter.dist = glm::distance ( inter.point, rayOrigin );
    return inter;
  }

  glm::vec3 center ( 0.0f, 0.0f, height / 2 );
  glm::vec3 normal ( 0.0f, 0.0f, 1.0f );

  float t1 = glm::dot ( center - rayOrigin, normal ) / glm::dot ( rayDirection, normal );
  if ( ( t1 > FLT_EPSILON ) && ( glm::distance ( center, rayOrigin + t1 * rayDirection ) <= radius * radius ) ) {
    inter.param = t1;
    inter.point = rayOrigin + t1 * rayDirection;
    inter.normal = normal;
    inter.dist = glm::distance ( inter.point, rayOrigin );
  }

  center =  glm::vec3 ( 0.0f, 0.0f, - height / 2 );
  normal =  glm::vec3 ( 0.0f, 0.0f, -1.0f );

  float t2 = glm::dot ( center - rayOrigin, normal ) / glm::dot ( rayDirection, normal );
  if ( t1 < t2 ) {
    return inter;
  }
  if ( ( t2 > FLT_EPSILON ) && ( glm::distance ( center, rayOrigin + t2 * rayDirection ) <= radius * radius ) ) {
    inter.param = t2;
    inter.point = rayOrigin + t2 * rayDirection;
    inter.normal = normal;
    inter.dist = glm::distance ( inter.point, rayOrigin );
    return inter;
  }

  return inter;
}
