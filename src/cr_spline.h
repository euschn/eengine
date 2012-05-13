// simple catmull-rom spline for vec3

#pragma once

#include "common.hpp"
#include "glm/glm.hpp"

class cr_spline {
public:
	cr_spline();
	cr_spline( const int start, const int end);
	virtual ~cr_spline() {}

	//functions
	void set_position(const float position);
	void advance(const float delta_position); //advance current position
	glm::vec3 interpolate(const float s);
	glm::vec3 interpolate() { return interpolate(current_position); }
	void go_to_start(); // go to start of the spline
	void clear();

	//members
	int start, end;
	std::vector<glm::vec3> points;
protected:
	float current_position; // position between two points [0,1]
};