// wrapper class for FBOS

#pragma once

#include "common.hpp"


class fbo {
public:
	fbo();
	virtual ~fbo();

	void init(int width, int height, GLuint min_filter = GL_LINEAR);

	GLuint id() const {return _id;}
	GLuint color_tex() const {return _color_tex;}
	GLuint depth_tex() const {return _id;}
	GLuint width() const {return _width;}
	GLuint height() const {return _height;}
private:
	GLuint _id;
	GLuint _color_tex;
	GLuint _depth_tex;
	GLuint _width;
	GLuint _height;
};