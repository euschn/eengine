// sat maker: wrapper class for computing summed area tables

#include "sat_maker.h"
#include "shader_manager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

sat_maker::sat_maker()
{
	width = 1;
	height = 1;
	fbo[0] = 0;
	fbo[1] = 1;
	texture[PING] = 0;
	texture[PONG] = 0;
	//depth_texture[PING] = 0;
	//depth_texture[PONG] = 0;
}

sat_maker::~sat_maker()
{
	glDeleteFramebuffers(2, fbo);
	glDeleteTextures(2, texture);
	//glDeleteTextures(2, depth_texture);
}

bool sat_maker::init(const GLuint w, const GLuint h)
{
	width = w;
	height = h;
	auto format = GL_RGBA32F;
	auto internal_format = GL_RGBA;

	// ----------------------------
	// load and init default shader
	// ----------------------------
	shader_manager::load_single("../shader/sat");
	gen = shm::get("sat");
	
	gen->bind();
	glm::mat4 ortho_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	gen->set_uniform("perspective_matrix",  ortho_matrix);
	gen->set_uniform("in_texture", 0);
	gen->set_uniform("width", static_cast<float>(width) );
	gen->set_uniform("offset", 1.0f);
	gen->unbind();

	// -------------
	// sampler state
	// -------------
	
	state.init();
	state.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Linear Filtering
	state.parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Linear Filtering
	state.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	state.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// ---------
	// fbo init
	// ---------
	glGenFramebuffers(2, fbo);
	glGenTextures(2, texture);
	//glGenTextures(2, depth_texture);

	for (int num_fbo = 0; num_fbo < 2; ++num_fbo) 
	{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo[num_fbo]);

	//depth texture
	//glBindTexture(GL_TEXTURE_2D, depth_texture[num_fbo]);
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
	//GL_TEXTURE_2D, depth_texture[num_fbo], 0);

	//color texture
	glBindTexture(GL_TEXTURE_2D, texture[num_fbo]);
	//TODO need to sort mipmapping here
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D(GL_TEXTURE_2D, 0, format, width,
	height, 0, internal_format, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);

	//glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 ,
	GL_TEXTURE_2D, texture[num_fbo], 0);

	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		// ToDo:
		printf("\nsat_maker: GL_FRAMEBUFFER_COMPLETE error 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	std::cout << "sat maker init successful\n";
	return true;
}



