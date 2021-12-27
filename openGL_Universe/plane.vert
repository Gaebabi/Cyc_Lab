#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;

out vec3 Position;
out vec3 Normal;
out float Shadow;

uniform mat4 u_viewProjection;

void main()
{
	// out
	Position = aPos;
    Normal = vertexNormal;
	gl_Position = u_viewProjection * vec4(aPos, 1.0f);
}