#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <limits>
#include <math.h>
#include <iostream>

using namespace std;
#include "variables.h"
#include "raytrace.h"

void constructCord() {
  vec3 a = camera - center;
  w = glm::normalize(a);
  u = glm::normalize(glm::cross(up, w));
  // TODO remove normalize
  v = glm::normalize(glm::cross(w, u));
}

vec3 getRayDirection(int i, int j) {
  // make sure the ray is shoot through the center of pixel, so add 0.5 to i and j
  float i_center = i + 0.5, j_center = j + 0.5;
  float alpha = tan_halffovx * (j_center - 0.5 * width) / (0.5 * width);
  float beta = tan(glm::radians(0.5 * fovy)) * (0.5 * height - i_center) / (0.5 * height);
  vec3 raydir = glm::normalize(alpha * u + beta * v - w);
  
  return raydir;
}

HitInfo hitDetection(vec4 ray, vec4 raydir) {
  float mindist = std::numeric_limits<float>::max();
  int object_no;
  bool is_hit = false;
  vec4 intersection;
  vec3 normal;
  vec4 p0 = ray, p1 = raydir;
  // params saved for the hitted object
  vec4 min_p0_t, min_p1_t;
  // std::cout << mindist << std::endl;
  for(int i = 0; i < (int)objects.size(); i++) {
    Object obj = objects[i];
    mat4 transform = obj.transform;
    mat4 inverse_transform = glm::inverse(transform);
    // std::cout << obj.type << std::endl;
    // apply inverse transform to ray
    vec4 p0_t = inverse_transform * p0;
    vec4 p1_t = inverse_transform * p1;
    float dist = 0;
    if (obj.type == sphere) {
      // std::cout << glm::to_string(obj.vertices[0]) << std::endl;
      dist = hitSphere(obj, p0_t, p1_t);
    } else if (obj.type == tri) {
      dist = hitTriangle(obj, p0_t, p1_t);
      // dist = hitTriangle_moller_trumbore(obj, p0_t, p1_t);
    }
    else {
      std::cerr << "unknown shape" << std::endl;
      exit(-1);
    }
    if (dist > 0 && dist < mindist) {
      is_hit = true;
      mindist = dist;
      object_no = i;
      min_p0_t = p0_t;
      min_p1_t = p1_t;
    }
  }
  // after 
  if (is_hit) {
    // calculate intersection point and normal
    Object obj = objects[object_no];
    if(obj.type == sphere) {
      vec4 center = vec4(obj.vertices[0], 1);
      mat4 transform = obj.transform;
      vec4 intersection_t = min_p0_t + min_p1_t * mindist;
      vec4 normal_t = glm::normalize(intersection_t - center);
      intersection = transform * intersection_t;
      normal = glm::normalize(vec3(glm::transpose(glm::inverse(transform)) * normal_t));
    }
    else if (obj.type == tri) {
      mat4 transform = obj.transform;
      vec4 intersection_t = min_p0_t + min_p1_t * mindist;
      vec3 A = obj.vertices[0], B = obj.vertices[1], C = obj.vertices[2];
      vec3 normal_t3 = glm::normalize(glm::cross((B - A), (C - A)));
      vec4 normal_t = vec4(normal_t3, 0);
      intersection = transform * intersection_t;
      normal = glm::normalize(vec3(glm::transpose(glm::inverse(transform)) * normal_t));
    }
  }

  HitInfo hit;
  hit.is_hit = is_hit;
  if(is_hit) {
    // hit.mindist = mindist;
    hit.object_no = object_no;
    hit.intersection = intersection;
    hit.normal = normal;
  }
  return hit;
}

float hitSphere(Object obj, vec4 p0_t, vec4 p1_t) {
  float dist = -1;
  vec3 center = obj.vertices[0];
  vec3 p0 = vec3(p0_t), p1 = vec3(p1_t);
  // calculate quadratic
  float a = glm::dot(p1, p1);
  float b = 2 * glm::dot(p1, p0 - center);
  float c = glm::dot(p0 - center, p0 - center) - obj.radius * obj.radius;
  float root = b*b - 4*a*c;
  if (root > 0) {
    float t1 = (-b + sqrt(root)) / (2 * a);
    float t2 = (-b - sqrt(root)) / (2 * a);
    if (t1 * t2 < 0) {
      // ray origin inside sphere, pick the positive one
      if (t2 > 0) {
        dist = t2;
      }
      else if (t1 > 0) {
        dist = t1;
      }
    } 
    else if (t1 >=0 && t2 >= 0) {
      // two intersection
      dist = t1 < t2 ? t1 : t2;
    }
    else {
      // no intersection
      dist = -1;
    }
  }
  else if (root == 0) {
    // tangent to sphere
    float t = -b / (2 * a);
    if (t > 0) {
      dist = t;
    }
    else {
      // no intersection
      dist = -1;
    }
  }
  else {
    // no intersection
    dist = -1;
  }
  return dist;
}

float hitTriangle(Object obj, vec4 p0_t, vec4 p1_t) {
  float dist = -1;
  // 0 - A, 1 - B, 2 - C
  vec3 A = obj.vertices[0], B = obj.vertices[1], C = obj.vertices[2];
  vec3 tri_normal = glm::normalize(glm::cross((B - A), (C - A)));
  if (glm::dot(vec3(p1_t), tri_normal) != 0) {
  // enable culling
  // if (glm::dot(vec3(p1_t), tri_normal) < 0) {
    // normal of triangle is not parallel to the ray's direction
    // check if the intersection is inside the triangle
    dist = (glm::dot(A, tri_normal) - glm::dot(vec3(p0_t), tri_normal)) / glm::dot(vec3(p1_t), tri_normal);
    vec3 P = vec3(p0_t + dist * p1_t);

    // check the relative postion of P and each edge
    // edge AB
    vec3 AB = B - A, AP = P - A;
    if(glm::dot(glm::cross(AB, AP), tri_normal) < 0) return -1;
    // edge BC
    vec3 BC = C - B, BP = P - B;
    if(glm::dot(glm::cross(BC, BP), tri_normal) < 0) return -1;
    // edge CA
    vec3 AC = C - A;
    if(glm::dot(glm::cross(AP, AC), tri_normal) < 0) return -1;
  }
  return dist;
}

float hitTriangle_moller_trumbore(Object obj, vec4 p0_t, vec4 p1_t) {
  float dist = -1;
  vec3 A = obj.vertices[0], B = obj.vertices[1], C = obj.vertices[2];
  vec3 E1 = B - A, E2 = C - A, T = vec3(p0_t) - A;
  vec3 P = glm::cross(vec3(p1_t), E2), Q = glm::cross(T, E1);
  float p_dot_E1 = glm::dot(P, E1);
  // enable culling
  if (p_dot_E1 > 0) {
    float u = glm::dot(P, T) / p_dot_E1;
    if (u < 0 || u > 1) return -1;
    float v = glm::dot(Q, vec3(p1_t)) / p_dot_E1;
    if (v < 0 || u + v > 1) return -1;
    dist = glm::dot(Q, E2) / p_dot_E1;
  }
  return dist;
}
