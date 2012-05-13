#include "transformation_node.h"

transformation_node::transformation_node(glm::mat4& matrix)
{
	transformation_matrix = matrix;
	_shader_type = SH_NONE;
}