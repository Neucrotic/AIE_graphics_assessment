#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "GLprogram.h"

class ShaderLoader;



class Tutorial10 : public GLprogram
{
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

private:

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
		glm::vec2 uv;
	};
	
	ShaderLoader* shaderLoader;

	unsigned int gridRows;
	unsigned int gridColumns;

	unsigned int perlinTexture;

	void GenerateHeightMap(unsigned int _rows, unsigned int _columns, glm::vec3 _colour);
	float* GeneratePerlinValues(int _dimensions, float _scale);

	void CreateShaders();
};