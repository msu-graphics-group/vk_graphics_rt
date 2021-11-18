#include <render/VulkanRTX.h>
#include "simple_render.h"
#include "raytracing_generated.h"

// ***************************************************************************************************************************
// setup full screen quad to display ray traced image
void SimpleRender::SetupQuadRenderer()
{
  vk_utils::RenderTargetInfo2D rtargetInfo = {};
  rtargetInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  rtargetInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  rtargetInfo.format = m_swapchain.GetFormat();
  rtargetInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  rtargetInfo.size   = m_swapchain.GetExtent();

  m_pFSQuad.reset();
  m_pFSQuad = std::make_shared<vk_utils::QuadRenderer>(0,0, m_width, m_height);
  m_pFSQuad->Create(m_device, "../resources/shaders/quad3_vert.vert.spv", "../resources/shaders/my_quad.frag.spv", rtargetInfo);
}

void SimpleRender::SetupQuadDescriptors()
{
  m_pBindings->BindBegin(VK_SHADER_STAGE_FRAGMENT_BIT);
  m_pBindings->BindImage(0, m_rtImage.view, m_rtImageSampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  m_pBindings->BindEnd(&m_quadDS, &m_quadDSLayout);
}

void SimpleRender::SetupRTImage()
{
  vk_utils::deleteImg(m_device, &m_rtImage);

  // change format and usage according to your implementation of RT
  m_rtImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createImgAllocAndBind(m_device, m_physicalDevice, m_width, m_height, VK_FORMAT_R8G8B8A8_UNORM,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, &m_rtImage);

  if(m_rtImageSampler == VK_NULL_HANDLE)
  {
    m_rtImageSampler = vk_utils::createSampler(m_device, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK);
  }
}
// ***************************************************************************************************************************

// convert geometry data and pass it to acceleration structure builder
void SimpleRender::SetupRTScene()
{
  m_pAccelStruct = std::shared_ptr<ISceneObject>(CreateSceneRT(""));
  m_pAccelStruct->ClearGeom();

  auto meshesData = m_pScnMgr->GetMeshData();
  std::unordered_map<uint32_t, uint32_t> meshMap;
  for(size_t i = 0; i < m_pScnMgr->MeshesNum(); ++i)
  {
    const auto& info = m_pScnMgr->GetMeshInfo(i);
    auto vertices = reinterpret_cast<float*>((char*)meshesData->VertexData() + info.m_vertexOffset * meshesData->SingleVertexSize());
    auto indices = meshesData->IndexData() + info.m_indexOffset;

    auto stride = meshesData->SingleVertexSize() / sizeof(float);
    std::vector<float4> m_vPos4f(info.m_vertNum);
    std::vector<uint32_t> m_indicesReordered(info.m_indNum);
    for(size_t v = 0; v < info.m_vertNum; ++v)
    {
      m_vPos4f[v] = float4(vertices[v * stride + 0], vertices[v * stride + 1], vertices[v * stride + 2], 1.0f);
    }
    memcpy(m_indicesReordered.data(), indices, info.m_indNum * sizeof(m_indicesReordered[0]));

    auto geomId = m_pAccelStruct->AddGeom_Triangles4f(m_vPos4f.data(), m_vPos4f.size(), m_indicesReordered.data(), m_indicesReordered.size());
    meshMap[i] = geomId;
  }

  m_pAccelStruct->ClearScene();
  for(size_t i = 0; i < m_pScnMgr->InstancesNum(); ++i)
  {
    const auto& info = m_pScnMgr->GetInstanceInfo(i);
    if(meshMap.count(info.mesh_id))
      m_pAccelStruct->AddInstance(meshMap[info.mesh_id], m_pScnMgr->GetInstanceMatrix(info.inst_id));
  }
  m_pAccelStruct->CommitScene();
}

// perform ray tracing on the CPU and upload resulting image on the GPU
void SimpleRender::RayTrace()
{
  if(!m_pRayTracer)
  {
    m_pRayTracer = std::make_unique<RayTracer_Generated>(m_width, m_height);
    m_pRayTracer->SetScene(m_pAccelStruct);

  }

  m_pRayTracer->UpdateView(m_cam.pos, m_inverseProjViewMatrix);
#pragma omp parallel for default(none)
  for (size_t j = 0; j < m_height; ++j)
  {
    for (size_t i = 0; i < m_width; ++i)
    {
      m_pRayTracer->CastSingleRay(i, j, m_raytracedImageData.data());
    }
  }

  m_pScnMgr->GetCopyHelper()->UpdateImage(m_rtImage.image, m_raytracedImageData.data(), m_width, m_height, 4, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void SimpleRender::RayTraceGPU()
{
  if(!m_pRayTracer)
  {
    m_pRayTracer = std::make_unique<RayTracer_Generated>(m_width, m_height);
    m_pRayTracer->InitVulkanObjects(m_device, m_physicalDevice, m_width * m_height);
    m_pRayTracer->InitMemberBuffers();

    const size_t bufferSize1 = m_width * m_height * sizeof(uint32_t);

    m_genColorBuffer = vk_utils::createBuffer(m_device, bufferSize1,  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_colorMem       = vk_utils::allocateAndBindWithPadding(m_device, m_physicalDevice, {m_genColorBuffer});

    auto tmp = std::make_shared<VulkanRTX>(m_device, m_physicalDevice, m_queueFamilyIDXs.transfer, m_queueFamilyIDXs.graphics);
    tmp->SetSceneAccelStruct(m_pScnMgr->getTLAS().handle);
    m_pRayTracer->SetScene(tmp);
    m_pRayTracer->SetVulkanInOutFor_CastSingleRay(m_genColorBuffer, 0);
    m_pRayTracer->UpdateAll(m_pScnMgr->GetCopyHelper());
  }

  m_pRayTracer->UpdateView(m_cam.pos, m_inverseProjViewMatrix);
  m_pRayTracer->UpdatePlainMembers(m_pScnMgr->GetCopyHelper());

  {
    VkCommandBuffer commandBuffer = vk_utils::createCommandBuffer(m_device, m_commandPool);

    VkCommandBufferBeginInfo beginCommandBufferInfo = {};
    beginCommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginCommandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginCommandBufferInfo);
    m_pRayTracer->CastSingleRayCmd(commandBuffer, m_width, m_height, nullptr);
    vkEndCommandBuffer(commandBuffer);

    vk_utils::executeCommandBufferNow(commandBuffer, m_graphicsQueue, m_device);

    m_pScnMgr->GetCopyHelper()->ReadBuffer(m_genColorBuffer, 0, m_raytracedImageData.data(), m_raytracedImageData.size()*sizeof(m_raytracedImageData[0]));
  }

  m_pScnMgr->GetCopyHelper()->UpdateImage(m_rtImage.image, m_raytracedImageData.data(), m_width, m_height, 4, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}