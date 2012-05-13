// wrapper class for sampler state

#include "sampler_state.h"

sampler_state::sampler_state()
{
	_state = 0;
	_bound = false;
}
sampler_state::~sampler_state()
{
	if (_state > 0) {
		glDeleteSamplers(1, &_state);
	}
}

void sampler_state::init()
{
	glGenSamplers(1, &_state);
}

void sampler_state::bind(const GLuint tex_unit)
{
	glBindSampler(tex_unit, _state);
	_bound = true;
}

void sampler_state::unbind(const GLuint tex_unit)
{
	glBindSampler(tex_unit, 0);
	_bound = false;
}