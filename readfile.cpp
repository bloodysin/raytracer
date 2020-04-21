/*****************************************************************************/
/* This is the program skeleton for homework 2 in CSE167 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  These functions are given to aid in setting up the 
// transformations properly, and to use glm functions in the right way.  
// Their use is optional in your program.  


// The functions readvals and readfile do basic parsing.  You can of course 
// rewrite the parser as you wish, but we think this basic form might be 
// useful to you.  It is a very simple parser.

// Please fill in parts that say YOUR CODE FOR HW 2 HERE. 
// Read the other parts to get a context of what is going on. 

/*****************************************************************************/

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Transform.h"

using namespace std;
#include "variables.h" 
#include "readfile.h"


// You may not need to use the following two functions, but it is provided
// here for convenience

// The function below applies the appropriate transform to a 4-vector
// void matransform(stack<mat4> &transfstack, GLfloat* values)
// {
  // mat4 transform = transfstack.top();
  // vec4 valvec = vec4(values[0],values[1],values[2],values[3]);
  // vec4 newval = transform * valvec;
  // for (int i = 0; i < 4; i++) values[i] = newval[i];
// }

void rightmultiply(const mat4 & M, stack<mat4> &transfstack)
{
  mat4 &T = transfstack.top();
  T = T * M;
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, float* values)
{
  for (int i = 0; i < numvals; i++) {
    s >> values[i];
    if (s.fail()) {
      cout << "Failed reading value " << i << " will skip\n";
      return false;
    }
  }
  return true;
}

vec3 getNormal(vec3 a, vec3 b, vec3 c) {
  // a,b,c is in counter-clockwise order
  vec3 ba = b - a;
  vec3 ca = c - a;
  return glm::normalize(glm::cross(ba, ca));
}

void readfile(const char* filename) 
{
  string str, cmd; 
  ifstream in;
  in.open(filename); 
  if (in.is_open()) {

    // I need to implement a matrix stack to store transforms.  
    // This is done using standard STL Templates 
    stack <mat4> transfstack; 
    transfstack.push(mat4(1.0));  // identity

    int vertexused = 0;
    // temp variables for material
    vec3 ambient, diffuse, specular, emission;
    float shininess;

    getline (in, str); 
    while (in) {
      if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
        // Ruled out comment and blank lines 

        stringstream s(str);
        s >> cmd; 
        // int i;
        float values[10]; // Position and color for light, colors for others
        // Up to 10 params for cameras.  
        bool validinput; // Validity of input 

        // Process the light, add it to database.
        // Lighting Command
        if (cmd == "directional") {
          // directional light
          validinput = readvals(s, 6, values); // Position/color for lts.
          if (validinput) {
            Light directional;
            directional.pos = vec4(values[0], values[1], values[2], 0);
            directional.color = vec3(values[3], values[4], values[5]);
            lights.push_back(directional);
            lightnum++;
          }
        }
        else if (cmd == "point") {
          // point light
          validinput = readvals(s, 6, values); // Position/color for lts.
          if (validinput) {
            Light point;
            point.pos = vec4(values[0], values[1], values[2], 1);
            point.color = vec3(values[3], values[4], values[5]);
            lights.push_back(point);
            lightnum++;
          }
        }

        else if (cmd == "ambient") {
          validinput = readvals(s, 3, values); // colors
          if (validinput) {
            // for (i = 0; i < 3; i++) {
              // ambient[i] = values[i];
            // }
            ambient = vec3(values[0], values[1], values[2]);
          }
        } else if (cmd == "diffuse") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            // for (i = 0; i < 3; i++) {
              // diffuse[i] = values[i];
            // }
            diffuse = vec3(values[0], values[1], values[2]);
          }
        } else if (cmd == "specular") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            // for (i = 0; i < 3; i++) {
              // specular[i] = values[i];
            // }
            specular = vec3(values[0], values[1], values[2]);
          }
        } else if (cmd == "emission") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            // for (i = 0; i < 3; i++) {
              // emission[i] = values[i];
            // }
            emission = vec3(values[0], values[1], values[2]);
          }
        } else if (cmd == "shininess") {
          validinput = readvals(s, 1, values);
          if (validinput) {
            shininess = values[0];
          }
        } else if (cmd == "size") {
          validinput = readvals(s,2,values);
          if (validinput) {
            width = (int) values[0]; height = (int) values[1];
          }
        } else if (cmd == "camera") {
          validinput = readvals(s,10,values); // 10 values eye cen up fov
          if (validinput) {
            camerainit = vec3(values[0], values[1], values[2]);
            center = vec3(values[3], values[4], values[5]);
            upinit = Transform::upvector(vec3(values[6], values[7], values[8]), camerainit - center);
            fovy = values[9];
          }
        } else if (cmd == "maxverts") {
          validinput = readvals(s, 1, values);
          if (validinput) {
            maxverts = (int) values[0];
            vertexindex = (vec3*)malloc(maxverts * sizeof(vec3));
          }
        } else if (cmd == "vertex") {
          // vertices index
          validinput = readvals(s, 3, values);
          if (validinput) {
            if(vertexused < maxverts) {
              vertexindex[vertexused] = vec3(values[0], values[1], values[2]);
            }
            else {
              std::cerr << "vertex number is more than max" << std::endl;
              exit(-1);
            }
          }
          vertexused++; 
        } else if (cmd == "sphere") {
          validinput = readvals(s, 4, values);
          if (validinput) {
            Object obj;
            obj.type = sphere;
            obj.vertices[0] = vec3(values[0], values[1], values[2]);
            obj.radius = values[3];
            obj.ambient = ambient;
            obj.emission = emission;
            obj.specular = specular;
            obj.diffuse = diffuse;
            // std::copy(ambient, ambient + 3, obj.ambient);
            // std::copy(emission, emission + 3, obj.emission);
            // std::copy(specular, specular + 3, obj.specular);
            // std::copy(diffuse, diffuse + 3, obj.specular);
            obj.shininess = shininess;
            obj.transform = transfstack.top();
            objects.push_back(obj);
          }
        } else if (cmd == "tri") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            Object obj;
            obj.type = tri;
            if (values[0] < maxverts && values[1] < maxverts && values[2] < maxverts) {
              obj.vertices[0] = vertexindex[(int)values[0]];
              obj.vertices[1] = vertexindex[(int)values[1]];
              obj.vertices[2] = vertexindex[(int)values[2]];
            }
            else {
              std::cerr << "the vertex requested doesnt't exist" << std::endl;
              exit(-1);
            }
            obj.normal = getNormal(obj.vertices[0], obj.vertices[1], obj.vertices[2]);
            obj.ambient = ambient;
            obj.emission = emission;
            obj.specular = specular;
            obj.diffuse = diffuse;
            // std::copy(ambient, ambient + 3, obj.ambient);
            // std::copy(emission, emission + 3, obj.emission);
            // std::copy(specular, specular + 3, obj.specular);
            // std::copy(diffuse, diffuse + 3, obj.specular);
            obj.shininess = shininess;
            obj.transform = transfstack.top();
            objects.push_back(obj);
          }
        } else if (cmd == "trinormal") {

        } else if (cmd == "translate") {
          validinput = readvals(s,3,values);
          if (validinput) {
            rightmultiply(Transform::translate(values[0], values[1], values[2]), transfstack);
          }
        }
        else if (cmd == "scale") {
          validinput = readvals(s,3,values);
          if (validinput) {
            rightmultiply(Transform::scale(values[0], values[1], values[2]), transfstack);
          }
        }
        else if (cmd == "rotate") {
          validinput = readvals(s,4,values);
          if (validinput) {
            vec3 axis = vec3(values[0], values[1], values[2]);
            mat3 rotateMat = Transform::rotate(values[3], axis);
            rightmultiply(mat4(rotateMat), transfstack);
          }
        }
        else if (cmd == "maxdepth") {
          validinput = readvals(s, 1, values);
          if(validinput) {
            maxdepth = values[0];
          }
        }
        else if (cmd == "attenuation") {
          validinput = readvals(s, 3, values);
          if(validinput) {
            attenuation[0] = values[0];
            attenuation[1] = values[1];
            attenuation[2] = values[2];
          }
        }

        else if (cmd == "pushTransform") {
          transfstack.push(transfstack.top());
        } else if (cmd == "popTransform") {
          if (transfstack.size() <= 1) {
            cerr << "Stack has no elements.  Cannot Pop\n";
            exit(-1);
          } else {
            transfstack.pop();
          }
        }

        else {
          cerr << "Unknown Command: " << cmd << " Skipping \n";
        }
      }
      getline (in, str);
    }

    // Set up initial position for eye, up and amount
    // As well as booleans 

    camera = camerainit;
    up = upinit;
    // amount = amountinit;
    // sx = sy = 1.0;  // keyboard controlled scales in x and y
    // tx = ty = 0.0;  // keyboard controllled translation in x and y
    // useGlu = false; // don't use the glu perspective/lookat fns

    // glEnable(GL_DEPTH_TEST);
  } else {
    cerr << "Unable to Open Input Data File " << filename << "\n"; 
    throw 2; 
  }
}
