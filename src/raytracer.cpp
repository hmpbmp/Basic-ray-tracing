#include "raytracer.h"
#include <algorithm>
#include <iostream>

#define PI_F 3.14159265359f

RayTracer::RayTracer ( int w, int h ) :camera ( Camera() ),
  width ( w ),
  height ( h ),
  distImage ( Image ( w, h ) ),
  normImage ( Image ( w, h ) ) {
}


void RayTracer::traceRays() {
  Ray ray;
  ray.setOrigin ( camera.getPosition() );
  max_dist = 0;
  min_dist = FLT_MAX;
  glm::vec3 dir = glm::normalize ( camera.getLookAt() - camera.getPosition() );
  glm::vec3 right = glm::normalize ( glm::cross ( dir, camera.getUp() ) );
  glm::vec3 up = camera.getUp();
  float c_w = 2 * tanf ( PI_F * camera.getFOVX() / 360 );
  float c_h = 2 * tanf ( PI_F * camera.getFOVY() / 360 );

  for ( int x = 0; x < width; x++ ) {
    if ((x % (width / 10) == 0) && (x != 0)) {
      std::cout << "Progress:" << (int)((x / (float)width) * 100) << "%" << std::endl;
    } 
    for ( int y = 0; y < height; y++ ) {
      glm::vec3 d_x = c_w / width * ( x - 0.5f * ( width - 1.0f ) ) * right;
      glm::vec3 d_y = c_h / height * ( 0.5f * ( height - 1.0f ) - y ) * up;
      ray.setDirection ( glm::normalize ( dir + d_x + d_y ) );
      std::pair<float,Color> colors = traceRay ( ray );
      distImage.setFloat ( x, y , colors.first );
      normImage.setPixelColor ( x, y , colors.second );
    }
  }
}

void RayTracer::saveToFile ( std::string &filename ) {
  distImage.normalize ( min_dist,  max_dist );
  distImage.saveImage ( ( "distance-" + filename ).c_str() );
  normImage.saveImage ( ( "normal-" + filename ).c_str() );
}

void RayTracer::traceRayForNode ( Ray &ray , Node &node ) {
  glm::vec4 tr_ray_orig = node.transform * glm::vec4 ( ray.getOrigin(), 1.0f );
  glm::vec4 tr_ray_dir  = node.transform * glm::vec4 ( ray.getDirection(), 0.0f );
  Ray trRay;
  trRay.setOrigin ( glm::vec3 ( tr_ray_orig.x, tr_ray_orig.y, tr_ray_orig.z ) );
  trRay.setDirection ( glm::vec3 ( tr_ray_dir.x, tr_ray_dir.y, tr_ray_dir.z ) );
  for ( auto ch : node.children ) {
    traceRayForNode ( trRay, ch);
  }
  Intersection inter = node.obj->intersect ( trRay );
  if ( inter.param != -1 ) {
    glm::mat4x4 inv_tr = glm::inverse ( node.transform );
    glm::vec4 new_point = inv_tr * glm::vec4 ( inter.point, 1.0f );
    inter.point = glm::vec3 ( new_point.x, new_point.y, new_point.z );
    glm::vec4 new_normal = glm::transpose ( inv_tr ) * glm::vec4 ( inter.normal, 0.0f );
    inter.normal = glm::vec3 ( new_normal.x, new_normal.y, new_normal.z );
    in.push_back ( inter );
  }

}

std::pair<float, Color> RayTracer::traceRay ( Ray &ray ) {
  traceRayForNode ( ray,head);
  if ( in.size() > 0 ) {
    std::pair<float, Color> pair;
    Intersection min = *std::min_element ( in.begin(), in.end(), DistanceComparator() );
    if ( min.dist > max_dist ) {
      max_dist = min.dist;
    }
    if ( min.dist < min_dist ) {
      min_dist = min.dist;
    }
    pair.first = min.dist;
    min = *std::min_element ( in.begin(), in.end(), NormalComparator() );
    pair.second = Color (
                    ( unsigned char ) ( 255 * ( min.normal.z ) ),
                    ( unsigned char ) ( 255 * ( min.normal.z ) ),
                    ( unsigned char ) ( 255 * ( min.normal.z ) ) );
    in.clear();
    return pair;
  } else {
    return std::pair<float, Color> ( 0.0f, Color ( 0, 0, 0 ) );
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
      glm::mat4x4 trans1 ( 1.0f ), trans2 ( 1.0f ) ,trans3 ( 1.0f );
      glm::vec3 x( 1.0,0.0,0.0 );
      glm::vec3 y( 0.0,1.0,0.0 );
      glm::vec3 z( 0.0,0.0,1.0 );
      glm::mat4x4 transS(1.0f);
      transS = glm::scale ( transS, glm::vec3 ( stof ( lcs->map["sx"]->value ), stof ( lcs->map["sy"]->value ), stof ( lcs->map["sz"]->value ) ) );

      trans1 = glm::rotate ( trans1, stof ( lcs->map["r"]->value ) / 180 * PI_F, y ) ;
      glm::vec4 tr_vec = trans1 * glm::vec4(x, 0.0f);
      x = glm::normalize(glm::vec3(tr_vec.x,tr_vec.y,tr_vec.z));
      z = glm::normalize(glm::cross(x,y));

      trans2 = glm::rotate ( trans2, stof ( lcs->map["p"]->value ) / 180 * PI_F, x ) ;
      tr_vec = trans2 * glm::vec4(y, 0.0f);
      y = glm::normalize(glm::vec3(tr_vec.x,tr_vec.y,tr_vec.z));
      z = glm::normalize(glm::cross(x,y));

      trans3 = glm::rotate ( trans3, stof ( lcs->map["h"]->value ) / 180 * PI_F, z ) ;
      tr_vec = trans2 * glm::vec4(x, 0.0f);
      x = glm::normalize(glm::vec3(tr_vec.x,tr_vec.y,tr_vec.z));
      y = glm::normalize(glm::cross(z,x));

      glm::mat4x4 transT(1.0f);
      transT = glm::translate ( transT, glm::vec3 ( stof ( lcs->map["x"]->value ), stof ( lcs->map["y"]->value ), stof ( lcs->map["z"]->value ) ) );
      node.transform =  transT * trans3 * trans2 * trans1 * transS ;
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
      if ( geom->map.count ( "cylinder" ) ) {
        geom = geom->map["cylinder"];
        node.obj = new Cylinder ( stof ( geom->map["radius"]->value ), stof ( geom->map["height"]->value ) );
      }
      if ( geom->map.count ( "torus" ) ) {
        geom = geom->map["torus"];
        node.obj = new Torus ( stof ( geom->map["radius"]->value ), stof ( geom->map["tube_radius"]->value ) );
      }
    }
    for ( size_t i = 2; i < yamlHead->array.size(); i++ ) {
      Node child;
      setHierarchyFromYAML ( yamlHead->array[i]->map["node"], child );
      node.children.push_back ( child );
    }
  }

}

