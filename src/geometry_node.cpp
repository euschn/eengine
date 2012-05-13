#include "geometry_node.h"

geometry_node::geometry_node()
{
	transformation_matrix = glm::mat4(1.0f);
	_shader_type = SH_NONE;
}

//create a new geometry node
geometry_node::geometry_node(glm::mat4& matrix,  GLuint vao_id, GLuint num_elements, int shader_type)
{
	transformation_matrix = matrix;
	this->vao_id = vao_id;
	this->num_elements = num_elements;
	_shader_type = shader_type;
}

//draw the vao specified by id with the supplied shader
//requires:
// - active ogl context
// - bound shader
//this needs to be done by the object traversion the scenegraph!
void geometry_node::render()
{

	glBindVertexArray(vao_id);
	// Draw triangle in VAO
	glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}