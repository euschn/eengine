// path instancing demo
// based on prideout.net tutorial

#pragma once

#include "common.hpp"
#include "deferred_demo.h"

class path_instance_demo : public deferred_demo {
public:
	path_instance_demo();
	virtual ~path_instance_demo();

	virtual bool init();

protected:
	//functions
	virtual void draw();
	virtual void process_keys(const float factor);
	//build path texture
	void init_path_texture();

	//members
	node_ptr path_root;
	GLuint path_texture;
	GLfloat pathScale;
	GLfloat pathOffset;
};