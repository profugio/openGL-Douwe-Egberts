#include "mesh.h"

Mesh::Mesh()
{
	this->model = glm::mat4();
	//Standard material values
	this->material = { glm::vec3(0.2,0.2,0.1), glm::vec3(0.5,0.5,0.3), glm::vec3(0.7,0.7,0.7), 1024 };
}

Mesh::Mesh(std::vector<Vertex> vertices, Material material, GLuint texture_id)
{
	this->model = glm::mat4();
	//this->vertices = vertices;
	//this->material = material;
	//this->texture_id = texture_id;
}

void Mesh::CreateMesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs)
{
	this->vertices = vertices;
	this->normals = normals;
	this->uvs = uvs;
}

void Mesh::CreateMesh(std::vector<Vertex> vertices, Material material)
{
	//this->vertices = vertices;
	this->material = material;

}



