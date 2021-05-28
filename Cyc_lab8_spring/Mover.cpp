
#include <iostream>
#include "Mover.h"
#include "gl/glut.h"
#include "timing.h"


float size = 1.0f;


///////////////////////////////////////////////////////////////////
Mover::Mover(cyclone::Vector3 & p)
{
	
	m_particle = new cyclone::Particle();

	m_particle->setPosition(p);
	m_particle->setVelocity(0, 0, 0);
	m_particle->setMass(10.0f);
	m_particle->setDamping(0.9f);
	m_particle->setAcceleration(0, 0, 0);
}



void Mover::setConnection(Mover * a)
{
	//m_spring = new cyclone::ParticleSpring(a->m_particle, 20.0f, 3);
	m_spring = new cyclone::Myspring(a->m_particle, 20.0f, 3.0f);
}


Mover::~Mover()
{
}


void Mover::update(float duration)
{
	
	m_particle->integrate(duration);


	checkEdges();

}

void Mover::checkEdges()
{

	cyclone::Vector3  pos = m_particle->getPosition();
	cyclone::Vector3  vel = m_particle->getVelocity();

/*
	if (pos.x + size >= 100) {
		m_particle->setPosition(100 - size, pos.y, pos.z);
		m_particle->setVelocity(vel.x*-1.0, vel.y, vel.z);
	}
	else if (pos.x - size < 0) {
		m_particle->setPosition(size, pos.y, pos.z);
		m_particle->setVelocity(vel.x*-1.0, vel.y, vel.z);
	}
*/
	if (pos.y < size) {
		m_particle->setPosition(pos.x, size, pos.z);
		m_particle->setVelocity(vel.x, vel.y*-1.0f, vel.z);

	}
}

void Mover::draw(int shadow)
{

	cyclone::Vector3 position;
	m_particle->getPosition(&position);

	if (shadow)
		glColor4f(0.1f, 0.1f, 0.1f, 0.7f);
	else
		glColor3f(1.0f, 0.0f, 0.0f);

	glPushMatrix();

	glTranslatef(position.x, position.y, position.z);
	glutSolidSphere(size, 30, 30);

	glPopMatrix();

}


//////////////////////////////

MoverConnection::MoverConnection()
{


	m_gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));


	m_forces = new cyclone::ParticleForceRegistry();
	Mover * a = new Mover(cyclone::Vector3(3, size, 3));

	m_forces->add(a->m_particle, m_gravity);
	m_movers.push_back(a);

	for (int i = 0; i < 2; i++) {
		Mover * b = new Mover(cyclone::Vector3(5 + i * 3, size, 3));
		a->setConnection(b);
		b->setConnection(a);

		m_movers.push_back(b);
		m_forces->add(b->m_particle, m_gravity);
		m_forces->add(a->m_particle, a->m_spring);
		m_forces->add(b->m_particle, b->m_spring);
		a = b;

	}
}

MoverConnection::~MoverConnection()
{


}


void MoverConnection::update(float duration)
{

	
	m_forces->updateForces(duration);


	for (unsigned int i = 0; i < m_movers.size(); i++) {
		m_movers[i]->update(duration);
	}
}


void MoverConnection::draw(int shadow)
{

	for (unsigned int i = 0; i < m_movers.size(); i++) {
		if (!shadow)
			glLoadName(i + 1);
		m_movers[i]->draw(shadow);

	}

	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < m_movers.size(); i++) {
		cyclone::Vector3 p = m_movers[i]->m_particle->getPosition();
		glVertex3f(p.x, p.y, p.z);

	}
	glEnd();
}