
#include "bullet_demo.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "config_manager.h"
#include "pointlight_node.h"
#include "motion_state_node.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
//swizzling
#include <glm/gtc/swizzle.hpp>

//use carefully
using namespace glm;

bullet_demo::bullet_demo()
{}

bullet_demo::~bullet_demo()
{
	
	//world cleanup
	btm::world()->removeRigidBody(fallRigidBody);
	delete fallRigidBody->getMotionState();
	delete fallRigidBody;
	
	btm::world()->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;
	
	btm::world()->removeRigidBody(boxRigidBody);
	//delete boxRigidBody->getMotionState();
	delete boxRigidBody;

	//shapes delete
	delete fallShape;
	delete groundShape;
	delete boxShape;

	cleanup();
}

bool bullet_demo::init()
{
	if (!super_init()) return false;
	
	//textures
	txm::load("../textures");
	txm::load("../textures/titles");
	txm::load("../textures/titles/demo_reel");

	//models
	shader_ptr gbg = shader_manager::get("gbuffer_gen");
	mm::load_single("../models/cube2.obj", gbg,
		glm::translate(-2.f, 50.f, 0.f) * glm::rotate(0.f, 1.0f, 0.0f, 0.0f), texture_manager::get("bla_inverse"));
	mesh_manager::load_single("../models/quad.obj", gbg, glm::translate(0.0f, 3.0f, 0.0f),
		texture_manager::get("wood19"));
	mm::load_single("../models/maya/klotz1.dae", gbg, 
		glm::mat4(1.0f), txm::get("wood19"));
	//mm::load_single("../models/sponza.obj", gbg, txm::get("green"));
	//root->add_child(mm::get("sponza"));
	mesh_manager::load_single("../models/maya/sphere.dae", gbg, txm::get("cloth04"),  glm::translate(0.0f, 50.0f, 0.0f));
	mesh_ptr ball(new mesh_node(mm::get("sphere")) );
	mm::add(ball, "ball");
	mm::get("ball")->set_texture_id(txm::get("green"));
	mm::get("ball")->set_shader_type(SH_TEXTURE);
	mm::get("ball")->transformation_matrix = glm::scale(0.25f, 0.25f, 0.25f);
	//ball scale node
	mesh_ptr ball_scale(new mesh_node());
	ball_scale->set_shader_type(SH_NONE);
	//ball_scale->transformation_matrix = glm::scale(0.25f, 0.25f, 0.25f);
	mm::add(ball_scale, "ball_scale");
	root->add_child(ball_scale);
	
	cam_center.y += 5;
	cam_eye.y += 5;
	cam_eye.z += 20;

	//scenegraph setup
	//root->add_child(mm::get("sphere"));
	//root->add_child(mm::get("cube2"));
	root->add_child(mm::get("quad"));

	//skybox
	sb.init();

	//create some bullet objects
	init_physics();
	physics_pause = true;

	//shadow
	shadow.width = 1024;
	shadow.height = 1024;
	shadow.near_far.y = 25;
	shadow.near_far.y = 130;
	GLfloat w = static_cast<GLfloat>(shadow.width);
	GLfloat h = static_cast<GLfloat>(shadow.height);
	float diff = 25.0f;
	shadow.perspective_matrix = glm::ortho(-w/diff, w/diff, -h/diff, h/diff, shadow.near_far.x, shadow.near_far.y);
	//shadow.perspective_matrix = glm::perspective(60.0f, w / h, shadow.near_far.x, shadow.near_far.y);
	shadow.init();
	light_0 = vec3(-15.0f, 62.0f, 5.0f);
	shadow.view_matrix = glm::lookAt(light_0, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	shm::bind("phong");
	shm::get("phong")->set_uniform("enable_shadowmap", true);
	shm::get("phong")->set_uniform("shadow_texture", 3);
	shm::get("phong")->set_uniform("shadow_dimension", vec2(static_cast<float>(shadow.width), static_cast<float>(shadow.height) ) );

	//scene select
	scene_select = 0;

	return true;
}

void bullet_demo::init_physics()
{
	
	//the bullet fun starts
	groundShape = new btStaticPlaneShape(btVector3(0,1,0), 1);
	//sphere shape, radius 1m
	fallShape = new btSphereShape(1);
	//box shape
	boxShape = new btBoxShape(btVector3(1,1,1));	
	//sphere shape, radius 1m
	ballShape = shared_ptr<btCollisionShape>(new btSphereShape(0.25));
	//rigid bodies
	groundMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo 
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0,0,0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);

	bullet_manager::world()->addRigidBody(groundRigidBody);

	//falling sphere
	fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,50,0)));
	btScalar mass = 1;
	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo
		fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	//bullet_manager::world()->addRigidBody(fallRigidBody);

	//falling box
	motion_node_ptr msn( new motion_state_node(mm::get("cube2")) );
	root->add_child(msn);
	boxMotionState = msn.get();
		//new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(-2,50,0)));

	boxShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo
		boxRigidBodyCI(mass, boxMotionState, boxShape, fallInertia);
	boxRigidBody = new btRigidBody(boxRigidBodyCI);
	bullet_manager::world()->addRigidBody(boxRigidBody);

	//screw that, lets get  a lot of boxes
	GLuint box_texture = txm::get("bla_inverse");
	int cubedim = 5;
	//for (int y=0; y < cubedim; ++y) {
	//	for (int x=0; x < cubedim; ++x) {
	//		//falling box
	//		//mesh associated with the motion state
	//		mesh_ptr mesh( new mesh_node(*mm::get("cube2")) );
	//		//add mesh to the scenegraph
	//		mesh->set_texture_id(box_texture);
	//		mesh->transformation_matrix = glm::translate(0.0f + static_cast<float>(2 * x), 
	//										30.0f + static_cast<float>(2 * y), 0.0f);
	//		root->add_child(mesh);
	//		//motion_state
	//		motion_state_ptr msn( new motion_state(mesh) );
	//		//add motion_state to the bullet manager
	//		btm::add_motion_state(msn);

	//		btRigidBody::btRigidBodyConstructionInfo
	//			boxRigidBodyCI(mass, msn.get(), boxShape, fallInertia);
	//		rigid_body_ptr body( new btRigidBody(boxRigidBodyCI) );
	//		btm::add_rigid_body(body);
	//	}
	//}

	//the klotz tower
	build_klotz_tower();

	//how to make it move?
	fallRigidBody->setLinearFactor(btVector3(1,1,1));
	//fallRigidBody->setLinearVelocity(btVector3(5.0f, 0.f, 0.f));
	fallRigidBody->setAngularVelocity(btVector3(1,0.5f,0));
}

//lets build a klotz tower
void bullet_demo::build_klotz_tower() 
{
	//build the shape
	const float kx = 2.f;
	const float ky = 0.4f;
	const float kz = 0.065f;
	klotzShape = shared_ptr<btBoxShape>(new btBoxShape(btVector3(kx, ky, kz) ) ); 
	btScalar mass = 0.5f;
	btVector3 klotzInertia(0,0,0);
	klotzShape->calculateLocalInertia(mass, klotzInertia);

	int x_dim = cfg::get_int("klotz_x");
	int z_dim = cfg::get_int("klotz_z");
	int y_dim = cfg::get_int("klotz_y");
	//rotate every even layer of klotze
	float rotate_even = 0.0f;
	float half_dist_x = 2.0f * kx * static_cast<float>(x_dim / 2);
	float half_dist_z =  static_cast<float>(z_dim / 2);
	for (int y = 0; y < y_dim; ++y) {
		for (int x = 0; x < x_dim; ++x) {
			for (int z = 0; z < z_dim; ++z) {
				//klotz mesh
				mesh_ptr klotz_mesh( new mesh_node(*mm::get("klotz1") ) );
				klotz_mesh->transformation_matrix =
					//glm::translate(
					//0.f + static_cast<float>(x) * 2 * rotate_even, 
					//0.4f + static_cast<float>(y) * ky * rotate_even,
					//0.0f + static_cast<float>(z) * rotate_even)
					//*
					glm::rotate(90.0f * rotate_even, 0.0f, 1.0f, 0.0f)
					*
					glm::translate(
					-half_dist_x + static_cast<float>(x) * 2.0f * kx,//* (1 - rotate_even), 
					ky + static_cast<float>(y) * 2.0f *  ky,// * (1 - rotate_even),
					-half_dist_z + static_cast<float>(z) )//* (1 - rotate_even) )
					;
				root->add_child(klotz_mesh);
				//motion state
				motion_state_ptr ms( new motion_state(klotz_mesh) );
				btm::add_motion_state(ms);

				//get the construction info 
				btRigidBody::btRigidBodyConstructionInfo cI(mass, ms.get(), klotzShape.get(), klotzInertia);
				rigid_body_ptr klotz_body( new btRigidBody(cI));
				btm::add_rigid_body(klotz_body);
			}
		}	
		rotate_even = 1.0f - rotate_even;
	}

}

void bullet_demo::boom()
{
	GLfloat speed = cfg::get("ball_speed");
	btScalar mass = 0.5;
	btVector3 fallInertia(0,0,0);
	//falling box
	vec3 pos = cam_eye - vec3(0.0f, 1.5f, 0.0f);
	motion_node_ptr ball_ms( new motion_state_node(glm::translate(pos)) );
	mm::get("ball_scale")->add_child(ball_ms);
	mesh_ptr new_ball( new mesh_node( mm::get("ball") ) );
	ball_ms->add_child(new_ball);
	glm::vec3 dir = glm::normalize((cam_center - pos)) * speed;
		//new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(-2,50,0)));

	ballShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo
		cI(mass, ball_ms.get(), ballShape.get(), fallInertia);
	rigid_body_ptr ball_body( new btRigidBody(cI));
	ball_body->setLinearVelocity(btVector3(dir.x, dir.y, dir.z) );
	btm::add_rigid_body(ball_body);
}

void bullet_demo::draw()
{
	switch (scene_select) {
	case 0:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0,0,width,height);
		draw_postprocess(shm::get("pp_echo"), txm::get("dr_vsm"));
		break;
	case 1:
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	load_identity(modelview_matrix);
	load_identity(view_matrix);

	look_at(view_matrix, cam_eye, cam_center, cam_up);
	//TODO: have proper normal matrix
	glm::mat4 normal_matrix = view_matrix * modelview_matrix;//glm::transpose(glm::inverse(view_matrix * modelview_matrix));

	/*------------------
		gbuffer creation
	--------------------*/
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,width, height);

	//draw skybox
	sb.draw(perspective_matrix, view_matrix);

	draw_gbuffer(root);

	
	/*--------------------
		shadowmap creation
	----------------------*/
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,shadow.width, shadow.height);
	//update shadow camera position
	shadow.view_matrix = glm::lookAt(light_0, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); 
	
	render_shadowmap(root, shadow);
	//render_shadowmap(mm::get("Tree3"), shadow); render_shadowmap(mm::get("Tree4"), shadow); render_shadowmap(mm::get("cube2"), shadow);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// generate mipmaps for shadowmap here
	glBindTexture(GL_TEXTURE_2D, shadow.texture);
	glGenerateMipmap(GL_TEXTURE_2D);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glViewport(0,0,width, height);

	// --------------
	// deferred stage
	// --------------

	//lighting
	glm::vec3 transformed_light_0 =  swizzle<X,Y,Z>(normal_matrix * glm::vec4(glm::normalize(light_0), 0.0f));
	shader_ptr sh = shader_manager::get("phong");
	sh->bind();
	sh->set_uniform("directional_light", transformed_light_0);
	sh->set_uniform("shadow_matrix",
		shadow.shadow_matrix() * glm::inverse(view_matrix));
	//sh->set_uniform("filter_width", sat_filter_width);
	draw_postprocess(sh, gbuffer_tex[ALBEDO], gbuffer_tex[NORMAL], gbuffer_tex[POSITION], shadow.texture);

	
	//glViewport(0, 0, width/5, height/5);
	//draw_postprocess(shader_manager::get("pp_echo"), shadow.texture, 0);
	
	glDisable(GL_FRAMEBUFFER_SRGB);
	break;
	}
}

void bullet_demo::process_keys(const float factor)
{
	//light 1 move
	light_0 += move_by_key(factor * 0.4f);

	//bullet!
	if (!physics_pause) {
		btm::world()->stepSimulation(factor * cfg::get("bullet_sim_speed"), 10);

		//btTransform trans;
		//fallRigidBody->getMotionState()->getWorldTransform(trans);
		//mm::get("sphere")->transformation_matrix = btm::to_glm(trans);//glm::translate( to_glm(trans.getOrigin()) );
	}
	
	//sim stepping
	if (glfwGetKey('R') && physics_pause) {

		btm::world()->stepSimulation(factor * cfg::get("bullet_sim_speed"), 10);

		btTransform trans;
		fallRigidBody->getMotionState()->getWorldTransform(trans);


		mm::get("sphere")->transformation_matrix = btm::to_glm(trans);//glm::translate( to_glm(trans.getOrigin()) );

	}

	if (glfwGetKey('R') && !pushed['R']) {
		physics_pause = !physics_pause;
		pushed['R'] = true;
	}
	if (glfwGetKey('R') == GLFW_RELEASE && pushed['R'])  pushed['R'] = false;

	if (glfwGetKey('E') && !pushed['E']) {
		boom();
		pushed['E'] = true;
	}
	if (glfwGetKey('E') == GLFW_RELEASE && pushed['E'])  pushed['E'] = false;
	
	//cam path
	update_camera_path(factor);

		//scene select 0
	if (glfwGetKey('3') && !pushed['3']) {
		scene_select = 0;
		pushed['3'] = true;
	}

	if (glfwGetKey('3') == GLFW_RELEASE) {
		pushed['3'] = false;
	}

	//scene select 1
	if (glfwGetKey('4') && !pushed['4']) {
		scene_select = 1;
		pushed['4'] = true;
	}

	if (glfwGetKey('4') == GLFW_RELEASE) {
		pushed['4'] = false;
	}


}
