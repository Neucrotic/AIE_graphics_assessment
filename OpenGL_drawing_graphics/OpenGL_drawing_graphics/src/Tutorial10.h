#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "GLprogram.h"

class Tutorial10 : public GLprogram
{
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

private:



};