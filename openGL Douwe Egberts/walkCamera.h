#pragma once
#include "camera.h"

class WalkCamera : public Camera {
public:
	WalkCamera();
	void Move(int, int);
};