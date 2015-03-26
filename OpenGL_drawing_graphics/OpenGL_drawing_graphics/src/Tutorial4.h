#ifndef _FBX_LOADING_H_
#define _FBX_LOADING_H_
#include "GLprogram.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FBXFile.h"

class ShaderLoader;

class Tutorial4 : public GLprogram
{
protected:

	ShaderLoader* shaderLoader;
	FBXFile* fbxID;

	void CreateOpenGLBuffers(FBXFile* _fbx);
	void CleanupOpenGLBuffers(FBXFile* _fbx);

	void CreateShaders();
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();
};
#endif