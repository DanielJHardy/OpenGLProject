#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 texCoord;

out vec2 frag_texCoord;

void main()
{
	gl_Position = position;
	frag_texCoord = texCoord;
}