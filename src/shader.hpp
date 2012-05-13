// Shader class
// originally from Realtime Rendering 2011 example
// modified by Eugen Jiresch

#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include "common.hpp"

class Shader
{
public:

	// Loads shaders from files and compiles them.
	// When path is "hello", the files "hello.frag" & "hello.vert"
	// will be loaded.
	Shader(const string &path);
	Shader() { _success = false; }
	virtual ~Shader();

	// Bind the shader to the OGL state-machine
	void bind() const
	{
		glUseProgram(_program);
	}

	// Unbind the shader
	void unbind() const
	{
		glUseProgram(0);
	}

	// Query the location of a vertex attribute inside the shader.
	GLint get_attrib_location(const std::string &name) const
	{
		return glGetAttribLocation(_program, name.c_str());
	}

	// Query the location of a uniform variable inside the shader.
	GLint get_uniform_location(const std::string &name) const
	{
		return glGetUniformLocation(_program, name.c_str());
	}

	//set a uniform value
	void set_uniform(const std::string &name, glm::vec2 data) const;
	void set_uniform(const std::string &name, glm::vec3 data) const;
	void set_uniform(const std::string &name, glm::vec4 data) const;
	void set_uniform(const std::string &name, float data) const;
	void set_uniform(const std::string &name, int data) const;
	void set_uniform(const std::string &name, glm::mat4 data) const;

	//set attribute values from currently active buffer
	void set_attrib_location(const std::string &name, const GLuint size, GLenum type = GL_FLOAT) const;

	// Define the name of the variable inside the shader which represents the final color for each fragment.
	void bind_frag_data_location(const std::string &name, const GLuint color_number = 0)
	{
		if(_program > 0)
		{
			glBindFragDataLocation(_program, color_number, name.c_str() );
			link();
		}
	}

	// A little cast helper.
	// With this you can simply do "if (shader) {...}" to test if a
	// shader has been compiled successfully.
	operator bool ()
	{
		return _success;
	}

protected:

	bool _success;

	GLuint _vertex_shader;
	GLuint _fragment_shader;
	GLuint _program;

	virtual GLuint compile(GLenum type, const string &source);
	virtual void link (void);

	void shader_log(GLuint shader);
	void program_log(GLuint program);
};

typedef std::tr1::shared_ptr<Shader> shader_ptr;

#endif //#ifndef _SHADER_HPP_

