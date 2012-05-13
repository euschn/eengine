// testing mipmap stuff

#pragma once

#include "simple_demo.h"

class mipmap_test : public simple_demo {
public:

	virtual bool init();

protected:
	GLuint mm_tex; //mipmapped texture

	void init_mm_texture();
	virtual void draw();
};