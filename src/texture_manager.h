// texture manager
// singleton

#pragma once

#include "common.hpp"
#include <unordered_map>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std::tr1;

class texture_manager
{
public:
	//access the instance
	static texture_manager* inst();
	static GLuint			get(const std::string& key) { return inst()->value(key); }
	static void load_single(const std::string& path) { inst()->load_single_texture(path); }
	static void load_cubemap(const std::string& path) { inst()->load_single_cubemap(path); }
	static void load(const boost::filesystem::path& directory, bool recurse_into_subdirs = false)  { inst()->load_from_directory(directory, recurse_into_subdirs); }
	static void bind(const std::string& key, GLenum target = GL_TEXTURE_2D) { glBindTexture(GL_TEXTURE_2D, get(key)); }

	GLuint value(const std::string& key);

	bool mipmap; //turn on mipmapping
	bool anisotropic; //turn on anisotropic filtering

private:
	texture_manager() { mipmap = true; anisotropic = true; }
	virtual ~texture_manager() { clear(); }
	//functions
	void load_single_texture(const std::string& path, const std::string& key = "");
	void load_single_cubemap(const std::string& path, const std::string& key = "");
	bool load_tga_texture(GLuint * texture_handle, const std::string& filepath, GLint color_format = GL_RGBA, bool mipmap = false, GLint texture_format = GL_TEXTURE_2D);
	void load_from_directory( const boost::filesystem::path& directory, bool recurse_into_subdirs = false );
	void clear();

	//members
	unordered_map<string, GLuint> _map;

};

//shortcut
typedef texture_manager txm;