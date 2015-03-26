#version 410
in vec4 vNormal; 
in vec4 vPosition;
out vec4 FragColor; 
uniform vec3 LightDir;
uniform vec3 LightColour;
uniform vec3 CameraPos;
uniform float SpecPower;
void main() 
{ 
	float d = max(0, dot(normalize(vNormal.xyz), LightDir));
	vec3 E = normalize(CameraPos - vPosition.xyz);
	vec3 R = reflect(-LightDir, vNormal.xyz);
	float s = max(0, dot(E, R));
	
	s = pow(s, SpecPower);
	FragColor = vec4(LightColour * d + LightColour * s, 1); 
};