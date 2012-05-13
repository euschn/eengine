// small demo to test summed area tables

#pragma once

#include "deferred_demo.h"
#include "sampler_state.h"
#include "sat_maker.h"

class sat_test : public deferred_demo {
public:
	sat_test();
	virtual ~sat_test();

	virtual bool init();
protected:
	virtual void draw();
	virtual void process_keys(const float factor);
	//init test texture
	bool init_texture();

	//members
	//test texture
	GLuint test_texture;
	GLuint test_width, test_height;
	//sat maker
	sat_maker sam;
	//sampler state test
	sampler_state test_sampler;
	float filter_width;
	bool filter_push;

};