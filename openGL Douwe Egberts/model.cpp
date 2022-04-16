#pragma once
#include "model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
Model::Model()
{
	
}

void Model::Translate(glm::vec3 v)
{
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->model = glm::translate(meshes[i]->model, v);
	}
}

void Model::Rotate(float angle, glm::vec3 v)
{
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->model = glm::rotate(meshes[i]->model,glm::radians(angle), v);
	}
}

void Model::Scale(glm::vec3 v)
{
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->model = glm::scale(meshes[i]->model, v);
	}
}

void Model::CreateModelViewMatrix(glm::mat4 view)
{
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->mv = view * meshes[i]->model;
	}
}

void Model::DrawModel(GLuint program_id, glm::mat4 projection)
{
	glUseProgram(program_id);
	for (int i = 0; i < meshes.size(); i++) {
		//glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(meshes[i]->mv));
	}
	
}

void Model::DrawModel(GLuint program_id, unsigned int size, glm::mat4 projection)
{

}

void Model::SetAmbient(glm::vec3 ambient)
{
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->material.ambient_color = ambient;
	}
}

void Model::SetDiffuse(glm::vec3 diffuse)
{
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->material.diffuse_color = diffuse;
	}
}

void Model::SetSpecular(glm::vec3 specular)
{
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->material.specular = specular;
	}
}
