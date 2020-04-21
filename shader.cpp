#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <math.h>
#include <iostream>

using namespace std;
#include "variables.h"
#include "raytrace.h"
#include "shader.h"

float getDistance(vec4 a, vec4 b) {
  return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z);
}

vec3 getBaseColor(vec3 eye, HitInfo hit){
  vec3 final_color = vec3(0, 0, 0);
  // bool is_shadow = true;
  // shoot shadow ray to each light
  for(int i = 0; i < lights.size(); i++) {
    Light light = lights[i];
    HitInfo hit_shadow;
    float atten = 1;
    vec4 origin = vec4(0, 0, 0, 0);
    if(light.pos.w == 0) {
      // directional light
      atten = 1;
      origin = hit.intersection + (float)EPSILON * (light.pos);
      hit_shadow = hitDetection(origin, light.pos);
    }
    else if(light.pos.w == 1) {
      // point light
      float r = getDistance(light.pos, hit.intersection);
      atten = 1 / (attenuation[0] + attenuation[1]*sqrt(r) + attenuation[2]*r);
      vec4 raydir = light.pos - hit.intersection;
      origin = hit.intersection + (float)EPSILON * raydir;
      hit_shadow = hitDetection(origin, raydir);
    }
    if(hit_shadow.is_hit && light.pos.w == 1) {
      // check if the hitted object is behind light source
      // if true then no hit actually
      float dist_light = getDistance(origin, light.pos);
      float dist_inter = getDistance(origin, hit_shadow.intersection);
      if(dist_inter >= dist_light) {
        hit_shadow.is_hit = false;
      }
    }

    if(!hit_shadow.is_hit) {
      // calculate material reflection for each light
      final_color += atten * lighting(eye, light, hit);
    }
  }
  return final_color;
}

vec3 lighting(vec3 eye, Light light, HitInfo hit) {
  // TODO change vec4 to vec3
  Object obj = objects[hit.object_no];
  vec4 light_color = vec4(light.color, 1);
  vec3 vertex = vec3(hit.intersection) / hit.intersection.w;
  vec3 normal = vec3(hit.normal);
  vec4 diffuse = vec4(obj.diffuse, 1);
  vec4 specular = vec4(obj.specular, 1);
  vec3 light_dir;
  if(light.pos.w == 0) {
    // directional light
    light_dir = glm::normalize(vec3(light.pos));
  }
  else if(light.pos.w == 1) {
    // point light
    light_dir = glm::normalize(vec3(light.pos) - vertex);
  }
  // lambert for diffuse
  float norm_dot_I = glm::dot(normal, light_dir);
  vec4 lambert = light_color * diffuse * max(norm_dot_I, 0.0f);
  // Blinn-Phong for specular
  vec3 eye_dir = glm::normalize(eye - vertex);
  vec3 halfvec = glm::normalize(eye_dir + light_dir);
  float norm_dot_half = glm::dot(normal, halfvec);
  vec4 phong = light_color * specular * pow(max(norm_dot_half, 0.0f), obj.shininess);
  return vec3(lambert + phong);
}

vec3 getReflectColor(vec4 origin, vec4 raydir, int level) {
  // recursively get reflection color
  vec4 origin_temp = origin + (float)EPSILON * raydir;
  HitInfo new_hit = hitDetection(origin_temp, raydir);
  if (new_hit.is_hit) {
    if (level < maxdepth) {
      Object obj = objects[new_hit.object_no];
      vec3 reflection_dir = 2 * (glm::dot(new_hit.normal, -vec3(raydir))) * new_hit.normal + vec3(raydir);
      return getBaseColor(vec3(origin), new_hit) 
        + obj.specular * getReflectColor(new_hit.intersection, vec4(reflection_dir, 0), ++level);
    }
    return getBaseColor(vec3(origin), new_hit);
  }
  return vec3(0, 0, 0);
}
