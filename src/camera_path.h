// class camera_path
// two splines for camera position and view direction
// plus some utility functions

#pragma once

#include "cr_spline.h"

class camera_path {
public:
	camera_path();
	virtual ~camera_path();

	void append(const glm::vec3 e, const glm::vec3 c);
	void clear();
	void go_to_start();
	void save(const std::string& path = "../camera_path.txt");
	bool load(const std::string& path = "../camera_path.txt");
	void advance(const float s);
	glm::vec3 get_eye();
	glm::vec3 get_cam();
	void set_move_speed(const float speed) { move_speed = speed; }
	void increase_move_speed(const float speed) { move_speed += speed; }

protected:
	cr_spline eye, cam;
	float move_speed;
};