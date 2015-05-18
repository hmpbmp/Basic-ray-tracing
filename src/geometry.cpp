#include "geometry.h"
#include <algorithm>
#include <poly34.h>
#include <tiny_obj_loader.h>
#include <iostream>


void Object::detmin ( glm::vec3 &min, glm::vec3 &v) {
  if (v.x < min.x) {
    min.x = v.x;
  }
  if (v.y < min.y) {
    min.y = v.y;
  }
  if (v.z < min.z) {
    min.z = v.z;
  }
}

void Object::detmax ( glm::vec3 &max, glm::vec3 &v) {
  if (v.x > max.x) {
    max.x = v.x;
  }
  if (v.y > max.y) {
    max.y = v.y;
  }
  if (v.z > max.z) {
    max.z = v.z;
  }
}


bool Object::intersectAABB ( Ray &ray ) {
  glm::vec3 tmin,tmax;
  glm::vec3 dir = ray.getDirection();
  glm::vec3 orig = ray.getOrigin();
  float a = 1.0f / dir.x;
  tmin.x = ( a > FLT_EPSILON ) ? ( min.x - orig.x ) * a : ( max.x - orig.x ) * a;
  tmax.x = ( a > FLT_EPSILON ) ? ( max.x - orig.x ) * a : ( min.x - orig.x ) * a;

  float b = 1.0f / dir.y;
  tmin.y = ( b > FLT_EPSILON ) ? ( min.y - orig.y ) * b : ( max.y - orig.y ) * b;
  tmax.y = ( b > FLT_EPSILON ) ? ( max.y - orig.y ) * b : ( min.y - orig.y ) * b;

  float c = 1.0f / dir.z;
  tmin.z = ( c > FLT_EPSILON ) ? ( min.z - orig.z ) * c : ( max.z - orig.z ) * c;
  tmax.z = ( c > FLT_EPSILON ) ? ( max.z - orig.z ) * c : ( min.z - orig.z ) * c;

  float t0,t1;
  if ( tmin.x > tmin.y ) {
    t0 = tmin.x;
  } else {
    t0 = tmin.y;
  }
  if ( tmin.z > t0 ) {
    t0 = tmin.z;
  }

  if ( tmax.x < tmax.y ) {
    t1 = tmax.x;
  } else {
    t1 = tmax.y;
  }
  if ( tmax.z < t1 ) {
    t1 = tmax.z;
  }

  return ( ( t1 > FLT_EPSILON ) && ( t0 < t1 ) );
}

Intersection Plane::intersect ( Ray &ray ) {
  Intersection inter;
  inter.dist = -1;
  if ( !intersectAABB ( ray ) ) {
    return inter;
  }
  glm::vec3 rayDirection = ray.getDirection();
  glm::vec3 rayOrigin = ray.getOrigin();
  float t = - glm::dot ( rayOrigin,norm ) / glm::dot ( rayDirection , norm );
  if ( t > FLT_EPSILON ) {
    inter.dist= t;
    inter.point = rayOrigin + t * rayDirection;
    inter.normal = norm;
    inter.distances.push_back ( t );
  }
  return inter;
}

//Muller-Trumbore
Intersection Triangle::intersect ( Ray &ray ) {
  Intersection inter;
  inter.dist = -1;
  if ( !intersectAABB ( ray ) ) {
    return inter;
  }
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
    inter.dist= t;
    inter.point = rayOrigin + t * rayDirection;
    inter.normal = glm::normalize ( glm::cross ( edge2,edge1 ) );
    inter.distances.push_back ( t );
  }
  return inter;
}



Intersection Sphere::intersect ( Ray &ray ) {
  Intersection inter;
  inter.dist = -1;
  if ( !intersectAABB ( ray ) ) {
    return inter;
  }
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
    inter.dist = x1;
    inter.point = rayOrigin + x1 * rayDirection;
    inter.normal = glm::normalize ( ( rayOrigin + x1 * rayDirection ) / radius );
    inter.distances.push_back ( x1 );
  }
  if ( x2 > FLT_EPSILON ) {
    if ( inter.dist == -1 ) {
      inter.dist = x2;
      inter.point = rayOrigin + x2 * rayDirection;
      inter.normal = glm::normalize ( ( rayOrigin + x2 * rayDirection ) / radius );
    }
    inter.distances.push_back ( x2 );
  }
  return inter;
}



Intersection Torus::intersect ( Ray &ray ) {
  Intersection inter;
  inter.dist = -1;
  if ( !intersectAABB ( ray ) ) {
    return inter;
  }
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

  for ( int i = 0; i < r_num; i++ ) {
    float k = ( float ) roots[i];
    if ( k > FLT_EPSILON ) {
      if ( k < inter.dist || inter.dist == -1 ) {
        inter.dist = ( float ) roots[i];
        inter.point = rayOrigin + inter.dist * rayDirection;
        float a = radius / sqrt ( inter.point.x * inter.point.x + inter.point.y * inter.point.y );
        inter.normal.x = ( 1 - a ) * inter.point.x;
        inter.normal.y = ( 1 - a ) * inter.point.y;
        inter.normal.z = inter.point.z;
        inter.normal = glm::normalize ( inter.normal );
      }
      inter.distances.push_back ( ( float ) roots[i] );
    }
  }
  std::sort ( std::begin ( inter.distances ), std::end ( inter.distances ) );
  return inter;
}

Intersection Cylinder::intersect ( Ray &ray ) {
  Intersection inter;
  inter.dist = -1;
  float fmin = FLT_MAX;
  if ( !intersectAABB ( ray ) ) {
    return inter;
  }
  glm::vec3 rayOrigin = ray.getOrigin();
  glm::vec3 rayDirection = ray.getDirection();


  float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y;
  float b = rayDirection.x * rayOrigin.x + rayDirection.y * rayOrigin.y;
  float c = rayOrigin.x * rayOrigin.x + rayOrigin.y * rayOrigin.y - radius * radius;

  glm::vec3 center ( 0.0f, 0.0f, height / 2 );
  glm::vec3 normal ( 0.0f, 0.0f, 1.0f );

  float t1 = glm::dot ( center - rayOrigin, normal ) / glm::dot ( rayDirection, normal );
  if ( ( t1 > FLT_EPSILON ) && ( glm::distance ( center, rayOrigin + t1 * rayDirection ) <= radius ) ) {
    inter.dist = t1;
    inter.point = rayOrigin + t1 * rayDirection;
    inter.normal = normal;
    inter.distances.push_back ( t1 );
    fmin = t1;
  }

  center =  glm::vec3 ( 0.0f, 0.0f, - height / 2 );
  normal =  glm::vec3 ( 0.0f, 0.0f, -1.0f );


  float t2 = glm::dot ( center - rayOrigin, normal ) / glm::dot ( rayDirection, normal );
  if ( t1 < t2 ) {
    inter.distances.push_back ( t2 );
  } else if ( ( t2 > FLT_EPSILON ) && ( glm::distance ( center, rayOrigin + t2 * rayDirection ) <= radius ) ) {
    inter.dist = t2;
    inter.point = rayOrigin + t2 * rayDirection;
    inter.normal = normal;
    inter.distances.push_back ( t2 );
    fmin = t2;
  }


  float rad = b * b - a * c;
  if ( rad < FLT_EPSILON ) {
    return inter;
  }

  float x1 = ( -b - sqrtf ( rad ) ) / a;
  float x2 = ( -b + sqrtf ( rad ) ) / a;

  if ( ( x1 > FLT_EPSILON ) && ( ( rayOrigin + x1 * rayDirection ).z > FLT_EPSILON - height / 2 ) && ( ( rayOrigin + x1 * rayDirection ).z < FLT_EPSILON + height / 2 ) ) {
    if ( x1 < fmin ) {
      fmin = x1;
      inter.dist = x1;
      inter.point = rayOrigin + x1 * rayDirection;
      inter.normal.x = inter.point.x / radius;
      inter.normal.y = inter.point.y / radius;
      inter.normal.z = 0.0f;
    }
    inter.distances.push_back ( x1 );
  }
  if ( ( x2 > FLT_EPSILON ) && ( ( rayOrigin + x2 * rayDirection ).z > FLT_EPSILON - height / 2 ) && ( ( rayOrigin + x2 * rayDirection ).z < FLT_EPSILON + height / 2 ) ) {
    if ( x2 < fmin ) {
      inter.dist = x2;
      inter.point = rayOrigin + x2 * rayDirection;
      inter.normal.x = inter.point.x / radius;
      inter.normal.y = inter.point.y / radius;
      inter.normal.z = 0.0f;
    }
    inter.distances.push_back ( x2 );
  }

  std::sort ( std::begin ( inter.distances ), std::end ( inter.distances ) );
  return inter;
}

Intersection Cone::intersect ( Ray &ray ) {
  Intersection inter;
  inter.dist = -1;
  float fmin = FLT_MAX;
  if ( !intersectAABB ( ray ) ) {
    return inter;
  }
  glm::vec3 rayOrigin = ray.getOrigin();
  glm::vec3 rayDirection = ray.getDirection();


  float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y - rayDirection.z * rayDirection.z * tg * tg;
  float b = rayDirection.x * rayOrigin.x + rayDirection.y * rayOrigin.y - rayDirection.z * rayOrigin.z * tg * tg;
  float c = rayOrigin.x * rayOrigin.x + rayOrigin.y * rayOrigin.y - rayOrigin.z * rayOrigin.z * tg * tg;

  glm::vec3 center ( 0.0f, 0.0f, zmax );
  glm::vec3 normal ( 0.0f, 0.0f, 1.0f );

  float t1 = glm::dot ( center - rayOrigin, normal ) / glm::dot ( rayDirection, normal );
  if ( ( t1 > FLT_EPSILON ) && ( glm::distance ( center, rayOrigin + t1 * rayDirection ) <= big_radius ) ) {
    inter.dist = t1;
    inter.point = rayOrigin + t1 * rayDirection;
    inter.normal = normal;
    inter.distances.push_back ( t1 );
    fmin = t1;
  }

  center =  glm::vec3 ( 0.0f, 0.0f, zmin );
  normal =  glm::vec3 ( 0.0f, 0.0f, -1.0f );

  float t2 = glm::dot ( center - rayOrigin, normal ) / glm::dot ( rayDirection, normal );
  if ( t1 < t2 ) {
    inter.distances.push_back ( t2 );
  } else if ( ( t2 > FLT_EPSILON ) && ( glm::distance ( center, rayOrigin + t2 * rayDirection ) <= small_radius ) ) {
    inter.dist= t2;
    inter.point = rayOrigin + t2 * rayDirection;
    inter.normal = normal;
    inter.distances.push_back ( t2 );
    fmin = t2;
  }


  float rad = b * b - a * c;
  if ( rad < FLT_EPSILON ) {
    return inter;
  }

  float x1 = ( -b - sqrtf ( rad ) ) / a;
  float x2 = ( -b + sqrtf ( rad ) ) / a;

  if ( ( x1 > FLT_EPSILON ) && ( ( rayOrigin + x1 * rayDirection ).z > zmin ) && ( ( rayOrigin + x1 * rayDirection ).z < zmax ) ) {
    if ( x1 < fmin ) {
      fmin = x1;
      inter.dist = x1;
      inter.point = rayOrigin + x1 * rayDirection;
      inter.normal.x = inter.point.x / ( inter.point.z * tg );
      inter.normal.y = inter.point.y / ( inter.point.z * tg );
      inter.normal.z = 0.0f;
    }
    inter.distances.push_back ( x1 );
  }
  if ( ( x2 > FLT_EPSILON ) && ( ( rayOrigin + x2 * rayDirection ).z > zmin ) && ( ( rayOrigin + x2 * rayDirection ).z < zmax ) ) {
    if ( x2 < fmin ) {
      fmin = x2;
      inter.dist = x2;
      inter.point = rayOrigin + x2 * rayDirection;
      inter.normal.x = inter.point.x / ( inter.point.z * tg );
      inter.normal.y = inter.point.y / ( inter.point.z * tg );
      inter.normal.z = 0.0f;
    }
    inter.distances.push_back ( x2 );
  }
  std::sort ( std::begin ( inter.distances ), std::end ( inter.distances ) );
  return inter;
}

Intersection ObjModel::intersect ( Ray  &ray ) {
  Intersection tmp, res;
  float min_dist = FLT_MAX;
  res.dist = -1;
  if ( !intersectAABB ( ray ) ) {
    return res;
  }
  for ( size_t i = 0; i < triangles.size(); ++i ) {
    tmp = triangles[i].intersect ( ray );
    if ( ( tmp.dist != -1 ) && ( tmp.dist < min_dist ) ) {
      min_dist   = tmp.dist;
      res.dist   = tmp.dist;
      res.point  = tmp.point;
      res.normal = tmp.normal;
    }
    res.distances.insert ( res.distances.end(), tmp.distances.begin(), tmp.distances.end () );
  }
  std::sort ( std::begin ( res.distances ), std::end ( res.distances ) );
  return res;
}




ObjModel::ObjModel ( std::string filename ) :name ( filename ) {
  min = glm::vec3 ( FLT_MAX, FLT_MAX, FLT_MAX );
  max = glm::vec3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::cout << "Loading obj file : " << name << std::endl;
  std::string err = tinyobj::LoadObj ( shapes, materials, name.c_str() );

  if ( !err.empty() ) {
    std::cout << err << std::endl;
    return;
  }

  for ( size_t i = 0; i < shapes.size(); ++i ) {
    for ( size_t j = 0; j < shapes[i].mesh.indices.size() / 3; ++j ) {
      int id0 = shapes[i].mesh.indices[3 * j];
      glm::vec3 p0 ( shapes[i].mesh.positions[ 3 * id0 + 0], shapes[i].mesh.positions[ 3 * id0 + 1], shapes[i].mesh.positions[ 3 * id0 + 2] );
      detmin(min,p0);
      detmax(max,p0);
      int id1 = shapes[i].mesh.indices[3 * j + 1];
      glm::vec3 p1 ( shapes[i].mesh.positions[ 3 * id1 + 0], shapes[i].mesh.positions[ 3 * id1 + 1], shapes[i].mesh.positions[ 3 * id1 + 2] );
      detmin(min,p1);
      detmax(max,p1);
      int id2 = shapes[i].mesh.indices[3 * j + 2];
      glm::vec3 p2 ( shapes[i].mesh.positions[ 3 * id2 + 0], shapes[i].mesh.positions[ 3 * id2 + 1], shapes[i].mesh.positions[ 3 * id2 + 2] );
      detmin(min,p2);
      detmax(max,p2);
      triangles.push_back ( Triangle ( p0, p1, p2 ) );

    }
  }
}


