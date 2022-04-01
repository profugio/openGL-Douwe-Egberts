#pragma once
#include "camera.h"

class DroneCamera : public Camera {
public:
	void Move(int, int);
	DroneCamera();
};