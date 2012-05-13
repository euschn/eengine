// sat_maker: wrapper class for computing a summed area table

#pragma once

#include "common.hpp"
#include "glm\glm.hpp"
#include "shader.hpp"
#include "sampler_state.h"

enum fbo_enums{PING, PONG};

class sat_maker {
public:
	//ctor/dtor
	sat_maker();
	virtual ~sat_maker();

	//members
	int width, height;
	GLuint fbo[2];
	GLuint texture[2];
	//GLuint depth_texture[2];
	shader_ptr gen;
	//sampler state
	sampler_state state;

	//functions
	bool init(const GLuint w = 512, const GLuint h = 512);
private:
};