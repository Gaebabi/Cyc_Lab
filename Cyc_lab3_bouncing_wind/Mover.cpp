
#include <iostream>
#include "Mover.h"
#include "gl/glut.h"

Mover::Mover()
{

	m_particle = new cyclone::Particle();

	m_particle->setPosition(5, 20, 0);
	m_particle->setVelocity(0,0, 0);
	m_particle->setMass(1.0f);
	m_particle->setDamping(0.99f);
	m_particle->setAcceleration(cyclone::Vector3::GRAVITY);
			
	
	m_drag = new cyclone::ParticleDrag(0.1, 0.1);
	
}


Mover::~Mover()
{
}

float size = 2.0f;
void Mover::update(float duration)
{
		
	cyclone::Vector3 wind(1.0f, 0, 0);
	
	
 	m_particle->addForce(wind);

	m_particle->integrate(duration);

	
	checkEdges();
	
}

void Mover::checkEdges()
{

	cyclone::Vector3  pos;
	cyclone::Vector3  vel;
	m_particle->getPosition(&pos);
	m_particle->getVelocity(&vel);
	
		
	if (pos.x+ size >= 100) {
		m_particle->setPosition(100-size, pos.y, pos.z);
		m_particle->setVelocity(vel.x*-1.0, vel.y, vel.z);
	}
	else if (pos.x-size < 0) {
		m_particle->setPosition(size, pos.y, pos.z);
		m_particle->setVelocity(vel.x*-1.0, vel.y, vel.z);	
	}

	if (pos.y < size) {
		m_particle->setPosition(pos.x, size, pos.z);
		m_particle->setVelocity(vel.x, vel.y*-1.0f, vel.z);
	}
}

void Mover::draw(int shadow)
{
	
	cyclone::Vector3 pos;
	m_particle->getPosition(&pos);

	if (shadow)
		glColor4f(0.1,0.1,0.1,0.5f);
	else
		glColor3f(1.0f, 0.0f, 0.0f);

	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	glutSolidSphere(size, 30, 30);

	glPopMatrix();

}