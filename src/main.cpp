#include "raytracer.h"
#include "argumentsparser.h"
#include "yamlparser.h"

int main ( int argc, char** argv ) {
  ArgumentsParser ap ( argc,argv );
  YAMLParser yp ( ap.getScene() );
  RayTracer rt ( ap.getXRes(), ap.getYRes() );
  rt.setCameraFromYAML ( yp.getCameraObject() );
  rt.setHierarchyFromYAML ( yp.getHeadObject() );
  rt.traceRays();
  rt.saveToFile ( ap.getOutput() );
  return 0;
}