#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <limits>

#include "CrossRT.h"
#include "scene_mgr.h" // RTX implementation of acceleration structures

class VulkanRTX : public ISceneObject
{
public:
  VulkanRTX(std::shared_ptr<SceneManager> a_pScnMgr);
  ~VulkanRTX();
  void ClearGeom() override;
  
  uint32_t AddGeom_Triangles4f(const LiteMath::float4* a_vpos4f, size_t a_vertNumber, const uint32_t* a_triIndices, size_t a_indNumber) override;
  void     UpdateGeom_Triangles4f(uint32_t a_geomId, const LiteMath::float4* a_vpos4f, size_t a_vertNumber, const uint32_t* a_triIndices, size_t a_indNumber) override;

  void ClearScene() override; 
  void CommitScene  () override; 
  
  uint32_t AddInstance(uint32_t a_geomId, const LiteMath::float4x4& a_matrix) override;
  void     UpdateInstance(uint32_t a_instanceId, const LiteMath::float4x4& a_matrix) override;

  CRT_Hit  RayQuery_NearestHit(LiteMath::float4 posAndNear, LiteMath::float4 dirAndFar) override;
  bool     RayQuery_AnyHit(LiteMath::float4 posAndNear, LiteMath::float4 dirAndFar) override;

  ////////////////////////////////////////////////////////////////////////////////////////////////

  void SetSceneAccelStruct(VkAccelerationStructureKHR handle) { m_accel = handle; }
  VkAccelerationStructureKHR GetSceneAccelStruct() const { return m_accel; }

protected:
  VkAccelerationStructureKHR m_accel;
  std::shared_ptr<SceneManager> m_pScnMgr;
  uint32_t m_meshTop;
};

