#include "Scene.h"
#include "MobileCamera.h"
#include "TextureLoader.h"
#include "MyObjLoader.h"

void Scene::Startup()
{
	//camera initialisation
	camera = new MobileCamera();
	camera->SetInputWindow(window);

	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);

	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//loaders initialisation
	//textureLoader = new TextureLoader();
}

bool Scene::Update(double _dt)
{
	return true;
}

void Scene::Render()
{

}

void Scene::Shutdown()
{

}