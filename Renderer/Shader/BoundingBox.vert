#version 330 core

layout(location = 0) in vec3 lPos;

uniform mat4 model;

struct PassCb
{
	mat4 view;
	mat4 proj;
};
uniform PassCb passCb;

void main()
{
	gl_Position = passCb.proj * passCb.view * model * vec4(lPos,1.0f);
}