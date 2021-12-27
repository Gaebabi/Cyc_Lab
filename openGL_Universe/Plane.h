#pragma once
#include <glm/mat4x4.hpp>

#include "Loader.h"
#include <vector>



class Plane
{
public:
	Plane(float, float, int, int, float smax = 1.0f, float tmax = 1.0f);
	~Plane();

private:
	unsigned int vaoHandle;
	int faces;
public:
	void draw() const;

};

