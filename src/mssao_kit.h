// class mssao kit - fbo and textures for mssao

#pragma once

#include "common.hpp"


class mssao_kit {
public:
	mssao_kit();
	virtual ~mssao_kit();

	virtual bool init();

	//members
	GLuint fbo;	
	GLuint tex[2];
	GLuint depth;
	//gbuffer dimensions
	GLuint width, height;
};