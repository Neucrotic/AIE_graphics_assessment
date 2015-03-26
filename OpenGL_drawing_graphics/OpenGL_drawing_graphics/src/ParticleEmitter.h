#pragma once
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class ShaderLoader;

struct Particle
	{
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec4 colour;
		float size;
		float lifetime;
		float lifespan;
	};

	struct ParticleVertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	};

class ParticleEmitter
{
protected:

	Particle* particles;
	ParticleVertex* vertexData;

	//particle array indexes
	unsigned int firstDead;
	unsigned int maxParticles;

	//particle limits
	glm::vec3 position;
	glm::vec4 startColour;
	glm::vec4 endColour;

	float emitTimer;
	float emitRate;

	float lifespanMin;
	float lifespanMax;

	float velocityMin;
	float velocityMax;

	float startSize;
	float endSize;

	//buffers
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;

	ShaderLoader* shaderLoader;
	
	void Emit();

	void CreateShaders();
	void CreateOpenGLBuffers();

	float PraiseRNGesus();
public:
	unsigned int programID;

	ParticleEmitter();
	~ParticleEmitter();

	void Init(unsigned int _maxParticles, unsigned int _emitRate, 
			  float _lifetimeMin, float _lifetimeMax, 
			  float _velocityMin, float _velocityMax, 
			  float _startSize, float _endSize, 
			  const glm::vec4& _startColour, const glm::vec4& _endColour);
	void Deinit();
	
	void Update(float _deltaTime, const glm::mat4& _cameraTransform);
	void Draw();
};