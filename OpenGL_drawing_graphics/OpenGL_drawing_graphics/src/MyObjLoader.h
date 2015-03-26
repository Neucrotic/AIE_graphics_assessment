#pragma once
#include "tiny_obj_loader.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class ShaderLoader;

using namespace std;

class MyObjLoader
{
private:
	
	struct OpenGLInfo
	{
		unsigned int VAO; //vertex array object
		unsigned int VBO; //vertex buffer object
		unsigned int IBO; //index buffer object
		unsigned int IndexCount;
	};
protected:
	
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	vector<OpenGLInfo> glInfo;

	
	ShaderLoader* shaderLoader;

	void Init(const char* _filename);
	void Deinit();
	
	void CreateShaders();
	void CreateOpenGLBuffers(vector<tinyobj::shape_t>& _shapes);
public:
	
	MyObjLoader(const char* _filename);
	~MyObjLoader();

	unsigned int programID;
	
	void Load(const char* _fileName);
	inline vector<OpenGLInfo> GetGLInfo() { return glInfo; }
};