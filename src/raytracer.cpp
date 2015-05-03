#include "raytracer.h"
#include <algorithm>
#include <iostream>
#include <omp.h>

#define PI_F 3.14159265359f

RayTracer::RayTracer ( int w, int h ) :camera ( Camera() ),
  width ( w ),
  height ( h ),
  distImage ( Image ( w, h ) ),
  normImage ( Image ( w, h ) ) {
}


void RayTracer::traceRays() {
  startRay.setOrigin ( camera.getPosition() );
  glm::vec3 dir = glm::normalize ( camera.getLookAt() - camera.getPosition() );
  glm::vec3 right = glm::normalize ( glm::cross ( dir, camera.getUp() ) );
  glm::vec3 up = camera.getUp();
  float c_w = 2 * tanf ( PI_F * camera.getFOVX() / 360 );
  float c_h = 2 * tanf ( PI_F * camera.getFOVY() / 360 );
  for ( x = 0; x < width; ++x ) {
    if ( ( x % ( width / 10 ) == 0 ) && ( x != 0 ) ) {
      std::cout << "Progress: " << ( int ) ( ( x / ( float ) width ) * 100 + 0.5f ) << " %" << std::endl;
    }
    glm::vec3 d_x = c_w / width * ( x - 0.5f * ( width - 1.0f ) ) * right;
    for ( y = 0; y < height; ++y ) {
      glm::vec3 d_y = c_h / height * ( 0.5f * ( height - 1.0f ) - y ) * up;
      startRay.setDirection ( glm::normalize ( dir + d_x + d_y ) );
      traceRay ( startRay );
    }
  }
}

void RayTracer::traceRay ( Ray &ray ) {
  auto v = traceRayForNode ( ray,head );
  if ( v.dist != -1 ) {
    distImage.setFloat ( x, y, v.dist );
    normImage.setPixelColor ( x, y , Color (
                                ( unsigned char ) ( 127.5f * ( v.normal.z + 1.0f ) ),
                                ( unsigned char ) ( 127.5f * ( v.normal.z + 1.0f ) ),
                                ( unsigned char ) ( 127.5f * ( v.normal.z + 1.0f ) ) ) );
  } else {
    distImage.setFloat ( x, y, 0.0f );
    normImage.setPixelColor ( x, y , Color ( 0,0,0 ) );
  }
}


Intersection RayTracer::traceRayForNode ( Ray &ray , Node &node ) {
  std::vector<Intersection> interVec;
  Intersection result;
  result.dist = -1;
  glm::vec4 tr_ray_orig = node.transform * glm::vec4 ( ray.getOrigin(), 1.0f );
  glm::vec4 tr_ray_dir  = node.transform * glm::vec4 ( ray.getDirection(), 0.0f );
  Ray trRay;
  trRay.setOrigin ( glm::vec3 ( tr_ray_orig.x, tr_ray_orig.y, tr_ray_orig.z ) );
  trRay.setDirection ( glm::vec3 ( tr_ray_dir.x, tr_ray_dir.y, tr_ray_dir.z ) );


  if ( !node.isCSG ) {
    for ( auto ch : node.children ) {
      auto v = traceRayForNode ( trRay, ch );
      if ( v.dist != -1 ) {
        interVec.push_back ( v );
      }
    }
    auto v = node.obj->intersect ( trRay );
    if ( v.dist  != -1 ) {
      interVec.push_back ( v );
    }
  } else {
    auto v = applyCSG ( node, trRay , node.csg_op );
    if ( v.dist  != -1 ) {
      interVec.push_back ( v );
    }
  }
  if ( !interVec.empty() ) {
    glm::mat4x4 inv_tr = glm::inverse ( node.transform );
    for ( size_t i = 0; i < interVec.size(); ++i )  {
      glm::vec4 new_point = inv_tr * glm::vec4 ( interVec[i].point, 1.0f );
      interVec[i].point = glm::vec3 ( new_point.x, new_point.y, new_point.z );
      glm::vec4 new_normal = glm::transpose ( inv_tr ) * glm::vec4 ( interVec[i].normal, 0.0f );
      interVec[i].normal = glm::vec3 ( new_normal.x, new_normal.y, new_normal.z );
    }
  }
  if ( interVec.size() == 0 ) {
    return result;
  }
  result = *std::min_element ( interVec.begin(), interVec.end(), DistanceComparator() );
  return result;
}

void RayTracer::saveToFile ( std::string &filename ) {
  distImage.normalize ();
  distImage.saveImage ( ( "distance-" + filename ).c_str() );
  normImage.saveImage ( ( "normal-" + filename ).c_str() );
}


Intersection RayTracer::applyCSG ( Node &node, Ray &ray, std::string &csg_op ) {
  auto left = traceRayForNode ( ray,node.children[0] );
  auto right = traceRayForNode ( ray,node.children[1] );
  Intersection result;
  result.dist = -1;

  if ( csg_op == "csg_intersection" ) {
    if ( left.dist == -1 || right.dist == -1 ) {
      return result;
    }

    auto left_min = left.distances.front();
    auto right_min = right.distances.front();

    auto left_max = left.distances.back();
    auto right_max = right.distances.back();

    if ( left_min < right_min  && right_min < left_max )  {
      result.dist = right_min;
      result.point = right.point;
      result.normal = right.normal;
      result.distances.push_back ( right_min );
      result.distances.push_back ( std::min ( left_max, right_max ) );
    } else if ( left_min > right_min  && right_max > left_min )  {
      result.dist = left_min;
      result.point = left.point;
      result.normal = left.normal;
      result.distances.push_back ( left_min );
      result.distances.push_back ( std::min ( left_max, right_max ) );
    } else {
      return result;
    }
  }
  if ( csg_op == "csg_union" ) {
    if ( left.dist == -1 && right.dist == -1 ) {
      return result;
    }
    if ( left.dist == -1 ) {
      return right;
    }
    if ( right.dist == -1 ) {
      return left;
    }
    if ( left.dist < right.dist ) {
      result.dist = left.dist;
      result.point = left.point;
      result.normal = left.normal;
    } else {
      result.dist = right.dist;
      result.point = right.point;
      result.normal = right.normal;
    }
    if ( left.dist != -1 ) {
      result.distances.insert ( result.distances.begin(), left.distances.begin(), left.distances.end() );
    }
    if ( right.dist != -1 ) {
      result.distances.insert ( result.distances.begin(), right.distances.begin(), right.distances.end() );
    }
    std::sort ( std::begin ( result.distances ), std::end ( result.distances ) );
    return result;
  }
  if ( csg_op == "csg_difference" ) {
    if ( left.dist == -1 ) {
      return result;
    }
    if ( right.dist == -1 ) {
      return left;
    }
    auto left_min = left.distances.front();
    auto right_min = right.distances.front();

    auto left_max = left.distances.back();
    auto right_max = right.distances.back();

    if ( right_max < left_min ) {
      return left;
    }
    if ( left_max < right_min ) {
      return left;
    }
    if ( left_min < right_min ) {
      result.dist = left_min;
      result.point = left.point;
      result.normal = left.normal;
    } else if ( right_max < left_max ) {
      result.dist = right_max;
      result.point = right.point;
      result.normal = right.normal;
    } else {
      return result;
    }
    return result;
    if ( left.dist != -1 ) {
      result.distances.insert ( result.distances.begin(), left.distances.begin(), left.distances.end() );
    }
    if ( right.dist != -1 ) {
      result.distances.insert ( result.distances.begin(), right.distances.begin(), right.distances.end() );
    }
    std::sort ( std::begin ( result.distances ), std::end ( result.distances ) );
    return result;
  }
  return result;
}


void RayTracer::setCameraFromYAML ( YAMLObject *yamlCamera ) {
  YAMLObject *pos = yamlCamera->map["position"];
  camera.setPosition ( stof ( pos->map["x"]->value ),stof ( pos->map["y"]->value ),stof ( pos->map["z"]->value ) );
  YAMLObject *or = yamlCamera->map["orientation"];
  camera.setOrientation ( stof ( or->map["h"]->value ),stof ( or->map["p"]->value ),stof ( or->map["r"]->value ) );
  camera.setFOV ( stof ( yamlCamera->map["fov_x"]->value ), stof ( yamlCamera->map["fov_y"]->value ) );
}

void RayTracer::setHierarchyFromYAML ( YAMLObject *yamlHead ) {
  prevTrans = glm::mat4x4 ( 1.0f );
  setHierarchyFromYAML ( yamlHead,head );
}

void RayTracer::setHierarchyFromYAML ( YAMLObject *yamlHead , Node &node ) {
  YAMLObject *lcs = yamlHead->array[0]->map["lcs"];
  YAMLObject *geom = yamlHead->array[1];
  node.isCSG = false;
  if ( yamlHead ) {
    if ( lcs ) {
      glm::mat4x4 trans1 ( 1.0f ), trans2 ( 1.0f ) ,trans3 ( 1.0f );
      glm::vec3 x ( 1.0,0.0,0.0 );
      glm::vec3 y ( 0.0,1.0,0.0 );
      glm::vec3 z ( 0.0,0.0,1.0 );
      glm::mat4x4 transS ( 1.0f );

      glm::vec4 tx = prevTrans * glm::vec4 ( x, 0.0f );
      glm::vec4 ty = prevTrans * glm::vec4 ( y, 0.0f );
      glm::vec4 tz = prevTrans * glm::vec4 ( z, 0.0f );

      x = glm::normalize ( glm::vec3 ( tx.x,tx.y,tx.z ) );
      y = glm::normalize ( glm::vec3 ( ty.x,ty.y,ty.z ) );
      z = glm::normalize ( glm::vec3 ( tz.x,tz.y,tz.z ) );

      transS = glm::scale ( transS, glm::vec3 ( stof ( lcs->map["sx"]->value ), stof ( lcs->map["sy"]->value ), stof ( lcs->map["sz"]->value ) ) );

      trans1 = glm::rotate ( trans1, stof ( lcs->map["r"]->value ) / 180 * PI_F, y ) ;
      glm::vec4 tr_vec = trans1 * glm::vec4 ( x, 0.0f );
      x = glm::normalize ( glm::vec3 ( tr_vec.x,tr_vec.y,tr_vec.z ) );
      z = glm::normalize ( glm::cross ( x,y ) );

      trans2 = glm::rotate ( trans2, stof ( lcs->map["p"]->value ) / 180 * PI_F, x ) ;
      tr_vec = trans2 * glm::vec4 ( y, 0.0f );
      y = glm::normalize ( glm::vec3 ( tr_vec.x,tr_vec.y,tr_vec.z ) );
      z = glm::normalize ( glm::cross ( x,y ) );

      trans3 = glm::rotate ( trans3, stof ( lcs->map["h"]->value ) / 180 * PI_F, z ) ;
      tr_vec = trans2 * glm::vec4 ( x, 0.0f );
      x = glm::normalize ( glm::vec3 ( tr_vec.x,tr_vec.y,tr_vec.z ) );
      y = glm::normalize ( glm::cross ( z,x ) );

      glm::mat4x4 transT ( 1.0f );
      transT = glm::translate ( transT, glm::vec3 ( stof ( lcs->map["x"]->value ), stof ( lcs->map["y"]->value ), stof ( lcs->map["z"]->value ) ) );
      node.transform =  transT * trans3 * trans2 * trans1 * transS ;
      prevTrans = node.transform * prevTrans;
    }
    if ( geom ) {
      if ( geom->map.count ( "plane" ) ) {
        geom = geom->map["plane"];
        glm::vec3 normal ( stof ( geom->map["normal"]->map["x"]->value ), stof ( geom->map["normal"]->map["y"]->value ), stof ( geom->map["normal"]->map["z"]->value ) );
        node.obj = new Plane ( normal );
      }
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
      if ( geom->map.count ( "cone" ) ) {
        geom = geom->map["cone"];
        node.obj = new Cone ( stof ( geom->map["big_radius"]->value ),stof ( geom->map["small_radius"]->value ), stof ( geom->map["height"]->value ) );
      }
      if ( geom->map.count ( "obj_model" ) ) {
        geom = geom->map["obj_model"];
        node.obj = new ObjModel ( geom->map["file_name"]->value );
        ;
      }
    }
    for ( size_t i = 2; i < yamlHead->array.size(); i++ ) {
      Node child;
      if ( yamlHead->array[i]->map.find ( "node" ) != yamlHead->array[i]->map.end() ) {
        setHierarchyFromYAML ( yamlHead->array[i]->map["node"], child );
      }
      if ( yamlHead->array[i]->map.find ( "csg_union" ) != yamlHead->array[i]->map.end() ) {
        setCSGOpFromYAML ( yamlHead->array[i], child, std::string ( "csg_union" ) );
      }
      if ( yamlHead->array[i]->map.find ( "csg_intersection" ) != yamlHead->array[i]->map.end() ) {
        setCSGOpFromYAML ( yamlHead->array[i], child, std::string ( "csg_intersection" ) );
      }
      if ( yamlHead->array[i]->map.find ( "csg_difference" ) != yamlHead->array[i]->map.end() ) {
        setCSGOpFromYAML ( yamlHead->array[i], child, std::string ( "csg_difference" ) );
      }
      node.children.push_back ( child );
    }
    prevTrans = glm::inverse ( node.transform ) * prevTrans;
  }

}

void RayTracer::setCSGOpFromYAML ( YAMLObject *head , Node &node , std::string &name ) {
  Node left, right;
  YAMLObject *next = head->map[name];
  if ( next->array[0]->map["left_node"]->array.size() == 2 ) {
    setHierarchyFromYAML ( next->array[0]->map["left_node"], left );
  } else {
    setCSGOpFromYAML ( next->array[0]->map["left_node"]->array[0], left, std::string ( next->array[0]->map["left_node"]->array[0]->map.begin()->first ) );
  }

  if ( next->array[1]->map["right_node"]->array.size() == 2 ) {
    setHierarchyFromYAML ( next->array[1]->map["right_node"], right );
  } else {
    setCSGOpFromYAML ( next->array[1]->map["right_node"]->array[0], right, std::string ( next->array[1]->map["right_node"]->array[0]->map.begin()->first ) );
  }
  node.children.push_back ( left );
  node.children.push_back ( right );
  node.isCSG = true;
  node.csg_op = name;
}



