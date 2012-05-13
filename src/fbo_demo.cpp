// cgse fbo demo
// author: Eugen Jiresch

#include "fbo_demo.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"

#include <glm/gtx/transform.hpp>
// glm::translate, glm::rotate, glm::scale
#include <glm/gtc/matrix_transform.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp> 
// swizzling
#include <glm/gtc/swizzle.hpp>

//use carefully
using namespace glm;


fbo_demo::fbo_demo() {}

fbo_demo::~fbo_demo() 
{
	cleanup();
}

bool fbo_demo::init() 
{
	if (!super_init()) return false;

	glClearColor(.2f, .2f, .2f, 1.f);

	//texture loading
	texture_manager::load("../textures");
	texture_manager::load("../textures/color");

	// Load and compile Shader files
	shader_manager::load_single("../shader/minimal");
	shader_manager::load_single("../shader/forward/simple");

	// fbo setup
	reflection_fbo.init(512,512);

	// quad setup
	quad_position = glm::vec3(0.0f, 0.0f, -7.0f);
	quad_normal = glm::vec3(0.0f, 0.0f, 1.0f);
	quad_rotation = glm::rotate(0.0f, 0.0f, 1.0f, 0.0f);
	
	//load meshes
	shader_ptr sp = shader_manager::get("simple");
	sp->bind();
	sp->set_uniform("in_texture", 0);
	mesh_manager::load_single("../models/Tree3.obj", sp, txm::get("green"));
	mesh_manager::load_single("../models/basic/torus.obj", sp, txm::get("white"), glm::translate(-4.0f, -2.0f, 0.0f) * glm::scale(0.3f, 0.3f, 0.3f));
	mesh_manager::load_single("../models/basic/sphere.obj", sp, txm::get("checker"), glm::translate(4.0f, 12.0f, -2.0f) * glm::scale(0.4f, 0.4f, 0.4f));
	mesh_manager::load_single("../models/basic/plane.obj", sp, txm::get("checker"), glm::translate(-7.0f, -3.0f, 7.0f) * glm::rotate(-90.0f, 1.0f, 0.0f, 0.0f));
	mesh_manager::load_single("../models/quad_small.obj", sp, reflection_fbo.color_tex(), glm::translate(quad_position) * quad_rotation);
	mesh_manager::load_single("../models/icosahedron.nff", sp, "ico");

	root->add_child(mm::get("Tree3"));
	root->add_child(mm::get("torus"));
	root->add_child(mm::get("sphere"));
	root->add_child(mm::get("plane"));
	//root->add_child(mm::get("quad_small"));

	//light
	light_1 = glm::vec3(-15.0f, 10.0f, 10.f);

	return true;

}

//drawing routine
void fbo_demo::draw() 
{
	//get our shader
	shader_ptr sp = shader_manager::get("simple");
	sp->bind();

	//draw from the view of the quad into the fbo
	if (reflection_fbo.id() > 0) {
		//bind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, reflection_fbo.id());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//change viewport
		glViewport(0, 0, 512, 512);
		//set modelview_matrix
		glm::vec3 view_vector = glm::reflect((quad_position - cam_eye), quad_normal);
		glm::vec3 quad_center = quad_position + view_vector;
		look_at(quad_position, quad_center, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 reflection_perspective_matrix = glm::perspective(45.0f, 1.0f, near_far.x, near_far.y);
		sp->set_uniform("modelview_matrix", modelview_matrix);
		sp->set_uniform("perspective_matrix", reflection_perspective_matrix);
		//light
		glm::vec4 transformed_light = modelview_matrix * glm::vec4(light_1, 0.0f);
		sp->set_uniform("light_position", glm::normalize( glm::vec3(transformed_light.x, transformed_light.y, transformed_light.z) ) );
	
		//draw the scene
		render_node(root, sp);

		//back to the default framebuffer and viewport
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);

	look_at(cam_eye, cam_center, cam_up);

	glEnable(GL_FRAMEBUFFER_SRGB);

	sp->set_uniform("modelview_matrix", modelview_matrix);
	sp->set_uniform("perspective_matrix", perspective_matrix);
	//light
	glm::vec4 transformed_light = modelview_matrix * glm::vec4(light_1, 0);
	sp->set_uniform("light_position", glm::normalize( glm::vec3(transformed_light.x, transformed_light.y, transformed_light.z) ) );

	//draw the scene
	render_node(root, sp);

	//draw the quad
	render_node(mm::get("quad_small"), sp);


	sp->unbind();

	//draw_postprocess(*shader_manager::get("pp_echo"), texture_manager::get("bark1"), 0);

	
	glEnable(GL_FRAMEBUFFER_SRGB);
}

// render the gbuffer
void fbo_demo::render_node(shared_ptr<scene_node> node, shader_ptr shp)
{
	//save current transformation
	push_modelview_matrix();

	//apply transformation of this node
	modelview_matrix = modelview_matrix * node->transformation_matrix;

	switch (node->shader_type())
	{
	case SH_TEXTURE:
		shp->set_uniform("modelview_matrix", view_matrix * modelview_matrix );

		node->pre_render();

		node->render();
		break;
	default:
		break;
	}
	
	//render children
	for (auto it = node->children.begin(); it != node->children.end(); it++)
	{
		render_node(*it, shp);
	}

	pop_modelview_matrix();
}