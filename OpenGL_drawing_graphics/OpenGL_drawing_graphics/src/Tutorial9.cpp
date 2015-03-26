#include "Tutorial9.h"
#include "MobileCamera.h"
#include "ShaderLoader.h"

void Tutorial9::Startup()
{
	MobileCamera* camera = new MobileCamera(100.0f);
	camera->SetInputWindow(window);
	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	buffers = new Buffers();

	shaderLoader = new ShaderLoader();

	fbxModel = new FBXFile();
	fbxModel->load("data/models/Bunny.fbx");

	//shader initialization
	SetupRenderTarget();
	SetupGBufferTarget();
	SetupLightBufferTarget();

	//render target initialization
	SetupRenderTarget();
	SetupGBufferTarget();
	SetupLightBufferTarget();
}

void Tutorial9::Shutdown()
{

}

bool Tutorial9::Update(double dt)
{
	return true;
}

void Tutorial9::Render()
{
	//G pass: render out the albedo, position and normal
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, gPassFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gPassShader);

	//binding camera transforms
	int location = glGetUniformLocation(gPassShader, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));
	
	location = glGetUniformLocation(gPassShader, "View");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(m_camera->GetView()[0][0]));

	//draw our scene - bunny fbx for now
	for (unsigned int i = 0; i < fbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxModel->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	//Light pass: render lights as geometry, sampling position and normals
	//disable depth testing and enable additive blending
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(directionLightShader);

	location = glGetUniformLocation(directionLightShader, "positionTexture");
	glUniform1i(location, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);

	//draw light as fullscreen quad
	DrawDirectionalLight(glm::vec3(-1), glm::vec3(-1));

	glDisable(GL_BLEND);

	//Composite pass: render a quad and combine albedo and light
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(compositeShader);

	location = glGetUniformLocation(compositeShader, "albedoTexture");
	glUniform1i(location, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoTexture);

	location = glGetUniformLocation(compositeShader, "lightTexture");
	glUniform1i(location, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightTexture);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Tutorial9::CreateGPassShaders()
{
	string* source;

	source = shaderLoader->LoadFile("data/shaders/deferredRendering/gBuffer.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/deferredRendering/gBuffer.frag");
	const char* fsSource = source->c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	gPassShader = glCreateProgram();
	glAttachShader(gPassShader, vertexShader);
	glAttachShader(gPassShader, fragmentShader);
	glLinkProgram(gPassShader);

	int success = false;
	glGetProgramiv(gPassShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(gPassShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(gPassShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	delete source;
	delete vsSource;
	delete fsSource;
}

void Tutorial9::CreatedLightShaders()
{
	string* source;

	source = shaderLoader->LoadFile("data/shaders/deferredRendering/postProcessing.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/deferredRendering/directionalLight.frag");
	const char* fsSource = source->c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	directionLightShader = glCreateProgram();
	glAttachShader(directionLightShader, vertexShader);
	glAttachShader(directionLightShader, fragmentShader);
	glLinkProgram(directionLightShader);

	int success = false;
	glGetProgramiv(directionLightShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(directionLightShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(directionLightShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	delete source;
	delete vsSource;
	delete fsSource;
}

void Tutorial9::CreateCompositeShaders()
{
	string* source;

	source = shaderLoader->LoadFile("data/shaders/deferredRendering/compositeShader.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/deferredRendering/compositeShader.frag");
	const char* fsSource = source->c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	compositeShader = glCreateProgram();
	glAttachShader(compositeShader, vertexShader);
	glAttachShader(compositeShader, fragmentShader);
	glLinkProgram(compositeShader);

	int success = false;
	glGetProgramiv(compositeShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(compositeShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(compositeShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	delete source;
	delete vsSource;
	delete fsSource;
}

void Tutorial9::SetupRenderTarget()
{
	//set up framebuffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTexture, 0);

	glGenRenderbuffers(1, &fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//fullscreen quad
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f;

	float vertexData[] =
	{
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,

		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y
	};

	glGenVertexArrays(1, &buffers->VAO);
	glBindVertexArray(buffers->VAO);

	glGenBuffers(1, &buffers->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Tutorial9::SetupGBufferTarget()
{
	//setup gpass framebuffer
	glGenFramebuffers(1, &gPassFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gPassFBO);

	glGenTextures(1, &albedoTexture);
	glBindTexture(GL_TEXTURE_2D, albedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &positionTexture);
	glBindTexture(GL_TEXTURE_2D, positionTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &gPassDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, gPassDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, albedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, positionTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, normalTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gPassDepth);

	GLenum gpassTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, gpassTargets);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tutorial9::SetupLightBufferTarget()
{
	glGenFramebuffers(1, &lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);

	glGenTextures(1, &lightTexture);
	glBindTexture(GL_TEXTURE_2D, lightTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightTexture, 0);

	GLenum lightTargets[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, lightTargets);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tutorial9::DrawDirectionalLight(const glm::vec3& _direction, const glm::vec3& _diffuse)
{
	glm::vec4 viewSpaceLight = m_camera->GetView() * glm::vec4(glm::normalize(_direction), 0);
	
	int loc = glGetUniformLocation(directionLightShader, "lightDirection");
	glUniform3fv(loc, 1, &viewSpaceLight[0]);

	loc = glGetUniformLocation(directionLightShader, "lightDiffuse");
	glUniform3fv(loc, 1, &_diffuse[0]);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}