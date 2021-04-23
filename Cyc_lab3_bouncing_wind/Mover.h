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

	int run;
	void update(float duration);
	void draw(int shadow);
	void checkEdges();
};

