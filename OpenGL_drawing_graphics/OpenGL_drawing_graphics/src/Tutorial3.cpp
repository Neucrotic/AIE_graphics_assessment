#include "Tutorial3.h"
#include "MobileCamera.h"

void Tutorial3::Startup()
{
	MobileCamera* camera = new MobileCamera();
	camera->SetInputWindow(window);

	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);

	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;

	objLoader = new MyObjLoader("data/objects/basic_sword_model.obj");

	return;
}

bool Tutorial3::Update(double dt)
{
	return true;
}

void Tutorial3::Render()
{
	glUseProgram(objLoader->programID);

	//initilising location int and passing through projectionView unform
	int location = glGetUniformLocation(objLoader->programID, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &m_camera->GetProjectionView()[0][0]);
	//passing through uniform for light direction
	location = glGetUniformLocation(objLoader->programID, "LightDir");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 0.5f, 0)));
	//passing through uniform for light colour
	location = glGetUniformLocation(objLoader->programID, "LightColour");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
	//passing through unifrm for camera position
	location = glGetUniformLocation(objLoader->programID, "CameraPos");
	glUniform3fv(location, 1, glm::value_ptr(m_camera->GetPosition()));
	//passing through uniform for specular power
	location = glGetUniformLocation(objLoader->programID, "SpecPower");
	glUniform1f(location, 128);

	for (unsigned int i = 0; i < objLoader->GetGLInfo().size(); ++i)
	{
		glBindVertexArray(objLoader->GetGLInfo()[i].VAO);
		glDrawElements(GL_TRIANGLES, objLoader->GetGLInfo()[i].IndexCount, GL_UNSIGNED_INT, 0);
	}
}

void Tutorial3::Shutdown()
{

}