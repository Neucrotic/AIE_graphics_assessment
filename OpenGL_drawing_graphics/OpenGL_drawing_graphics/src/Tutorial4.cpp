#include "Tutorial4.h"
#include "MobileCamera.h"
#include "ShaderLoader.h"

void Tutorial4::Startup()
{
	MobileCamera* camera = new MobileCamera();
	camera->SetInputWindow(window);

	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);

	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;

	shaderLoader = new ShaderLoader();

	fbxID = new FBXFile();
	fbxID->load("data/models/Pyro/pyro.fbx");
	CreateOpenGLBuffers(fbxID);

	CreateShaders();
}

bool Tutorial4::Update(double _dt)
{
	return true;
}

void Tutorial4::Render()
{
	glUseProgram(s_programID);

	//binding camera
	int location = glGetUniformLocation(s_programID, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));
	//passing through uniform for light direction
	location = glGetUniformLocation(s_programID, "LightDir");
	glUniform3fv(location, 1, glm::value_ptr(tw_Light));
	//passing through uniform for light colour
	location = glGetUniformLocation(s_programID, "LightColour");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
	//passing through unifrm for camera position
	location = glGetUniformLocation(s_programID, "CameraPos");
	glUniform3fv(location, 1, glm::value_ptr(m_camera->GetPosition()));
	//passing through uniform for specular power
	location = glGetUniformLocation(s_programID, "SpecPower");
	glUniform1f(location, 64);

	//binding the vertex array and drawing the mesh
	for (unsigned int i = 0; i < fbxID->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxID->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void Tutorial4::Shutdown()
{
	CleanupOpenGLBuffers(fbxID);

	glDeleteProgram(s_programID);
}

void Tutorial4::CreateOpenGLBuffers(FBXFile* _fbx)
{
	for (unsigned int i = 0; i < _fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = _fbx->getMeshByIndex(i);

		//storage for GLdata in 3 Uints 
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex),
									  mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int),
											  mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); //normals
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Tutorial4::CleanupOpenGLBuffers(FBXFile* _fbx)
{
	for (unsigned int i = 0; i < _fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = _fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void Tutorial4::CreateShaders()
{
	string* source;

	source = shaderLoader->LoadFile("data/shaders/fbxShader_vert.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/fbxShader_frag.frag");
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

	int success = false;
	glGetProgramiv(s_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(s_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(s_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}