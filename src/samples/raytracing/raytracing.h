#ifndef VK_GRAPHICS_RT_RAYTRACING_H
#define VK_GRAPHICS_RT_RAYTRACING_H

#include <cstdint>
#include <memory>
#include <iostream>

#define LAYOUT_STD140
#include "LiteMath.h"
#include "render/CrossRT.h"

class RayTracer
{
public:
  RayTracer() { std::cout << "sizeof(float3) = " << sizeof(LiteMath::float3) << std::endl; }
  RayTracer(uint32_t a_width, uint32_t a_height) : m_width(a_width), m_height(a_height) { std::cout << "sizeof(float3) = " << sizeof(LiteMath::float3) << std::endl;};

  void UpdateView(const LiteMath::float3& a_camPos, const LiteMath::float4x4& a_invProjView ) { m_camPos = a_camPos; m_invProjView = a_invProjView; }
  void SetScene(std::shared_ptr<ISceneObject> a_pAccelStruct) { m_pAccelStruct = a_pAccelStruct; };

  void CastSingleRay(uint32_t tidX, uint32_t tidY, uint32_t* out_color);
  void kernel_InitEyeRay(uint32_t tidX, uint32_t tidY, LiteMath::float4* rayPosAndNear, LiteMath::float4* rayDirAndFar);
  void kernel_RayTrace(uint32_t tidX, uint32_t tidY, const LiteMath::float4* rayPosAndNear, const LiteMath::float4* rayDirAndFar, uint32_t* out_color);

protected:
  uint32_t m_width;
  uint32_t m_height;

  LiteMath::float3   m_camPos;
  LiteMath::float4x4 m_invProjView;

  std::shared_ptr<ISceneObject> m_pAccelStruct;

  static constexpr uint32_t palette_size = 20;
  // color palette to select color for objects based on mesh/instance id
  static constexpr uint32_t m_palette[palette_size] = {
    0xffe6194b, 0xff3cb44b, 0xffffe119, 0xff0082c8,
    0xfff58231, 0xff911eb4, 0xff46f0f0, 0xfff032e6,
    0xffd2f53c, 0xfffabebe, 0xff008080, 0xffe6beff,
    0xffaa6e28, 0xfffffac8, 0xff800000, 0xffaaffc3,
    0xff808000, 0xffffd8b1, 0xff000080, 0xff808080
  };
};

#endif// VK_GRAPHICS_RT_RAYTRACING_H
