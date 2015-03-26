#version 410
in vec2 vTexCoord; 
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;
out vec4 FragColor; 
uniform vec3 LightDir = vec3(0.808, -0.909, 0);
uniform sampler2D diffuse; 
uniform sampler2D normal;
void main() 
{ 
	mat3 TBN = mat3(
					normalize(vTangent),
					normalize(vBiTangent),
					normalize(vNormal));

	vec3 N = texture(normal, vTexCoord).xyz * 2 - 1;
	float d = max(0, dot(normalize(TBN * N), normalize(LightDir)));
	//FragColor = texture(diffuse, vTexCoord); 

	vec4 diffuseColour = texture(diffuse, vTexCoord); 
	FragColor = diffuseColour * d;
};