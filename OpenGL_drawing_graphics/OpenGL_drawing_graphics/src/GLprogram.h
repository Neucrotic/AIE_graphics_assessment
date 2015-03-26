#pragma once
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "AntTweakBar.h"

class Camera;

class GLprogram
{
public:
	GLFWwindow* GetWindow();

	int Run();

	virtual void Startup() = 0;
	virtual void Shutdown() = 0;

	virtual bool Update(double dt) = 0;
	virtual void Render() = 0;

	inline unsigned int GetProgramID() { return s_programID; }
protected:
	//index and vertex buffers
	struct Buffers
	{
		unsigned int VAO; //vertex array object
		unsigned int VBO; //vertex buffer object
		unsigned int IBO; //index buffer object
	};

	GLFWwindow* window;
	glm::vec4 tw_ClearColour; //used to change the bg colour in runtime
	glm::vec3 tw_Light;

	Camera* m_camera;
	double deltaTime;
	Buffers* buffers;

	//shader progrma ID
	unsigned int s_programID;

	//texture ID
	unsigned int t_textureID;

	TwBar* tweakBar;

	int Init();
	int Deinit();
};