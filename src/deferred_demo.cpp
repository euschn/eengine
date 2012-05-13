// class deferred_demo

#include "deferred_demo.h"
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

//part of the initialisation that is inherited
bool deferred_demo::super_init()
{
	if (!simple_demo::super_init()) return false;

	//load single color textures
	txm::load("../textures/color");

	shader_manager::load_single("../shader/gbuffer_gen");
	shm::bind("gbuffer_gen");
	shm::get("gbuffer_gen")->set_uniform("perspective_matrix", perspective_matrix);
	shm::get("gbuffer_gen")->set_uniform("in_texture", 0);

	shader_manager::load_single("../shader/pointlight");
	//pointlight model
	mesh_manager::load_single("../models/icosahedron.nff", shm::get("gbuffer_gen"),
		"ico", glm::mat4(1.f), texture_manager::get("yellow"));
	mm::get("ico")->set_shader_type(SH_POINTLIGHT);

	shm::load_single("../shader/phong");
	shm::get("phong")->bind();
	shader_manager::get("phong")->set_uniform("perspective_matrix", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) );	
	shader_manager::get("phong")->set_uniform("in_texture", 0);
	shader_manager::get("phong")->set_uniform("normal_texture", 1);
	shader_manager::get("phong")->set_uniform("position_texture", 2);
	shm::get("phong")->set_uniform("color_mask", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) );

	shader_ptr pl = shm::get("pointlight");
	pl->bind();
	pl->set_uniform("perspective_matrix", perspective_matrix);
	pl->set_uniform("albedo_texture", 0);
	pl->set_uniform("normal_texture", 1);
	pl->set_uniform("position_texture", 2);

	
	//gbuffer
	if (!init_gbuffer()) return false;

	return true;
}

bool deferred_demo::init()
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
	//root->add_child(mm::get("ambulance"));
	//point light
	light_1 = glm::vec3(0,0,0);
	//pointlight node test
	shared_ptr<pointlight_node> pl1( new pointlight_node(mm::get("ico"), 2.0f, light_1 ) );
	pl1->set_color(glm::vec3(1.0f, 0.0f, 0.0f));
	mm::add(pl1, "pl1");
	root->add_child(mm::get("pl1"));
	//second point light
	pointlight_ptr pl2( new pointlight_node(mm::get("ico"), 2.0f, glm::vec3(-1.f, 0.0f, 0.0f)) );
	pl2->set_color(0,1,0);
	mm::add(pl2, "pl2");
	pl1->add_child(pl2);

	//skybox
	sb.init();

	return true;
}

void deferred_demo::draw()
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
	glViewport(0, 0, width, height);
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
	glActiveTexture(GL_TEXTURE0);

	render_pointlight(mm::get("pl1"));
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_FRAMEBUFFER_SRGB);
}


void deferred_demo::draw_gbuffer(shared_ptr<scene_node> node, shader_ptr shp)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0,0,width, height);

	shp->bind();
	
	render_gbuffer(node, shp);

	glBindTexture(GL_TEXTURE_2D, 0 );
	shm::unbind();

	//TODO this should be fixed
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// render the gbuffer
void deferred_demo::render_gbuffer(shared_ptr<scene_node> node, shader_ptr shp)
{
	//save current transformation
	push_modelview_matrix();

	//apply transformation of this node
	modelview_matrix = modelview_matrix * node->transformation_matrix;

	switch (node->shader_type())
	{
	case SH_TEXTURE:
		shp->set_uniform("modelview_matrix", view_matrix * modelview_matrix );
		shp->set_uniform("normal_matrix", view_matrix * modelview_matrix );

		node->pre_render();

		node->render();
		break;
	case SH_POINTLIGHT:
		shp->set_uniform("modelview_matrix", view_matrix * modelview_matrix * glm::scale(0.05f, 0.05f, 0.05f) );
		shp->set_uniform("normal_matrix", view_matrix * modelview_matrix );

		texture_manager::bind("yellow");
		mesh_manager::render("cube2");
		break;
	default:
		break;
	}
	
	//render children
	for (auto it = node->children.begin(); it != node->children.end(); it++)
	{
		render_gbuffer(*it, shp);
	}

	pop_modelview_matrix();
}

void deferred_demo::render_pointlight(shared_ptr<scene_node> node)
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
		glm::vec3 viewspace_light_position = glm::swizzle<glm::X, glm::Y, glm::Z>( view_matrix * glm::vec4(light_position, 1.0f) );
		pl->set_uniform("light_position", viewspace_light_position);

		//node->pre_render();

		node->render();

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
	
	//render children
	for (auto it = node->children.begin(); it != node->children.end(); it++)
	{
		render_pointlight(*it);
	}

	pop_modelview_matrix();
}

void deferred_demo::render_shadowmap(node_ptr node, shadow_kit& sm_kit)
{
	sm_kit.gen->bind();
	sm_kit.gen->set_uniform("perspective_matrix", sm_kit.perspective_matrix);

	render_shadowmap_recursive(node, sm_kit);

	sm_kit.gen->unbind();

}

void deferred_demo::render_shadowmap_recursive(node_ptr node, shadow_kit& sm_kit)
{
	//save current transformation
	push_modelview_matrix();

	//apply transformation of this node
	modelview_matrix *= node->transformation_matrix;

	//setup shader and render node
	//Shader* current_shader; //point to the shader of the node
	//GLint modelview_location, perspective_location, light_location;
	switch (node->shader_type())
	{
	case SH_NONE:
		break;
	case SH_TEXTURE:
		////view frustrum culling
		//if (in_frustrum(node)) {
		//		cull = true;
		//		number_culled++;
		//		break;
		//}
		// Set our matrix uniform

		//do preparations before rendering
		//node->pre_render();

		sm_kit.gen->set_uniform("modelview_matrix", sm_kit.view_matrix * modelview_matrix);

		node->render();

		break;
	default:
		break;

	}

	//if (!cull) {
	//}

	//render children
	for (auto it = node->children.begin(); it != node->children.end(); it++)
	{
		render_shadowmap_recursive(*it, sm_kit);
	}

	//restore previous transformation
	pop_modelview_matrix();
}

void deferred_demo::process_keys(const float factor)
{
	if (glfwGetKey(GLFW_KEY_ENTER)) {
		std::cout << glm::length(light_1 - cam_eye) << std::endl;
	}

	update_camera_path(factor);

	update_light(factor);
}

void deferred_demo::update_light(float factor)
{
	if (glfwGetKey(GLFW_KEY_RIGHT))
	{
		//light_0.x += factor;
		light_1.x += factor;
		//poly_offset_factor += factor * weight;
	}	
	if (glfwGetKey(GLFW_KEY_LEFT))
	{
		//light_0.x -= factor;
		light_1.x -= factor;
		//poly_offset_factor -= factor * weight;
	}
	if (glfwGetKey(GLFW_KEY_UP))
	{
		//light_0.z -= factor;
		light_1.z -= factor;
		//poly_offset_units += factor * weight;
	}	
	if (glfwGetKey(GLFW_KEY_DOWN))
	{
		//light_0.z += factor;
		light_1.z += factor;
		//poly_offset_units -= factor * weight;
	}
	if (glfwGetKey(GLFW_KEY_PAGEDOWN))
	{
		//light_0.z -= factor;
		light_1.y -= factor;
		//poly_offset_units += factor * weight;
	}	
	if (glfwGetKey(GLFW_KEY_PAGEUP))
	{
		//light_0.z += factor;
		light_1.y += factor;
		//poly_offset_units -= factor * weight;
	}
	mm::get("pl1")->transformation_matrix = glm::translate(light_1);
}


void deferred_demo::cleanup()
{
	//TODO implement
}


bool deferred_demo::init_gbuffer()
{
	glGenFramebuffers(1, &gbuffer_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
	glGenTextures(3, gbuffer_tex);
	glGenTextures(1, &gbuffer_depth);

	//depth texture
	glBindTexture(GL_TEXTURE_2D, gbuffer_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width,
	height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_TEXTURE_2D, gbuffer_depth, 0);

	//albedo texture
	glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F , width,
	height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 ,
	GL_TEXTURE_2D, gbuffer_tex[ALBEDO], 0);

	//normal texture
	glBindTexture(GL_TEXTURE_2D, gbuffer_tex[NORMAL]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width,
	height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	//glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 ,
	GL_TEXTURE_2D, gbuffer_tex[NORMAL], 0);

	//position texture
	glBindTexture(GL_TEXTURE_2D, gbuffer_tex[POSITION]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width,
	height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2 ,
	GL_TEXTURE_2D, gbuffer_tex[POSITION], 0);

	//MRT
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, buffers);

	//completeness check
	std::cout << "gbuffer fbo completeness check:";
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		// ToDo:
		printf("\ngbuffer GL_FRAMEBUFFER_COMPLETE error 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return false;
	}
	else
	{
		std::cout << "passed\n";
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	////shadowmapping fbo
	//glGenFramebuffers(1, &shadow_fbo);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	//glGenTextures(1, &shadow_tex);
	//glGenTextures(1, &shadow_depth);

	////TODO: change shadowmap dimension/precision
	////depth texture
	//glBindTexture(GL_TEXTURE_2D, shadow_depth);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
	//	GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width,
	//height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	//
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	//GL_TEXTURE_2D, shadow_depth, 0);

	////variance shadow texture texture
	//glBindTexture(GL_TEXTURE_2D, shadow_tex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width,
	//height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	////glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO]);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 ,
	//GL_TEXTURE_2D, shadow_tex, 0);

	//
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//{
	//	// ToDo:
	//	printf("\nshadow fbo: GL_FRAMEBUFFER_COMPLETE error 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	//	return false;
	//}

	return true;
}