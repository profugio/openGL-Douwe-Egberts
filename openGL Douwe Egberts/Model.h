#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/freeglut.h>
#include "mesh.h"



class Model {
private:

public:
	std::vector<Mesh> meshes;

	Model();
	~Model();
	void Translate(glm::vec3);
	void Rotate(float, glm::vec3);
	void Scale(glm::vec3);
};