
#include <iostream>
#include "Mover.h"
#include <gl/glut.h>
#include "timing.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/matrix_transform_2d.hpp"



Mover::Mover()
{
	m_position = cyclone::Vector3(rand() % 100 - 50, rand() % 100 - 50 + 100, rand() % 100 - 50);
	if (rand() % 100 < 50) {
		sign = -1.0f;
	}
	else {
		sign = 1.0f;
	}
	velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

Mover::~Mover()
{
}
float fx(int timer) {
	return (0.06 * (sin(32) * timer / 12) + pow(3.141592, cos(timer)) - 2 * cos(4 * timer)) / sin(32);
}
float gx(int x) {
	return sin(fx(x))* x + cos(x);
}

float rad2deg(double radian)
{
	return radian * 180 / 3.141592;
}
float deg2rad(double degree)
{
	return degree * 3.141592 / 180;
}
glm::vec3 rad2normal(float rad) {
	glm::vec3 velocity = glm::vec3(0, 1, 1) * glm::rotate(glm::mat3(1.0f), rad);
	velocity = glm::vec3(velocity.x, 0, velocity.y);
	return glm::normalize(velocity);
}

void Mover::update(float duration)
{
	// 배속
	//duration *= 1.0f;
	// 시간 증가 (=가속도)
	//timer += duration * 0.05f;
	//nowRadian = deg2rad(rad2deg(nowRadian) - (10 + timer) * duration * sign);
	//glm::vec3 vec = rad2normal(nowRadian) * (timer);

	//glm::vec3 vec = rad2normal(nowRadian) * (duration);
	m_position += velocity;
	//if (m_position.y > 100) m_position.y = 0;
}

void Mover::stop()
{

	TimingData::get().deinit();

}
void Mover::draw(int shadow)
{
	if (shadow) {
		glColor3f(0,0,0);
	}
	else {
		glColor3f(1.0f, 0,0 );
	}

	glPushMatrix();
	//glTranslatef(0,1,0);
	glTranslatef(m_position.x, m_position.y, m_position.z);
	glutSolidSphere(1.0f, 60, 60);
	glPopMatrix();

}