// node that inherits btmotionstate

//TODO better make a motion state class that is handled by bt manager

#pragma once

#include "mesh_node.h"
#include "btBulletDynamicsCommon.h"

class motion_state_node : public mesh_node, public btMotionState {
public:
	motion_state_node();
	motion_state_node(mesh_node& node);
	motion_state_node(mesh_ptr node);
	motion_state_node(const glm::mat4& mat);
	virtual ~motion_state_node() {}

	virtual void getWorldTransform(btTransform &worldTrans) const;
	virtual void setWorldTransform(const btTransform &worldTrans);

protected:
};

typedef std::tr1::shared_ptr<motion_state_node> motion_node_ptr;