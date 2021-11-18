#include "raytracing.h"


using namespace LiteMath;

float3 EyeRayDir(float x, float y, float w, float h, float4x4 a_mViewProjInv)
{
  float4 pos = LiteMath::make_float4( 2.0f * (x + 0.5f) / w - 1.0f,
    2.0f * (y + 0.5f) / h - 1.0f,
    0.0f,
    1.0f );

  pos = a_mViewProjInv * pos;
  pos /= pos.w;

  //  pos.y *= (-1.0f);

  return normalize(to_float3(pos));
}

void RayTracer::CastSingleRay(uint tidX, uint tidY, uint* out_color)
{
  float4 rayPosAndNear = to_float4(m_camPos, 0.0f);
  const float3 rayDir  = EyeRayDir(tidX, tidY, m_width, m_height, m_invProjView);
  float4 rayDirAndFar  = to_float4(rayDir, MAXFLOAT);

  kernel_RayTrace(tidX, tidY, &rayPosAndNear, &rayDirAndFar, out_color);
}

void RayTracer::kernel_RayTrace(uint tidX, uint tidY, const float4* rayPosAndNear, float4* rayDirAndFar, uint* out_color)
{
  const float4 rayPos = *rayPosAndNear;
  const float4 rayDir = *rayDirAndFar ;

  auto hit = m_pAccelStruct->RayQuery_NearestHit(rayPos, rayDir);

  out_color[tidY * m_width + tidX] = m_palette[hit.instId % (sizeof(m_palette) / sizeof(m_palette[0]))];
}