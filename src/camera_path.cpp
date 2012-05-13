// class camera_path
// two splines for camera position and view direction
// plus some utility functions

#include "camera_path.h"
#include <fstream>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

camera_path::camera_path() 
{
	move_speed = 0.1f;
}
camera_path::~camera_path() {}

void camera_path::append(const glm::vec3 e, const glm::vec3 c) 
{
	eye.points.push_back(e);
	cam.points.push_back(c);
}

void camera_path::clear() 
{
	eye.clear();
	cam.clear();
}

void camera_path::go_to_start() 
{
	eye.go_to_start();
	cam.go_to_start();
}

void camera_path::save(const std::string& path) 
{
	std::ofstream outfile;
	unsigned int length = eye.points.size();
	//there should be as many eye as cam points
	assert(length == cam.points.size());
	outfile.open(path);
	for (unsigned int i=0; i<length; ++i) {
		outfile 
			<< "// " << i << "\n"
			<< "eye " << eye.points[i].x << " " << eye.points[i].y << " " << eye.points[i].z 
			<< " center " << cam.points[i].x << " " << cam.points[i].y << " " << cam.points[i].z
			<< "\n\n"; 
	}
	outfile.close();
}

bool camera_path::load(const std::string& path) 
{
	using qi::phrase_parse;
	using qi::_1;
	using ascii::space;
	using ascii::alpha;
	using ascii::alnum;

	std::ifstream fp_in;
	fp_in.open(path, std::ios::in);
	if (!fp_in.is_open()) {
		std::cout << "error opening file " << path << std::endl;
		return false;
	}

	std::string str;
	bool result = false;
	while ( getline(fp_in, str) )
	{
		if (str.empty())
			continue;
		// ignore commented line
		if (str[0] == '/' || str[1] == '/')
			continue;

		//eye and center point parsing
		// ---------------------------
		//std::vector<parse_net::raw_agent> raws;
		//parse_net::raw_agent_parser p;
		//result = phrase_parse(str.begin(), str.end(), 
		//	(
		//		(p >> '=' >> p) % ','
		//	)
		//	, space, raws);
		//if (result) {
		//	raws_vector.push_back(raws);
		//	std::cout << "----------------------------\nparsed string: " << str << std::endl;
		//}
	}

	return result;
}

void camera_path::advance(const float s) 
{
	const float f = s * move_speed;
	eye.advance(f);
	cam.advance(f);
}

glm::vec3 camera_path::get_eye() 
{
	return eye.interpolate();
}

glm::vec3 camera_path::get_cam() 
{
	return cam.interpolate();
}