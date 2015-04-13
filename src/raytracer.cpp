#include "raytracer.h"



RayTracer::RayTracer() :camera ( Camera() ),
  width ( 1024 ),
  height ( 768 ),
  image ( Image ( 1024,768 ) ),
  sphere ( Sphere ( 75 ) ) {
}


void RayTracer::traceRays() {
  float u,v;
  Ray ray;
  ray.setDirection ( glm::vec3 ( 0.0f, -1.0f, 0.0f ) );
  camera.calculateViewingCoordinates();
  for ( int x = 0; x < width; x++ ) {
    for ( int y = 0; y < height; y++ ) {
      u = ( x - 0.5f * ( 1024 - 1.0f ) );
      v = ( y - 0.5f * ( 768 - 1.0f ) );
      ray.setOrigin ( glm::vec3 ( u, 3.0f, v ) );
      image.setPixelColor ( x,y,traceRay ( ray ) );
    }
  }
  image.saveImage ( "img.bmp" );
}

Color RayTracer::traceRay ( Ray ray ) {
  std::vector<Intersection> in = sphere.intersect ( ray );
  if ( in.size() > 0 ) {
    return Color ( 255, 0, 0 );
  } else {
    return Color ( 0, 0, 0 );
  }
}