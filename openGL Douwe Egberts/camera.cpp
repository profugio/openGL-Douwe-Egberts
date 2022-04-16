#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>



void Camera::CalculateCameraDirection()
{
    cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection.y = sin(glm::radians(pitch));
    cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraDirection);
}

void Camera::Move(int key, int deltaTime) {

    if (key == 97) { //a
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed * deltaTime);
    }
    if (key == 100) { //d
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed * deltaTime);
    }
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}