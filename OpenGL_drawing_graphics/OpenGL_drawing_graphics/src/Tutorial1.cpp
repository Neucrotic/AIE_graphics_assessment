#include "Tutorial1.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "MobileCamera.h"

using namespace std;

void Tutorial1::Startup()
{

	MobileCamera* camera = new MobileCamera();
	camera->SetInputWindow(window);

	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);

	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;
	
	return;
}

bool Tutorial1::Update(double dt)
{
	return true;
}

void Tutorial1::Render()
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

}

void Tutorial1::Shutdown()
{
	return;
}