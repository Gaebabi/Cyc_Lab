#ifndef  __GLOBAL
#define  __GLOBAL

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

namespace global
{
	extern glm::vec3 lightPos[];
	extern glm::vec3 lightIntensity[];
	extern glm::vec3 backGround;
	extern glm::ivec2 floorSize;
	glm::vec3 computeNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
};

#endif