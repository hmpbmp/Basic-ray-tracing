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
  bool isCSG;
  std::string csg_op;
};

class RayTracer {
public:
  RayTracer ( int w, int h );
  void traceRays();
  void saveToFile ( std::string &s );
  void setCameraFromYAML ( YAMLObject *camera );
  void setHierarchyFromYAML ( YAMLObject *head );
private:
  void traceRay ( Ray &ray );
  Intersection traceRayForNode ( Ray &ray, Node &node );
  void setHierarchyFromYAML ( YAMLObject *head , Node &node );
  void setCSGOpFromYAML ( YAMLObject *head , Node &node , std::string &name );
  Intersection applyCSG ( Node &node, Ray &ray, std::string &oper );
  //std::vector<Intersection> in;
  //float max_dist,min_dist;
  int x ,y;
  Camera camera;
  Image distImage, normImage;
  int width, height;
  Node head;
  Ray startRay;
  glm::mat4x4 prevTrans;
};

#endif