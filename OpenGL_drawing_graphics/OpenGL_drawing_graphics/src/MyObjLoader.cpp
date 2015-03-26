#include "MyObjLoader.h"
#include "MobileCamera.h"
#include "GLprogram.h"
#include "ShaderLoader.h"

MyObjLoader::MyObjLoader(const char* _filename)
{
	Init(_filename);
}

MyObjLoader::~MyObjLoader()
{
	Deinit();
}

void MyObjLoader::Init(const char* _filename)
{
	shaderLoader = new ShaderLoader();
	Load(_filename);
	CreateShaders();
	CreateOpenGLBuffers(shapes);
	return;
}

void MyObjLoader::Deinit()
{
	glDeleteProgram(programID);
}

void MyObjLoader::Load(const char* _fileName)
{
	string error = tinyobj::LoadObj(shapes, materials, _fileName);
}

void MyObjLoader::CreateOpenGLBuffers(vector<tinyobj::shape_t>& _shapes)
{
	glInfo.resize(_shapes.size());

	for (unsigned int meshIndex = 0; meshIndex < _shapes.size(); ++meshIndex)
	{
		//generating biffers and binding vertrex array
		glGenVertexArrays(1, &glInfo[meshIndex].VAO);
		glGenBuffers(1, &glInfo[meshIndex].IBO);
		glGenBuffers(1, &glInfo[meshIndex].VBO);
		glBindVertexArray(glInfo[meshIndex].VAO);

		unsigned int floatCount = _shapes[meshIndex].mesh.positions.size();
		floatCount += _shapes[meshIndex].mesh.normals.size();
		floatCount += _shapes[meshIndex].mesh.texcoords.size();

		vector<float> vertexData;
		vertexData.reserve(floatCount);

		//populating vertexData with the mesh positions follow by their normals 
		vertexData.insert(
			vertexData.end(),
			_shapes[meshIndex].mesh.positions.begin(), 
			_shapes[meshIndex].mesh.positions.end()
						 );

		vertexData.insert(
			vertexData.end(),
			_shapes[meshIndex].mesh.normals.begin(),
			_shapes[meshIndex].mesh.normals.end()
			);

		glInfo[meshIndex].IndexCount = _shapes[meshIndex].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, glInfo[meshIndex].VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glInfo[meshIndex].IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					 _shapes[meshIndex].mesh.indices.size() * sizeof(unsigned int), 
					 _shapes[meshIndex].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position data
		glEnableVertexAttribArray(1); //normals data

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)* _shapes[meshIndex].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void MyObjLoader::CreateShaders()
{
	string* source;

	source = shaderLoader->LoadFile("data/shaders/ObjectLoader_vertShader.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/ObjectLoader_fragShader.frag");
	const char* fsSource = source->c_str();

	int success = GL_FALSE;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == FALSE)
	{
		printf("vertex shader didnt compile");

	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}