// bloom demo

#include "bloom_demo.h"
#include "texture_manager.h"
#include "mesh_manager.h"
#include "shader_manager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
//swizzling
#include <glm/gtc/swizzle.hpp>

using namespace glm;

bloom_demo::bloom_demo()
{}

bloom_demo::~bloom_demo()
{}

bool bloom_demo::init()
{
	if (!super_init()) return false;
	
	//textures
	txm::load("../textures");

	//models
	shader_ptr gbg = shader_manager::get("gbuffer_gen");
	mm::load_single("../models/Tree3.obj", gbg,
		glm::mat4(1.f), texture_manager::get("bark1"));
	mm::load_single("../models/cube2.obj", gbg,
		glm::translate(-4.f, 0.f, 0.f), texture_manager::get("cloth04"));
	mesh_manager::load_single("../models/quad.obj", gbg,
		texture_manager::get("wood19"));
	mm::load_single("../models/raf22031.ctm", shm::get("gbuffer_gen"),
		glm::translate(-4.f, -2.f, -5.f) * glm::rotate(270.0f, 1.0f, 0.0f, 0.0f),
		texture_manager::get("raf22031"), "ambulance");	
	mm::load_single("../models/maya/sphere.dae", gbg,
		glm::translate(4.f, 0.f, 0.f), texture_manager::get("green"));
	//mm::load_single("../models/sponza.obj", gbg, txm::get("green"));
	//root->add_child(mm::get("sponza"));

	//primitive type testing

	//scenegraph setup
	root->add_child(mm::get("Tree3"));
	root->add_child(mm::get("cube2"));
	root->add_child(mm::get("quad"));
	root->add_child(mm::get("sphere"));

	sb.init("../textures/skybox/sahara");

	//init mssao shaders
	glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

	//fbos
	bloom_width = width / 8;
	bloom_height = height / 8;
	post_process_fbo.init(width, height);
	bloom_vertical.init(bloom_width, bloom_height);
	bloom_horizontal.init(bloom_width, bloom_height);

	//bloom shaders
	shm::load_single("../shader/bloom_mask");
	shader_ptr mask = shm::get("bloom_mask");
	mask->bind();
	mask->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );
	mask->set_uniform("in_texture", 0);
	
	shm::load_single("../shader/mix");
	shader_ptr mix = shm::get("mix");
	mix->bind();
	mix->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );
	mix->set_uniform("in_texture", 0);
	mix->set_uniform("in_texture2", 1);

	//gauss blurs
	shm::load_single("../shader/blur/dgauss_vertical");
	shader_ptr gvert = shm::get("dgauss_vertical");
	gvert->bind();
	gvert->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );
	gvert->set_uniform("in_texture", 0);
	gvert->set_uniform("height", static_cast<float>(bloom_height));

	shm::load_single("../shader/blur/dgauss_horizontal");
	shader_ptr ghori = shm::get("dgauss_horizontal");
	ghori->bind();
	ghori->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );
	ghori->set_uniform("in_texture", 0);
	ghori->set_uniform("width", static_cast<float>(bloom_width));

	//bloom phong shader
	shm::load_single("../shader/phong_bloom");
	shm::get("phong_bloom")->bind();
	shader_manager::get("phong_bloom")->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );	
	shader_manager::get("phong_bloom")->set_uniform("in_texture", 0);
	shader_manager::get("phong_bloom")->set_uniform("normal_texture", 1);
	shader_manager::get("phong_bloom")->set_uniform("position_texture", 2);
	shm::get("phong_bloom")->set_uniform("color_mask", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) );

	//glow gbuffer
	shader_manager::load_single("../shader/gbuffer_glow");
	shm::bind("gbuffer_glow");
	shm::get("gbuffer_glow")->set_uniform("perspective_matrix", perspective_matrix);
	shm::get("gbuffer_glow")->set_uniform("in_texture", 0);
	shm::get("gbuffer_glow")->set_uniform("glow_texture", 1);

	mm::load_single("../models/cube2.obj", shm::get("gbuffer_glow"),
		glm::translate(-9.f, 0.f, 0.f), texture_manager::get("base_logo2"), "glow_cube");

	return true;
}

void bloom_demo::draw()
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
	//draw glowing cube
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, txm::get("glow_logo"));
	glActiveTexture(GL_TEXTURE0);
	draw_gbuffer(mm::get("glow_cube"), shm::get("gbuffer_glow"));

	glViewport(0,0,width, height);

	// --------------
	// deferred stage
	// --------------
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glBindFramebuffer(GL_FRAMEBUFFER, post_process_fbo.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,width, height);
	

	//lighting
	glm::vec3 transformed_light_0 =  swizzle<X,Y,Z>(normal_matrix * glm::vec4(glm::normalize(light_0), 0.0f));
	shader_ptr sh = shader_manager::get("phong_bloom");
	sh->bind();
	sh->set_uniform("directional_light", transformed_light_0);
	draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// ---------------------
	// bloom post-processing
	// ---------------------
	// timer 
	glFinish();
	millisecs_1 = glfwGetTime();
	//filter the bloom color
	// -------------------
	//glBindTexture(GL_TEXTURE_2D, post_process_fbo.color_tex());
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, bloom_vertical.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, bloom_width, bloom_height);
	draw_postprocess(shm::get("bloom_mask"), post_process_fbo.color_tex());

	// draw bloom color
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width / 2, height / 2);
	draw_postprocess(shm::get("pp_echo"), bloom_vertical.color_tex());

	//blur the bloom color
	//--------------------
	glBindFramebuffer(GL_FRAMEBUFFER, bloom_horizontal.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, bloom_width, bloom_height);
	draw_postprocess(shm::get("dgauss_vertical"), bloom_vertical.color_tex());

	glBindFramebuffer(GL_FRAMEBUFFER, bloom_vertical.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, bloom_width, bloom_height);
	draw_postprocess(shm::get("dgauss_horizontal"), bloom_horizontal.color_tex());

	glBindFramebuffer(GL_FRAMEBUFFER, bloom_horizontal.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, bloom_width, bloom_height);
	draw_postprocess(shm::get("dgauss_vertical"), bloom_vertical.color_tex());

	glBindFramebuffer(GL_FRAMEBUFFER, bloom_vertical.id());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, bloom_width, bloom_height);
	draw_postprocess(shm::get("dgauss_horizontal"), bloom_horizontal.color_tex());

	//timer
	glFinish();
	millisecs_1 = glfwGetTime() - millisecs_1;
	millisecs_1 *= 1000.0;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//draw stuff on the screen
	//------------------------
	//timer
	millisecs_2 = glfwGetTime();

	glEnable(GL_FRAMEBUFFER_SRGB);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, height / 2, width / 2, height / 2);
	draw_postprocess(shm::get("pp_echo"), post_process_fbo.color_tex());

	//draw bloom mask
	glViewport(width / 2, 0, width / 2, height / 2);
	draw_postprocess(shm::get("pp_echo"), bloom_vertical.color_tex());

	//draw blur 1
	//glViewport(0, 0, width / 2, height / 2);
	//draw_postprocess(shm::get("pp_echo"), bloom_horizontal.color_tex());

	//draw applied bloom
	glViewport(width / 2, height / 2, width / 2, height / 2);
	draw_postprocess(shm::get("mix"), post_process_fbo.color_tex(),  bloom_vertical.color_tex());

	
	//timer
	glFinish();
	millisecs_2 = glfwGetTime() - millisecs_2;
	millisecs_2 *= 1000.0;

	glDisable(GL_FRAMEBUFFER_SRGB);

}

void bloom_demo::process_keys(const float factor)
{

}