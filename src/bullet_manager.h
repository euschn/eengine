// class bullet manager
// singleton

#pragma once

#include <map>
#include <unordered_set>
#include "btBulletDynamicsCommon.h"
#include "glm/glm.hpp"
#include "motion_state.h"

using namespace std::tr1;

typedef shared_ptr<btRigidBody> rigid_body_ptr;

class bullet_manager
{
public:
	//get the instance
	static bullet_manager* inst();

	static void init() { inst()->initialize(); }
	static btDiscreteDynamicsWorld* world() { return inst()->dynamicsWorld; }
	static vector<motion_state_ptr>* motion_states() { return &(inst()->motion_states_set); }
	static void add_motion_state(motion_state_ptr m);
	static void add_rigid_body(rigid_body_ptr b);

	static glm::vec3 to_glm(const btVector3& vec)
	{
		return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
	}

	static glm::mat4 to_glm(const btTransform& trans)
	{
			float f[16];
			trans.getOpenGLMatrix(f);
			return glm::mat4( f[0], f[1], f[2], f[3], 
							f[4], f[5], f[6], f[7], 
							f[8], f[9], f[10], f[11], 
							f[12], f[13], f[14], f[15] );
	}

private:
	bullet_manager();
	virtual ~bullet_manager();

	void initialize();

	//bullet stuff
	//broadphase
	btBroadphaseInterface* broadphase;
	//collision configuration
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	//solver
	btSequentialImpulseConstraintSolver* solver;
	//finally, the world
	btDiscreteDynamicsWorld* dynamicsWorld;

	//motion state set
	vector<motion_state_ptr> motion_states_set;
	//bodies
	vector<rigid_body_ptr> rigid_bodies_set;

};

typedef bullet_manager btm;