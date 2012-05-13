#include "mesh_node.h"
#include "boost/foreach.hpp"

mesh_node::mesh_node()
{
	vbos = std::vector<GLuint>(2);
	transformation_matrix = glm::mat4(1.0f);
	_shader_type = SH_NONE;
	primitive_type = GL_TRIANGLES;
}

//create a new object node
mesh_node::mesh_node(const glm::mat4& matrix,  GLuint vao_id, GLuint num_elements, int shader_type, const glm::vec3& dimension)
{
	vbos = std::vector<GLuint>(2);
	transformation_matrix = matrix;
	this->vao_id = vao_id;
	this->num_elements = num_elements;
	_shader_type = shader_type;
	texture_id=0;
	this->dimension = dimension;
	primitive_type = GL_TRIANGLES;
}


//create a new object node
mesh_node::mesh_node(mesh_ptr mesh)
{
	vbos = std::vector<GLuint>(2);
	transformation_matrix = mesh->transformation_matrix;
	this->vao_id = mesh->vao_id;
	this->num_elements = mesh->num_elements;
	_shader_type = mesh->shader_type();
	texture_id= mesh->get_texture_id();
	//todo is this correct?
	this->dimension = mesh->get_dimension();
	primitive_type = GL_TRIANGLES;
}

mesh_node::~mesh_node() 
{
	glDeleteVertexArrays(1, &vao_id);
	BOOST_FOREACH(GLuint v, vbos)
	{
		glDeleteBuffers(1, &v);
	}
}

void mesh_node::pre_render()
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void mesh_node::pre_normal()
{
	glBindTexture(GL_TEXTURE_2D, texture_norm_id);
}

//draw the vao specified by id with the supplied shader
//requires:
// - active ogl context
// - bound shader
//this needs to be done by the object traversion the scenegraph!
void mesh_node::render()
{

	glBindVertexArray(vao_id);
	// Draw triangle in VAO
	glDrawElements(primitive_type, num_elements, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

//this needs to be done by the object traversion the scenegraph!
void mesh_node::render(const GLint primitive)
{

	glBindVertexArray(vao_id);
	// Draw triangle in VAO
	glDrawElements(primitive, num_elements, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}