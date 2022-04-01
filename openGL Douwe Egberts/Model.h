#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/freeglut.h>

struct Material {
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular;
	float power;
};


class Model {
private:
	glm::mat4 model;
	glm::vec3 vertices;
	glm::vec2 uvs;
	glm::vec3 normals;
	glm::mat4 mv;

	GLuint texture;
	GLuint vao;

	Material material;
public:

};