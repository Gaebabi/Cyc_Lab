#pragma once


#include "particle.h"
#include "fgen.h"

class Mover
{
public:
	Mover();
	~Mover();

	
	cyclone::Particle * m_particle;

	int run;
	void update(float duration);
	void draw(int shadow);
	void checkEdges();
};

