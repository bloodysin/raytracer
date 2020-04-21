// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
  // YOUR CODE FOR HW2 HERE
  // Please implement this.  Likely the same as in HW 1.  
  mat3 ret = mat3(1.0f)
           * cos(glm::radians(degrees))
           + mat3(axis.x * axis.x, axis.x * axis.y, axis.x * axis.z,
                  axis.y * axis.x, axis.y * axis.y, axis.y * axis.z,
                  axis.z * axis.x, axis.z * axis.y, axis.z * axis.z)
           * (1 - cos(glm::radians(degrees)))
           + mat3(0, axis.z, -axis.y,
                  -axis.z, 0, axis.x,
                  axis.y, -axis.x, 0)
           * sin(glm::radians(degrees));
  // You will change this return call
  return ret;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
  // YOUR CODE FOR HW2 HERE
  // Likely the same as in HW 1.  
  eye = rotate(degrees, up) * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
  // YOUR CODE FOR HW2 HERE 
  // Likely the same as in HW 1.  
  vec3 axis = glm::cross(eye, up);
  axis = glm::normalize(axis);
  mat3 rotation = rotate(degrees, axis);
  eye = rotation * eye;
  up = rotation * up;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
  // YOUR CODE FOR HW2 HERE
  // Likely the same as in HW 1.  
  vec3 offset = eye - center;
  vec3 waxis = glm::normalize(offset);
  vec3 uaxis = glm::normalize(glm::cross(up, waxis));
  vec3 vaxis = glm::cross(waxis, uaxis);
  mat3 rotation3d = glm::transpose(mat3(uaxis, vaxis, waxis));
  mat4 rotation = mat4(rotation3d);
  // build translation matrix
  mat4 translation = mat4(1.0f, 0.0f, 0.0f, -offset.x,
                          0.0f, 1.0f, 0.0f, -offset.y,
                          0.0f, 0.0f, 1.0f, -offset.z,
                          0.0f, 0.0f, 0.0f, 1.0f);
  translation = glm::transpose(translation);
  // You will change this return call
  return rotation * translation;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  float d = 0;
  float theta = glm::radians(fovy / 2);
  // distance between camera and screen
  if(tan(theta)) {
    d = 1 / tan(theta);
  }
  mat4 per = mat4(d / aspect, 0.0f, 0.0f, 0.0f,
                  0.0f, d,    0.0f, 0.0f,
                  0.0f, 0.0f, -((zFar + zNear) / (zFar - zNear)), -(2 * zFar * zNear / (zFar - zNear)),
                  0.0f, 0.0f, -1.0f, 0.0f);
  per = glm::transpose(per);
  return per;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
  mat4 matScal = mat4(sx, 0.0f, 0.0f, 0.0f,
                      0.0f, sy, 0.0f, 0.0f,
                      0.0f, 0.0f, sz, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f);
  return glm::transpose(matScal);
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
  mat4 matTranslate = mat4(1.0f, 0.0f, 0.0f, tx,
                           0.0f, 1.0f, 0.0f, ty,
                           0.0f, 0.0f, 1.0f, tz,
                           0.0f, 0.0f, 0.0f, 1.0f);
  return glm::transpose(matTranslate);
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
  vec3 x = glm::cross(up,zvec); 
  vec3 y = glm::cross(zvec,x); 
  vec3 ret = glm::normalize(y); 
  return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
