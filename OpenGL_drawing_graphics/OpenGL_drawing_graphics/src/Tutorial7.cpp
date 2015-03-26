#include "Tutorial7.h"
#include "MobileCamera.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"

void Tutorial7::Startup()
{
	MobileCamera* camera = new MobileCamera(1000.0f);
	camera->SetInputWindow(window);
	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	fbxModel = new FBXFile();
	fbxModel->load("data/models/characters/Pyro/pyro.fbx");
	timer = 2.0f;

	shaderLoader = new ShaderLoader();

	//textureLoader = new TextureLoader();
	//textureLoader->LoadTexture((string)"data/models/Pyro/Pyro_D.tga");

	buffers = new Buffers();

	CreateOpenglBuffers(fbxModel);
	CreateShaders();
	CreateUpdateShader();
}

void Tutorial7::Shutdown()
{
	CleanupOpenglBuffers(fbxModel);

	glDeleteProgram(s_programID);
}

bool Tutorial7::Update(double dt)
{
	//grab the skeleton and animation we want to use
	FBXSkeleton* skeleton = fbxModel->getSkeletonByIndex(0);
	FBXAnimation* animation = fbxModel->getAnimationByIndex(0);

	//evaluate the animation to update bones
	skeleton->evaluate(animation, timer);

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}

	return true;
}

void Tutorial7::Render()
{
	FBXSkeleton* skeleton = fbxModel->getSkeletonByIndex(0);
	skeleton->updateBones();

	int bones_location = glGetUniformLocation(s_programID, "bones");
	glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	/*
	glUseProgram(s_programID);
	int location = glGetUniformLocation(s_programID, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));

	for (unsigned int i = 0; i < fbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxModel->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}*/
}

void Tutorial7::CreateOpenglBuffers(FBXFile* _model)
{
	for (unsigned int i = 0; i < _model->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = _model->getMeshByIndex(i);

		//buffer objects
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
		glEnableVertexAttribArray(1); //normal
		glEnableVertexAttribArray(2); //tangents
		glEnableVertexAttribArray(3); //texcoords
		glEnableVertexAttribArray(4); //weights
		glEnableVertexAttribArray(5); //indices

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Tutorial7::CleanupOpenglBuffers(FBXFile* _model)
{
	for (unsigned int i = 0; i < _model->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = _model->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void Tutorial7::CreateShaders()
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

	delete source;
}

void Tutorial7::CreateUpdateShader()
{
	string* source;

	source = shaderLoader->LoadFile("data/shader/animationShader.vert");
	const char* vsSource = source->c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	updateShader = glCreateProgram();
	glAttachShader(updateShader, vertexShader);
	glLinkProgram(updateShader);

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

	delete source;

	/*glEnableVertexAttribArray(0); //position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

	glEnableVertexAttribArray(1); //normal
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //normal
	glEnableVertexAttribArray(2); //tangents
	glEnableVertexAttribArray(3); //texcoords
	glEnableVertexAttribArray(4); //weights
	glEnableVertexAttribArray(5); //indices

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);*/
}