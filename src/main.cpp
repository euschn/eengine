#include "common.hpp"
#include "config_manager.h"
#include "bullet_manager.h"
#include "pointlight_stress.h"
#include "shadow_demo.h"
#include "bullet_demo.h"
#include "path_instance_demo.h"
#include "mssao_demo.h"
#include "bloom_demo.h"
#include "fbo_demo.h"
#include "dof_demo.h"
#include "sat_test.h"
#include "mipmap_test.h"
#include "simple_rt_demo.h"

#include <iostream>
#include "btBulletDynamicsCommon.h"

using namespace std::tr1;

void bullet_test()
{
	////get a broadphase
	//btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	////collision configuration
	//btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	//btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	////solver
	//btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	////finally, the world
	//btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	////gravity
	//dynamicsWorld->setGravity(btVector3(0,-10,0));
	// boilerplate end!
	// ----------------

	//the fun starts
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0), 1);
	//sphere shape, radius 1m
	btCollisionShape* fallShape = new btSphereShape(1);
	//rigid bodies
	btDefaultMotionState* groundMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo 
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0,0,0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	bullet_manager::world()->addRigidBody(groundRigidBody);

	//falling sphere
	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,50,0)));
	btScalar mass = 1;
	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo
		fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	bullet_manager::world()->addRigidBody(fallRigidBody);

	//sim stepping
	for (int i=0; i<300; ++i) {

		btm::world()->stepSimulation(1/60.f, 10);

		btTransform trans;
		fallRigidBody->getMotionState()->getWorldTransform(trans);

		std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;

	}

	//world cleanup
	btm::world()->removeRigidBody(fallRigidBody);
	delete fallRigidBody->getMotionState();
	delete fallRigidBody;
	
	btm::world()->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;

	//cleanup crap
	//delete dynamicsWorld;
	//delete solver;
	//delete dispatcher;
	//delete collisionConfiguration;
	//delete broadphase;
}

void run_demo()
{
	config_manager::load();
	shared_ptr<demo_base> demo;
	//select the demo specified in config.txt
	int demo_type = config_manager::get_int("demo");
	switch (demo_type) {
	case 1:
		demo = shared_ptr<demo_base>(new mipmap_test());
		break;
	case 2:
		demo = shared_ptr<demo_base>(new deferred_demo());
		break;
	case 3:
		demo = shared_ptr<demo_base>(new pointlight_stress());
		break;
	case 4:
		demo = shared_ptr<demo_base>(new shadow_demo());
		break;
	case 5:
		demo = shared_ptr<demo_base>(new bullet_demo());
		break;
	case 6:
		demo = shared_ptr<demo_base>(new sat_test());
		break;
	case 7:
		demo = shared_ptr<demo_base>(new path_instance_demo());
		break;
	case 8:
		demo = shared_ptr<demo_base>(new mssao_demo());
		break;
	case 9:
		demo = shared_ptr<demo_base>(new bloom_demo());
		break;
	case 10:
		demo = shared_ptr<demo_base>(new fbo_demo());
		break;
	case 11:
		demo = shared_ptr<demo_base>(new dof_demo());
		break;
	case 12:
		demo = shared_ptr<demo_base>(new simple_rt_demo());
		break;
	default: 
		demo = shared_ptr<demo_base>(new demo_base());
		break;
	}

	try {
		if (demo->init())
		{
			demo->run();
		}
		else
		{
			std::cout << "failed to init!\n";
		}
		std::cout << "done!\n";
	} catch(std::exception& e) {
		std::cout << "an exception was thrown, terminating!\n";
	}

	glfwTerminate();
}

int main()
{
	run_demo();
	//bullet_test();

	//fizzbuzz
	//for (int i =1; i < 101; ++i) {
	//	bool mult3 = (i % 3 == 0);
	//	bool mult5 = (i % 5 == 0);
	//	if (mult3 && mult5) {
	//		std::cout << "FizzBuzz\n";
	//	}
	//	else if (mult3) {
	//		std::cout << "Fizz\n";
	//	}
	//	else if (mult5) {
	//		std::cout << "Buzz\n";
	//	}
	//	else {
	//		cout << i << std::endl;
	//	}
	//}

	return 0;
}