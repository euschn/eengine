#pragma once

#include "common.hpp"
#include <unordered_map>
#include <memory>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include "tesselation_shader.hpp"

using namespace std::tr1;

class shader_manager
{
public:
	//access the instance
	static shader_manager* inst();
	static shader_ptr	get(const std::string& key) { return inst()->value(key); }
	static void			bind(const std::string& key) { inst()->value(key)->bind(); }
	static void			unbind() { glUseProgram(0); }
	static void load_single(const std::string& path) { inst()->load_single_shader(path); }
	void load_from_directory( const boost::filesystem::path& directory, bool recurse_into_subdirs = false );

	shader_ptr value(const std::string& key);

private:
	shader_manager() {}
	virtual ~shader_manager() { clear(); }
	//functions
	void load_single_shader(const std::string& shader_path, const std::string& key = "");
	void clear();

	//members
	unordered_map<string, shader_ptr> _map;

};

//shortcut
typedef shader_manager shm;