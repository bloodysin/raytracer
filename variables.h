#include <vector>
#include <glm/glm.hpp>

#ifdef MAINPROGRAM
#define EXTERN
#else
#define EXTERN extern
#endif

#define EPSILON 0.0001

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

EXTERN vec3 camera;
EXTERN vec3 up;
// EXTERN vec4 view;
// params for ray shooting
EXTERN vec3 u, v, w;
EXTERN float tan_halffovx;

#ifdef MAINPROGRAM
vec3 camerainit(0.0, 0.0, 5.0);
vec3 upinit(0.0, 1.0, 0.0);
vec3 center(0.0, 0.0, 0.0);
int height = 500, width = 500, maxdepth = 5;
int maxverts = 10, maxvertnorms = 10;
float fovy = 90.0;
char* filename = "test.png";
float attenuation[3] = {1, 0, 0};
#else
EXTERN vec3 camerainit;
EXTERN vec3 upinit;
EXTERN vec3 center;
EXTERN int height, width, maxdepth;
EXTERN int maxverts, maxvertnorms;
EXTERN float fovy;
EXTERN char* filename;
EXTERN float attenuation[3];
#endif

enum shape {sphere, tri};

// light
EXTERN int lightnum;
typedef struct light {
  vec4 pos;
  vec3 color;
} Light;
EXTERN std::vector<Light> lights;
// attenuation const linear quadratic

EXTERN vec3 *vertexindex;
EXTERN vec3 *vertnormindex;

typedef struct object{
  shape type;
  vec3 vertices[3];
  vec3 normal;
  float radius;
  float size;
  vec3 emission;
  vec3 specular;
  vec3 diffuse;
  vec3 ambient;
  float shininess;
  mat4 transform;
} Object;
EXTERN std::vector<Object> objects;

typedef struct hitInfo{
  bool is_hit;
  // float mindist;
  int object_no;
  vec4 intersection;
  vec3 normal;
} HitInfo;

// typedef struct intersectInfo{
  // bool if_inter;
  // float mindist;
  // vec4 intersection;
  // vec3 normal;
// } IntersectInfo;
