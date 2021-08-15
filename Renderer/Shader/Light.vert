#version 330 core

layout (location =0) in vec3 lPos;
layout (location = 1) in vec3 lNormal;
layout (location = 2) in vec3 lTangentU;
layout (location = 3) in vec2 lTexC;

struct PassCb
{
	mat4 view;
	mat4 proj;
};

uniform PassCb passCb;
uniform mat4 model;

void main()
{
	gl_Position = passCb.proj * passCb.view * model * vec4(lPos, 1.f);
}