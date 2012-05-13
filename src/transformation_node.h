#pragma once

#include "scenenode.h"

class transformation_node : public scene_node {
private:

public:
	//members

	//functions
	transformation_node() { transformation_matrix = glm::mat4(1.0f); _shader_type = SH_NONE; }
	transformation_node(glm::mat4& matrix);
	virtual ~transformation_node() {}

	virtual void render() {} //do no rendering

};