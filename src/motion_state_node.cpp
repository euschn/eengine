#include "motion_state_node.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 


motion_state_node::motion_state_node()
{
}

motion_state_node::motion_state_node(mesh_node& node)
{
	vbos = std::vector<GLuint>(2);
	transformation_matrix = node.transformation_matrix;
	this->vao_id = node.vao_id;
	this->num_elements = node.num_elements;
	_shader_type = node.shader_type();
	texture_id= node.get_texture_id();
	//TODO is this dimension setting correct?
	this->dimension = node.get_dimension();
}

motion_state_node::motion_state_node(mesh_ptr node)
{
	vbos = std::vector<GLuint>(2);
	transformation_matrix = node->transformation_matrix;
	this->vao_id = node->vao_id;
	this->num_elements = node->num_elements;
	_shader_type = node->shader_type();
	texture_id= node->get_texture_id();
	//TODO is this dimension setting correct?
	this->dimension = node->get_dimension();
}

//create a motion state node that has nothing to draw
motion_state_node::motion_state_node(const glm::mat4& mat)
{
	vbos = std::vector<GLuint>(2);
	transformation_matrix = mat;
	this->vao_id = 0;
	this->num_elements = 0;
	_shader_type = SH_NONE;
	texture_id = 0;
	//TODO is this dimension setting correct?
	this->dimension = glm::vec3(0);
}

void motion_state_node::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans.setFromOpenGLMatrix(glm::value_ptr(transformation_matrix));
}

void motion_state_node::setWorldTransform(const btTransform &worldTrans)
{
	float f[16];
	worldTrans.getOpenGLMatrix(f);
	transformation_matrix = glm::mat4( f[0], f[1], f[2], f[3], 
										f[4], f[5], f[6], f[7], 
										f[8], f[9], f[10], f[11], 
										f[12], f[13], f[14], f[15] );
}