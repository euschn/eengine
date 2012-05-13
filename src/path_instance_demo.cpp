// path instancing demo
// based on prideout.net tutorial

#include "path_instance_demo.h"
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

path_instance_demo::path_instance_demo() 
{
	path_root = node_ptr(new scene_node());
}

path_instance_demo::~path_instance_demo() 
{
	if (path_texture > 0) glDeleteTextures(1, &path_texture);

	cleanup();
}

bool path_instance_demo::init() 
{
	if (!super_init()) return false;
	
	//textures
	txm::load("../textures");

	//path bending shader
	shader_manager::load_single("../shader/path_bend");
	shader_ptr path_bend = shm::get("path_bend");
	path_bend->bind();
	path_bend->set_uniform("perspective_matrix", perspective_matrix);
	path_bend->set_uniform("in_texture", 0);
	path_bend->set_uniform("path_texture", 1);
	path_bend->set_uniform("inverseHeight", 1.0f / 128.0f);
	path_bend->set_uniform("inverseWidth", 1.0f / 128.0f);
	pathScale = 0.05f;
	pathOffset = 0.1f;
	path_bend->set_uniform("pathOffset", pathOffset);
	path_bend->set_uniform("pathScale", pathScale);

	//models
	shader_ptr gbg = shader_manager::get("gbuffer_gen");
	mm::load_single("../models/Tree3.obj", gbg,
		glm::mat4(1.f), texture_manager::get("bark1"));
	mm::load_single("../models/cube2.obj", gbg,
		glm::translate(0.f, -2.f, 0.f), texture_manager::get("cloth04"));
	mm::load_single("../models/maya/bkite2.dae", gbg,
		glm::translate(0.f, 0.f, 0.f) * glm::scale(0.05f, 0.05f, 0.05f), texture_manager::get("cloth04"));
	mesh_manager::load_single("../models/quad.obj", gbg,
		texture_manager::get("wood19"));
	//mm::load_single("../models/sponza.obj", gbg, txm::get("green"));
	//root->add_child(mm::get("sponza"));
	

	//scenegraph setup
	path_root->add_child(mm::get("Tree3"));
	//path_root->add_child(mm::get("bkite"));
	//path_root->add_child(mm::get("cube2"));
	root->add_child(mm::get("quad"));
	root->add_child(mm::get("bkite2"));
	root->add_child(mm::get("cube2"));

	//path_texture
	init_path_texture();

	sb.init("../textures/skybox/sahara");

	//init went ok
	return true;
}

void path_instance_demo::draw()
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
	//glViewport(0,0,width, height);

	//draw skybox
	sb.draw(perspective_matrix, view_matrix);

	draw_gbuffer(root);
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, path_texture);
	glActiveTexture(GL_TEXTURE0);
	shader_ptr path_bend = shm::get("path_bend");
	path_bend->bind();
	path_bend->set_uniform("pathOffset", pathOffset);
	draw_gbuffer(path_root, path_bend);

	glEnable(GL_FRAMEBUFFER_SRGB);
	//glViewport(0,0,width, height);

	// --------------
	// deferred stage
	// --------------

	//lighting
	glm::vec3 transformed_light_0 =  swizzle<X,Y,Z>(normal_matrix * glm::vec4(glm::normalize(light_0), 0.0f));
	shader_ptr sh = shader_manager::get("phong");
	sh->bind();
	sh->set_uniform("directional_light", transformed_light_0);
	draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION]);

	glDisable(GL_FRAMEBUFFER_SRGB);
}

void path_instance_demo::process_keys(const float factor) 
{
	pathOffset += factor * 0.02f;

}

void path_instance_demo::init_path_texture()
{
	const float PI = 3.141592f;

	glGenTextures(1, &path_texture);
	glBindTexture(GL_TEXTURE_2D, path_texture);

	int width = 128;
	int height = 128;

	float* pData = (float*) malloc(sizeof(float) * 3 * width * height);
	float* pDest = pData;

	int row = 0;

	// Ellipse
	{
		float theta = 0;
		const float dtheta = 2 * PI / (float) width;
		float* pTemp = (float*) malloc(width * 3 * sizeof(float));
		//pDest = pTemp;
		float length = 0;

		// Path Centers:
		float px, py;
		for (int slice = 0; slice < width; ++slice, theta += dtheta) {
			float x = 2 * cos(theta) * 1;
			float y = 1 * sin(theta) * 1;
			*pDest++ = x;
			*pDest++ = 0;
			*pDest++ = y;
			if (slice > 1) {
				length += sqrt((x - px) * (x - px) + (y - py) * (y - py));
			}
			px = x; py = y;
		}

		// Restribute:
		pDest = pData;
		//const float* pSrc = pTemp;
		//RedistributePath(pSrc, pDest, length, width);
		pDest += width * 3;
		free(pTemp);
	
		// Path Normals:
		for (int slice = 0; slice < width; ++slice) {
			*pDest++ = 0;
			*pDest++ = 1;
			*pDest++ = 0;
		}
		row++;
	}

	// texture data and options
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, pData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	//free data
	free(pData);
}