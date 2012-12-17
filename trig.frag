precision highp float;

float sinf(float x)
{
  x*=0.159155;
  x-=floor(x);
  float xx=x*x;
  float y=-6.87897;
  y=y*xx+33.7755;
  y=y*xx-72.5257;
  y=y*xx+80.5874;
  y=y*xx-41.2408;
  y=y*xx+6.28077;
  return x*y;
}
float cosf(float x)
{
  return sinf(x+1.5708);
}

//
// There is no speed advantage in vectorising the following
// Maybe the compiler figures out the vectorisation itself?
//
vec2 sinf(vec2 x)
{
  return vec2(sinf(x.x),sinf(x.y));
}
vec2 cosf(vec2 x)
{
  return vec2(cosf(x.x),cosf(x.y));
}
vec3 sinf(vec3 x)
{
  return vec3(sinf(x.x),sinf(x.y),sinf(x.z));
}
vec3 cosf(vec3 x)
{
  return vec3(cosf(x.x),cosf(x.y),cosf(x.z));
}
vec4 sinf(vec4 x)
{
  return vec4(sinf(x.x),sinf(x.y),sinf(x.z),sinf(x.w));
}
vec4 cosf(vec4 x)
{
  return vec4(cosf(x.x),cosf(x.y),cosf(x.z),cosf(x.w));
}
