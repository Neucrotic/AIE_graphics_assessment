#pragma once
#include "GLprogram.h"

class ParticleEmitter;
class GPUParticleEmitter;

class ParticleSystem : public GLprogram
{
protected:
	void Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

	ParticleEmitter* emitter;
	GPUParticleEmitter* grassEmitter;
	GPUParticleEmitter* rainEmitter;
	GPUParticleEmitter* cloudEmitter;
};