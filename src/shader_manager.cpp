// class shader_manager
// singleton

#include "shader_manager.h"
#include <exception>

using namespace boost::filesystem;

shader_manager* shader_manager::inst()
{
	static shader_manager the_instance;
	return &the_instance;
}

shader_ptr shader_manager::value(const std::string& key)
{
	auto it = _map.find(key);
	if (it == _map.end()) {
		std::cerr << "shader_manager: error: cannot find shader " << key << std::endl;
		_map[key] = 0;
		throw std::exception();
	}


	return it->second;
}

void shader_manager::load_from_directory( const boost::filesystem::path& directory, bool recurse_into_subdirs )
{}

void shader_manager::load_single_shader(const std::string& shader_path, const std::string& key)
{
	//get key
	string final_key = key;
	path p(shader_path);
	if (key.empty()) {
		final_key = p.stem().generic_string();
	}
	//do we have a tess shader?
	path tess(shader_path + ".tess");
	path geom(shader_path + ".geom");
	if ( exists(tess) ) {
		//tesselation shader
		shader_ptr sh = shader_ptr( new tesselation_shader (shader_path) );
		if (!(*sh)) {
			cerr << "Could not compile tesselation shader program " << shader_path << endl;
			throw std::exception();
		}
		else {
			_map[final_key] = sh;
		}
	}
	else if ( exists(geom) ) {
		//geometry shader
		shader_ptr sh = shader_ptr( new geometry_shader (shader_path) );
		if (!(*sh)) {
			cerr << "Could not compile geometry shader program " << shader_path << endl;
			throw std::exception();
		}
		else {
			_map[final_key] = sh;
		}
	}
	else {
		//just a regular shader
		shader_ptr sh = shader_ptr( new Shader (shader_path) );
		if (!(*sh)) {
			cerr << "Could not compile shader program " << shader_path << endl;
			throw std::exception();
		}
		else {
			_map[final_key] = sh;
		}
	}
}

void shader_manager::clear()
{}