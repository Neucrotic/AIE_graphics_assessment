#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "GLprogram.h"
#include "FBXFile.h"

class ShaderLoader;

class Tutorial9 : public GLprogram
{
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

private:

	ShaderLoader* shaderLoader;

	FBXFile* fbxModel;

	unsigned int quadVAO;

	//frame buffer handles
	unsigned int FBO;
	unsigned int fboDepth;
	unsigned int fboTexture;

	//deferred rendering-specific buffer handles
	unsigned int gPassFBO;
	unsigned int gPassDepth;
	unsigned int albedoTexture;
	unsigned int positionTexture;
	unsigned int normalTexture;

	//light framebuffer handles
	unsigned int lightFBO;
	unsigned int lightTexture;

	//shader program handles
	unsigned int gPassShader;
	unsigned int directionLightShader;
	unsigned int compositeShader;

	//shader creation functions
	void CreateGPassShaders();
	void CreatedLightShaders();
	void CreateCompositeShaders();

	//buffer creation functions
	void SetupRenderTarget();
	void SetupGBufferTarget();
	void SetupLightBufferTarget();

	void DrawDirectionalLight(const glm::vec3& _direction, const glm::vec3& _diffuse);

};