#pragma once

#include "scenenode.h"
#include "common.hpp"
#include "shader.hpp"


//basic class to contain a geometry object
//object is specified by an id of a VAO and the number of elements to be drawn
//(via glDrawElements)
class geometry_node : public scene_node {
private:

public:
	//members
	GLuint vao_id; //handle to the vao containing the data
	GLuint num_elements; //number of elements in the vao

	//functions
	geometry_node();
	geometry_node(glm::mat4& matrix, GLuint vao_id, GLuint num_elements, int shader_type = SH_NONE);
	virtual ~geometry_node() {}
	//set shader for the object
	void set_shader_type(int type) { _shader_type = type; }
	//render with the object
	virtual void render();

};