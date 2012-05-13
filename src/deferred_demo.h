// class deferred_demo
// based class for deferred rendering demos

#pragma once

#include "common.hpp"
#include "simple_demo.h"
#include "skybox.h"
#include "shadow_kit.h"
#include "shader_manager.h"

class deferred_demo : public simple_demo {
public:
	deferred_demo() {}
	virtual ~deferred_demo() {cleanup();}
	
	virtual bool init();
	//part of the initialisation that is inherited
	virtual bool super_init();

protected:
	//-------
	//members
	//-------

	//gbuffer fbo and textures
	enum gbuffer_indices{ALBEDO, NORMAL, POSITION, DEPTH};
	GLuint gbuffer_fbo;
	GLuint gbuffer_tex[3];
	GLuint gbuffer_depth;
	
	//shadowmapping
	glm::mat4 lightview_matrix; //view from light position
	GLuint shadow_fbo;
	GLuint shadow_tex;
	GLuint shadow_depth;

	//skybox
	skybox sb;
	
	//----------
	// functions
	//----------
	void render_node(scene_node& node);
	bool init_gbuffer();
	virtual void process_keys(const float factor);
	void update_light(float factor);

	//prepare gbuffer framebuffer and draw stuff
	void draw_gbuffer(shared_ptr<scene_node> node, shader_ptr shp = shm::get("gbuffer_gen"));
	void render_gbuffer(shared_ptr<scene_node> node, shader_ptr shp);
	void render_shadowmap(node_ptr node, shadow_kit& sm_kit);
	void render_shadowmap_recursive(node_ptr node, shadow_kit& sm_kit);
	void render_pointlight(shared_ptr<scene_node> node);
	virtual void draw();	//main drawing method
	virtual void cleanup();
};