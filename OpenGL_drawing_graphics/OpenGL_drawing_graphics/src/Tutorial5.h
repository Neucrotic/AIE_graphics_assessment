#include "GLprogram.h"

class ShaderLoader;
class MyObjLoader;

class Tutorial5 : public GLprogram
{
public:

	//frame buffer handlers
	unsigned int FBO;
	unsigned int fboTexture;
	//fbo depth buffer handler
	unsigned int fboDepth;

	ShaderLoader* shaderLoader;
	MyObjLoader* objLoader;

protected:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

};