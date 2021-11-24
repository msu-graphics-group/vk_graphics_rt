#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) buffer data0 { uint colorBuff[]; }; //

layout (location = 0 ) in VS_OUT
{
  vec2 texCoord;
} surf;


layout(push_constant) uniform params_t
{
  vec4 resolution;
} params;

void main()
{
  ivec2 coord = ivec2(surf.texCoord*params.resolution.xy);
  uint rgbaPacked = colorBuff[coord.y*int(params.resolution.x) + coord.x];
  uint red   = (rgbaPacked & 0x000000FF);
  uint green = (rgbaPacked & 0x0000FF00) >> 8;
  uint blue  = (rgbaPacked & 0x00FF0000) >> 16;
  color = vec4(float(red), float(green), float(blue), 0.0f)*(1.0f/255.0f);
  //color = textureLod(colorTex, surf.texCoord, 0);
}
