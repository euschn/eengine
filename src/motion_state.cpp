
#include "motion_state.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

motion_state::motion_state() 
{
	node = NULL;
	position_1 = btTransform();
}

motion_state::motion_state(mesh_ptr node) {
	this->node = node;
	position_1.setFromOpenGLMatrix(glm::value_ptr(node->transformation_matrix));
}

motion_state::~motion_state() {}

void motion_state::getWorldTransform(btTransform &worldTrans) const
{
	if (node == NULL) {
		worldTrans = btTransform();
		return;
	}

	worldTrans.setFromOpenGLMatrix(glm::value_ptr(node->transformation_matrix));
}


void motion_state::setWorldTransform(const btTransform &worldTrans)
{
	if (node == NULL) {
		return;
	}

	float f[16];
	worldTrans.getOpenGLMatrix(f);
	node->transformation_matrix = glm::mat4( f[0], f[1], f[2], f[3], 
										f[4], f[5], f[6], f[7], 
										f[8], f[9], f[10], f[11], 
										f[12], f[13], f[14], f[15] );
}