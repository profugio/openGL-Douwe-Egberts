#pragma once
#include <glm/glm.hpp>

//Base class, make different implementation for Drone and Walking camera.
class Camera {
public:

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;
	glm::vec3 cameraRight;

	float cameraSpeed;

	float yaw;
	float pitch;

	glm::mat4 view;
	glm::mat4 projection;

	void CalculateCameraDirection();
	virtual void Move(int, int);
private:

};