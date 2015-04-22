#include "raytracer.h"
#include "argumentsparser.h"
#include "yamlparser.h"
#include <iostream>

int main ( int argc, char** argv ) {
  ArgumentsParser ap ( argc,argv );
  std::cout << "Creating scene from file..." << std::endl;
  YAMLParser yp ( ap.getScene() );
  RayTracer rt ( ap.getXRes(), ap.getYRes() );
  rt.setCameraFromYAML ( yp.getCameraObject() );
  rt.setHierarchyFromYAML ( yp.getHeadObject() );
  std::cout << "Start raytracing..." << std::endl;
  rt.traceRays();
  std::cout << "Saving images..." << std::endl;
  rt.saveToFile ( ap.getOutput() );
  return 0;
}