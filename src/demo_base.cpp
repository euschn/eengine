#include "demo_base.h"
#include "tga.h"
#include "config_manager.h"

#include <fstream>
#include <glm/gtx/transform.hpp>
// glm::perspective
//#include <glm/gtc/matrix_projection.hpp>
// glm::translate, glm::rotate, glm::scale
#include <glm/gtc/matrix_transform.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp> 
//thread stuff
//#include "boost\thread.hpp"
//#include "boost\thread\future.hpp"
//#include "boost\bind.hpp"
//#include "boost\interprocess\detail\move.hpp"


demo_base::demo_base(void):
	perspective_matrix(1.0f),
	modelview_matrix(1.0f),
	view_matrix(1.0f)
{
	width = cfg::get_int("width");
	height = cfg::get_int("height");
	near_far = glm::vec2(0.1f, 1000.0f);
	perspective_matrix = glm::perspective(45.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), near_far.x, near_far.y);
	title = "EEngine";
	frames = 0;
	average_frames = 0;
	average_steps = 1;
	delta_ = 0;
	fullscreen = false;
	loaded = false;
	mousex = 0;
	mousey = 0;
	fps_timer = 0;
	current_time = 0;
	last_time = 0;
	// TODO does 512 suffice to cover all keys?
	pushed = std::vector<bool>(512, false);
}


demo_base::~demo_base(void)
{
}

bool demo_base::init()
{
	if (!init_window())
	{
		return false;
	}

	//do opengl initialization here

	return true; //everything went fine
}

bool demo_base::threaded_init()
{
	//boost::packaged_task<bool> pt(boost::bind(&demo_base::init, this));
	//boost::unique_future<bool> fi=pt.get_future();

	//boost::thread task(boost::move(pt)); // launch task on a thread

	//fi.wait();
	//if (fi.is_ready() && fi.get())
	//	std::cout << "future result is good: " << fi.get() << std::endl;

	//return fi.get();
}

//main loop of the demo
//parts taken from RTR ogl3.2 demos
void demo_base::run()
{
	bool running = true;

	while (running) 
	{
		//update time
		current_time = glfwGetTime();
		delta_ = current_time - last_time;
		last_time = current_time;

		update();

		draw();

		glfwSwapBuffers();


		//update fps
		frames++;
		double time = glfwGetTime();
		if ((time - fps_timer) > 1.0)
		{
			//lets calculate the fps average
			average_frames += (static_cast<float>(frames) - average_frames) / average_steps++;
			char c_title[256] = {0};
			sprintf(c_title, "%s %d fps, %.2f avg, ms 1: %.3f, ms 2:  %.3f", title.c_str(), frames, average_frames, millisecs_1, millisecs_2);
			glfwSetWindowTitle(c_title);
			frames = 0;
			fps_timer = time;
		}

		// Get OGL errors

		get_errors();

		// Check if the window has been closed
		running = running && !glfwGetKey( GLFW_KEY_ESC );
		running = running && glfwGetWindowParam( GLFW_OPENED );
	}

	cleanup();


}

//init glfw window and gl3w
//parts taken from RTR ogl3.2 demos
bool demo_base::init_window(const bool full_screen)
{
	glfwInit();

	
	// Set flags so GLFW creates the desired OpenGL context
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	int win = GLFW_WINDOW;
	if (full_screen) {
		win = GLFW_FULLSCREEN;
	}
	if (glfwOpenWindow(	width,height, 0,0,0,0, 24, 8,
		win) != GL_TRUE) {
			cerr << "Failed to initialize OpenGL window." << endl;
			glfwTerminate();
			return 1;
	}

	glfwSetWindowTitle(title.c_str());

	// Load extensions and OGL>1.1 functions with gl3w
	// http://github.com/skaslev/gl3w

	if (gl3wInit()) 
	{
		cerr << "failed to initialize OpenGL" << endl;
		return 1;
	}
	if (!gl3wIsSupported(3, 2)) 
	{
		cerr << "OpenGL 3.2 not supported" << endl;
		return 1;
	}

	cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;


	// Enable sRGB gamma correction for framebuffer output.
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// Clear errors. GLFW setup seems to cause an opengl error while setup.
	glGetError();

	glViewport(0, 0, width, height);


	return true;
}


//update demo state
void demo_base::update() 
{
	//camera movement
	float factor = static_cast<float>(delta()) * 5;
	update_camera_movement(factor);
	//handle user input
	process_keys(factor);
}

//handle user input
void demo_base::process_keys(const float factor)
{
}

void demo_base::update_camera_movement(float factor)
{
	glm::vec3 view_dir = glm::normalize(cam_eye - cam_center);
	glm::vec3 side_dir = glm::cross(view_dir, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey('D'))
	{
		cam_eye -= factor*side_dir;
		cam_center -= factor*side_dir;
	}	
	if (glfwGetKey('A'))
	{
		cam_eye += factor*side_dir;
		cam_center += factor*side_dir;
	}
	if (glfwGetKey(GLFW_KEY_SPACE))
	{
		cam_eye[1] += factor;
		cam_center[1] += factor;
	}	
	if (glfwGetKey(GLFW_KEY_LSHIFT))
	{
		cam_eye[1] -= factor;
		cam_center[1] -= factor;
	}
	
	if (glfwGetKey('S'))
	{
		cam_eye += factor*view_dir;
		cam_center += factor*view_dir;
	}	
	if (glfwGetKey('W'))
	{
		cam_eye -= factor*view_dir;
		cam_center -= factor*view_dir;
	}

	int newx, newy;
	glfwGetMousePos(&newx, &newy);
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_2))
	{
	if (newx != mousex)
	{
		glm::vec4 temp = 
			glm::translate(cam_eye) *
			glm::rotate(static_cast<GLfloat>((mousex - newx)*0.2), 0.0f, 1.0f, 0.0f) * 
			glm::translate(-cam_eye) *
			glm::vec4(cam_center, 1.0f);
		cam_center = glm::vec3(temp.x, temp.y, temp.z);
	}
	if (newy != mousey)
	{
		glm::vec4 temp = 
			glm::translate(cam_eye) *
			glm::rotate(static_cast<GLfloat>((newy - mousey)*0.2), side_dir) * 
			glm::translate(-cam_eye) *
			glm::vec4(cam_center, 1.0f);
		cam_center = glm::vec3(temp.x, temp.y, temp.z);
	}
	}
	mousex = newx;
	mousey = newy;
}

void demo_base::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void demo_base::cleanup()
{
}

//returns the top perspective matrix, or identity if stack is empty
void demo_base::pop_perspective_matrix()
{
	if (!perspective_stack.empty())
	{
		perspective_matrix = perspective_stack.top();
		perspective_stack.pop();
	}
	else
	{
		perspective_matrix = glm::mat4(1.0f);
	}
}

//returns the top modelview matrix, or identity if stack is empty
void demo_base::pop_modelview_matrix()
{
	if (!modelview_stack.empty())
	{
		modelview_matrix = modelview_stack.top();
		modelview_stack.pop();
	}
	else
	{
		modelview_matrix = glm::mat4(1.0f);
	}
}

//replacement for gluLookAt
//sets the camera position and view direction for the modelview matrix
//source: http://www.opengl.org/wiki/GluLookAt_code
void demo_base::look_at(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	glm::vec3 forward = glm::normalize(center - eye);
	//side = forward x up
	glm::vec3 side = glm::normalize(glm::cross(forward,up));
	//recompute up: up = side x forward
	glm::vec3 up2 = glm::cross(side, forward);
	
	glm::mat4 temp(1.0f);

	temp[0][0] = side[0];
	temp[1][0] = side[1];
	temp[2][0] = side[2];
	temp[3][0] = 0.0;

	temp[0][1] = up2[0];
	temp[1][1] = up2[1];
	temp[2][1] = up2[2];
	temp[3][1] = 0.0;

	temp[0][2] = -forward[0];
	temp[1][2] = -forward[1];
	temp[2][2] = -forward[2];
	temp[3][2] = 0.0;

	temp[0][3] = 0.0;
	temp[1][3] = 0.0;
	temp[2][3] = 0.0;
	temp[3][3] = 1.0;

	modelview_matrix = temp;
	modelview_matrix = glm::translate(modelview_matrix, -eye);
}

void demo_base::look_at(glm::mat4& matrix, glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{	glm::vec3 forward = glm::normalize(center - eye);
	//side = forward x up
	glm::vec3 side = glm::normalize(glm::cross(forward,up));
	//recompute up: up = side x forward
	glm::vec3 up2 = glm::cross(side, forward);
	
	glm::mat4 temp(1.0f);

	temp[0][0] = side[0];
	temp[1][0] = side[1];
	temp[2][0] = side[2];
	temp[3][0] = 0.0;

	temp[0][1] = up2[0];
	temp[1][1] = up2[1];
	temp[2][1] = up2[2];
	temp[3][1] = 0.0;

	temp[0][2] = -forward[0];
	temp[1][2] = -forward[1];
	temp[2][2] = -forward[2];
	temp[3][2] = 0.0;

	temp[0][3] = 0.0;
	temp[1][3] = 0.0;
	temp[2][3] = 0.0;
	temp[3][3] = 1.0;
	
	matrix = temp;
	matrix = glm::translate(matrix, -eye);
}

//overloaded function of look_at(vec3, vec3, vec3)
void demo_base::look_at(GLfloat eyex, GLfloat eyey, GLfloat eyez,
	GLfloat centerx, GLfloat centery, GLfloat centerz,
	GLfloat upx, GLfloat upy, GLfloat upz)
{
	look_at(glm::vec3(eyex, eyey, eyez), 
		glm::vec3(centerx, centery, centerz), 
		glm::vec3(upx, upy, upz) );
}

void demo_base::look_at(glm::mat4& matrix, GLfloat eyex, GLfloat eyey, GLfloat eyez,
		GLfloat centerx, GLfloat centery, GLfloat centerz,
		GLfloat upx, GLfloat upy, GLfloat upz)
{
	look_at(matrix, glm::vec3(eyex, eyey, eyez), 
		glm::vec3(centerx, centery, centerz), 
		glm::vec3(upx, upy, upz) );
}

//read and build a texture from a tga image
bool demo_base::load_tga_texture(GLuint * texture_handle, const std::string& filepath, GLint color_format, bool mipmap, GLint texture_format)
{
	bool status = false;
	tga::Texture tex;

	if (tga::LoadTGA(&tex, filepath.c_str()))
	{
		status = true;

		glGenTextures(1, texture_handle);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(texture_format, *texture_handle);

		if(mipmap)
		{
			glTexImage2D(texture_format, 0, color_format, tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);
			glTexParameterf(texture_format, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtering
			glTexParameterf(texture_format, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
			glTexParameterf(texture_format, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameterf(texture_format, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);	//generate the mipmaps
		}
		else
		{
		// Generate The Texture
			glTexImage2D(texture_format, 0, color_format, tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);
			glTexParameteri(texture_format, GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
			glTexParameteri(texture_format, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
			glTexParameteri(texture_format, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameteri(texture_format, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	}
	else {
		//reading the file failed
		return false;
	}

	if (tex.imageData)					// If Texture Image Exists
	{
		free(tex.imageData);				// Free The Texture Image Memory
	}

	return status;
}

//very simple function to set some values from a config file
void demo_base::load_config(const std::string& filepath)
{
	std::ifstream fp_in;
	fp_in.open(filepath, std::ios::in);

	string key;
	string bla;

	while (getline(fp_in, key, ' '))
	{
		if (key.compare("width") == 0) {
			fp_in >> width;
		}
		else if (key.compare("height") == 0) {
			fp_in >> height;
		}
		//extend as needed
		//pattern:
		//else if (key.compare("something") == 0) {
		//	fp_in >> some_member;
		//}
		else {
			std::cout << "unknown option '" << key << "' !\n";
			fp_in >> bla;
		}
		//ignore content until end of file
		getline(fp_in, key);
	}
	std::cout << "done loading config\n";
}