void constructCord();
vec3 getRayDirection(int x, int y);
HitInfo hitDetection(vec4 ray, vec4 raydir);
float hitSphere(Object obj, vec4 p0_t, vec4 p1_t);
float hitTriangle(Object obj, vec4 p0_t, vec4 p1_t);
float hitTriangle_moller_trumbore(Object obj, vec4 p0_t, vec4 p1_t);
