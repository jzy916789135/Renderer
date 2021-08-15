#version 330 core

layout (location = 0) in vec3 lPos;

out vec3 texCoord;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	texCoord = lPos;
	gl_Position = (proj * view * vec4(lPos, 1.0)).xyww;
}
