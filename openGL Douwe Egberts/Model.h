#pragma once
#include <glm/glm.hpp>
#include <vector>
//#include <GL/freeglut.h>
#include "mesh.h"



class Model {
private:

public:
	std::vector<Mesh*> meshes;
	bool isRoof;
	Model();
	~Model();
	void Translate(glm::vec3);
	void Rotate(float, glm::vec3);
	void Scale(glm::vec3);
	void CreateModelViewMatrix(glm::mat4);
	void DrawModel(GLuint, glm::mat4);
	void DrawModel(GLuint, unsigned int, glm::mat4);
	void SetAmbient(glm::vec3);
	void SetDiffuse(glm::vec3);
	void SetSpecular(glm::vec3);
};