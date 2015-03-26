#version 410

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D perlinTexture;

void main()
{
	FragColor = texture(perlinTexture, vTexCoord).rrrr;
	FragColor.a = 1;
}