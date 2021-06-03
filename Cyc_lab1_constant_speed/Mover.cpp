#include <iostream>
#include "Mover.h"
#include <gl/glut.h>
#include "timing.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/matrix_transform_2d.hpp"

Mover::Mover()
{
	m_position = cyclone::Vector3(rand() % 10000 - 5000, rand() % 10000 - 5000 + 100, rand() % 10000 - 5000);
	velocity = glm::f64vec3(0, 0, 0);
}
Mover::~Mover()
{
}
void Mover::update()
{
	m_position += velocity / (ballMass);
}
void Mover::stop()
{
	TimingData::get().deinit();
}
void Mover::draw()
{
	glColor3f(ballMass / 5.0, 2.0 / ballMass, 0.5);

	glPushMatrix();
	glTranslatef(m_position.x, m_position.y, m_position.z);

	ballSize = glm::pow((ballMass * 3.0 / 4.0 / 3.1415), 0.3333);
	
	glutSolidSphere(ballSize, 30, 30);

	glPopMatrix();
}