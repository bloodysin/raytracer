// #include "lib/lodepng.h"
#include <FreeImage.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iomanip>

using namespace std;

// header of global variables
#define MAINPROGRAM
#include "variables.h"
#include "readfile.h"
#include "raytrace.h"
#include "shader.h"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "no input file" << std::endl;
    exit(-1);
  }

  readfile(argv[1]);
  tan_halffovx = tan(glm::radians(0.5 * fovy)) * (float) width / (float) height;

  // generate image by freeimage
  FreeImage_Initialise();
  BYTE *pixels = new BYTE[3*width*height];

  BYTE temp = 255;
  // steps for calculating prgress 
  int steps = 50;
  int step_size = width * height / steps;
  int step_completed = 0;
  int local_count = 0;
  time_t start, end;
  time(&start);
  // the order of image array is row-first
#pragma omp parallel for private(local_count)
  for(int x = 0; x < height; x++) {
    for(int y = 0; y < width; y++) {
      vec3 final_color = vec3(0, 0, 0);
      // ray tracing by pixel
      constructCord();
      vec3 raydir = getRayDirection(x, y);
      
      // shooting primary ray from camera
      HitInfo hit = hitDetection(vec4(camera, 1), vec4(raydir, 0));
      if(hit.is_hit) {
        // shooting shadow ray to each light to find the color of this pixel
        final_color += getBaseColor(camera, hit);
        // shoot recursive ray from intersection point
        vec3 reflection_dir = 2 * (glm::dot(hit.normal, -raydir)) * hit.normal + raydir;
        Object obj = objects[hit.object_no];
        // final_color += obj.specular * getReflectColor(hit.intersection, vec4(reflection_dir, 0), 1);
        // add global ambient and emission
        final_color += obj.ambient + obj.emission;
      }
      
      // B
      pixels[3*y + 3*width*x + 0] = final_color.z * 255;
      // G
      pixels[3*y + 3*width*x + 1] = final_color.y * 255;
      // R
      pixels[3*y + 3*width*x + 2] = final_color.x * 255;

      // calculate progress
      if(local_count++ % step_size == step_size - 1) {
// #pragma omp atomic
        step_completed++;
#pragma omp critical 
        std::cout << 2 * step_completed << "%" << "\t\r" << std::flush;
      }
    }
  }
  std::cout << std::endl;
  time(&end);
  free(vertexindex);
  // color order of pixels is BGR
  FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
  FreeImage_Save(FIF_PNG, img, filename, 0);

  FreeImage_DeInitialise();
  std::cout << "generate successfully" << std::endl;
  std::cout << "time :" << std::fixed << double(end - start) << std::setprecision(5) << "s" << std::endl;
  return 0;
}
