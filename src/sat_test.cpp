// sat test: small demo to test summed area tables

#include "sat_test.h"

#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
//swizzling
#include <glm/gtc/swizzle.hpp>

sat_test::sat_test() {
	test_texture = 0;
	filter_width = 0;
	filter_push = false;
}

sat_test::~sat_test()
{
	if (test_texture > 0) {
		glDeleteTextures(1, &test_texture);
	}
}

bool sat_test::init()
{
	test_width = cfg::get_int("test_width");
	test_height = cfg::get_int("test_height");
	if (!super_init()) return false;
	if (!init_texture()) return false;

	if (!sam.init(test_width, test_height)) return false;
	
	//texture loading
	texture_manager::load("../textures");

	test_sampler.init();
	test_sampler.parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	test_sampler.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtering
	test_sampler.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	test_sampler.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//filter test shader
	shader_manager::load_single("../shader/sat_filter");
	shm::get("sat_filter");
	shm::get("sat_filter")->bind();
	glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	shm::get("sat_filter")->set_uniform("perspective_matrix",  ortho_matrix);
	shm::get("sat_filter")->set_uniform("in_texture", 0);
	shm::get("sat_filter")->set_uniform("filter_width", filter_width);
	shm::get("sat_filter")->set_uniform("dimension", glm::vec2(static_cast<float>(test_width), static_cast<float>(test_height)) );

	return true;
}

void sat_test::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint tex = txm::get("bla_inverse");

	glViewport(0,0,width, height/3);
	draw_postprocess(shm::get("pp_echo"), tex, 0);

	//generate the sat
	int pong = generate_sat(tex, sam);

	// -----------
	// show result
	glViewport(0, height/3, width, height/3);
	set_left_postprocess_border(0.f);

	glBindTexture(GL_TEXTURE_2D, sam.texture[pong]);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	shm::bind("sat_filter");
	shm::get("sat_filter")->set_uniform("filter_width", filter_width);
	test_sampler.bind(0);
	draw_postprocess(shm::get("sat_filter"), sam.texture[pong], 0);
	test_sampler.unbind();
	glViewport(0, 2*height/3, width, height/3);

	draw_postprocess(shm::get("pp_echo"), sam.texture[pong], 0);

}


//init test texture
bool sat_test::init_texture()
{
	GLuint texture_format = GL_TEXTURE_2D;

	//generate a new texture
	glGenTextures(1, &test_texture);
	glBindTexture(texture_format, test_texture);

	//generate the image
	GLuint format = 4;
	std::vector<GLfloat> data(format * test_width * test_height);
	float step_size = 1.0f / static_cast<float>(test_width * test_height);
	float step = 0.0f;
	for (unsigned int y = 0; y < test_height; ++y) {
		for (unsigned int x = 0; x < test_width; ++x) {
			unsigned int index = (y * test_width * format) + x*format;
			step += 0;//step_size;
			data[index] = step_size;//0.001f + step;
			data[index + 1] = 0.000f + step;
			data[index + 2] = 0.000f + step;
			data[index + 3] = 1.0f + step;
		}
	}
	//
	//for (unsigned int i=0; i< data.size(); ++i) {
	//	std::cout << data[i] << std::endl;
	//}

	glTexImage2D(texture_format, 0, GL_RGBA32F, test_width,
		test_height, 0, GL_RGBA, GL_FLOAT, &data[0]);
	glTexParameterf(texture_format, GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameterf(texture_format, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameterf(texture_format, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	glTexParameterf(texture_format, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void sat_test::process_keys(const float factor)
{
	if (glfwGetKey('Q') && !test_sampler.bound()) {
		test_sampler.bind();
	}

	
	if (glfwGetKey('E') && test_sampler.bound()) {
		test_sampler.unbind();
	}

	if (glfwGetKey('1') && filter_width > 0 && !pushed['1']) {
		filter_width -= 1.0f;
		pushed['1'] = true;
	}

	if (glfwGetKey('1') == GLFW_RELEASE) {
		pushed['1'] = false;
	}

	if (glfwGetKey('2') == GLFW_RELEASE) {
		pushed['2'] = false;
	}

	if (glfwGetKey('2') && filter_width < 50 && !pushed['2']) {
		filter_width += 1.0f;
		pushed['2'] = true;
	}
}
