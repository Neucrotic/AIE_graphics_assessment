#include "Tutorial2.h"
#include <gl_core_4_4.h>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderLoader.h"

#include "MobileCamera.h"

void Tutorial2::Startup()
{
	MobileCamera* camera = new MobileCamera();
	camera->SetInputWindow(window);

	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);

	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;

	buffers = new Buffers();

	//create shaders
	const char* vsSource = "#version 410\n \
						    layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Colour; \
							out vec4 vColour; \
							uniform mat4 ProjectionView; \
							uniform float time; \
							uniform float heightScale; \
							void main() { vColour = Colour; \
							vec4 P = Position; \
							P.y += sin( time + Position.x) * heightScale; \
							gl_Position = ProjectionView * P; }";


	const char* fsSource = "#version 410\n \
						   	in vec4 vColour;\
							out vec4 FragColor; \
							void main() { FragColor = vColour; }";

	//compiling shaders
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	//creating shader program
	s_programID = glCreateProgram();
	glAttachShader(s_programID, vertexShader);
	glAttachShader(s_programID, fragmentShader);
	glLinkProgram(s_programID);

	int success = GL_FALSE;
	glGetProgramiv(s_programID, GL_LINK_STATUS, &success);
	if (success = GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(s_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(s_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	GenerateGrid(20, 20);

	return;
}

bool Tutorial2::Update(double _dt)
{
	return true;
}

void Tutorial2::Render()
{	
	glUseProgram(s_programID);
	int projectionViewUniform = glGetUniformLocation(s_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_camera->GetProjectionView()));

	glBindVertexArray(buffers->VAO);
	unsigned int indexCount = (gridRows - 1) * (gridColumns - 1) * 6;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	
}

void Tutorial2::Shutdown()
{
	return;
}

void Tutorial2::GenerateGrid(unsigned int _rows, unsigned int _columns)
{
	Vertex* vertices = new Vertex[_rows * _columns];
	for (unsigned int r = 0; r < _rows; ++r)
	{
		for (unsigned int c = 0; c < _columns; ++c)
		{
			vertices[r * _columns + c].position = glm::vec4((float)c, 0, (float)r, 1);

			glm::vec3 colour = glm::vec3(sinf((c / (float)(_columns - 1))
				* (r / (float)(_rows - 1))));

			vertices[r * _columns + c].colour = glm::vec4(colour, 1);
		}
	}

	//defining index count based of quad count (2 tri's = 1 quad)
	unsigned int* indices = new unsigned int[(_rows - 1) * (_columns - 1) * 6 ];
	unsigned int index = 0;
	
	for (unsigned int r = 0; r < (_rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (_columns - 1); ++c)
		{
			//tri 1
			indices[index++] = r * _columns + c;
			indices[index++] = (r + 1) * _columns + c;
			indices[index++] = (r + 1) * _columns + (c + 1);

			//tri 2
			indices[index++] = r * _columns + c;
			indices[index++] = (r + 1) * _columns + (c + 1);
			indices[index++] = r * _columns + (c + 1);
		}
	}
	
	//generating VAO, VBO and IBO
	glGenBuffers(1, &buffers->VBO);
	glGenBuffers(1, &buffers->IBO);
	
	glGenVertexArrays(1, &buffers->VAO);
	glBindVertexArray(buffers->VAO);

	//binding and filling VBO
	glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);
	glBufferData(GL_ARRAY_BUFFER, (_rows * _columns) * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	//binding and filling IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (_rows - 1) * (_columns - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//binding VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//save function parametres for member varibles for external use
	gridRows = _rows;
	gridColumns = _columns;

	delete[] vertices;
	delete[] indices;
}