#pragma once
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "GLprogram.h"

class ShaderLoader;

struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
};

class Tutorial2 : public GLprogram
{
protected:

	ShaderLoader* shaderLoader;

	unsigned int gridRows;
	unsigned int gridColumns;
public:

	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

	void GenerateGrid(unsigned int _rows, unsigned int _columns);
};