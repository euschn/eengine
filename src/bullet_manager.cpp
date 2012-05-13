#include "bullet_manager.h"
#include "boost\foreach.hpp"

bullet_manager::bullet_manager()
{

	//get a broadphase
	broadphase = new btDbvtBroadphase();//bt32BitAxisSweep3(btVector3(-200, -10, -200), btVector3(200,200,200));
	//collision configuration
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	//solver
	solver = new btSequentialImpulseConstraintSolver;
	//finally, the world
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//set default gravity
	dynamicsWorld->setGravity(btVector3(0,-10,0));

	//motion states
	motion_states_set = vector<motion_state_ptr>();
	//rigid bodies
	rigid_bodies_set = vector<rigid_body_ptr>();
}

bullet_manager::~bullet_manager()
{
	//remove bodies from world
	BOOST_FOREACH(rigid_body_ptr b, rigid_bodies_set) {
		dynamicsWorld->removeRigidBody(b.get());
	}
	//cleanup
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}

bullet_manager* bullet_manager::inst()
{
	static bullet_manager the_instance;
	return &the_instance;
}

void bullet_manager::add_motion_state(motion_state_ptr m)
{
	inst()->motion_states_set.push_back(m);
}

//add the body to the world
void bullet_manager::add_rigid_body(rigid_body_ptr b)
{
	inst()->dynamicsWorld->addRigidBody(b.get());
	inst()->rigid_bodies_set.push_back(b);
}

//initialize some stuff, maybe for later
void bullet_manager::initialize()
{}