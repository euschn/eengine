#include "shadow_demo.h"

#include "deferred_demo.h"
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

//use carefully
using namespace glm;

shadow_demo::shadow_demo() 
{
	near_far.x = 0.5;
	near_far.y = 400;
	perspective_matrix = glm::perspective(45.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), near_far.x, near_far.y);
}
shadow_demo::~shadow_demo() {}

bool shadow_demo::super_init() 
{
	return deferred_demo::super_init();
}

bool shadow_demo::init()
{
	if (!super_init()) return false;

	
	//textures
	txm::load("../textures");
	txm::load("../textures/titles");
	//txm::load("../models/sponza/textures");

	//models
	shader_ptr gbg = shader_manager::get("gbuffer_gen");
	mm::load_single("../models/Tree3.obj", gbg,
		glm::mat4(1.f), texture_manager::get("bark1"));
	mm::load_single("../models/Tree4.obj", gbg,
		glm::translate(6.0f, -3.0f, 0.f) * glm::scale(7.0f, 7.0f, 7.0f),
		txm::get("bark1"));
	mm::load_single("../models/cube2.obj", gbg,
		glm::translate(-4.f, 0.f, 0.f), texture_manager::get("cloth04"), "cube2");
	mesh_manager::load_single("../models/quad.obj", gbg,
		texture_manager::get("wood19"));
	//mesh_manager::load_single("../models/sponza/sponza.obj", gbg, scale(0.01f, 0.01f, 0.01f), txm::get("white"), "sponza");
	//mm::get("quad")->transformation_matrix = glm::translate(0.0f, -2.0f, 0.0f) * glm::scale(0.15f, 0.15f, 0.15f);
	
	//scenegraph setup
	root->add_child(mm::get("Tree3"));
	root->add_child(mm::get("Tree4"));
	root->add_child(mm::get("cube2"));
	root->add_child(mm::get("quad"));
	//root->add_child(mm::get("sponza"));


	//skybox
	sb.init();
	//sb.init("../textures/skybox/city");

	//set camera
	//cam_eye.y += 13.0f;
	//cam_eye.x -= 4.0f;

	//shadow
	shadow.width = cfg::get_int("test_width");
	shadow.height = cfg::get_int("test_height");
	shadow.init();
	light_0 = vec3(-10.0f, 12.0f, 5.0f);
	shadow.view_matrix = glm::lookAt(light_0, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	shm::bind("phong");
	shm::get("phong")->set_uniform("enable_shadowmap", true);
	shm::get("phong")->set_uniform("shadow_texture", 3);
	shm::get("phong")->set_uniform("shadow_dimension", vec2(static_cast<float>(shadow.width), static_cast<float>(shadow.height) ) );

	sat_filter_width = 2.0f;
	//sat maker
	sam.init(shadow.width, shadow.height);
	//sat maker for base scene
	sam2.init(shadow.width, shadow.height);
	//filter test shader
	shader_manager::load_single("../shader/sat_filter");
	shm::get("sat_filter");
	shm::get("sat_filter")->bind();
	glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	shm::get("sat_filter")->set_uniform("perspective_matrix",  ortho_matrix);
	shm::get("sat_filter")->set_uniform("in_texture", 0);
	shm::get("sat_filter")->set_uniform("filter_width", 2.0f);
	shm::get("sat_filter")->set_uniform("dimension", glm::vec2(static_cast<float>(shadow.width), static_cast<float>(shadow.height)) );
	// sat filter texture state
	filter_state.init();
	filter_state.parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	filter_state.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtering
	filter_state.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	filter_state.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	test_sampler.init();
	test_sampler.parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	test_sampler.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
	test_sampler.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	test_sampler.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//pl1 shows position of the shadow cam
	shared_ptr<pointlight_node> pl1( new pointlight_node(mm::get("ico"), 1.0f, light_0 ) );
	pl1->set_color(glm::vec3(1.0f, 0.0f, 0.0f));
	mm::add(pl1, "pl1");
	root->add_child(pl1);

	//scene select
	scene_select = 0;

	return true;
}

void shadow_demo::draw() 
{
	switch (scene_select) {
	case 0:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0,0,width,height);
		draw_postprocess(shm::get("pp_echo"), txm::get("vsm"));
		break;
	case 1:
		draw_sat_scene();
		break;
	case 2:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0,0,width,height);
		draw_postprocess(shm::get("pp_echo"), txm::get("sat2"));
		break;
	case 3:
		draw_shadow_scene();
		break;
	}
}

//draw the scene with the shadow
void shadow_demo::draw_shadow_scene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);
	load_identity(view_matrix);

	look_at(view_matrix, cam_eye, cam_center, cam_up);
	//TODO: have proper normal matrix
	glm::mat4 normal_matrix = view_matrix * modelview_matrix;//glm::transpose(glm::inverse(view_matrix * modelview_matrix));

	/*------------------
		gbuffer creation
	--------------------*/
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,width, height);

	//draw skybox
	sb.draw(perspective_matrix, view_matrix);

	draw_gbuffer(root);

	/*--------------------
		shadowmap creation
	----------------------*/
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,shadow.width, shadow.height);
	//update shadow camera position
	shadow.view_matrix = glm::lookAt(light_0, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); 
	
	render_shadowmap(root, shadow);
	//render_shadowmap(mm::get("Tree3"), shadow); render_shadowmap(mm::get("Tree4"), shadow); render_shadowmap(mm::get("cube2"), shadow);

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*----------------
		SAT generation
	------------------*/


	// generate mipmaps for shadowmap here
	glBindTexture(GL_TEXTURE_2D, shadow.texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	//SAT vsm
	int sat_tex = generate_sat(shadow.texture, sam);
	glBindTexture(GL_TEXTURE_2D, sam.texture[sat_tex]);
	glGenerateMipmap(GL_TEXTURE_2D);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glViewport(0,0,width, height);

	// --------------
	// deferred stage
	// --------------

	//lighting
	glm::vec3 transformed_light_0 =  swizzle<X,Y,Z>(normal_matrix * glm::vec4(glm::normalize(light_0), 0.0f));
	shader_ptr sh = shader_manager::get("phong");
	sh->bind();
	sh->set_uniform("directional_light", transformed_light_0);
	sh->set_uniform("shadow_matrix",
		shadow.shadow_matrix() * glm::inverse(view_matrix));
	sh->set_uniform("filter_width", sat_filter_width);
	draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION], shadow.texture);
	//filter_state.bind();
	//filter_state.bind(3);
	//draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION], sam.texture[sat_tex]);
	//filter_state.unbind();
	//filter_state.unbind(3);

	
	// -------------------
	//see gbuffers and textures
	// -------------------
	glViewport(0, 0, width/5, height/5);
	draw_postprocess(shader_manager::get("pp_echo"), shadow.texture, 0);

	//glViewport(0, height/5, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), sam.texture[sat_tex], 0);

	//glViewport(0, 2*height/5, width/5, height/5);
	//shm::bind("sat_filter");
	//shm::get("sat_filter")->set_uniform("filter_width", sat_filter_width);
	//filter_state.bind();
	//draw_postprocess(shader_manager::get("sat_filter"), sam.texture[sat_tex]);
	//filter_state.unbind();

	glDisable(GL_FRAMEBUFFER_SRGB);
}

void shadow_demo::draw_sat_scene()
{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint tex = txm::get("bla_inverse");

	glViewport(0,0,width/2, height);
	draw_postprocess(shm::get("pp_echo"), tex, 0);

	//generate the sat
	int pong = generate_sat(tex, sam2);

	// -----------
	// show result
	glViewport(width/2, 0, width/2, height);
	set_left_postprocess_border(0.f);

	glBindTexture(GL_TEXTURE_2D, sam2.texture[pong]);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	shm::bind("sat_filter");
	shm::get("sat_filter")->set_uniform("filter_width", sat_filter_width2);
	test_sampler.bind(0);
	draw_postprocess(shm::get("sat_filter"), sam2.texture[pong], 0);
	test_sampler.unbind();
	//glViewport(0, 2*height/3, width, height/3);

	//draw_postprocess(shm::get("pp_echo"), sam2.texture[pong], 0);

}

void shadow_demo::process_keys(const float factor)
{
	light_0 += move_by_key(factor * 0.4f);
	move_node_by_key(mm::get("pl1"), factor * 0.4f);

	//filter width 1
	if (glfwGetKey('1') && sat_filter_width > 2.0f && !pushed['1']) {
		sat_filter_width -= 1.0f;
		pushed['1'] = true;
	}

	if (glfwGetKey('1') == GLFW_RELEASE) {
		pushed['1'] = false;
	}

	if (glfwGetKey('2') && sat_filter_width < 50 && !pushed['2']) {
		sat_filter_width += 1.0f;
		pushed['2'] = true;
	}

	if (glfwGetKey('2') == GLFW_RELEASE) {
		pushed['2'] = false;
	}
	
	//filter width 2
	if (glfwGetKey('Q') && sat_filter_width2 > 0 && !pushed['Q']) {
		sat_filter_width2 -= 1.0f;
		pushed['Q'] = true;
	}

	if (glfwGetKey('Q') == GLFW_RELEASE) {
		pushed['Q'] = false;
	}

	if (glfwGetKey('E') && sat_filter_width2 < 50 && !pushed['E']) {
		sat_filter_width2 += 1.0f;
		pushed['E'] = true;
	}

	if (glfwGetKey('E') == GLFW_RELEASE) {
		pushed['E'] = false;
	}

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

	//scene select 2
	if (glfwGetKey('5') && !pushed['5']) {
		scene_select = 2;
		pushed['5'] = true;
	}

	if (glfwGetKey('5') == GLFW_RELEASE) {
		pushed['5'] = false;
	}
	
	//scene select 3
	if (glfwGetKey('6') && !pushed['6']) {
		scene_select = 3;
		pushed['6'] = true;
	}

	if (glfwGetKey('6') == GLFW_RELEASE) {
		pushed['6'] = false;
	}
}