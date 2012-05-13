// sampler state wrapper class

#pragma once

#include "common.hpp"

class sampler_state {
public:
	sampler_state();
	virtual ~sampler_state();

	// methods
	virtual void init();
	virtual void bind(const GLuint tex_unit = 0);
	virtual void unbind(const GLuint tex_unit = 0);
	bool bound() const {return _bound;}
	//set parameters
	void parameteri(const GLuint target, const GLuint value) { glSamplerParameteri(_state, target, value); }
	void parameterf(const GLuint target, const float value) { glSamplerParameterf(_state, target, value); }

protected:
	GLuint _state;
	bool _bound;
};