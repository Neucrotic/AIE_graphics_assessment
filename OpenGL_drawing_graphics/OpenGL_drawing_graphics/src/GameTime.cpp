#include "GameTime.h"
#include <GLFW\glfw3.h>

double GameTime::previousTime = 0;
double GameTime::deltaTime = 0;

void GameTime::Update()
{
	double currentTime = glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;
}

double GameTime::GetDeltaTime()
{
	return deltaTime;
}