#include "raytracer.h"
#include <algorithm>

RayTracer::RayTracer ( int w, int h ) :camera ( Camera() ),
  width ( w ),
  height ( h ),
  distImage ( Image ( w, h ) ),
  normImage ( Image ( w, h ) ),
  sphere ( Sphere ( 100 ) ) {
}


void RayTracer::traceRays() {
  float u,v;
  Ray ray;
  ray.setDirection ( glm::vec3 ( 0.0f, -1.0f, 0.0f ) );
  camera.calculateViewingCoordinates();
  for ( int x = 0; x < width; x++ ) {
    for ( int y = 0; y < height; y++ ) {
      u = ( x - 0.5f * ( width - 1.0f ) );
      v = ( y - 0.5f * ( height - 1.0f ) );
      ray.setOrigin ( glm::vec3 ( u, 0.5f, v ) );
      std::pair<Color,Color> colors = traceRay ( ray );
      distImage.setPixelColor ( x, y , colors.first );
      normImage.setPixelColor ( x, y , colors.second );
    }
  }
}

void RayTracer::saveToFile ( std::string filename ) {
  distImage.saveImage ( ( "distance-" + filename ).c_str() );
  normImage.saveImage ( ( "normal-" + filename ).c_str() );
}

void RayTracer::traceRayForNode ( Ray ray , Node node ) {
  std::vector<Intersection> inter = node.obj->intersect ( ray );
  in.insert ( in.end(), inter.begin(), inter.end() );
}

std::pair<Color, Color> RayTracer::traceRay ( Ray ray ) {
  traceRayForNode ( ray,head );
  if ( in.size() > 0 ) {
    std::pair<Color, Color> pair;
    Intersection min = *std::min_element ( in.begin(), in.end(), DistanceComparator() );
    pair.first = Color (
                   ( unsigned char ) min.dist,
                   ( unsigned char ) min.dist ,
                   ( unsigned char ) min.dist );
    min = *std::min_element ( in.begin(), in.end(), NormalComparator() );
    pair.second = Color (
                    ( unsigned char ) ( 255 * min.normal.y ),
                    ( unsigned char ) ( 255 * min.normal.y ),
                    ( unsigned char ) ( 255 * min.normal.y ) );
    in.clear();
    return pair;
  } else {
    return std::pair<Color, Color> ( Color ( 0, 0, 0 ), Color ( 0, 0, 0 ) );
  }
}


void RayTracer::setCameraFromYAML ( YAMLObject *yamlCamera ) {
  YAMLObject *pos = yamlCamera->map["position"];
  camera.setPosition ( stof ( pos->map["x"]->value ),stof ( pos->map["y"]->value ),stof ( pos->map["z"]->value ) );
  YAMLObject *or = yamlCamera->map["orientation"];
  camera.setOrientation ( stof ( or->map["h"]->value ),stof ( or->map["p"]->value ),stof ( or->map["r"]->value ) );
  camera.setFOV ( stof ( yamlCamera->map["fov_x"]->value ), stof ( yamlCamera->map["fov_y"]->value ) );
}

void RayTracer::setHierarchyFromYAML ( YAMLObject *yamlHead ) {
  setHierarchyFromYAML ( yamlHead,head );
}

void RayTracer::setHierarchyFromYAML ( YAMLObject *yamlHead , Node &node ) {
  YAMLObject *lcs = yamlHead->array[0]->map["lcs"];
  YAMLObject *geom = yamlHead->array[1];
  if ( yamlHead ) {
    if ( lcs ) {
      glm::mat4x4 trans ( 1.0f );
      trans = glm::scale ( trans, glm::vec3 ( stof ( lcs->map["sx"]->value ), stof ( lcs->map["sy"]->value ), stof ( lcs->map["sz"]->value ) ) );
      trans = glm::rotate ( trans, stof ( lcs->map["r"]->value ), glm::vec3 ( 0.0,1.0,0.0 ) ) ;
      trans = glm::rotate ( trans, stof ( lcs->map["p"]->value ), glm::vec3 ( 1.0,0.0,0.0 ) ) ;
      trans = glm::rotate ( trans, stof ( lcs->map["h"]->value ), glm::vec3 ( 0.0,0.0,1.0 ) ) ;
      trans = glm::translate ( trans, glm::vec3 ( stof ( lcs->map["x"]->value ), stof ( lcs->map["y"]->value ), stof ( lcs->map["z"]->value ) ) );
      node.transform = trans;
    }
    if ( geom ) {
      if ( geom->map.count ( "triangle" ) ) {
        geom = geom->map["triangle"];
        glm::vec3 p0 ( stof ( geom->map["p0"]->map["x"]->value ), stof ( geom->map["p0"]->map["y"]->value ), stof ( geom->map["p0"]->map["z"]->value ) );
        glm::vec3 p1 ( stof ( geom->map["p1"]->map["x"]->value ), stof ( geom->map["p1"]->map["y"]->value ), stof ( geom->map["p1"]->map["z"]->value ) );
        glm::vec3 p2 ( stof ( geom->map["p2"]->map["x"]->value ), stof ( geom->map["p2"]->map["y"]->value ), stof ( geom->map["p2"]->map["z"]->value ) );
        node.obj = new Triangle ( p0,p1,p2 );
      }
      if ( geom->map.count ( "sphere" ) ) {
        geom = geom->map["sphere"];
        node.obj = new Sphere ( stof ( geom->map["radius"]->value ) );
      }
      /*if ( geom->map.count ( "cylinder" ) ) {
        geom = geom->map["cylinder"];
        node.obj = new Cylinder ( stof ( geom->map["radius"]->value ), stof ( geom->map["height"]->value ) );
      }
      if ( geom->map.count ( "torus" ) ) {
        geom = geom->map["torus"];
        node.obj = new Torus ( stof ( geom->map["radius"]->value ), stof ( geom->map["tube_radius"]->value ) );
      }*/
    }
    for ( size_t i = 2; i < yamlHead->array.size(); i++ ) {
      Node child;
      setHierarchyFromYAML ( yamlHead->array[i]->map["node"], child );
      node.children.push_back ( child );
    }
  }

}

