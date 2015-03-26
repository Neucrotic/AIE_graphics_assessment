#include "Tutorial5.h"
#include "MobileCamera.h"
#include "ShaderLoader.h"
#include "MyObjLoader.h"

void Tutorial5::Startup()
{
#pragma region camerasAndBuffer
	MobileCamera* camera1 = new MobileCamera();
	camera1->SetInputWindow(window);
	camera1->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera1->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera1;

	MobileCamera* camera2 = new MobileCamera();
	camera2->SetInputWindow(window);
	camera2->SetUpPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera2->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	buffers = new Buffers();

#pragma endregion

	objLoader = new MyObjLoader("data/objects/dragon.obj");

	//generate and bind the frame buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//create a texture and bind it
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);

	//specify the texture format to be stored
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//attatch the texture to the framebuffer as the first colour attatchment
	//FBO must still be bound
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTexture, 0);

	//setup and bind 24bit depth buffer as render buffer
	glGenRenderbuffers(1, &fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);
	
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error! Framebuffer did not bind...");
	}
	
	//unbind the FBO to allow us to render to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	//setting up a plane to render a texture to
	float vertexData[] = 
	{
		-5, 0,  -5, 1, 0, 0,
		 5, 0,  -5, 1, 1, 0,
		 5, 10, -5, 1, 1, 1,
		-5, 10, -5, 1, 0, 1
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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//creating shaders
	shaderLoader = new ShaderLoader();
	string* source;

	source = shaderLoader->LoadFile("data/shaders/simpleTex.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/simpleTex.frag");
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

	//cleanup shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	delete source;

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, 512, 512);
}

bool Tutorial5::Update(double _dt)
{
	return true;
}

void Tutorial5::Render()
{
#pragma region objDrawing
	glUseProgram(objLoader->programID);

	//initilising location int and passing through projectionView unform
	int location = glGetUniformLocation(objLoader->programID, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &m_camera->GetProjectionView()[0][0]);
	//passing through uniform for light direction
	location = glGetUniformLocation(objLoader->programID, "LightDir");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 0.5f, 0)));
	//passing through uniform for light colour
	location = glGetUniformLocation(objLoader->programID, "LightColour");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
	//passing through unifrm for camera position
	location = glGetUniformLocation(objLoader->programID, "CameraPos");
	glUniform3fv(location, 1, glm::value_ptr(m_camera->GetPosition()));
	//passing through uniform for specular power
	location = glGetUniformLocation(objLoader->programID, "SpecPower");
	glUniform1f(location, 128);

	for (unsigned int i = 0; i < objLoader->GetGLInfo().size(); ++i)
	{
		glBindVertexArray(objLoader->GetGLInfo()[i].VAO);
		glDrawElements(GL_TRIANGLES, objLoader->GetGLInfo()[i].IndexCount, GL_UNSIGNED_INT, 0);
	}
#pragma endregion

	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	glClearColor(0.25f, 0.25f, 0.25f, 1);

	glUseProgram(s_programID);

	int loc = glGetUniformLocation(s_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glUniform1i(glGetUniformLocation(s_programID, "diffuse"), 0);

	glBindVertexArray(buffers->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Tutorial5::Shutdown()
{

}