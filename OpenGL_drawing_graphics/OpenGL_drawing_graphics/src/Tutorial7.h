#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FBXFile.h"
#include "GLprogram.h"

using namespace std;

class ShaderLoader;
class TextureLoader;

class Tutorial7 : public GLprogram
{
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

private:

	FBXFile* fbxModel;
	ShaderLoader* shaderLoader;
	TextureLoader* textureLoader;

	float timer;
	unsigned int updateShader;

	void CreateOpenglBuffers(FBXFile* _model);
	void CleanupOpenglBuffers(FBXFile* _model);
	
	void CreateShaders();
	void CreateUpdateShader();
};