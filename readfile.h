// Readfile definitions 

// void matransform (stack<mat4> &transfstack, GLfloat * values) ;
void rightmultiply (const mat4 & M, stack<mat4> &transfstack) ;
bool readvals (stringstream &s, const int numvals, float * values) ;
vec3 getNormal(vec3 a, vec3 b, vec3 c);
void readfile (const char * filename) ;
