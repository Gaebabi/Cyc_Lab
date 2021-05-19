#pragma once


#include "particle.h"
#include "fgen.h"

class Mover
{
public:
	Mover();
	~Mover();

	cyclone::ParticleGravity * m_gravity;
	cyclone::ParticleDrag * m_drag;
	cyclone::Particle * m_particle;
	cyclone::ParticleForceRegistry * m_forces;
	void update(float duration);
	void draw(int shadow);
	void checkEdges();
};

