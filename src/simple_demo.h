/*
	simple demo class
	contains sample models, textures and scenegraph nodes
*/

#pragma once

#include <memory>
#include <vector>

#include "demo_base.h"
#include "tesselation_shader.hpp"
#include "object_node.h"
#include "glm\glm.hpp"
#include "sat_maker.h"
#include "camera_path.h"

class simple_demo : public demo_base {
public:
	simple_demo(void);
	virtual ~simple_demo();

	virtual bool init();
	//part of the initialisation that is inherited
	virtual bool super_init();
	// run including showing of the loadscreen
	virtual void run();
	void start_loop();
	virtual void draw_start_screen();

protected:
	//triangle
	GLuint triangle_vao;
	GLuint triangle_vbo[4];
	object_node triangle_node;
	//textures
	GLuint sample_texture;	
	//scenegraph
	std::tr1::shared_ptr<scene_node> root;
	// move a node in the scene
	void move_node_by_key(std::tr1::shared_ptr<scene_node> node, const float factor,
					int key_pos_x = GLFW_KEY_RIGHT,
					int key_neg_x = GLFW_KEY_LEFT,
					int key_pos_y = GLFW_KEY_PAGEUP,
					int key_neg_y = GLFW_KEY_PAGEDOWN,
					int key_pos_z = GLFW_KEY_DOWN,
					int key_neg_z = GLFW_KEY_UP);
	glm::vec3 move_by_key(const float factor,
					int key_pos_x = GLFW_KEY_RIGHT,
					int key_neg_x = GLFW_KEY_LEFT,
					int key_pos_y = GLFW_KEY_PAGEUP,
					int key_neg_y = GLFW_KEY_PAGEDOWN,
					int key_pos_z = GLFW_KEY_DOWN,
					int key_neg_z = GLFW_KEY_UP);

	//light
	glm::vec3 light_0;
	glm::vec3 light_1;

	//two catmul rom splines to direct camera 
	camera_path cam_path;

	//load triangle vbos
	void init_triangle(const Shader &shader);
	//post-processing test
	GLuint pp_vao_id, pp_vbo[4];
	void init_postprocess();
	void draw_postprocess(shader_ptr shader, GLuint source_tex_id, GLuint tex_id_1 = 0, GLuint tex_id_2 = 0, GLuint tex_id_3 = 0);
	//set post-processing left border
	void set_left_postprocess_border(const GLfloat border);
	void set_bottom_postprocess_border(const GLfloat border);
	//generate a SAT from a texture
	int generate_sat(GLuint tex, sat_maker& sm);
	//update camera_path
	void update_camera_path(const float factor);
	
	virtual void draw();	//main drawing method
	virtual void cleanup();

};