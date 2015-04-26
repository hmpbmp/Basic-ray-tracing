#include "geometry.h"
#include <algorithm>
#include <poly34.h>
#include <tiny_obj_loader.h>
#include <iostream>


bool Object::vectorLess ( glm::vec3 &v1, glm::vec3 &v2 ) {
  return ( ( v1.x < v2.x ) && ( v1.y < v2.y ) && ( v1.z < v2.z ) );
}

bool Object::vectorGreater ( glm::vec3 &v1, glm::vec3 &v2 ) {
  return ( vectorLess ( v2,v1 ) && ( v1 != v2 ) );
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
    inter.normal = glm::normalize ( glm::cross ( edge1,edge2 ) );
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

//Intersection ObjModel::intersect ( Ray  &ray ) {
/*Intersection tmp, min;
min.dist = FLT_MAX;
min.param = -1;
if ( !intersectAABB ( ray ) ) {
  return min;
}
for ( size_t i = 0; i < triangles.size(); i++ ) {
  //tmp = triangles[i].intersect ( ray );
  if ( ( tmp.param != -1 ) && ( tmp.dist < min.dist ) ) {
    min = tmp;
  }
}
return min; */
//Intersection emp;
//return emp;
//}




/*ObjModel::ObjModel ( std::string filename ) :name ( filename ) {
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::cout << "Loading obj file : " << name << std::endl;
  std::string err = tinyobj::LoadObj ( shapes, materials, name.c_str() );

  if ( !err.empty() ) {
    std::cout << err << std::endl;
    return;
  }

  for ( size_t i = 0; i < shapes.size(); i++ ) {
    for ( size_t v = 0; v < shapes[i].mesh.positions.size() - 6; v += 3 ) {
      glm::vec3 p0 ( shapes[i].mesh.positions[ ( v + 0 ) + 0], shapes[i].mesh.positions[ ( v + 0 ) + 1], shapes[i].mesh.positions[ ( v + 0 ) + 2] );
      if ( vectorLess ( p0, min ) ) {
        min = p0;
      }
      if ( vectorGreater ( p0, max ) ) {
        max = p0;
      }
      glm::vec3 p1 ( shapes[i].mesh.positions[ ( v + 3 ) + 0], shapes[i].mesh.positions[ ( v + 3 ) + 1], shapes[i].mesh.positions[ ( v + 3 ) + 2] );
      if ( vectorLess ( p1, min ) ) {
        min = p1;
      }
      if ( vectorGreater ( p1, max ) ) {
        max = p1;
      }
      glm::vec3 p2 ( shapes[i].mesh.positions[ ( v + 6 ) + 0], shapes[i].mesh.positions[ ( v + 6 ) + 1], shapes[i].mesh.positions[ ( v + 6 ) + 2] );
      if ( vectorLess ( p2, min ) ) {
        min = p2;
      }
      if ( vectorGreater ( p2, max ) ) {
        max = p2;
      }
      triangles.push_back ( Triangle ( p0,p1,p2 ) );
    }
  }
}   */


