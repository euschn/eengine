#ifndef _TESSELATIONSHADER_HPP_
#define _TESSELATIONSHADER_HPP_

#include "geometry_shader.hpp"

// adds a tesselation shader to the basic shader class

class tesselation_shader : public geometry_shader {
public:
	tesselation_shader(const string &path);
	tesselation_shader() {}
	virtual ~tesselation_shader();

protected:
	//handle for the tesselation shader
	GLuint _control_shader; 	
	GLuint _evaluation_shader;

	virtual GLuint compile(GLenum type, const string &source);
	virtual void link (void);

};

#endif