#pragma once

#include <core.h>

class Mover
{
public:
	Mover();
	~Mover();
	
	bool active = true;

	cyclone::Vector3 m_position;
	float sign;
	float timer = 0;
	float nowRadian = 0;
	float ballSize = 0.0f;
	float ballMass = 1.0f;

	glm::vec3 velocity;

	void update(float duration);
	void stop();
	void draw(int shadow);
};

