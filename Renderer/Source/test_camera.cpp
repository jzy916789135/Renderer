#include"test_camera.h"

void Camera::ProcessKeyboard(EcameraMovement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == EcameraMovement::FORWARD)
		position += front * velocity;
	if (direction == EcameraMovement::BACKWARD)
		position -= front * velocity;
	if (direction == EcameraMovement::LEFT)
		position -= right * velocity;
	if (direction == EcameraMovement::RIGHT)
		position += right * velocity;

	UpdateView();
}

void Camera::ProcessMouseMovement(float x, float y, EInputButton input)
{
	glm::vec2 pos{x,y};

	if (input == EInputButton::RIGHT)
	{
		glm::vec2 delta = (pos - cursorLastPos);

		float sign = up.y < 0 ? -1.0f : 1.0f;
		yaw += sign * delta.x;
		pitch -= delta.y;
	}
	else if (input == EInputButton::NONE)
	{

	}
	cursorLastPos = pos;
	
	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;
	UpdateCameraVectors();
	UpdateView();
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 newfront;
	newfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newfront.y = sin(glm::radians(pitch));
	newfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newfront);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}