#ifndef _GEOMETRYSHADER_HPP_
#define _GEOMETRYSHADER_HPP_

#include "shader.hpp"

// adds a geometry shader to the basic shader class

class geometry_shader : public Shader {
public:
	geometry_shader(const string &path);
	geometry_shader() {}
	virtual ~geometry_shader();

protected:
	//handle for the geometry shader
	GLuint _geometry_shader; 

	virtual GLuint compile(GLenum type, const string &source);
	virtual void link (void);

};

#endif