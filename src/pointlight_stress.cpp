
#include "pointlight_stress.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"
#include "pointlight_node.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
//swizzling
#include <glm/gtc/swizzle.hpp>
//time for random
#include <time.h>

//use carefully
using namespace glm;

pointlight_stress::pointlight_stress() :
	batch_lights(0),
	batch_matrix(0),
	batch_color_radius(0),
	directional_light_mask(1.0f)
{
	pointlight_root = std::tr1::shared_ptr<scene_node>(new scene_node());
}

bool pointlight_stress::init()
{
	if (!super_init()) return false;

	//different light vector
	light_0 = glm::normalize(glm::vec3(-1.0f, 1.0f, -0.5f));

	//textures
	txm::load("../textures");
	txm::load("../textures/titles");
	//txm::load("../models/sponza/textures");

	//models
	shader_ptr gbg = shader_manager::get("gbuffer_gen");
	mm::load_single("../models/Tree3.obj", gbg,
		glm::mat4(1.f), texture_manager::get("bark1"));
	mm::load_single("../models/cube2.obj", gbg,
		glm::translate(-4.f, 0.f, 0.f), texture_manager::get("cloth04"));
	mesh_manager::load_single("../models/quad.obj", gbg, 
		glm::translate(0.f, 0.5f, 0.f),
		texture_manager::get("wood19"));
	//mesh_manager::load_single("../models/sponza/sponza.obj", gbg, scale(0.1f, 0.1f, 0.1f), txm::get("white"), "sponza");
	//mm::load_single("../models/celtic_cross_Tombstone.obj", gbg, txm::get("stone09"), mat4(1.0f), "cross");

	//instanced pointlights
	shm::load_single("../shader/pl_instanced");
	shader_ptr pl = shm::get("pl_instanced");
	pl->bind();
	pl->set_uniform("perspective_matrix", perspective_matrix);
	pl->set_uniform("albedo_texture", 0);
	pl->set_uniform("normal_texture", 1);
	pl->set_uniform("position_texture", 2);
	//instanced pointlight positions
	pl->set_uniform("pl_positions", 3);
	pl->set_uniform("num_instances", cfg::get("numlights"));

	//scenegraph setup
	root->add_child(mm::get("quad"));
	root->add_child(mm::get("Tree3"));
	//root->add_child(mm::get("sponza") );
	//make a grid of trees
	//srand (time(NULL));
	srand(0);
	int dim = 10;
	int scale = 4;
	for (int x=0; x < 10; ++x) {
		for (int y=0; y < 10; ++y) {
			node_ptr p(new scene_node());
			//random float between 1 and 0
			float r = (float)rand() / RAND_MAX;
			p->transformation_matrix = glm::translate(static_cast<float>(x * scale - (dim * scale / 2)), 0.0f, static_cast<float>(y * scale - (dim * scale)))
				* glm::rotate(360.0f * r, 0.f, 1.f, 0.f);
			p->add_child(mm::get("Tree3"));
			root->add_child(p);
		}
	}

	//lights
	// many pointlights connected to a common root node
	float pl_size = config_manager::get("pl_size");
	shared_ptr<pointlight_node> pl1( new pointlight_node(mm::get("ico"), pl_size, glm::vec3(0.0f) ) );
	pl1->set_color(glm::vec3(1.0f, 0.0f, 0.0f));
	mm::add(pl1, "pl1");
	root->add_child(pointlight_root);
	pointlight_root->transformation_matrix = glm::translate(0.f, -1.5f, static_cast<float>(scale - (dim * scale / 2)) );
	pointlight_root->add_child(mm::get("pl1"));
	//many lights
	int numlights = config_manager::get_int("numlights");
	//array for instanced lights
	std::vector<vec3> pl_positions(numlights);
	float light_dim = 15;
	for (int i=0; i < numlights; ++i) {
		//random float between 1 and 0
		float r = (float)rand() / RAND_MAX;
		float g = (float)rand() / RAND_MAX;
		float b = (float)rand() / RAND_MAX;
		float dist = (float)rand() / RAND_MAX;
		pointlight_ptr p(new pointlight_node(mm::get("ico"), pl_size, glm::vec3(0.0f)));
		p->set_color(r,g,b);
		p->transformation_matrix = glm::rotate(360.0f * static_cast<float>(i)/numlights, 0.f, 1.f, 0.f)
			* glm::translate(light_dim * dist * 0.9f + light_dim * 0.1f, 0.0f, 0.0f);
		pointlight_root->add_child(p);
		//instancing preparation
		vec3 position = glm::swizzle<X,Y,Z>(pointlight_root->transformation_matrix * p->transformation_matrix * glm::vec4(0.0f, 0.f, 0.f, 1.f));
		pl_positions[i] = position;
	}

	//generate texture

	glGenTextures(1, &tex_pl_positions);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_1D, tex_pl_positions);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, numlights, 0, GL_RGB, GL_FLOAT, &pl_positions[0]);

	glBindTexture(GL_TEXTURE_1D, 0);
	glActiveTexture(GL_TEXTURE0);

	scene_select = 0;

	return true;
}

void pointlight_stress::draw()
{

	switch (scene_select) {
	case 1:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0,0,width,height);
		draw_postprocess(shm::get("pp_echo"), txm::get("pl"));
		break;
	case 0:
	millisecs_1 = glfwGetTime();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);
	load_identity(normal_matrix);

	look_at(view_matrix, cam_eye, cam_center, cam_up);
	//TODO: have proper normal matrix
	glm::mat4 normal_matrix = view_matrix * modelview_matrix;//glm::transpose(glm::inverse(view_matrix * modelview_matrix));

	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,width, height);

	draw_gbuffer(root);

	//glFinish();
	millisecs_1 = (glfwGetTime() - millisecs_1) * 1000;

	// --------------
	// deferred stage
	// --------------
	millisecs_2 = glfwGetTime();

	glEnable(GL_FRAMEBUFFER_SRGB);
	glViewport(0,0,width, height);

	//lighting
	shader_ptr sh = shader_manager::get("phong");
	sh->bind();
	sh->set_uniform("directional_light", swizzle<X,Y,Z>(normal_matrix * glm::vec4(glm::normalize(light_0), 0.0f)) );
	sh->set_uniform("color_mask", glm::vec4(directional_light_mask, 1.0f) );
	draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION]);

	// -------------------
	//see all the gbuffers
	// -------------------
	//glViewport(0, 0, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), gbuffer_tex[ALBEDO], 0);

	//glViewport(0, height/5, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), gbuffer_tex[NORMAL], 0);

	//glViewport(0, height/5*2, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), gbuffer_tex[POSITION], 0);

	
	//---------------
	//pointlight test
	//---------------
	glViewport(0,0,width, height);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	shader_ptr pl = shader_manager::get("pointlight");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO] );
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer_tex[NORMAL] );
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gbuffer_tex[POSITION] );
	//texture of positions
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_1D, tex_pl_positions);
	glActiveTexture(GL_TEXTURE0);

	//if (config_manager::get_bool("light_batch_test")) {
	//	render_pointlight_batch(pointlight_root);
	//}
	//else {
		render_pointlight(pointlight_root);
	//}
	//instance test
	//render_pointlight_instanced(mm::get("pl1"), cfg::get_int("numlights"));
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_FRAMEBUFFER_SRGB);
	//glFinish();
	millisecs_2 = (glfwGetTime() - millisecs_2) * 1000;

	break;
	}

}

void pointlight_stress::render_pointlight_instanced(shared_ptr<scene_node> node, const int num_instances)
{
	//save current transformation
	push_modelview_matrix();

	//apply transformation of this node
	modelview_matrix = modelview_matrix * node->transformation_matrix;

	//setup shader and render node
	shader_ptr pl = shm::get("pl_instanced");

	switch (node->shader_type())
	{
	case SH_POINTLIGHT:
		bool inside_light = false;
		float light_radius = node->get_scalar_dimension();
		glm::vec3 light_position = glm::swizzle<glm::X, glm::Y, glm::Z>(modelview_matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		// if we are inside the sphere representing the light radius
		// draw the back face of the sphere
		float length = glm::length(light_position - cam_eye);
		//TODO: inside/outside test works, but may have small visual glitches
		if (length < (light_radius + near_far.x)) {
			inside_light = true;
			glCullFace(GL_FRONT);
		}

		pl->bind();
		pl->set_uniform("modelview_matrix", view_matrix * modelview_matrix * glm::scale(light_radius, light_radius, light_radius));
		pl->set_uniform("light_radius", light_radius);
		pl->set_uniform("light_color", node->get_dimension());
		//viewspace light position
		//glm::vec3 viewspace_light_position = glm::swizzle<glm::X, glm::Y, glm::Z>( view_matrix * glm::vec4(light_position, 1.0f) );
		//pl->set_uniform("light_position", viewspace_light_position);

		//node->pre_render();

		node->render_instanced(num_instances);

		//
		if (inside_light) {
			glCullFace(GL_BACK);
			inside_light = false;
		}
		break;
	/*
	default:
		break;*/
	}
	
	//dont render children for now
	//for (auto it = node->children.begin(); it != node->children.end(); it++)
	//{
	//	render_pointlight(*it);
	//}

	pop_modelview_matrix();
}

void pointlight_stress::render_pointlight_batch(shared_ptr<scene_node> node)
{
	// clear batch arrays
	batch_lights.clear(); batch_matrix.clear(); batch_color_radius.clear();

	// fill batch arrays
	prepare_pointlight_batch(node);

	// render
	// TODO: Culling is not correct!
	glCullFace(GL_FRONT);
	for (unsigned int i=0; i < batch_lights.size(); ++i) {

		glm::vec4 color_radius = batch_color_radius[i];
		float light_radius = color_radius.w;
		glm::mat4 matrix = batch_matrix[i];

		shader_ptr pl = shm::get("pointlight");
		pl->bind();
		pl->set_uniform("modelview_matrix", matrix * glm::scale(light_radius, light_radius, light_radius));
		pl->set_uniform("light_radius", light_radius);
		pl->set_uniform("light_color", swizzle<X,Y,Z>(color_radius) );
		//viewspace light position
		glm::vec3 viewspace_light_position = glm::swizzle<glm::X, glm::Y, glm::Z>( matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) );
		pl->set_uniform("light_position", viewspace_light_position);

		batch_lights[i]->render();
	}
	glCullFace(GL_BACK);

}

void pointlight_stress::prepare_pointlight_batch(shared_ptr<scene_node> node)
{
	//save current transformation
	push_modelview_matrix();

	//apply transformation of this node
	modelview_matrix = modelview_matrix * node->transformation_matrix;

	//setup shader and render node
	shader_ptr pl = shm::get("pointlight");

	switch (node->shader_type())
	{
	case SH_POINTLIGHT:
		bool inside_light = false;
		//float light_radius = node->get_scalar_dimension();
		//glm::vec3 light_position = glm::swizzle<glm::X, glm::Y, glm::Z>(modelview_matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		// if we are inside the sphere representing the light radius
		// draw the back face of the sphere
		//float length = glm::length(light_position - cam_eye);
		//TODO: inside/outside test works, but may have small visual glitches
		//if (length < (light_radius + near_far.x)) {
		//	inside_light = true;
		//	glCullFace(GL_FRONT);
		//}

		//pl->bind();
		//pl->set_uniform("modelview_matrix", view_matrix * modelview_matrix * glm::scale(light_radius, light_radius, light_radius));
		//pl->set_uniform("light_radius", light_radius);
		//pl->set_uniform("light_color", node->get_dimension());
		////viewspace light position
		//glm::vec3 viewspace_light_position = glm::swizzle<glm::X, glm::Y, glm::Z>( view_matrix * glm::vec4(light_position, 1.0f) );
		//pl->set_uniform("light_position", viewspace_light_position);

		//node->pre_render();
		//node->render();
		//do not render, but store in batch arrays
		batch_lights.push_back(node);
		batch_matrix.push_back(view_matrix * modelview_matrix);
		batch_color_radius.push_back(glm::vec4(node->get_dimension(), node->get_scalar_dimension()));

		//
		//if (inside_light) {
		//	glCullFace(GL_BACK);
		//	inside_light = false;
		//}
		break;
	/*
	default:
		break;*/
	}
	
	//render children
	for (auto it = node->children.begin(); it != node->children.end(); it++)
	{
		prepare_pointlight_batch(*it);
	}

	pop_modelview_matrix();
}

void pointlight_stress::process_keys(const float factor)
{
	update_light(factor);
	pointlight_root->transformation_matrix = pointlight_root->transformation_matrix * glm::rotate(factor, 0.f, 1.f, 0.f);
	//pointlight_root->transformation_matrix = glm::translate(move_by_key(factor)) * pointlight_root->transformation_matrix;

	//color mask
	directional_light_mask += move_by_key(factor * 0.1f, '+', '-', '+', '-', '+', '-');

	update_camera_path(factor);

		//scene select 0
	if (glfwGetKey('3') && !pushed['3']) {
		scene_select = 0;
		pushed['3'] = true;
	}

	if (glfwGetKey('3') == GLFW_RELEASE) {
		pushed['3'] = false;
	}

	//scene select 1
	if (glfwGetKey('4') && !pushed['4']) {
		scene_select = 1;
		pushed['4'] = true;
	}

	if (glfwGetKey('4') == GLFW_RELEASE) {
		pushed['4'] = false;
	}

}

void pointlight_stress::update_light(float factor)
{
	float weight = 0.33f;
	glm::vec3 movement(0.0f);
	if (glfwGetKey(GLFW_KEY_RIGHT))
	{
		//light_0.x += factor;
		movement.x += factor;
		//poly_offset_factor += factor * weight;
	}	
	if (glfwGetKey(GLFW_KEY_LEFT))
	{
		//light_0.x -= factor;
		movement.x -= factor;
		//poly_offset_factor -= factor * weight;
	}
	if (glfwGetKey(GLFW_KEY_UP))
	{
		//light_0.z -= factor;
		movement.z -= factor;
		//poly_offset_units += factor * weight;
	}	
	if (glfwGetKey(GLFW_KEY_DOWN))
	{
		//light_0.z += factor;
		movement.z += factor;
		//poly_offset_units -= factor * weight;
	}
	if (glfwGetKey(GLFW_KEY_PAGEDOWN))
	{
		//light_0.z -= factor;
		movement.y -= factor;
		//poly_offset_units += factor * weight;
	}	
	if (glfwGetKey(GLFW_KEY_PAGEUP))
	{
		//light_0.z += factor;
		movement.y += factor;
		//poly_offset_units -= factor * weight;
	}

	//light_1 += movement;
	pointlight_root->transformation_matrix = pointlight_root->transformation_matrix * glm::translate(movement);
}


void pointlight_stress::cleanup()
{
	if (tex_pl_positions > 0) glDeleteTextures(1, &tex_pl_positions);
}