#pragma once

#include "common.hpp"
#include <glm/glm.hpp>
#include <stack>

//needed for boost future
#define BOOST_NO_RVALUE_REFERENCES

class demo_base
{
public:
	demo_base(void);
	virtual ~demo_base(void);

	virtual bool init();	//init ogl stuff
	bool threaded_init();
	virtual void run();		//main loop

protected:
	//members
	glm::mat4 perspective_matrix;
	glm::mat4 modelview_matrix;
	glm::mat4 view_matrix;
	glm::mat4 normal_matrix;
	glm::vec2 near_far;
	bool fullscreen;
	bool loaded;

	//camera handling
	glm::vec3 cam_eye, cam_center, cam_up;

	//mouselook
	int mousex, mousey;

	//tracking fps
	double fps_timer;
	int frames;
	float average_frames, average_steps, millisecs_1, millisecs_2;

	int width, height;
	string title;

	//pushed buttons
	std::vector<bool> pushed;

	//functions
	bool init_window(const bool full_screen = false);		//initialize glfw
	virtual void update();	//update the state of the demo (before drawing)
	virtual void process_keys(const float factor);	//process keypresses
	void update_camera_movement(const float factor);
	virtual void draw();	//main drawing method
	virtual void cleanup(); //do some cleanup if necessary
	//matrix and camera functions
	void push_perspective_matrix() { perspective_stack.push(perspective_matrix); }
	void push_modelview_matrix() { modelview_stack.push(modelview_matrix); }
	void pop_perspective_matrix();
	void pop_modelview_matrix();
	void load_identity(glm::mat4& matrix) { matrix = glm::mat4(1.0f); }
	void look_at(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	void look_at(GLfloat eyex, GLfloat eyey, GLfloat eyez,
		GLfloat centerx, GLfloat centery, GLfloat centerz,
		GLfloat upx, GLfloat upy, GLfloat upz);
	void look_at(glm::mat4& matrix, glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	void look_at(glm::mat4& matrix, GLfloat eyex, GLfloat eyey, GLfloat eyez,
		GLfloat centerx, GLfloat centery, GLfloat centerz,
		GLfloat upx, GLfloat upy, GLfloat upz);
	//framerate independent time tracking
	double delta() {return delta_;}
	//texture loading
	bool load_tga_texture(GLuint * texture_handle, const std::string& filepath, GLint color_format = GL_RGBA, bool mipmap = false, GLint texture_format = GL_TEXTURE_2D);
	//loading from a config file
	virtual void load_config(const std::string& filepath = "../../config.txt");
	void set_title(const std::string& title_string) {title = title_string;}

private:
	//replacement for opengl FF stacks
	std::stack<glm::mat4> perspective_stack;
	std::stack<glm::mat4> modelview_stack;

	//framerate independent time tracking
	double current_time, last_time, delta_;

};

