#pragma once

#include <core.h>

class Mover
{
public:
	Mover();
	~Mover();

	cyclone::Vector3 m_position;
	float sign;
	float timer = 0;
	float nowRadian = 0;
	glm::vec3 velocity;

	void update(float duration);
	void stop();
	void draw(int shadow);
};

