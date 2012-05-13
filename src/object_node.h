#pragma once

#include "geometry_node.h"
#include "common.hpp"
#include "shader.hpp"

class object_node : public geometry_node {
private:
	GLuint texture_id;
	GLuint texture_norm_id;

	//bounding sphere dimension
	glm::vec3 dimension;
public:


	//functions
	object_node();
	object_node(glm::mat4& matrix, GLuint vao_id, GLuint num_elements, int shader_type = SH_TEXTURE, const glm::vec3& dim = glm::vec3(0.0f));
	virtual ~object_node() {}

	void set_texture_id(GLuint id) {texture_id = id;}
	void set_texture_norm_id(GLuint id) {texture_norm_id = id;}
	virtual glm::vec3 get_dimension() const {return dimension;}
	void set_dimension(const glm::vec3 dim) {dimension = dim;}

	//before rendering, setup texture
	virtual void pre_render();
	//render with the object
	virtual void render();
	virtual void render(const GLint primitive);
	//get the normal information set up
	virtual void pre_normal();

};