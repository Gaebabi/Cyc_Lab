#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;

layout(binding = 0) buffer ssbo1 // trans, rotate
{
	mat4 aInstanceMatrix[];
};
out vec3 Position;
out vec4 worldPosition;
out vec3 Normal;

uniform mat4 u_viewProjection;

void main()
{
	// out
	Position = vec3(
		aPos.x,
		aPos.y,
		aPos.z
	);
	Normal =  vertexNormal;
	worldPosition = aInstanceMatrix[gl_InstanceID] * vec4(Position, 1.0f);
	
	gl_Position = u_viewProjection * worldPosition;
}