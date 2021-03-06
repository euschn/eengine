
#include <fstream>

#include "geometry_shader.hpp"

geometry_shader::geometry_shader(const string &path)
{
	// Load the shader files
	_success = false;
	string vertex_shader_source;
	if (file_exists(path+".vert")) {
		vertex_shader_source = read_file(path+".vert");
	} else {
		cerr << "Vertex shader file "
			<< path <<".vert does not exist." << endl;
		return;
	}

	string geometry_shader_source;
	if (file_exists(path+".geom")) {
		geometry_shader_source = read_file(path+".geom");
	} else {
		cerr << "Geometry shader file "
			<< path <<".geom does not exist." << endl;
		return;
	}

	string fragment_shader_source;
	if (file_exists(path+".frag")) {
		fragment_shader_source = read_file(path+".frag");
	} else {
		cerr << "Fragment shader file "
			<< path <<".frag does not exist." << endl;
		return;
	}

	// Compile the shaders

	_vertex_shader = compile(GL_VERTEX_SHADER, vertex_shader_source);
	if (_vertex_shader == 0)
		return;

	get_errors();

	_geometry_shader = compile(GL_GEOMETRY_SHADER, geometry_shader_source);
	if (_geometry_shader == 0)
		return;

	get_errors();

	_fragment_shader = compile(GL_FRAGMENT_SHADER, fragment_shader_source);
	if (_fragment_shader == 0)
		return;

	get_errors();

	// Link the shaders into a program

	link();
	if (_program == 0)
		return;

	_success = true;
	get_errors();
}

geometry_shader::~geometry_shader()
{
	glDeleteProgram(_program);
	glDeleteShader(_vertex_shader);
	glDeleteShader(_geometry_shader);
	glDeleteShader(_fragment_shader);
}

GLuint geometry_shader::compile (GLenum type, const string &source)
{
	// Create shader object

	GLuint shader = glCreateShader(type);

	if (shader == 0) {
		cerr << "Could not create shader object." << endl;
		return 0;
	}

	// Define shader source and compile

	const char* src = source.data();
	int len = source.size();

	glShaderSource(shader, 1, &src, &len);

	glCompileShader(shader);

	// Check for errors

	int status;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		cout << "Shader compilation failed." << endl;
		shader_log(shader);
	}

	get_errors();

	return shader;    
}

void geometry_shader::link(void)
{
	// Create program handle
	_program = glCreateProgram();

	// Attach shaders and link

	glAttachShader(_program, _vertex_shader);
	glAttachShader(_program, _geometry_shader);
	glAttachShader(_program, _fragment_shader);

	glLinkProgram(_program);

	// Check for problems

	int status;

	glGetProgramiv(_program, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		cout << "Shader linking failed." << endl;
		program_log(_program);

		glDeleteProgram(_program);
		_program = 0;
	}

	get_errors();
}