#pragma once

#include <glm/gtc/matrix_transform.hpp> // translate()




struct InsData {
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;
	glm::vec3 Velocity;

	glm::mat4 aInstanceMatrix;
};

struct Instance {
public:
	//float speed;
	//float getAnimationOffset();
	InsData getData();
	glm::vec3 getPosition();
	glm::vec3 getRotation();
	//glm::vec3 getVelocity();
	glm::mat4 getInstanceMatrix();

	int windVelocityIndex = 0;
	glm::vec3 windVelocity = glm::vec3(0);
	int lod = 0;
	float bright = 1.0f;
	//glm::vec3 windVelocityRebound;
public:
	void setInstanceMatrix(glm::mat4 mat);
	//void setAnimationOffset(float offset);
	void setPosition(glm::vec3 vec);
	void setRotation(glm::vec3 vec);
	void setScale(glm::vec3 vec);
	void setVelocity(glm::vec3 vec);
	//void setVelocity(glm::vec3 vec);

	//void Update();
	bool updateMatrix();
private:
	InsData data;
	//float AnimationOffset;
	//glm::vec3 Velocity;

	bool init = true;
	bool hasVelocity = false;
	bool doUpdateMatrix = true;
};