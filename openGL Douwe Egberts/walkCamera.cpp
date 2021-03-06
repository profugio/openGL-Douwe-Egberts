#include "walkCamera.h"
#include <glm/gtc/matrix_transform.hpp>

WalkCamera::WalkCamera() {
	yaw = -90.0f;
	pitch = 0.0f;
	cameraSpeed = 0.02f;

	cameraPos = glm::vec3(0.0f, 7.0f, 3.0f);
	cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	CalculateCameraDirection();

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	//Create view and projection matrix
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(
		glm::radians(45.0f),
		1.0f * 800 / 600, 0.1f,
		1000.0f);
}

//Change view matrix to make it appear as if you're walking/looking around
void WalkCamera::Move(int key, int deltaTime) {
	if (key == 119) { // w
		cameraPos += cameraSpeed * deltaTime * glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
	}else if (key == 115) { //s
		cameraPos -= cameraSpeed * deltaTime * glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
	}else if (key == 105) {  //i
		if (pitch < 89) {
			pitch += 0.1f * deltaTime;
			CalculateCameraDirection();
		}
	}else if (key == 106) { //j
		yaw -= 0.1f * deltaTime;
		CalculateCameraDirection();
	}else if (key == 107) { //k
		if (pitch > -91) {
			pitch -= 0.1f * deltaTime;
			CalculateCameraDirection();
		}
	}else if (key == 108) { //l
		yaw += 0.1f * deltaTime;
		CalculateCameraDirection();
	}
	else {
		Camera::Move(key, deltaTime);
	}
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}