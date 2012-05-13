
#include "simple_demo.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"

#include <glm/gtx/transform.hpp>
// glm::translate, glm::rotate, glm::scale
#include <glm/gtc/matrix_transform.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp> 

simple_demo::simple_demo()
{
	near_far.x = 0.5;
	near_far.y = 500;
	perspective_matrix = glm::perspective(45.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), near_far.x, near_far.y);
	root = std::tr1::shared_ptr<scene_node>(new scene_node());
}

simple_demo::~simple_demo()
{cleanup();}

bool simple_demo::init()
{
	if (!super_init()) return false;

	// Load and compile Shader files
	shader_manager::load_single("../shader/minimal");
	
	//load meshes
	shader_ptr sp = shader_manager::get("minimal");
	mesh_manager::load_single("../models/Tree3.obj", sp);
	mesh_manager::load_single("../models/icosahedron.nff", sp, "ico");

	//triangle
	init_triangle(*sp);
	triangle_node = object_node(glm::mat4(), triangle_vao, 3);

	//texture loading
	texture_manager::load("../textures");

	return true;
}

bool simple_demo::super_init()
{
	if (!init_window())
	{
		return false;
	}

	//cam
	cam_eye = glm::vec3(0.0f, 0.0f, 10.f);
	cam_center = glm::vec3(0.0f, 0.0f, 0.0f);
	cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

	//light
	light_0 = glm::vec3(5.0f, 5.0f, 10.0f);
	light_1 = glm::vec3(5.0f, 5.0f, 10.0f);
	
	//do opengl initialization here
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glViewport(0,0,width,height);
	glEnable(GL_CULL_FACE);

	//echo post_process shader
	shader_manager::load_single("../shader/pp_echo");
	init_postprocess();
	
	// options
	//config_manager::load();
	texture_manager::inst()->anisotropic = config_manager::get_bool("anisotropic");

	// all went well
	return true;
}

//main loop of the demo
//parts taken from RTR ogl3.2 demos
void simple_demo::run()
{
	if (cfg::get_bool("show_start_screen")) {
		start_loop();
	}
	
	demo_base::run();
}

//draw some sort of start screen until a key is pressed
void simple_demo::start_loop()
{

	glViewport(0,0, width, height);
	bool pressed = false;

	while (true) {

		//draw something here
		draw_start_screen();
		
		//swapbuffers
		glfwSwapBuffers();

		//exit condition
		if (!pressed &&
			glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS ||
			glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) {
			pressed = true;
		}
		//make sure the butter has been released
		if (pressed &&
			glfwGetKey(GLFW_KEY_SPACE) == GLFW_RELEASE &&
			glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE ) {
			break;
		}
	}
}

void simple_demo::draw_start_screen() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_postprocess(shader_manager::get("pp_echo"), texture_manager::get("eelogo43"), 0);
}


void simple_demo::init_triangle(const Shader &shader)
{
	// Our triangle data
	GLfloat bg_positions[] = {	 
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,	};

	GLfloat bg_normals[] = {	
		0, 0, 1,
		0, 0, 1,
		0, 0, 1	};

	GLfloat bg_texcoords[] = {	 
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f	};

	GLuint bg_indices[] = {
		0,1,2
	};

	glGenVertexArrays(1, &triangle_vao);
	glBindVertexArray(triangle_vao);

	glGenBuffers(4, triangle_vbo);

	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)	
	glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(GLfloat), bg_positions, GL_STATIC_DRAW);
	shader.set_attrib_location("in_position", 3);
	get_errors();

	// Repeat for second VBO storing vertex colors
	glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), bg_normals, GL_STATIC_DRAW);
	shader.set_attrib_location("in_normal", 3);
	get_errors();

	//vbo for texcoords
	glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), bg_texcoords, GL_STATIC_DRAW);
	shader.set_attrib_location("in_texcoord", 2);

	//vbo for indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bg_indices), bg_indices, GL_STATIC_DRAW);

	get_errors();

	// unbind VAO
	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void simple_demo::draw_postprocess(shader_ptr shader, GLuint source_tex_id, GLuint tex_id_1, GLuint tex_id_2, GLuint tex_id_3) 
{
		shader->bind();

		if ( tex_id_1 > 0) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex_id_1);
		}
		
		if ( tex_id_2 > 0) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, tex_id_2);
		}
				
		if ( tex_id_3 > 0) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, tex_id_3);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, source_tex_id);

	
		glBindVertexArray(pp_vao_id);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLE_FAN , 0, 4);

		glBindVertexArray(0);

		// Unbind VBOs
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		shader->unbind();
}
	
void simple_demo::draw()	//main drawing method
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);

	look_at(cam_eye, cam_center, cam_up);

	glEnable(GL_FRAMEBUFFER_SRGB);
	shader_ptr sp = shader_manager::get("minimal");
	sp->bind();

	sp->set_uniform("modelview_matrix", modelview_matrix);
	sp->set_uniform("perspective_matrix", perspective_matrix);

	mesh_ptr tree_mesh = mesh_manager::get("ico");
	//tree_mesh->render();
	//tree_node.render();
	//triangle_node.render();

	sp->unbind();

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	draw_postprocess(shader_manager::get("pp_echo"), txm::get("bark1"), 0);

	
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void simple_demo::cleanup()
{
	//pp stuff
	glDeleteVertexArrays(1, &pp_vao_id);
	glDeleteBuffers(3, pp_vbo);
	//
	glDeleteVertexArrays(1, &triangle_vao);

	glDeleteBuffers(4, triangle_vbo);
	
	//texture
	glDeleteTextures(1, &sample_texture);
}


//post-processing initialization
void simple_demo::init_postprocess()
{

	// Our triangle data
	GLfloat positions[] = {	 
		-1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f	};

	GLfloat texcoords[] = {	 
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f	};

	//build vao, vbo
	glGenVertexArrays(1, &pp_vao_id);
	glBindVertexArray(pp_vao_id);

	glGenBuffers(2, pp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pp_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	auto pp_echo = shader_manager::get("pp_echo");
	pp_echo->bind();
	pp_echo->set_attrib_location("in_position", 4);

	glBindBuffer(GL_ARRAY_BUFFER, pp_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	pp_echo->set_attrib_location("in_texcoord", 2);
	
	glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	pp_echo->set_uniform("perspective_matrix",  ortho_matrix);
	pp_echo->set_uniform("in_texture", 0);

}

//determine the left start of the post_process
void simple_demo::set_left_postprocess_border(const GLfloat border)
{
	
	// Our triangle data
	GLfloat positions[] = {	 
		-1.0f + 2 * border, -1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f + 2 * border, 1.0f, 0.0f, 1.0f	};

	GLfloat texcoords[] = {	 
		0.0f + border, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f + border, 1.0f	};

	//build vao, vbo
	glBindVertexArray(pp_vao_id);

	glBindBuffer(GL_ARRAY_BUFFER, pp_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	auto pp_echo = shader_manager::get("pp_echo");
	//pp_echo->bind();
	pp_echo->set_attrib_location("in_position", 4);

	glBindBuffer(GL_ARRAY_BUFFER, pp_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	pp_echo->set_attrib_location("in_texcoord", 2);
	//pp_echo->unbind();
}

//determine the left start of the post_process
void simple_demo::set_bottom_postprocess_border(const GLfloat border)
{
	
	// Our triangle data
	GLfloat positions[] = {	 
		-1.0f, -1.0f + 2 * border, 0.0f, 1.0f,
		1.0f, -1.0f + 2 * border, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f	};

	GLfloat texcoords[] = {	 
		0.0f, 0.0f + border,
		1.0f, 0.0f + border,
		1.0f, 1.0f,
		0.0f, 1.0f	};

	//build vao, vbo
	glBindVertexArray(pp_vao_id);

	glBindBuffer(GL_ARRAY_BUFFER, pp_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	auto pp_echo = shader_manager::get("pp_echo");
	//pp_echo->bind();
	pp_echo->set_attrib_location("in_position", 4);

	glBindBuffer(GL_ARRAY_BUFFER, pp_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	pp_echo->set_attrib_location("in_texcoord", 2);
	//pp_echo->unbind();
}


int simple_demo::generate_sat(GLuint tex, sat_maker& sm)
{
		// TODO also set that for height
	float tw =  static_cast<float>(sm.width);
	float th =  static_cast<float>(sm.height);
	float step_size = 1.0f / tw;

	//sampler state
	sm.state.bind();
	//shader
	sm.gen->bind();
	sm.gen->set_uniform("stride", 1.0f);
	sm.gen->set_uniform("direction", glm::vec2(1.0f, 0.0f));
	// sat generation
	// TEX -> PING
	glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[PING]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,sm.width, sm.height);

	//set_left_postprocess_border(step_size);
	draw_postprocess(sm.gen, tex, 0 );
	
	// TEX -> PONG
	glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[PONG]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,sm.width, sm.height);
	draw_postprocess(sm.gen, tex, 0 );
	
	int pong = 1;
	float stride = 2.0f;
	float step = step_size;
	// HORIZONTAL PHASE
	//back and forth till where outside the picture
	while (step < 0.5f) {
		// PING -> PONG: stride 2
		glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[pong]);
		//glViewport(0,0,sm.width, sm.height);

		set_left_postprocess_border(step);
		sm.gen->bind();
		sm.gen->set_uniform("stride", stride);
		draw_postprocess(sm.gen, sm.texture[1-pong], 0 );
		
		//update variables
		pong = 1 - pong;
		step = step_size*stride;
		stride *= 2.0f;
	}
	
	// VERTICAL PHASE
	step_size = 1.0f / th;
	stride = 1.0f;
	step = 0.0f;
	sm.gen->bind();
	sm.gen->set_uniform("direction", glm::vec2(0.0f, 1.0f) );
	
	while (step < 0.5f) {
		// PING -> PONG: stride 2
		glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[pong]);
		//glViewport(0,0,sm.width, sm.height);

		set_bottom_postprocess_border(step);
		sm.gen->bind();
		sm.gen->set_uniform("stride", stride);
		draw_postprocess(sm.gen, sm.texture[1-pong], 0 );
		
		//update variables
		pong = 1 - pong;
		step = step_size*stride;
		stride *= 2.0f;
	}
	//fullscreen post_process again
	set_bottom_postprocess_border(0);
	//default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// default sampler state
	sm.state.unbind();

	return  1 - pong;
}

// move a node in the scene
void simple_demo::move_node_by_key(std::tr1::shared_ptr<scene_node> node, const float factor,
				int key_pos_x,
				int key_neg_x,
				int key_pos_y,
				int key_neg_y,
				int key_pos_z,
				int key_neg_z)
{
	glm::vec3 movement(0.0f);
	if (glfwGetKey(key_pos_x))
	{
		movement.x += factor;
	}	
	if (glfwGetKey(key_neg_x))
	{
		movement.x -= factor;
	}
	if (glfwGetKey(key_neg_z))
	{
		movement.z -= factor;
	}	
	if (glfwGetKey(key_pos_z))
	{
		movement.z += factor;
	}
	if (glfwGetKey(key_neg_y))
	{
		movement.y -= factor;
	}	
	if (glfwGetKey(key_pos_y))
	{
		movement.y += factor;
	}

	node->transformation_matrix = glm::translate(movement) * node->transformation_matrix ;
}

// move a node in the scene
glm::vec3 simple_demo::move_by_key(const float factor,
				int key_pos_x,
				int key_neg_x,
				int key_pos_y,
				int key_neg_y,
				int key_pos_z,
				int key_neg_z)
{
	glm::vec3 movement(0.0f);
	if (glfwGetKey(key_pos_x))
	{
		movement.x += factor;
	}	
	if (glfwGetKey(key_neg_x))
	{
		movement.x -= factor;
	}
	if (glfwGetKey(key_neg_z))
	{
		movement.z -= factor;
	}	
	if (glfwGetKey(key_pos_z))
	{
		movement.z += factor;
	}
	if (glfwGetKey(key_neg_y))
	{
		movement.y -= factor;
	}	
	if (glfwGetKey(key_pos_y))
	{
		movement.y += factor;
	}

	return movement;
}

void simple_demo::update_camera_path(const float factor)
{
	
	//spline test
	if (glfwGetKey('C')) {
		cam_path.advance(factor);
		cam_eye = cam_path.get_eye();
		cam_center = cam_path.get_cam();
		//position_spline.advance(0.1f * factor);
		//mm::get("cube2")->transformation_matrix = glm::translate( position_spline.interpolate() );
	}

	if (glfwGetKey('V')) {
		cam_path.advance(-factor);
		cam_eye = cam_path.get_eye();
		cam_center = cam_path.get_cam();
		//position_spline.advance(-0.1f * factor);
		//mm::get("cube2")->transformation_matrix = glm::translate( position_spline.interpolate() );
	}

	if (glfwGetKey('B')) {
		cam_path.go_to_start();
		cam_eye = cam_path.get_eye();
		cam_center = cam_path.get_cam();
		//position_spline.go_to_start();
		//mm::get("cube2")->transformation_matrix = glm::translate( position_spline.interpolate() );
	}

	
	if (glfwGetKey('X') && !pushed['X']) {
		cam_path.append(cam_eye, cam_center);
		pushed['X'] = true;
	}
	
	if (glfwGetKey('X') == GLFW_RELEASE) {
		//position_spline.go_to_start();
		pushed['X'] = false;
		//mm::get("cube2")->transformation_matrix = glm::translate( position_spline.interpolate() );
	}

	if (glfwGetKey('P') && !pushed['P']) {
		cam_path.save();
		pushed['P'] = true;
	}
	
	if (glfwGetKey('P') == GLFW_RELEASE) {
		//position_spline.go_to_start();
		pushed['P'] = false;
		//mm::get("cube2")->transformation_matrix = glm::translate( position_spline.interpolate() );
	}
}