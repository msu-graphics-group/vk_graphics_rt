#ifndef RayTracer_UBO_H
#define RayTracer_UBO_H

#ifndef GLSL
#define LAYOUT_STD140
#include "LiteMath.h"
typedef LiteMath::float4x4 mat4;
typedef LiteMath::float2   vec2;
typedef LiteMath::float3   vec3;
typedef LiteMath::float4   vec4;
typedef unsigned int       uint;
#else
#define MAXFLOAT 1e37f
#define M_PI          3.14159265358979323846f
#define M_TWOPI       6.28318530717958647692f
#define INV_PI        0.31830988618379067154f
#define INV_TWOPI     0.15915494309189533577f
#endif

struct RayTracer_UBO_Data
{
  mat4 m_invProjView; 
  vec4 m_camPos; 
  uint m_height; 
  uint m_width; 
  uint m_pAccelStruct_capacity; 
  uint m_pAccelStruct_size; 
  uint dummy_last;
};

#endif


