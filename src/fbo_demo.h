// cgse fbo demo
// author: Eugen Jiresch

#pragma once

#include "simple_demo.h"
#include "glm\glm.hpp"
#include "fbo.h"

class fbo_demo : public simple_demo {
public:
	fbo_demo();
	virtual ~fbo_demo();

	virtual bool init();

protected:
	virtual void draw();
	void render_node(shared_ptr<scene_node> node, shader_ptr shp);

	fbo reflection_fbo;
	//position of the reflecting quad
	glm::vec3 quad_position;
	glm::vec3 quad_normal;
	glm::mat4 quad_rotation;
};
