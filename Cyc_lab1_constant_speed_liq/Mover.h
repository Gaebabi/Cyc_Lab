#pragma once

#include <core.h>

class Mover
{
public:
	Mover();
	~Mover();
	
	bool active = true;

	cyclone::Vector3 m_position;
	double ballSize = 0.0;
	double ballMass = 2.0;

	glm::f64vec3 velocity;

	void update();
	void stop();
	void draw();
};

