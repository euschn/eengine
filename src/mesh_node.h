#pragma once

#include "geometry_node.h"
#include "common.hpp"
#include "shader.hpp"

class mesh_node : public geometry_node {
protected:
	GLuint texture_id;
	GLuint texture_norm_id;

	//bounding sphere dimension
	glm::vec3 dimension;
public:
	//vbos
	std::vector<GLuint> vbos;
	//primitive type
	GLenum primitive_type;

	//functions
	mesh_node();
	mesh_node(const glm::mat4& matrix, GLuint vao_id, GLuint num_elements, int shader_type = SH_TEXTURE, const glm::vec3& dim = glm::vec3(0.0f));
	mesh_node(std::tr1::shared_ptr<mesh_node> mesh); //copy ctor
	virtual ~mesh_node();

	void set_texture_id(GLuint id) {texture_id = id;}
	void set_texture_norm_id(GLuint id) {texture_norm_id = id;}
	GLuint get_texture_id() {return texture_id;}
	GLuint get_norm_texture_id() {return texture_norm_id;}
	virtual glm::vec3 get_dimension() const {return dimension;}
	void set_dimension(const glm::vec3 dim) {dimension = dim;}
	//return a scalar as dimension
	virtual float get_scalar_dimension() const {return glm::length(dimension);}

	//before rendering, setup texture
	virtual void pre_render();
	//render with the object
	virtual void render();
	virtual void render(const GLint primitive);
	//get the normal information set up
	virtual void pre_normal();

};

typedef std::tr1::shared_ptr<mesh_node> mesh_ptr;