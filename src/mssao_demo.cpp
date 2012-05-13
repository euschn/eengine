// mssao demo

#include "mssao_demo.h"
#include "texture_manager.h"
#include "mesh_manager.h"
#include "shader_manager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
//swizzling
#include <glm/gtc/swizzle.hpp>

using namespace glm;

mssao_demo::mssao_demo() {}

mssao_demo::~mssao_demo() {}

bool mssao_demo::init() 
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
	//mm::load_single("../models/sponza.obj", gbg, txm::get("green"));
	//root->add_child(mm::get("sponza"));

	//primitive type testing
	//mm::get("Tree3")->primitive_type = GL_LINE_STRIP;

	//scenegraph setup
	root->add_child(mm::get("Tree3"));
	root->add_child(mm::get("cube2"));
	root->add_child(mm::get("quad"));

	sb.init("../textures/skybox/sahara");

	//init mssao shaders
	glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	shm::load_single("../shader/mssao/downsample");
	shader_ptr ds = shm::get("downsample");
	ds->bind();
	ds->set_uniform("perspective_matrix",  ortho_matrix);
	ds->set_uniform("position_texture", 0);
	ds->set_uniform("normal_texture", 1);
	ds->set_uniform("texture_width", static_cast<float>(width));
	ds->set_uniform("texture_height", static_cast<float>(height));

	//ao kit
	ao_kit.init();

	//all went well
	return true;
}
void mssao_demo::draw() 
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
	
	/*-----------------
	    mssao
	-------------------*/
	glBindFramebuffer(GL_FRAMEBUFFER, ao_kit.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, ao_kit.width / 2, ao_kit.height / 2);
	shm::get("downsample")->bind();

	draw_postprocess(shm::get("downsample"), gbuffer_tex[POSITION], gbuffer_tex[NORMAL]); 

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION]);

	// --------------
	// mssao tests
	// --------------
	glViewport(0,0, width/ 5, height / 5);
	draw_postprocess(shm::get("pp_echo"), ao_kit.tex[0]);


	glDisable(GL_FRAMEBUFFER_SRGB);
}

void mssao_demo::process_keys(const float factor)
{

}