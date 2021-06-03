#pragma once
#include <iostream>
#include <core.h>
#include <vector>
#include <gl/glut.h>
#include "timing.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/matrix_transform_2d.hpp"

class Mover
{
public:
	int drawType = 0;
	Mover();
	~Mover();
	
	bool active = true;

	cyclone::Vector3 m_position;
	double ballSize = 0.0;
	double ballMass = 2.0;

	glm::f64vec3 velocity;
	std::vector<glm::vec3> positionHistory;

	void update();
	void stop();
	void draw();
};

