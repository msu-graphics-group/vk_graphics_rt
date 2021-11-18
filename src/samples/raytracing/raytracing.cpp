#include "raytracing.h"

LiteMath::float3 EyeRayDir(float x, float y, float w, float h, LiteMath::float4x4 a_mViewProjInv)
{
  LiteMath::float4 pos = LiteMath::make_float4( 2.0f * (x + 0.5f) / w - 1.0f,
    2.0f * (y + 0.5f) / h - 1.0f,
    0.0f,
    1.0f );

  pos = a_mViewProjInv * pos;
  pos /= pos.w;

  //  pos.y *= (-1.0f);

  return normalize(to_float3(pos));
}

void RayTracer::CastSingleRay(uint32_t tidX, uint32_t tidY, uint32_t* out_color)
{
  LiteMath::float4 rayPosAndNear, rayDirAndFar;
  kernel_InitEyeRay(tidX, tidY, &rayPosAndNear, &rayDirAndFar);

  kernel_RayTrace(tidX, tidY, &rayPosAndNear, &rayDirAndFar, out_color);
}

void RayTracer::kernel_InitEyeRay(uint32_t tidX, uint32_t tidY, LiteMath::float4* rayPosAndNear, LiteMath::float4* rayDirAndFar)
{
  *rayPosAndNear = to_float4(m_camPos, 0.0f);

  const LiteMath::float3 rayDir  = EyeRayDir(tidX, tidY, m_width, m_height, m_invProjView);
  *rayDirAndFar  = to_float4(rayDir, MAXFLOAT);
}


void RayTracer::kernel_RayTrace(uint32_t tidX, uint32_t tidY, const LiteMath::float4* rayPosAndNear, const LiteMath::float4* rayDirAndFar, uint32_t* out_color)
{
  const LiteMath::float4 rayPos = *rayPosAndNear;
  const LiteMath::float4 rayDir = *rayDirAndFar ;

  CRT_Hit hit = m_pAccelStruct->RayQuery_NearestHit(rayPos, rayDir);

  out_color[tidY * m_width + tidX] = m_palette[hit.instId % palette_size];
}