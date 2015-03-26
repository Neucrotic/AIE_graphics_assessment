#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;

//view-space normal and position
out vec4 vPosition;
out vec4 vNormal;

uniform mat4 View //needed in addition to Projection * View
uniform mat4 ProjectionView;

void main()
{
	//first view-space position and normal is stored
	vPosition = View * Position;
	vNormal = normalize(View * Normal);
	
	gl_Position = ProjectionView * Position;
}