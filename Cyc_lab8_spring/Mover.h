#pragma once

#include "core.h"
#include "particle.h"
#include "pfgen.h"


#include "MySpring.h"



class Mover
{
public:
	Mover(cyclone::Vector3 &p);
	~Mover();



	//cyclone::ParticleSpring * m_spring;
	cyclone::Myspring * m_spring;


	cyclone::Particle * m_particle;
	
	void update(float duration);
	void draw(int shadow);
	void setConnection(Mover * a);
	void checkEdges();
	
};


class MoverConnection
{
public:
	MoverConnection();
	~MoverConnection();

	cyclone::ParticleGravity * m_gravity;
	cyclone::ParticleForceRegistry * m_forces;

	std::vector<Mover *> m_movers;
	void update(float duration);
	void draw(int shadow);

};
