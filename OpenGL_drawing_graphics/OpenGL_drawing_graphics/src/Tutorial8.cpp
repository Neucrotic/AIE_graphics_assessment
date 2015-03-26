#include "Tutorial8.h"
#include "MobileCamera.h"
#include "ShaderLoader.h"
#include <Gizmos.h>

void Tutorial8::Startup()
{
	MobileCamera* camera = new MobileCamera(100.0f);
	camera->SetInputWindow(window);
	camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	buffers = new Buffers();

	shaderLoader = new ShaderLoader();

	SetupRenderTarget();
	CreateShaders();
}

void Tutorial8::Shutdown()
{

}

bool Tutorial8::Update(double dt)
{
	return true;
}

void Tutorial8::Render()
{
	//bind the render target
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, 1280, 720);

	//clear target
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Gizmos::addTransform(glm::mat4(1));

	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
			glm::vec3(-10 + i, 0, -10),
			i == 10 ? white : black);

		Gizmos::addLine(glm::vec3(10, 0, -10 + i),
			glm::vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

	Gizmos::addSphere(glm::vec3(0, 5, 0), 5.0f, 5, 5, glm::vec4(1, 0, 0, 1), &m_camera->GetWorldTransform());

	Gizmos::addSphere(glm::vec3(0, 20, 0), 2.0f, 5, 5, glm::vec4(0, 1, 0, 1), &m_camera->GetWorldTransform());

	Gizmos::addSphere(glm::vec3(0, -15, 0), 4.0f, 5, 5, glm::vec4(0, 0, 1, 1), &m_camera->GetWorldTransform());


	//draw our 3D scene with gizmos for now
	Gizmos::draw(m_camera->GetProjectionView());

	//bind the back-buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	//just clear the back-buffer depth as each pixel will be filled
	glClear(GL_DEPTH_BUFFER_BIT);

	//draw out the full-screen
	glUseProgram(s_programID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboTexture);

	int location = glGetUniformLocation(s_programID, "target");
	glUniform1i(location, 0);

	glBindVertexArray(buffers->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Tutorial8::SetupRenderTarget()
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

void Tutorial8::CreateShaders()
{
	string* source;

	source = shaderLoader->LoadFile("data/shaders/postProcessing.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/postProcessing.frag");
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