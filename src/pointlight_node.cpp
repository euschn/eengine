// class pointlight_node

#include "pointlight_node.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

//ctor
pointlight_node::pointlight_node()
{
	this->transformation_matrix = glm::mat4(1.0f);
	this->vao_id = 0;
	this->num_elements = 0;
	this->radius = 1.0f;
	_shader_type = SH_POINTLIGHT;
	shader = 0;
	color = glm::vec3(1.0f);
}


pointlight_node::pointlight_node(const glm::mat4& matrix, GLuint vao_id, GLuint num_elements, float radius, glm::vec3 position)
{
	this->transformation_matrix = matrix * glm::translate(position);
	this->vao_id = vao_id;
	this->num_elements = num_elements;
	this->radius = radius;
	_shader_type = SH_POINTLIGHT;
	shader = 0;
	color = glm::vec3(1.0f);
}

pointlight_node::pointlight_node(mesh_ptr mesh, float radius, glm::vec3 position)
{
	this->transformation_matrix = mesh->transformation_matrix * glm::translate(position);
	this->vao_id = mesh->vao_id;
	this->num_elements = mesh->num_elements;
	this->radius = radius;
	_shader_type = SH_POINTLIGHT;
	shader = 0;
	color = glm::vec3(1.0f);
}

//dtor
pointlight_node::~pointlight_node()
{
}

void pointlight_node::pre_render()
{
}

void pointlight_node::set_position(glm::vec3 position)
{}

glm::vec3 pointlight_node::get_position()
{
	glm::vec4 result = transformation_matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return glm::vec3(result.x, result.y, result.z);
}

void pointlight_node::render_instanced(const int num_instances)
{
	glBindVertexArray(vao_id);
	// Draw triangle in VAO
	//glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, 0, num_instances);

	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
