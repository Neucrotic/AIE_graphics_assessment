#pragma once
#include "MyObjLoader.h"
#include "GLprogram.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class MyObjLoader;

class Tutorial3 : public GLprogram
{
private:

	MyObjLoader* objLoader;
protected:
	
	void Startup();
	void Shutdown();
	bool Update(double dt);
	void Render();
};