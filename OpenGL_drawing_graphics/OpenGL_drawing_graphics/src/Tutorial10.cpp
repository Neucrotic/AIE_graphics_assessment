#include "Tutorial10.h"
#include "MobileCamera.h"
#include "ShaderLoader.h"

void Tutorial10::Startup()
{
	MobileCamera* camera = new MobileCamera(100.0f);
	camera->SetInputWindow(window);
	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	shaderLoader = new ShaderLoader();

	buffers = new Buffers();

	CreateShaders();

	GenerateHeightMap(64, 64, glm::vec3(1, 1, 1));
}

void Tutorial10::Shutdown()
{

}

bool Tutorial10::Update(double _dt)
{
	return true;
}

void Tutorial10::Render()
{
	glUseProgram(s_programID);
	int projectionViewUniform = glGetUniformLocation(s_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_camera->GetProjectionView()));
	
	int location = glGetUniformLocation(s_programID, "Texcoord");
	glUniform1i(location, 0);

	location = glGetUniformLocation(s_programID, "perlinTexture");
	glUniform1i(location, 0);
	
	glBindVertexArray(buffers->VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, perlinTexture);

	unsigned int indexCount = (gridRows - 1) * (gridColumns - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Tutorial10::GenerateHeightMap(unsigned int _rows, unsigned int _columns, glm::vec3 _colour)
{
	Vertex* vertices = new Vertex[_rows * _columns];
	for (unsigned int r = 0; r < _rows; ++r)
	{
		for (unsigned int c = 0; c < _columns; ++c)
		{
			vertices[r * _columns + c].position = glm::vec4((float)c, 0, (float)r, 1);

			glm::vec3 colour = _colour;

			vertices[r * _columns + c].colour = glm::vec4(colour, 1);
			vertices[r * _columns + c].uv = glm::vec2((float)c / _columns, (float)r / _rows);
		}
	}

	//defining index count based of quad count (2 tri's = 1 quad)
	unsigned int* indices = new unsigned int[(_rows - 1) * (_columns - 1) * 6];
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

	////generating perlin data
	float* perlinData = GeneratePerlinValues(64, 8);

	//generating noise texture
	glGenTextures(1, &perlinTexture);
	glBindTexture(GL_TEXTURE_2D, perlinTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, perlinData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) + sizeof(glm::vec4)));

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

float* Tutorial10::GeneratePerlinValues(int _dimensions, float _scale)
{
	float* perlin_data = new float[_dimensions * _dimensions];
	float scale = (1.0f / _dimensions) * _scale;
	int octaves = 6;

	for (int x = 0; x < 64; ++x)
	{
		for (int y = 0; y < 64; ++y)
		{
			float amplitude = 1.0f;
			float persistance = 0.3f;
			perlin_data[y * _dimensions + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlinSample = glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;

				perlin_data[y * _dimensions + x] += perlinSample * amplitude;
				amplitude *= persistance;
			}
		}
	}

	return perlin_data;
}

void Tutorial10::CreateShaders()
{
	string* source;
	
	source = shaderLoader->LoadFile("data/shaders/heightMap.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/heightMap.frag");
	const char* fsSource = source->c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

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

	delete source;
}

