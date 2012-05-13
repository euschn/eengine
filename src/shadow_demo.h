#pragma once

#include "common.hpp"
#include "deferred_demo.h"
#include "shadow_kit.h"
#include "skybox.h"

class shadow_demo : public deferred_demo {
public:
	shadow_demo();
	virtual ~shadow_demo();

	virtual bool init();
	virtual bool super_init();

protected:
	//members
	shadow_kit shadow;
	sat_maker sam, sam2;
	float sat_filter_width, sat_filter_width2;
	sampler_state filter_state;
	int scene_select;
	//basic sat scene
	GLuint test_texture;
	GLuint test_width, test_height;
	//sampler state test
	sampler_state test_sampler;

	//functions
	virtual void draw();
	virtual void process_keys(const float factor);
	void draw_shadow_scene();
	void draw_sat_scene();
};