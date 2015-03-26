#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Colour;
layout(location=2) in vec2 Texcoord;

out vec2 vTexCoord;

uniform mat4 ProjectionView;
uniform sampler2D perlinTexture;

void main()
{
	vec4 pos = position;
	pos.y += texture(perlinTexture, Texcoord).r * 5;

	vTexCoord = Texcoord;
	gl_Position = ProjectionView * Position;
}