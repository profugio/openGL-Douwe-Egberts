#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct Vertex {
	glm::vec3 vertices;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;
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
	glm::mat4 model;
	glm::mat4 mv;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> color;
	Material material;
	GLuint texture_id;
	bool isPrimitive;
	GLuint vao;
	GLuint vbo;
	Mesh();
	void CreateMesh(std::vector<glm::vec3>, std::vector<glm::vec3>, std::vector<glm::vec2>);


};

