#include "mipmap_test.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"

#include <glm/gtx/transform.hpp>
// glm::translate, glm::rotate, glm::scale
#include <glm/gtc/matrix_transform.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp> 


bool mipmap_test::init()
{
	if (!super_init()) return false;

	//set far plane very large to see mipmap effect
	near_far = glm::vec2(0.1f, 5000.0f);
	perspective_matrix = glm::perspective(45.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), near_far.x, near_far.y);

	//texture loading
	texture_manager::load("../textures");
	txm::load("../textures/color");
	init_mm_texture(); //generate our custom mipmap texture

	// Load and compile Shader files
	shader_manager::load_single("../shader/minimal");
	shader_manager::load_single("../shader/forward/simple");
	shm::bind("simple");
	shm::get("simple")->set_uniform("in_texture", 0);
	shm::get("simple")->set_uniform("light_position", glm::vec3(1.0f, 1.0f, 1.0f) );
	
	//load meshes
	shader_ptr sp = shader_manager::get("simple");
	mesh_manager::load_single("../models/Tree3.obj", sp);
	mesh_manager::load_single("../models/icosahedron.nff", sp, "ico");
	mesh_manager::load_single("../models/quad_long.obj", sp, mm_tex, glm::mat4(1.0f), "quad");


	//triangle
	init_triangle(*sp);
	triangle_node = object_node(glm::mat4(), triangle_vao, 3);


	return true;
}

void mipmap_test::init_mm_texture() 
{
	//generate a texture manually with differently colored mipmap levels
	glGenTextures(1, &mm_tex);
	glBindTexture(GL_TEXTURE_2D, mm_tex);

	//parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<int>(cfg::get("mm_max")));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, static_cast<int>(cfg::get("mm_min")));


	//generate texture data
	int size[] = {128, 64, 32, 16, 8, 4, 2, 1};
	GLfloat colors[] = {1.0f, 1.0f, 1.0f, 1.0f, //0
						0.0f, 0.0f, 1.0f, 1.0f, //1
						0.0f, 1.0f, 0.0f, 1.0f, //2
						1.0f, 0.0f, 0.0f, 1.0f, //3
						1.0f, 1.0f, 0.0f, 1.0f, //4
						0.0f, 1.0f, 1.0f, 1.0f, //5
						1.0f, 0.0f, 1.0f, 1.0f, //6
						0.0f, 0.0f, 1.0f, 1.0f}; //7
	
	int num_mipmaps = 8;
	for (int mm_level = 0; mm_level < num_mipmaps; ++mm_level) 
	{
		std::vector<GLfloat> texels_0;
		int num_pixels = size[mm_level]*size[mm_level];
		for (int pixel = 0; pixel < num_pixels; ++pixel) {
			texels_0.push_back(colors[4 * mm_level]);
			texels_0.push_back(colors[4 * mm_level + 1]);
			texels_0.push_back(colors[4 * mm_level + 2]);
			texels_0.push_back(colors[4 * mm_level + 3]);
		}

		//assign the data
		int dim = size[mm_level];
		glTexImage2D(GL_TEXTURE_2D, mm_level, GL_RGBA16F, dim, dim, 0, GL_RGBA, GL_FLOAT, &texels_0[0]);	
	}



	//were done, unbind
	glBindTexture(GL_TEXTURE_2D, mm_tex);
}

void mipmap_test::draw()	//main drawing method
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);

	look_at(cam_eye, cam_center, cam_up);

	glEnable(GL_FRAMEBUFFER_SRGB);
	shader_ptr sp = shader_manager::get("simple");
	sp->bind();

	sp->set_uniform("modelview_matrix", modelview_matrix);
	sp->set_uniform("perspective_matrix", perspective_matrix);


	mesh_ptr tree_mesh = mesh_manager::get("quad");
	glBindTexture(GL_TEXTURE_2D, tree_mesh->get_texture_id());
	tree_mesh->render();
	//tree_node.render();
	//triangle_node.render();

	sp->unbind();

	//draw_postprocess(*shader_manager::get("pp_echo"), texture_manager::get("bark1"), 0);

	
	glEnable(GL_FRAMEBUFFER_SRGB);
}