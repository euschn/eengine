// node that inherits btmotionstate

//TODO better make a motion state class that is handled by bt manager

#pragma once

#include "common.hpp"
#include "mesh_node.h"
#include "btBulletDynamicsCommon.h"

class motion_state : public btMotionState {
public:
	motion_state();
	motion_state(mesh_ptr node);
	virtual ~motion_state();

	virtual void getWorldTransform(btTransform &worldTrans) const;
	virtual void setWorldTransform(const btTransform &worldTrans);

protected:
	mesh_ptr node;
	btTransform position_1;
};

typedef std::tr1::shared_ptr<motion_state> motion_state_ptr;