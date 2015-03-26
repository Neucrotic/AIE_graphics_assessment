#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "GLprogram.h"

class GLprogram;

class Tutorial1 : public GLprogram
{
private:
	glm::mat4 projection;
	glm::mat4 view;

public:
	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();
};