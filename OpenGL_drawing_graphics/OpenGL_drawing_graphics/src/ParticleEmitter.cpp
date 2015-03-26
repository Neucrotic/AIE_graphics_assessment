#include "ParticleEmitter.h"
#include "ShaderLoader.h"

ParticleEmitter::ParticleEmitter()
{
	particles = nullptr;
	vertexData = nullptr;
	firstDead = 0;
	maxParticles = 0;
	position = glm::vec3(0, 0, 0);
	VAO = 0;
	VBO = 0;
	IBO = 0;
}

ParticleEmitter::~ParticleEmitter()
{
	Deinit();
}

void ParticleEmitter::Init(unsigned int _maxParticles, unsigned int _emitRate,
						   float _lifetimeMin, float _lifetimeMax,
						   float _velocityMin, float _velocityMax,
						   float _startSize, float _endSize,
						   const glm::vec4& _startColour, const glm::vec4& _endColour)
{
	shaderLoader = new ShaderLoader();
	
	//emit timers
	emitTimer = 3;
	emitRate = 1.0f / _emitRate;

	//initialisating parameter variables
	maxParticles = _maxParticles;
	lifespanMin = _lifetimeMin;
	lifespanMax = _lifetimeMax;
	velocityMin = _velocityMin;
	velocityMax = _velocityMax;
	startSize = _startSize;
	endSize = _endSize;
	startColour = _startColour;
	endColour = _endColour;

	//initialising particle array
	particles = new Particle[maxParticles];
	firstDead = 0;

	//array of vertices to be filled during update
	vertexData = new ParticleVertex[maxParticles * 4];
	
	CreateOpenGLBuffers();
	CreateShaders();
	
}

void ParticleEmitter::Deinit()
{
	delete[] particles;
	delete[] vertexData;

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

void ParticleEmitter::Update(float _deltaTime, const glm::mat4& _cameraTransform)
{
	//spawn particles
	emitTimer += _deltaTime;
	while (emitTimer > emitRate)
	{
		Emit();
		emitTimer -= emitRate;
	}

	unsigned int quad = 0;

	//updating particles and turn live particles into billboard quads
	for (unsigned int i = 0; i < firstDead; ++i)
	{
		Particle* particle = &particles[i];

		particle->lifetime += _deltaTime;
		if (particle->lifetime >= particle->lifespan)
		{
			//swapping last alive with current
			*particle = particles[firstDead - 1];
			firstDead--;
		}
		else 
		{
			//move particle
			particle->position += particle->velocity * _deltaTime;

			//resize particle
			particle->size = glm::mix(startSize, endSize, particle->lifetime / particle->lifespan);

			//colour particle
			particle->colour = glm::mix(startColour, endColour, particle->lifetime / particle->lifespan);

			//make the quad the right size and colour
			float halfSize = particle->size * 0.5f;

			vertexData[quad * 4 + 0].position = glm::vec4(halfSize, halfSize, 0, 1);
			vertexData[quad * 4 + 0].colour = particle->colour;

			vertexData[quad * 4 + 1].position = glm::vec4(-halfSize, halfSize, 0, 1);
			vertexData[quad * 4 + 1].colour = particle->colour;

			vertexData[quad * 4 + 2].position = glm::vec4(-halfSize, -halfSize, 0, 1);
			vertexData[quad * 4 + 2].colour = particle->colour;

			vertexData[quad * 4 + 3].position = glm::vec4(halfSize, -halfSize, 0, 1);
			vertexData[quad * 4 + 4].colour = particle->colour;

			//create billboards transform
			glm::vec3 zAxis = glm::normalize(glm::vec3(_cameraTransform[3]) - particle->position);
			glm::vec3 xAxis = glm::cross(glm::vec3(_cameraTransform[1]), zAxis);
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);

			glm::mat4 billboard(glm::vec4(xAxis, 0),
								glm::vec4(yAxis, 0),
								glm::vec4(zAxis, 0),
								glm::vec4(0, 0, 0, 1));

			vertexData[quad * 4 + 0].position = billboard * vertexData[quad * 4 + 0].position + glm::vec4(particle->position, 0);
			vertexData[quad * 4 + 1].position = billboard * vertexData[quad * 4 + 1].position + glm::vec4(particle->position, 0);
			vertexData[quad * 4 + 2].position = billboard * vertexData[quad * 4 + 2].position + glm::vec4(particle->position, 0);
			vertexData[quad * 4 + 3].position = billboard * vertexData[quad * 4 + 3].position + glm::vec4(particle->position, 0);

			++quad;
		}
	}
}

void ParticleEmitter::Draw()
{

	glUseProgram(programID);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, firstDead * 4 * sizeof(ParticleVertex), vertexData);

	//draw particles
	glDrawElements(GL_TRIANGLES, firstDead * 6, GL_UNSIGNED_INT, 0);
}

void ParticleEmitter::Emit()
{
	if (firstDead >= maxParticles)
		return;

		//ressurect first dead particle
		Particle& particle = particles[firstDead++];

		particle.position = position;

		//randomising lifespan
		particle.lifespan = PraiseRNGesus();
		particle.lifetime = 0;

		particle.colour = startColour;
		particle.size = startSize;

		//randomise velocity, direction and strength
		float randVelocity = (rand() / (float)RAND_MAX) * (velocityMax - velocityMin) + velocityMin;
		particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
		particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
		particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;
		particle.velocity = glm::normalize(particle.velocity) * randVelocity;
}

void ParticleEmitter::CreateShaders()
{
	string* source;

	source = shaderLoader->LoadFile("data/shaders/particle_shader.vert");
	const char* vsSource = source->c_str();

	source = shaderLoader->LoadFile("data/shaders/particle_shader.frag");
	const char* fsSource = source->c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vsSource, nullptr);
	glShaderSource(fragmentShader, 1, &fsSource, nullptr);
	glCompileShader(fragmentShader);
	glCompileShader(vertexShader);

	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);

	glLinkProgram(programID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int success = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (success = GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	delete source;
}

void ParticleEmitter::CreateOpenGLBuffers()
{
	//creating index buffer and filling it with 2 tri = 1 quad
	unsigned int* indexData = new unsigned int[maxParticles * 6];
	for (unsigned int i = 0; i < maxParticles; ++i)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	//creating opengl buffers
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(ParticleVertex), vertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxParticles * 6 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //colour
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
}

float ParticleEmitter::PraiseRNGesus()
{
	float holySacrifice;

	holySacrifice = (rand() / (float)RAND_MAX) * (lifespanMax - lifespanMin) + lifespanMin;

	return holySacrifice;
}