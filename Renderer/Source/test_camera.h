#pragma once

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum  class EcameraMovement
{
	FORWARD = 0,
	BACKWARD = 1,
	LEFT = 2,
	RIGHT = 3
};

enum class EInputButton
{
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2,
	NONE = 3
};

class Camera
{
public:
	Camera(glm::vec3 position, float fov, float aspect, float nearPlane, float farPlane) : position(position), fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane)
	{
		SetProjection();
		UpdateView();
		UpdateCameraVectors();
	}

	inline void SetProjection()
	{
		projMatrix = glm::perspective(fov, aspect, nearPlane, farPlane);
	}
	inline void UpdateView()
	{
		viewMatrix = glm::lookAt(position, position + front, up);
		viewProjMatrix = projMatrix * viewMatrix;
	}
	inline glm::mat4 GetViewMatrix()
	{
		return viewMatrix;
	}
	inline glm::mat4 GetProjMatrix()
	{
		return projMatrix;
	}
	inline glm::mat4 GetViewProjMatrix()
	{
		return viewProjMatrix;
	}

	void ProcessKeyboard(EcameraMovement direction,float deltaTime);

	void ProcessMouseMovement(float x, float y, EInputButton input);

private:

	void UpdateCameraVectors();
public:

	glm::mat4 projMatrix = glm::mat4(1.f);
	glm::mat4 viewMatrix = glm::mat4(1.f);
	glm::mat4 viewProjMatrix = glm::mat4(1.f);


	glm::vec3 front = glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 right = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);

	float yaw = 0.f;
	float pitch = 0.f;
	float fov = 45.f;
	float aspect = 1920.f / 1080.f;
	float nearPlane = 0.1f;
	float farPlane = 100.f;
	float movementSpeed = 2.5f;
	float mouseSensitivity = 0.2f;

	glm::vec2 cursorLastPos = glm::vec2(0.f);
};