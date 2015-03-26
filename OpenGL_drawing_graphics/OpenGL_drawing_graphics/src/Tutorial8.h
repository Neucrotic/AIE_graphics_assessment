#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "GLprogram.h"

class ShaderLoader;

class Tutorial8 : public GLprogram
{
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

private:

	ShaderLoader* shaderLoader;

	unsigned int FBO;
	unsigned int fboDepth;
	unsigned int fboTexture;

	void SetupRenderTarget();
	void CreateShaders();
	void CreateUpdateShader();
};