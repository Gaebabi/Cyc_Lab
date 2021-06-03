#include "Mover.h"

Mover::Mover()
{
	m_position = cyclone::Vector3(rand() % 5000 - 2500, rand() % 5000 - 2500, rand() % 5000 - 2500);
	velocity = glm::f64vec3(0, 0, 0);

	ballSize = glm::pow((ballMass * 3.0 / 4.0 / 3.1415), 0.3333);
}
Mover::~Mover()
{
}

// 중력 상수 G
const double G = 9.8 * 0.01;// 정밀도
void Mover::update()
{
	m_position += G * velocity / (ballMass);
}
void Mover::stop()
{
	TimingData::get().deinit();
}
void Mover::draw()
{
	glColor3f(ballMass / 5.0, 2.0 / ballMass, 0.5);

	glLineWidth(1.0f);
	if (positionHistory.size() > 0) {
		glBegin(GL_LINES);
		glVertex3f(positionHistory[0].x, positionHistory[0].y, positionHistory[0].z);
		for (int i = 1; i < positionHistory.size(); i++) {
			//glLineWidth(glm::length(positionHistory[0] - positionHistory[0]));
			glVertex3f(positionHistory[i].x, positionHistory[i].y, positionHistory[i].z);
			glVertex3f(positionHistory[i].x, positionHistory[i].y, positionHistory[i].z);
		}
		glEnd();
		glLineWidth(1.0f);
	}

	glPushMatrix();
	glTranslatef(m_position.x, m_position.y, m_position.z);


	glutSolidCube(ballSize);
	//glutSolidSphere(ballSize, 30, 30);

	glPopMatrix();
}