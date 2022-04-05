#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct Vertex {
	glm::vec3 vertices;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Material {
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular;
	float power;
};
class Mesh
{
public:
	std::vector<Vertex> vertices;
	GLuint texture;
	Material material;
	Mesh(std::vector<Vertex>, GLuint);

};

