#pragma once
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "AntTweakBar.h"
#include "GLprogram.h"

class MyObjLoader;
class TextureLoader;
class MobileCamera;

class Scene : public GLprogram
{	
protected:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

	MobileCamera* camera;
	TextureLoader* textureLoader;
	MyObjLoader* objectloader;
public:
	

};