//FBO wrapper class

#include "fbo.h"

fbo::fbo() {
	_id = 0;
	_color_tex = 0;
	_depth_tex = 0;
}

fbo::~fbo() {
	if (_color_tex != 0) {
		glDeleteTextures(1, &_color_tex);
	}

	if (_depth_tex != 0) {
		glDeleteTextures(1, &_depth_tex);
	}

	if (_id != 0) {
		glDeleteFramebuffers(1, &_id);
	}
}

void fbo::init(int width, int height, GLuint min_filter) {
	auto format = GL_RGBA16F;
	auto internal_format = GL_RGBA;
	_width = width;
	_height = height;
	
	// ---------
	// fbo init
	// ---------
	glGenFramebuffers(1, &_id);
	glGenTextures(1, &_color_tex);
	glGenTextures(1, &_depth_tex);
	//glGenTextures(2, depth_texture);

	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	//depth texture
	glBindTexture(GL_TEXTURE_2D, _depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width,
	height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_TEXTURE_2D, _depth_tex, 0);

	//color texture
	glBindTexture(GL_TEXTURE_2D, _color_tex);
	//TODO need to sort mipmapping here
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D(GL_TEXTURE_2D, 0, format, width,
	height, 0, internal_format, GL_FLOAT, NULL);
	//no mipmaps for now
	//glGenerateMipmap(GL_TEXTURE_2D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 ,
		GL_TEXTURE_2D, _color_tex, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		// ToDo:
		printf("\nfbo: GL_FRAMEBUFFER_COMPLETE error 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		//return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}