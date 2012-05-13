#include "object_node.h"

object_node::object_node()
{
	transformation_matrix = glm::mat4(1.0f);
	_shader_type = SH_NONE;
}

//create a new object node
object_node::object_node(glm::mat4& matrix,  GLuint vao_id, GLuint num_elements, int shader_type, const glm::vec3& dimension)
{
	transformation_matrix = matrix;
	this->vao_id = vao_id;
	this->num_elements = num_elements;
	_shader_type = shader_type;
	texture_id=0;
	this->dimension = dimension;
}


void object_node::pre_render()
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void object_node::pre_normal()
{
	glBindTexture(GL_TEXTURE_2D, texture_norm_id);
}

//draw the vao specified by id with the supplied shader
//requires:
// - active ogl context
// - bound shader
//this needs to be done by the object traversion the scenegraph!
void object_node::render()
{

	glBindVertexArray(vao_id);
	// Draw triangle in VAO
	glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

//this needs to be done by the object traversion the scenegraph!
void object_node::render(const GLint primitive)
{

	glBindVertexArray(vao_id);
	// Draw triangle in VAO
	glDrawElements(primitive, num_elements, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}