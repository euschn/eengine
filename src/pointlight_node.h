// node for pointlights

#pragma once

#include "mesh_node.h"

class pointlight_node : public mesh_node
{
public:
	//light parameters
	float radius;
	glm::vec3 color;

	//ctor dtor
	pointlight_node();
	pointlight_node(const glm::mat4& matrix, GLuint vao_id, GLuint num_elements, float radius = 1.0f, glm::vec3 position = glm::vec3(0.0f));
	pointlight_node(mesh_ptr mesh, float radius = 1.0f, glm::vec3 position = glm::vec3(0.0f));
	virtual ~pointlight_node();

	//shader setter
	void set_shader(shader_ptr shader) { this->shader = shader; }
	void set_position(glm::vec3 position);
	//HACK: return color instead of dimension
	virtual glm::vec3 get_dimension() const {return color;}
	//return a scalar as dimension
	virtual float get_scalar_dimension() const {return radius;}
	virtual void set_scalar_dimension(const float dim) { radius = dim;}
	glm::vec3 get_position();
	//set color
	void set_color (const glm::vec3 color) { this->color = color;}
	void set_color (const float r, const float g, const float b) { this->color = glm::vec3(r,g,b);}
	//instanced rendering
	virtual void render_instanced(const int num_instances);


protected:
	shader_ptr shader;

	//methods
	virtual void pre_render();

};
typedef std::tr1::shared_ptr<pointlight_node> pointlight_ptr;