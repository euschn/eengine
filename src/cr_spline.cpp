// simple catmull-rom spline for vec3

#include "cr_spline.h"


cr_spline::cr_spline() 
{
	points = std::vector<glm::vec3>();
	this->current_position = 0.0f;
	this->start = 0;
	this->end = 0;
}

cr_spline::cr_spline(const int start, const int end) 
{
	this->current_position = 0.0f;
	this->start = start;
	this->end = start;
}

//set current position
void cr_spline::set_position(const float position) 
{
	current_position = std::max<float>(0.0f, std::min<float>(1.0f, current_position));
}

 //advance current position
void cr_spline::advance(const float delta_position) 
{
	//exit conditions
	if (start == (points.size() - 1)  && delta_position > 0 //at the end and move forward
		||
		end == 0 && delta_position < 0 //at the start and move backward
		||
		points.size() == 0
		) {
		return;
	}

	current_position = current_position + delta_position;

	if (current_position > 1) {
		current_position -= 1.0f;
		//update start/end
		start = std::min<int>(points.size() - 1, start + 1);
		end = std::min<int>(points.size() - 1, start + 1);

	}
	else if (current_position < 0) {
		current_position += 1.0f;
		end = start;
		start = std::max<int>(0, start - 1);
	}
}

//interpolation
// based on www.cubic.org/docs/hermite.htm
// s must be between 0 and 1
glm::vec3 cr_spline::interpolate(const float s) 
{
	glm::vec3 result(0.0f, 0.0f, 1.0f);

	//just prevent a crash
	//TODO improve this
	if (points.size() == 0) return result;

	//simple tangent
	//actual tangent calculation for CR spline
	// T_i = 0.5 * ( P_(i+1) - P_(i-1) )
	//get indicices
	int pre_start = std::max<int>(0, start - 1);
	int post_end = std::min<int>(points.size() - 1, end + 1);
	glm::vec3 tangent_start = 0.5f * (points[end] - points[pre_start]);
	glm::vec3 tangent_end = 0.5f * (points[post_end] - points[start]);

	const float s_cube = s*s*s;
	const float s_squared = s*s;
	//interpolation code
	float h1 = 2 * s_cube - 3 * s_squared + 1;
	float h2 = -2 * s_cube + 3 * s_squared;
	float h3 = s_cube - 2 * s_squared + s;
	float h4 = s_cube - s_squared;
	result = h1*points[start] +
			 h2*points[end]   +
			 h3*tangent_start +
			 h4*tangent_end;

	return result;
}

void cr_spline::go_to_start()
{
	current_position = 0.0f;
	start = 0;
	end = std::min<int>(points.size() - 1, 1);
}

void cr_spline::clear()
{
	points.clear();
	current_position = 0.0f;
	start = 0;
	end = 1;
}