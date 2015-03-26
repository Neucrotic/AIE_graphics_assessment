#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FBXFile.h"
#include "GLprogram.h"

using namespace std;

class ShaderLoader;

class Tutorial6 : public GLprogram
{
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

private:

	FBXFile* fbxModel;
	ShaderLoader* shaderLoader;

	unsigned int FBO;
	unsigned int fboDepth;

	unsigned int shadowGenprogram;
	unsigned int useShadowProgram;

	glm::vec3 lightDirection;
	glm::mat4 lightMatrix;

	void CreateOpenGLBuffers(FBXFile* _model);
	void CleanupOpenGLBuffers(FBXFile* _model);

	void SetUpShaders();
	void SetUpPlane();

	void CreateShadowMap();
};