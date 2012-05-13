// simple rt demo
// author: Eugen Jiresch

#pragma once

#include "simple_demo.h"

class simple_rt_demo : public simple_demo {
public:
	virtual bool init();

protected:
	virtual void draw();
	void init_background_texture();

	GLuint background_tex;
};