// pointlight stresstest

#pragma once

#include "deferred_demo.h"


class pointlight_stress : public deferred_demo
{
public:
	pointlight_stress();
	virtual ~pointlight_stress() {cleanup();}

	virtual bool init();

protected:
	virtual void draw();
	virtual void cleanup();
	virtual void update_light(const float factor);
	virtual void process_keys(const float factor);

	//pointlight instancing
	void render_pointlight_instanced(shared_ptr<scene_node> node, const int num_instances);
	//faster version of drawing lights
	void prepare_pointlight_batch(shared_ptr<scene_node> node);
	void render_pointlight_batch(shared_ptr<scene_node> node);

	//members
	glm::vec3 directional_light_mask;
	std::vector<node_ptr> batch_lights;
	std::vector<glm::mat4> batch_matrix;
	std::vector<glm::vec4> batch_color_radius;
	std::tr1::shared_ptr<scene_node> pointlight_root;
	//instance texture
	GLuint tex_pl_positions;
	//scene selection
	int scene_select;

};