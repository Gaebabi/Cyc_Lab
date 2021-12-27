#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <glm/glm.hpp>

#include <cstdlib>

namespace global
{
	glm::vec3 lightPos[5] = {
		glm::vec3(0, 1000, 0),
		glm::vec3(2, 10, 10),
		glm::vec3(-2, -2, 10),
		glm::vec3(1, -5, 10),
		glm::vec3(5, -1, 10)
	};
	glm::vec3 lightIntensity[5] = {
		glm::vec3(1.0, 1.0, 1.0),
		glm::vec3(0.0, 0.0, 0.8),
		glm::vec3(0.8, 0.0, 0.0),
		glm::vec3(0.0, 0.8, 0.0),
		glm::vec3(0.8, 0.8, 0.8)
	};

	glm::vec3 backGround = glm::vec3(0.2, 0.2, 0.2);
	glm::ivec2 floorSize = glm::vec2(10, 5);

	glm::vec3 computeNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		glm::vec3 v1 = p2 - p1;
		glm::vec3 v2 = p3 - p1;
		glm::vec3 n = glm::cross(v1, v2);
		return glm::normalize(n);
	}
}