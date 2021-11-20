#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout (binding = 0, set = 0, r32ui) uniform readonly uimage2D colorImage;

//layout(binding = 0, set = 0, rgba32f) uniform readonly  image2D a_texture2dFullRes;
//layout(binding = 2, set = 0, r32ui) uniform writeonly uimage2D out_color;

layout (location = 0 ) in VS_OUT
{
  vec2 texCoord;
} surf;

void main()
{
  uint rgbaPacked = imageLoad(colorImage, ivec2(surf.texCoord*1024.0f)).x;
  uint red   = (rgbaPacked & 0x000000FF);
  uint green = (rgbaPacked & 0x0000FF00) >> 8;
  uint blue  = (rgbaPacked & 0x00FF0000) >> 16;
  color = vec4(float(red), float(green), float(blue), 0.0f)*(1.0f/256.0f);
}
