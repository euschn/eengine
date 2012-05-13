// demo to test bullet stuff

#pragma once

#include "deferred_demo.h"
#include "bullet_manager.h"

class bullet_demo : public deferred_demo {
public:
	bullet_demo();
	virtual ~bullet_demo();

	virtual bool init();

protected:
	virtual void draw();
	virtual void process_keys(const float factor);

	glm::vec3 to_glm(const btVector3& vec) const;
	glm::mat4 to_glm(const btTransform& trans) const;
	virtual void init_physics();
	void build_klotz_tower();
	void boom(); //shoot a ball
	//bullet objects
	// shapes
	btCollisionShape* groundShape;
	btCollisionShape* fallShape;
	btCollisionShape* boxShape;
	shared_ptr<btCollisionShape> klotzShape;
	shared_ptr<btCollisionShape> ballShape;
	// motion states
	btDefaultMotionState* groundMotionState;
	btDefaultMotionState* fallMotionState;
	btMotionState* boxMotionState;
	// bodies
	btRigidBody* groundRigidBody;
	btRigidBody* fallRigidBody;
	btRigidBody* boxRigidBody;
	//btRigidBody* one_more_box[25];

	//pausing physics
	bool physics_pause;

	//shadow
	shadow_kit shadow;

	//scene selector
	int scene_select;
};