#include "shadow_kit.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
//swizzling
#include <glm/gtc/swizzle.hpp>


shadow_kit::shadow_kit()
{
	//TODO implement
	//set matrices	
	this->width = 512;
	this->height = 512;
	this->near_far = glm::vec2(5.0f, 150.0f);
	GLfloat w = static_cast<GLfloat>(width);
	GLfloat h = static_cast<GLfloat>(height);
	this->perspective_matrix = glm::perspective(45.0f, w / h, near_far.x, near_far.y);
	//float diff = 50.0f;
	//perspective_matrix = glm::ortho(- w/diff, w/diff, -h/diff, h/diff, near_far.x, near_far.y);
	this->view_matrix = glm::lookAt(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

}

shadow_kit::~shadow_kit()
{
	//TODO implement
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &depth_texture);
}

bool shadow_kit::init()
{	
	//load default shader
	shader_manager::load_single("../shader/shadow/gen");
	gen = shm::get("gen");

	//shadowmapping fbo
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &texture);
	glGenTextures(1, &depth_texture);

	//TODO: change shadowmap dimension/precision
	//depth texture
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width,
	height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_TEXTURE_2D, depth_texture, 0);

	//variance shadow texture texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
	//TODO does this improve quality?
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width,
	height, 0, GL_RGBA, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);

	//glBindTexture(GL_TEXTURE_2D, gbuffer_tex[ALBEDO]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 ,
	GL_TEXTURE_2D, texture, 0);

	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		// ToDo:
		printf("\nshadow_kit: GL_FRAMEBUFFER_COMPLETE error 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

glm::mat4 shadow_kit::shadow_matrix()
{
	return glm::translate(0.5f, 0.5f, 0.5f) * glm::scale(0.5f, 0.5f, 0.5f) *
		this->perspective_matrix * this->view_matrix;
}