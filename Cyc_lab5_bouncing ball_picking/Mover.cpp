
#include <iostream>
#include "Mover.h"
#include "gl/glut.h"
#include "timing.h"

#include "pfgen.h"



Mover::Mover()
{
	
	m_particle = new cyclone::Particle();

	m_particle->setPosition(0, 50, 0);
	m_particle->setVelocity(100.0f, 0, 100.0f);
	m_particle->setMass(10.0f);
	m_particle->setDamping(1.0f);
	m_particle->setAcceleration(0, 0, 0);
			
	m_gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));
	m_drag = new cyclone::ParticleDrag(0.1, 0.1);

	m_forces = new cyclone::ParticleForceRegistry();
	m_forces->add(m_particle, m_gravity);
	m_forces->add(m_particle, m_drag);
	
	

}


Mover::~Mover()
{
}

float size = 2.0f;
void Mover::update(float duration)
{

	m_forces->updateForces(duration);

 	m_particle->integrate(duration);

	
	checkEdges();
	
}

float boxSize = 100.0f;
float boxSize_small = 50.0f;
void Mover::checkEdges()
{

	cyclone::Vector3 pos;
	cyclone::Vector3 vel;
	m_particle->getPosition(&pos);
	m_particle->getVelocity(&vel);


	if (pos.z >= 0) {
		cyclone::Vector3 wind(-10.0f, 0, 0);
		m_particle->addForce(wind);
	}
	else {
		cyclone::Vector3 wind(10.0f, 0, 0);
		m_particle->addForce(wind);
	}

	if (pos.z >= 0) {
		cyclone::Vector3 wind(-10.0f, 0, 0);
		m_particle->addForce(wind);
	}
	else {
		cyclone::Vector3 wind(10.0f, 0, 0);
		m_particle->addForce(wind);
	}

	if (pos.x + size >= boxSize_small) {
		cyclone::Vector3 wind(0, 10.0f, 0);
		m_particle->addForce(wind);
	}
	else if (pos.x - size < -boxSize_small) {
		cyclone::Vector3 wind(0, 10.0f, 0);
		m_particle->addForce(wind);
	}
	if (pos.z + size >= boxSize_small) {
		cyclone::Vector3 wind(0, 10.0f, 0);
		m_particle->addForce(wind);
	}
	else if (pos.z - size < -boxSize_small) {
		cyclone::Vector3 wind(0, 10.0f, 0);
		m_particle->addForce(wind);
	}

	if (pos.x + size >= boxSize) {
		m_particle->setPosition(boxSize - size, pos.y, pos.z);
		m_particle->setVelocity(vel.x * -1.0, vel.y, vel.z);
		m_particle->getPosition(&pos);
	}
	else if (pos.x - size < -boxSize) {
		m_particle->setPosition(size - boxSize, pos.y, pos.z);
		m_particle->setVelocity(vel.x * -1.0, vel.y, vel.z);
		m_particle->getPosition(&pos);
	}
	if (pos.z + size >= boxSize) {
		m_particle->setPosition(pos.x, pos.y, 100 - boxSize);
		m_particle->setVelocity(vel.x, vel.y, vel.z * -1.0);
		m_particle->getPosition(&pos);
	}
	else if (pos.z - size < -boxSize) {
		m_particle->setPosition(pos.x, pos.y, size - boxSize);
		m_particle->setVelocity(vel.x, vel.y, vel.z * -1.0);
		m_particle->getPosition(&pos);
	}

	/*if (pos.y < size) {
		m_particle->setPosition(pos.x, size, pos.z);
		m_particle->setVelocity(vel.x, vel.y * -1.0f, vel.z);
		m_particle->getPosition(&pos);
	}*/


}

void Mover::draw(int shadow)
{
	
	cyclone::Vector3 position;
	m_particle->getPosition(&position);

	if (shadow)
		glColor4f(0.1f, 0.1f, 0.1f,0.7f);
	else {
		glLoadName(1);
		glColor3f(1.0f, 0.0f, 0.0f);
	}

	glPushMatrix();

	glTranslatef(position.x, position.y, position.z);
	glutSolidSphere(size, 30, 30);

	glPopMatrix();

}