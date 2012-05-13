// bloom demo
// author: Eugen Jiresch

#pragma once

#include "common.hpp"
#include "deferred_demo.h"
#include "fbo.h"

class bloom_demo : public deferred_demo {
public:
	bloom_demo();
	virtual ~bloom_demo();
	
	virtual bool init();
protected:
	virtual void draw();
	virtual void process_keys(const float factor);

	//members
	fbo post_process_fbo;
	fbo bloom_vertical, bloom_horizontal;
	int bloom_width, bloom_height;

};