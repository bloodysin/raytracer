vec3 getBaseColor(vec3 eye, HitInfo hit);
vec3 lighting(vec3 eye, Light light, HitInfo hit);
vec3 getReflectColor(vec4 origin, vec4 raydir, int level);
