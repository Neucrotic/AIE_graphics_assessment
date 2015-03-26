#include "GPUParticleEmitter.h"
#include <string>
#include "ShaderLoader.h"
#include "MobileCamera.h"

GPUParticleEmitter::GPUParticleEmitter()
	: particles(nullptr), maxParticles(0),
	position(0, 0, 0),
	drawShader(0),
	updateShader(0),
	lastDrawTime(0)
{
	VAO[0] = 0;
	VAO[1] = 0;
	VBO[0] = 0;
	VBO[1] = 0;
}

GPUParticleEmitter::~GPUParticleEmitter()
{
	delete[] particles;

	//delete buffers
	glDeleteVertexArrays(2, VAO);
	glDeleteVertexArrays(2, VBO);

	//delete shaders
	glDeleteProgram(drawShader);
	glDeleteProgram(updateShader);
}

void GPUParticleEmitter::Init(unsigned int _maxParticles,
	float _lifespanMin, float _lifespanMax,
	float _velocityMin, float _velocityMax,
	float _startSize, float _endSize,
	const glm::vec4& _startColour,
	const glm::vec4& _endColour)
{
	//storing passed in varibles
	startColour = _startColour;
	endColour = _endColour;
	startSize = _startSize;
	endSize = _endSize;
	velocityMin = _velocityMin;
	velocityMax = _velocityMax;
	lifespanMin = _lifespanMin;
	lifespanMax = _lifespanMax;
	maxParticles = _maxParticles;

	//initilialise particle array
	particles = new GPUParticle[maxParticles];

	//initilise shader loader
	shaderLoader = new ShaderLoader();

	//set starting transform feedback buffer
	activeBuffer = 0;

	CreateBuffers();
	//CreateUpdateShader();
	CreateDrawShader();
}

void GPUParticleEmitter::CreateBuffers()
{
	//create opengl buffers
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	//setting up first buffer
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(GPUParticle), particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	//setting up second buffer
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(GPUParticle), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUParticleEmitter::CreateUpdateShader(const char* _updateShader)
{
	unsigned int vs = shaderLoader->LoadShader(GL_VERTEX_SHADER, _updateShader);

	updateShader = glCreateProgram();
	glAttachShader(updateShader, vs);

	//specify the data we will stream back in, this must be done in order
	const char* variants[] = { "position", "velocity", "lifetime", "lifespan" };

	glTransformFeedbackVaryings(updateShader, 4, variants, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(updateShader);

	int success = false;
	glGetProgramiv(updateShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(updateShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(updateShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	//removing unneeded handles
	glDeleteShader(vs);

	//binding the shader so we can set uniforms that will remain constant
	glUseProgram(updateShader);
	int location = glGetUniformLocation(updateShader, "lifeMin");
	glUniform1f(location, lifespanMin);
	location = glGetUniformLocation(updateShader, "lifeMax");
	glUniform1f(location, lifespanMax);
}

void GPUParticleEmitter::CreateDrawShader()
{
	unsigned int vs = shaderLoader->LoadShader(GL_VERTEX_SHADER, "data/shaders/particles/GPUparticle.vert");
	unsigned int gs = shaderLoader->LoadShader(GL_GEOMETRY_SHADER, "data/shaders/particles/GPUparticle.geom");
	unsigned int fs = shaderLoader->LoadShader(GL_FRAGMENT_SHADER, "data/shaders/particles/GPUparticle.frag");

	drawShader = glCreateProgram();

	glAttachShader(drawShader, vs);
	glAttachShader(drawShader, fs);
	glAttachShader(drawShader, gs);
	glLinkProgram(drawShader);

	int success = false;
	glGetProgramiv(drawShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(drawShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(drawShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	//removing unneeded handles
	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	//binding shader so we can set uniforms that wont change per-frame
	glUseProgram(drawShader);

	//bind size information for interpolation that wont change
	int location = glGetUniformLocation(drawShader, "sizeStart");
	glUniform1f(location, startSize);
	location = glGetUniformLocation(drawShader, "sizeEnd");
	glUniform1f(location, endSize);

	//bind colour information for interpolation that wont change
	location = glGetUniformLocation(drawShader, "colourStart");
	glUniform4fv(location, 1, &startColour[0]);
	location = glGetUniformLocation(drawShader, "colourEnd");
	glUniform4fv(location, 1, &endColour[0]);
}

void GPUParticleEmitter::Draw(float _time, const glm::mat4& _cameraTransform, const glm::mat4& _projetionView)
{
	glUseProgram(updateShader);

	//binding time information
	int location = glGetUniformLocation(updateShader, "time");
	glUniform1f(location, _time);

	float deltaTime = _time - lastDrawTime;
	lastDrawTime = _time;

	location = glGetUniformLocation(updateShader, "deltaTime");
	glUniform1f(location, deltaTime);

	//binding emitters position
	location = glGetUniformLocation(updateShader, "emitterPosition");
	glUniform3fv(location, 1, &position[0]);

	//disable rasterization
	glEnable(GL_RASTERIZER_DISCARD);

	//binding the buffer we will update
	glBindVertexArray(VAO[activeBuffer]);

	//calculate the other buffer
	unsigned int otherBuffer = (activeBuffer + 1) % 2;

	//binding the buffer we will update and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, maxParticles);

	//disbaling transform feedback and re-enabling rasterization
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	//billbaord the particles using the geomerty shader
	glUseProgram(drawShader);

	location = glGetUniformLocation(drawShader, "projectionView");
	glUniformMatrix4fv(location, 1, false, &_projetionView[0][0]);
	location = glGetUniformLocation(drawShader, "cameraTransform");
	glUniformMatrix4fv(location, 1, false, &_cameraTransform[0][0]);

	//draw particles in the otherBuffer
	glBindVertexArray(VAO[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, maxParticles);

	//swap for the next frame
	activeBuffer = otherBuffer;
}

void GPUParticleEmitter::SetOrigin(glm::vec3 _origin)
{
	position = _origin;
}