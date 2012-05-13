// cgse depth of field demo
// author: Eugen Jiresch
// 2012

#include "dof_demo.h"
#include "texture_manager.h"
#include "mesh_manager.h"
#include "pointlight_node.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
//swizzling
#include <glm/gtc/swizzle.hpp>

//use carefully
using namespace glm;


bool dof_demo::init() {
	if (!super_init()) return false;

	//textures
	txm::load("../textures");
	txm::load("../textures/color");

	//models
	shader_ptr gbg = shader_manager::get("gbuffer_gen");
	mm::load_single("../models/Tree3.obj", gbg,
		glm::mat4(1.f), texture_manager::get("bark1"));
	mm::load_single("../models/cube2.obj", gbg,
		glm::translate(-4.f, 0.f, 0.f), texture_manager::get("cloth04"));
	mesh_manager::load_single("../models/quad.obj", gbg,
		texture_manager::get("wood19"));


	//scenegraph setup
	root->add_child(mm::get("Tree3"));
	root->add_child(mm::get("cube2"));
	root->add_child(mm::get("quad"));

	//skybox
	sb.init("../textures/skybox/city");

	//fbos
	blur_fbo[0].init(width / 2, height / 2);
	blur_fbo[1].init(width / 2, height / 2);
	coc_fbo.init(width / 4, height / 4);
	scene_fbo.init(width, height);

	
	//gauss blurs
	shm::load_single("../shader/blur/dgauss_vertical");
	shader_ptr gvert = shm::get("dgauss_vertical");
	gvert->bind();
	gvert->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );
	gvert->set_uniform("in_texture", 0);
	gvert->set_uniform("height", static_cast<float>(blur_fbo[0].height()));

	shm::load_single("../shader/blur/dgauss_horizontal");
	shader_ptr ghori = shm::get("dgauss_horizontal");
	ghori->bind();
	ghori->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );
	ghori->set_uniform("in_texture", 0);
	ghori->set_uniform("width", static_cast<float>(blur_fbo[0].width()));

	//coc generator shader
	shm::load_single("../shader/blur/coc_gen");
	shader_ptr coc = shm::get("coc_gen");
	coc->bind();
	coc->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );
	coc->set_uniform("in_texture", 0);
	coc->set_uniform("near_far", near_far);

	//coc generator shader
	shm::load_single("../shader/blur/dof");
	shader_ptr dof = shm::get("dof");
	dof->bind();
	dof->set_uniform("in_texture", 0);
	dof->set_uniform("blur_texture", 1);
	dof->set_uniform("coc_texture", 2);

	return true;
}

void dof_demo::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);
	load_identity(view_matrix);

	look_at(view_matrix, cam_eye, cam_center, cam_up);
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

	glViewport(0,0,width, height);

	// --------------
	// deferred stage
	// --------------
	glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//deferred lighting
	glm::vec3 transformed_light_0 =  swizzle<X,Y,Z>(normal_matrix * glm::vec4(glm::normalize(light_0), 0.0f));
	shader_ptr sh = shader_manager::get("phong");
	sh->bind();
	sh->set_uniform("directional_light", transformed_light_0);
	draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION]);
	
	// --------------
	// DOF stage
	// -------------
	//time
	glFinish();
	millisecs_1 = glfwGetTime();

	//generate the coc
	glBindFramebuffer(GL_FRAMEBUFFER, coc_fbo.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, coc_fbo.width(), coc_fbo.height());
	//downsample the scene
	draw_postprocess(shader_manager::get("coc_gen"), gbuffer_tex[POSITION]);

	//blur the coc
	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo[1].id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, blur_fbo[1].width(), blur_fbo[1].height());
	//horizontal blur
	draw_postprocess(shm::get("dgauss_horizontal"), coc_fbo.color_tex());
	
	glBindFramebuffer(GL_FRAMEBUFFER, coc_fbo.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, coc_fbo.width(), coc_fbo.height());
	//vertical blur
	draw_postprocess(shm::get("dgauss_vertical"), blur_fbo[1].color_tex());

	//blur the coc a second time
	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo[1].id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, blur_fbo[1].width(), blur_fbo[1].height());
	//horizontal blur
	draw_postprocess(shm::get("dgauss_horizontal"), coc_fbo.color_tex());
	
	glBindFramebuffer(GL_FRAMEBUFFER, coc_fbo.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, coc_fbo.width(), coc_fbo.height());
	//vertical blur
	draw_postprocess(shm::get("dgauss_vertical"), blur_fbo[1].color_tex());

	//blur the coc a third time
	//glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo[1].id());
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0,0, blur_fbo[1].width(), blur_fbo[1].height());
	////horizontal blur
	//draw_postprocess(shm::get("dgauss_horizontal"), coc_fbo.color_tex());
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, coc_fbo.id());
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0,0, coc_fbo.width(), coc_fbo.height());
	////vertical blur
	//draw_postprocess(shm::get("dgauss_vertical"), blur_fbo[1].color_tex());

	//downsample and blur the scene
	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo[0].id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, blur_fbo[0].width(), blur_fbo[0].height());
	//downsample the scene
	draw_postprocess(shader_manager::get("pp_echo"), scene_fbo.color_tex());
	
	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo[1].id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, blur_fbo[1].width(), blur_fbo[1].height());
	//horizontal blur
	draw_postprocess(shm::get("dgauss_horizontal"), blur_fbo[0].color_tex());
	
	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo[0].id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, blur_fbo[0].width(), blur_fbo[0].height());
	//vertical blur
	draw_postprocess(shm::get("dgauss_vertical"), blur_fbo[1].color_tex());


	// -------------
	// final drawing
	// -------------

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_FRAMEBUFFER_SRGB);

	//draw the scene without dof
	glViewport(0, height / 2, width / 2, height / 2);
	draw_postprocess(shader_manager::get("pp_echo"), scene_fbo.color_tex());
	//blurred scene
	glViewport(width / 2, 0, width / 2, height / 2);
	draw_postprocess(shader_manager::get("pp_echo"), blur_fbo[0].color_tex());
	//coc
	glViewport(width / 2, height / 2, width / 2, height / 2);
	draw_postprocess(shader_manager::get("pp_echo"), coc_fbo.color_tex());
	//dof
	glViewport(0, 0, width / 2, height / 2);
	draw_postprocess(shader_manager::get("dof"), scene_fbo.color_tex(), blur_fbo[0].color_tex(), coc_fbo.color_tex());
	
	//timer
	glFinish();
	millisecs_1 = (glfwGetTime() - millisecs_1) * 1000.0f;
	// -------------------
	//see all the gbuffers
	// -------------------
	//glViewport(0, 0, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), gbuffer_tex[ALBEDO]);

	//glViewport(0, height/5, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), gbuffer_tex[NORMAL]);

	//glViewport(width / 5, 0, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), gbuffer_tex[POSITION]);

	glDisable(GL_FRAMEBUFFER_SRGB);
}

void dof_demo::process_keys(const float factor)
{
	update_camera_path(factor);
}
