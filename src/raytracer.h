#ifndef RAYTRACER_H_INCLUDED
#define RAYTRACER_H_INCLUDED
#include "image.h"
#include "camera.h"
#include "geometry.h"
#include "yamlparser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <utility>

struct Node {
  glm::mat4 transform;
  Object *obj;
  std::vector<Node> children;
};

class RayTracer {
public:
  RayTracer ( int w, int h );
  void traceRays();
  void saveToFile ( std::string &s );
  void setCameraFromYAML ( YAMLObject *camera );
  void setHierarchyFromYAML ( YAMLObject *head );
private:
  std::pair<float,Color> traceRay ( Ray &ray );
  void traceRayForNode ( Ray &ray, Node &node );
  void setHierarchyFromYAML ( YAMLObject *head , Node &node );
  std::vector<Intersection> in;
  float max_dist,min_dist;
  Camera camera;
  Image distImage, normImage;
  int width, height;
  Node head;
};

#endif