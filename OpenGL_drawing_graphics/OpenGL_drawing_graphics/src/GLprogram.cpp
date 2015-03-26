#include "GLprogram.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GameTime.h"
#include "Camera.h"
#include "GameTime.h"
#include <Gizmos.h>

using namespace std;

int GLprogram::Run()
{
	if (Init() != 0)
		return 1;

	deltaTime = GameTime::GetDeltaTime();
	Startup();

	while (glfwWindowShouldClose(window) == false &&
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Gizmos::addTransform(glm::mat4(1));

		glm::vec4 white(1);
		glm::vec4 black(0, 0, 0, 1);

		for (int i = 0; i < 21; ++i)
		{
			Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
				glm::vec3(-10 + i, 0, -10),
				i == 10 ? white : black);

			Gizmos::addLine(glm::vec3(10, 0, -10 + i),
				glm::vec3(-10, 0, -10 + i),
				i == 10 ? white : black);
		}

		GameTime::Update();
		deltaTime = GameTime::GetDeltaTime();

		m_camera->Update(deltaTime);
		
		Update(deltaTime);
		
		
		Gizmos::clear();
		Render();
		Gizmos::draw(m_camera->GetProjectionView());

		TwDraw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Deinit();

	return 0;
}

#pragma region Tw glfw Overwrite
void OnMouseButton(GLFWwindow*, int _b, int _a, int _m)
{
	TwEventMouseButtonGLFW(_b, _a);
}

void OnMousePosition(GLFWwindow*, double _x, double _y)
{
	TwEventMousePosGLFW((int)_x, (int)_y);
}

void OnMouseScroll(GLFWwindow*, double _x, double _y)
{
	TwEventMouseWheelGLFW((int)_y);
}

void OnKey(GLFWwindow*, int _k, int _s, int _a, int _m)
{
	TwEventKeyGLFW(_k, _a);
}

void OnChar(GLFWwindow*, unsigned int _c)
{
	TwEventCharGLFW(_c, GLFW_PRESS);
}

void OnWindowResize(GLFWwindow*, int _w, int _h)
{
	TwWindowSize(_w, _h);
	glViewport(0, 0, _w, _h);
}
#pragma endregion

int GLprogram::Init()
{
	if (glfwInit() == false)
		return -1;

	window = glfwCreateWindow(1280, 720, "my graphics", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();

		return -2;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);


	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST); //depth buffer now enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Gizmos::create();

	//tweak bar initialisation
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	
	tw_ClearColour = glm::vec4(0.0f, 0.50f, 0.0f, 1.0f);
	tw_Light = glm::vec3(0, 1, 0);

	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePosition);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCharCallback(window, OnChar);
	glfwSetWindowSizeCallback(window, OnWindowResize);
	
	tweakBar = TwNewBar("debug_GUI");
	TwAddVarRW(tweakBar, "clear colour", TW_TYPE_COLOR4F, &tw_ClearColour[0], ""); //changing bg colour
	TwAddVarRW(tweakBar, "light direction", TW_TYPE_DIR3F, &tw_Light[0], "group=light");

	return 0;
}

int GLprogram::Deinit()
{
	Gizmos::destroy();

	TwDeleteAllBars();
	TwTerminate();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

GLFWwindow* GLprogram::GetWindow()
{
	return window;
}