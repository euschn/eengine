#pragma once

#include "common.hpp"
#include "shader.hpp"
#include "glm\glm.hpp"

class skybox {
public:
	skybox();
	virtual ~skybox();
	void init(const std::string& path = "../textures/skybox/alpine");
	void draw(const glm::mat4& perspective_matrix, const glm::mat4& modelview_matrix);
protected:
	//functions

	//members
	GLuint skybox_texture, skybox_vao, skybox_vbo[3];
	std::tr1::shared_ptr<Shader> skybox_shader;

};