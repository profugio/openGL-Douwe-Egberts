#include "droneCamera.h"
#include <glm/gtc/matrix_transform.hpp>



DroneCamera::DroneCamera()
{
	yaw = 0.0f;
	pitch = -90.0f;
	cameraSpeed = 0.02f;

	cameraPos = glm::vec3(0.0f, 50.0f, 3.0f);
	cameraTarget = glm::vec3(20.0f, 50.0f, 0.0f);
	CalculateCameraDirection();

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
	
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(
		glm::radians(45.0f),
		1.0f * 800 / 600, 0.1f,
		1000.0f);	
}

//Change view matrix to make it appear as if you're walking/looking around
void DroneCamera::Move(int key, int deltaTime) {
	if (key == 119) { // w
		cameraPos += cameraSpeed * deltaTime * cameraUp;
	}else if (key == 115) { //s
		cameraPos -= cameraSpeed * deltaTime * cameraUp;
	}else if (key == 113) {//q
		cameraPos -= cameraSpeed * deltaTime * cameraFront;
	}else if (key == 101) {//e
		cameraPos += cameraSpeed * deltaTime * cameraFront;
	}
	else {
		Camera::Move(key, deltaTime);
	}
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}



