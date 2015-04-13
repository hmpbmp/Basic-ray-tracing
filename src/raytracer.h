#ifndef RAYTRACER_H_INCLUDED
#define RAYTRACER_H_INCLUDED
#include "image.h"
#include "camera.h"
#include "geometry.h"

class RayTracer {
public:
  RayTracer();
  void traceRays();
private:
  Color traceRay ( Ray ray );

  Camera camera;
  Image image;
  int width, height;
  //Node node;
  Sphere sphere;
};

#endif