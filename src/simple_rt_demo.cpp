// simple rt demo
// author: Eugen Jiresch

#include "simple_rt_demo.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"
#include "tga.h"

#include <glm/gtx/transform.hpp>
// glm::translate, glm::rotate, glm::scale
#include <glm/gtc/matrix_transform.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp> 


bool simple_rt_demo::init() 
{
	if (!super_init()) return false;

	// Load and compile Shader files
	shader_manager::load_single("../shader/rt/simple_rt");
	
	shader_ptr sp = shader_manager::get("simple_rt");

	//triangle
	init_triangle(*sp);
	triangle_node = object_node(glm::mat4(), triangle_vao, 3);

	//texture loading
	texture_manager::load("../textures");

	init_background_texture();
	//add cm texture
	glActiveTexture(GL_TEXTURE2);

	glBindTexture(GL_TEXTURE_CUBE_MAP, background_tex);
	sp->bind();
	sp->set_uniform("background_texture", 2);

	glActiveTexture(GL_TEXTURE0);



	return true;
}

void simple_rt_demo::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);
	look_at(cam_eye, cam_center, cam_up);

	glEnable(GL_FRAMEBUFFER_SRGB);

	shader_ptr shader = shm::get("simple_rt");
	shader->bind();
	shader->set_uniform("modelview_matrix", modelview_matrix);
	shader->set_uniform("inverse_modelview_matrix", glm::inverse(modelview_matrix));
	shader->set_uniform("time", static_cast<GLfloat>(glfwGetTime()));
	draw_postprocess(shader, 0);

	
	glDisable(GL_FRAMEBUFFER_SRGB);
}

void simple_rt_demo::init_background_texture()
{
	std::string path = "../textures/skybox/sahara";
	//texture setup
	glGenTextures(1, &background_tex);
	get_errors();

	glBindTexture(GL_TEXTURE_CUBE_MAP, background_tex);

	tga::Texture tex;

	string filepaths[] = {
		path + "_right.tga",
		path + "_left.tga",
		path + "_top.tga",
		path + "_bottom.tga",
		path + "_front.tga",
		path + "_back.tga",
	};

	//string filepaths[] = {
	//	"../textures/skybox/alpine_right.tga",
	//	"../textures/skybox/alpine_left.tga",
	//	"../textures/skybox/alpine_top.tga",
	//	"../textures/skybox/alpine_bottom.tga",
	//	"../textures/skybox/alpine_front.tga",
	//	"../textures/skybox/alpine_back.tga",
	//};

	for(int side = 0; side < 6; side++) {

		if (tga::LoadTGA(&tex, filepaths[side].c_str()))
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, GL_SRGB8_ALPHA8 , tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);

		
			if (tex.imageData)					// If Texture Image Exists
			{
				free(tex.imageData);				// Free The Texture Image Memory
			}
		}
		else {
			std::cout << "background: loading " << filepaths[side] << "failed\n";
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}