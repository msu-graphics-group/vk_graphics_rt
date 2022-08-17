#ifndef MAIN_CLASS_DECL_RayTracer_H
#define MAIN_CLASS_DECL_RayTracer_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "vk_pipeline.h"
#include "vk_buffers.h"
#include "vk_utils.h"
#include "vk_copy.h"
#include "vk_context.h"

#include "raytracing.h"

#include "include/RayTracer_ubo.h"

class RayTracer_Generated : public RayTracer
{
public:

  RayTracer_Generated(uint32_t a_width, uint32_t a_height) : RayTracer(a_width, a_height) {}
  virtual void InitVulkanObjects(VkDevice a_device, VkPhysicalDevice a_physicalDevice, size_t a_maxThreadsCount);
  virtual void SetVulkanContext(vk_utils::VulkanContext a_ctx) { m_ctx = a_ctx; }

  virtual void SetVulkanInOutFor_CastSingleRay(
    VkBuffer out_colorBuffer,
    size_t   out_colorOffset,
    uint32_t dummyArgument = 0)
  {
    CastSingleRay_local.out_colorBuffer = out_colorBuffer;
    CastSingleRay_local.out_colorOffset = out_colorOffset;
    InitAllGeneratedDescriptorSets_CastSingleRay();
  }

  virtual ~RayTracer_Generated();


  virtual void InitMemberBuffers();

  virtual void UpdateAll(std::shared_ptr<vk_utils::ICopyEngine> a_pCopyEngine)
  {
    UpdatePlainMembers(a_pCopyEngine);
    UpdateVectorMembers(a_pCopyEngine);
    UpdateTextureMembers(a_pCopyEngine);
  }
  
  
  virtual void UpdatePlainMembers(std::shared_ptr<vk_utils::ICopyEngine> a_pCopyEngine);
  virtual void UpdateVectorMembers(std::shared_ptr<vk_utils::ICopyEngine> a_pCopyEngine);
  virtual void UpdateTextureMembers(std::shared_ptr<vk_utils::ICopyEngine> a_pCopyEngine);
  
  virtual void CastSingleRayCmd(VkCommandBuffer a_commandBuffer, uint32_t tidX, uint32_t tidY, uint32_t* out_color);

  virtual void copyKernelFloatCmd(uint32_t length);
  
  virtual void CastSingleRayMegaCmd(uint32_t tidX, uint32_t tidY, uint32_t* out_color);
  
  struct MemLoc
  {
    VkDeviceMemory memObject = VK_NULL_HANDLE;
    size_t         memOffset = 0;
    size_t         allocId   = 0;
  };

  virtual MemLoc AllocAndBind(const std::vector<VkBuffer>& a_buffers); ///< replace this function to apply custom allocator
  virtual MemLoc AllocAndBind(const std::vector<VkImage>& a_image);    ///< replace this function to apply custom allocator
  virtual void   FreeAllAllocations(std::vector<MemLoc>& a_memLoc);    ///< replace this function to apply custom allocator

protected:

  VkPhysicalDevice        physicalDevice = VK_NULL_HANDLE;
  VkDevice                device         = VK_NULL_HANDLE;
  vk_utils::VulkanContext m_ctx          = {};

  VkCommandBuffer         m_currCmdBuffer   = VK_NULL_HANDLE;
  uint32_t                m_currThreadFlags = 0;

  std::vector<MemLoc>     m_allMems;

  std::unique_ptr<vk_utils::ComputePipelineMaker> m_pMaker = nullptr;
  VkPhysicalDeviceProperties m_devProps;

  VkBufferMemoryBarrier BarrierForClearFlags(VkBuffer a_buffer);
  VkBufferMemoryBarrier BarrierForSingleBuffer(VkBuffer a_buffer);
  void BarriersForSeveralBuffers(VkBuffer* a_inBuffers, VkBufferMemoryBarrier* a_outBarriers, uint32_t a_buffersNum);

  virtual void InitHelpers();
  virtual void InitBuffers(size_t a_maxThreadsCount, bool a_tempBuffersOverlay = true);
  virtual void InitKernels(const char* a_filePath);
  virtual void AllocateAllDescriptorSets();

  virtual void InitAllGeneratedDescriptorSets_CastSingleRay();

  virtual void AssignBuffersToMemory(const std::vector<VkBuffer>& a_buffers, VkDeviceMemory a_mem);

  virtual void AllocMemoryForMemberBuffersAndImages(const std::vector<VkBuffer>& a_buffers, const std::vector<VkImage>& a_image);
  virtual std::string AlterShaderPath(const char* in_shaderPath) { return std::string(in_shaderPath); }

  
  

  struct CastSingleRay_Data
  {
    VkBuffer out_colorBuffer = VK_NULL_HANDLE;
    size_t   out_colorOffset = 0;
  } CastSingleRay_local;



  struct MembersDataGPU
  {
  } m_vdata;

  size_t m_maxThreadCount = 0;
  VkBuffer m_classDataBuffer = VK_NULL_HANDLE;

  VkPipelineLayout      CastSingleRayMegaLayout   = VK_NULL_HANDLE;
  VkPipeline            CastSingleRayMegaPipeline = VK_NULL_HANDLE; 
  VkDescriptorSetLayout CastSingleRayMegaDSLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout CreateCastSingleRayMegaDSLayout();
  void InitKernel_CastSingleRayMega(const char* a_filePath);


  virtual VkBufferUsageFlags GetAdditionalFlagsForUBO() const;

  VkPipelineLayout      copyKernelFloatLayout   = VK_NULL_HANDLE;
  VkPipeline            copyKernelFloatPipeline = VK_NULL_HANDLE;
  VkDescriptorSetLayout copyKernelFloatDSLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout CreatecopyKernelFloatDSLayout();

  VkDescriptorPool m_dsPool = VK_NULL_HANDLE;
  VkDescriptorSet  m_allGeneratedDS[1];

  RayTracer_UBO_Data m_uboData;
  
  constexpr static uint32_t MEMCPY_BLOCK_SIZE = 256;
  constexpr static uint32_t REDUCTION_BLOCK_SIZE = 256;
};

#endif

