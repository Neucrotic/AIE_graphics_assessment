#include "Tutorial6.h"
#include "MobileCamera.h"
#include "ShaderLoader.h"

void Tutorial6::Startup()
{
	MobileCamera* camera = new MobileCamera();
	camera->SetInputWindow(window);
	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	fbxModel = new FBXFile();
	fbxModel->load("data/models/Bunny.fbx");

	shaderLoader = new ShaderLoader();

	buffers = new Buffers();

	SetUpPlane();
	SetUpShaders();

	CreateShadowMap();
	CreateOpenGLBuffers(fbxModel);
}

bool Tutorial6::Update(double dt)
{
	return true;
}

void Tutorial6::Render()
{
	//rendering the shadow map
	//shadow pass binding shadow map target and clear the depth
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shadowGenprogram);

	//binding light matrix
	int location = glGetUniformLocation(shadowGenprogram, "LightMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(lightMatrix[0][0]));

	//draw all shadow-casting geometry
	for (unsigned int i = 0; i < fbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxModel->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	//final pass: bind back-buffer and clear colour depth
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(useShadowProgram);

	//bind the camera
	location = glGetUniformLocation(useShadowProgram, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));

	//bringing the light matrix from clip to texture space
	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f);

	glm::mat4 lightMatrix = textureSpaceOffset * Tutorial6::lightMatrix;

	location = glGetUniformLocation(useShadowProgram, "LightMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE, &lightMatrix[0][0]);

	location = glGetUniformLocation(useShadowProgram, "lightDir");
	glUniform3fv(location, 1, &lightDirection[0]);

	location = glGetUniformLocation(useShadowProgram, "shadowMap");
	glUniform1i(location, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboDepth);

	//binding VAO and drawing mesh
	for (unsigned int i = 0; i < fbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxModel->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	//drawing the plane under the fbxModel
	glBindVertexArray(buffers->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Tutorial6::Shutdown()
{
	CleanupOpenGLBuffers(fbxModel);

	glDeleteProgram(s_programID);
}

void Tutorial6::CreateOpenGLBuffers(FBXFile* _model)
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
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); //normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Tutorial6::CleanupOpenGLBuffers(FBXFile* _model)
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

void Tutorial6::SetUpShaders()
{
	string* source;

	//loading useShadowProgram shaders
	source;

	source = shaderLoader->LoadFile("data/shaders/shadowProgram.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/shadowProgram.frag");
	const char*fsSource = source->c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	useShadowProgram = glCreateProgram();
	glAttachShader(useShadowProgram, vertexShader);
	glAttachShader(useShadowProgram, fragmentShader);
	glLinkProgram(useShadowProgram);

	int success = false;
	glGetProgramiv(useShadowProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(useShadowProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(useShadowProgram, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//loading shadowGenProgram shaders
	source;

	source = shaderLoader->LoadFile("data/shaders/shadowMapRender.vert");
	vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/shadowMapRender.frag");
	fsSource = source->c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	shadowGenprogram = glCreateProgram();
	glAttachShader(shadowGenprogram, vertexShader);
	glAttachShader(shadowGenprogram, fragmentShader);
	glLinkProgram(shadowGenprogram);

	success = false;
	glGetProgramiv(shadowGenprogram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(shadowGenprogram, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(shadowGenprogram, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	delete source;

}

void Tutorial6::SetUpPlane()
{
	//used for rendering a plane
	float vertexData[] = 
	{
		-10, 0, -10, 1, 0, 0,
		10, 0, -10, 1, 1, 0,
		10, 0, 10, 1, 1, 1,
		-10, 0, 10, 1, 0, 1
	};
	
	unsigned int indexData[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &buffers->VAO);
	glBindVertexArray(buffers->VAO);
	glGenBuffers(1, &buffers->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &buffers->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Tutorial6::CreateShadowMap()
{
	//set up shadow map buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &fboDepth);
	glBindTexture(GL_TEXTURE_2D, fboDepth);

	//texture uses a 16-bit depth component format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attatch as a depth attatchment to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fboDepth, 0);

	//specify no colour targets being used
	glDrawBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//settig up the lighting
	lightDirection = glm::normalize(glm::vec3(1, 2.5f, 1));

	glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);

	glm::mat4 lightView = glm::lookAt(lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));

	lightMatrix = lightProjection * lightView;
}