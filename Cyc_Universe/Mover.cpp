#include "Mover.h"

Mover::Mover()
{
	m_position = cyclone::Vector3(rand() % 500 - 250, rand() % 500 - 250, rand() % 500 - 250);
	//m_position = cyclone::Vector3(0, 0, 0);



	double theta = (rand() % (3600000));
	double v = (rand() % 10000) * 0.0001;
	double phi = acos((2 * v) - 1);
	double r = pow((rand() % 10000) * 0.0001, 1 / 3);
	double x = r * sin(phi) * cos(theta);
	double y = r * sin(phi) * sin(theta);
	double z = r * cos(phi);
	m_position = cyclone::Vector3(x, y, z);

	velocity = glm::f64vec3((rand() % 1000 - 500) * 0.01, (rand() % 1000 - 500) * 0.01, (rand() % 1000 - 500) * 0.01);
	//velocity = glm::f64vec3((rand() % 1000 - 500) * 0.01, (rand() % 1000 - 500) * 0.01, (rand() % 1000 - 500) * 0.01);

	m_position *= ((rand()) * 0.1);
	//velocity = glm::f64vec3(0);

	// 공의 부피 공식
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

	if(drawType == 0)
		glutSolidSphere(ballSize, 30, 30);
	else
		glutSolidCube(ballSize);

	glPopMatrix();
}