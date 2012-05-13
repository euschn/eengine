// shadow_kit: wrapper class for drawing a shadowmap

#pragma once

#include "common.hpp"
#include "scenenode.h"
#include "glm\glm.hpp"
#include "shader.hpp"

class shadow_kit {
public:
	//ctor/dtor
	shadow_kit();
	virtual ~shadow_kit();

	//members
	int width, height;
	glm::vec2 near_far;
	glm::mat4 perspective_matrix;
	glm::mat4 view_matrix;
	GLuint fbo;
	GLuint texture;
	GLuint depth_texture;
	shader_ptr gen;

	//functions
	bool init();
	glm::mat4 shadow_matrix();

private:
};