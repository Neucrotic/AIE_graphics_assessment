#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "MobileCamera.h"
#include "GPUParticleEmitter.h"
#include <Gizmos.h>

void ParticleSystem::Startup()
{
	MobileCamera* camera = new MobileCamera();
	camera->SetInputWindow(window);

	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);

	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;

	//emitter = new ParticleEmitter();
	//emitter->Init(1000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));

	//loop through array and initiliaze emitters
	grassEmitter = new GPUParticleEmitter();
	grassEmitter->Init(3000, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 0.75, 0, 1));
	grassEmitter->CreateUpdateShader("data/shaders/particles/GPUparticleUpdateGrass.vert");
	grassEmitter->SetOrigin(glm::vec3(-10, 0, -10));

	rainEmitter = new GPUParticleEmitter();
	rainEmitter->Init(50, 1.5f, 1.0f, -800, -1000, 1, 0.5, glm::vec4(0, 0, 0.5f, 1), glm::vec4(0, 0, 1, 1));
	rainEmitter->CreateUpdateShader("data/shaders/particles/GPUparticleUpdateRain.vert");
	rainEmitter->SetOrigin(glm::vec3(-10, 10, -10));

	cloudEmitter = new GPUParticleEmitter();
	cloudEmitter->Init(2000, 0.5f, 1.0f, 0.25f, 0.5f, 1.0f, 0.5f, glm::vec4(1, 1, 1, 1), glm::vec4(0.5f, 0.5f, 0.5f, 0));
	cloudEmitter->CreateUpdateShader("data/shaders/particles/GPUparticleUpdateCloud.vert");
	cloudEmitter->SetOrigin(glm::vec3(-10, 10, -10));
}

bool ParticleSystem::Update(double _dt)
{
	//emitter->Update(_dt, m_camera->GetWorldTransform());
	return true;
}

void ParticleSystem::Render()
{
	//DOESN'T USE GPU
	//glUseProgram(emitter->programID);
	//int location = glGetUniformLocation(emitter->programID, "projectionView");
	//glUniformMatrix4fv(location, 1, GL_FALSE, &m_camera->GetProjectionView()[0][0]);
	//emitter->Draw();

	//drawing a grid
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

	//USES GPU
	grassEmitter->Draw((float)glfwGetTime(), m_camera->GetWorldTransform(), m_camera->GetProjectionView());
	rainEmitter->Draw((float)glfwGetTime(), m_camera->GetWorldTransform(), m_camera->GetProjectionView());
	cloudEmitter->Draw((float)glfwGetTime(), m_camera->GetWorldTransform(), m_camera->GetProjectionView());
}

void ParticleSystem::Shutdown()
{

}