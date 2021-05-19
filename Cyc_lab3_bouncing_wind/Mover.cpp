
#include <iostream>
#include "Mover.h"
#include "gl/glut.h"

Mover::Mover()
{

	m_particle = new cyclone::Particle();

	m_particle->setPosition(5, 20, 0);

	switch (0) {
	case 0: //기본
		m_particle->setMass(1.0f);
		m_particle->setVelocity(0, 0, 0);
		m_particle->setAcceleration(cyclone::Vector3::GRAVITY);
		m_particle->setDamping(0.99f);
		break;
	case 1://레이저
		m_particle->setMass(0.1f);
		m_particle->setVelocity(0, 0, 100);
		m_particle->setAcceleration(0, 0, 0);
		m_particle->setDamping(0.99f);
		break;
	case 2://Fireball
		m_particle->setMass(1.0f); // 1.0kg - mostly blast damage
		m_particle->setVelocity(0.0f, 0.0f, 10.0f); // 5m/s
		m_particle->setAcceleration(0.0f, 0.6f, 0.0f); // Floats up
		m_particle->setDamping(0.9f);
		break;
	case 3://Artillery
		m_particle->setMass(200.0f); // 200.0kg
		m_particle->setVelocity(0.0f, 30.0f, 40.0f); // 50m/s
		m_particle->setAcceleration(0.0f, -20.0f, 0.0f);
		m_particle->setDamping(0.99f);
		break;
	case 4://Pistol
		m_particle->setMass(2.0f); // 2.0kg
		m_particle->setVelocity(0.0f, 0.0f, 35.0f); // 35m/s
		m_particle->setAcceleration(0.0f, -1.0f, 0.0f);
		m_particle->setDamping(0.99f);
		break;
	}

	
	m_drag = new cyclone::ParticleDrag(0.1, 0.1);
	
}


Mover::~Mover()
{
}

float size = 2.0f;
void Mover::update(float duration)
{
	
	cyclone::Vector3 wind(10.0f, 0, 10.0f);
 	m_particle->addForce(wind);

	m_particle->integrate(duration);

	
	checkEdges();
	
}

void Mover::checkEdges()
{

	cyclone::Vector3 pos;
	cyclone::Vector3 vel;
	m_particle->getPosition(&pos);
	m_particle->getVelocity(&vel);


	if (pos.x + size >= 100) {
		m_particle->setPosition(100 - size, pos.y, pos.z);
		m_particle->setVelocity(vel.x * -1.0, vel.y, vel.z);
		m_particle->getPosition(&pos);
	}
	else if (pos.x - size < 0) {
		m_particle->setPosition(size, pos.y, pos.z);
		m_particle->setVelocity(vel.x * -1.0, vel.y, vel.z);
		m_particle->getPosition(&pos);
	}
	if (pos.z + size >= 100) {
		m_particle->setPosition(pos.x, pos.y, 100 - size);
		m_particle->setVelocity(vel.x, vel.y, vel.z * -1.0);
		m_particle->getPosition(&pos);
	}
	else if (pos.z - size < 0) {
		m_particle->setPosition(pos.x, pos.y, size);
		m_particle->setVelocity(vel.x, vel.y, vel.z * -1.0);
		m_particle->getPosition(&pos);
	}

	if (pos.y < size) {
		m_particle->setPosition(pos.x, size, pos.z);
		m_particle->setVelocity(vel.x, vel.y*-1.0f, vel.z);
		m_particle->getPosition(&pos);
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