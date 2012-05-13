// class texture_manager
// singleton


#include "texture_manager.h"
#include "tga.h"
#include "boost\filesystem.hpp"

using namespace boost::filesystem;
using namespace std;
using namespace std::tr1;

texture_manager* texture_manager::inst()
{
	static texture_manager the_instance;
	return &the_instance;
}

GLuint texture_manager::value(const std::string& key)
{
	auto it = _map.find(key);
	if (it == _map.end()) {
		std::cerr << "texture_manager: error: cannot find texture " << key << std::endl;
		_map[key] = 0;
		return 0;
	}

	return it->second;
}

void texture_manager::load_single_texture(const std::string& path, const std::string& key)
{
	GLuint sample_texture;

	//texture
	if (!load_tga_texture(&sample_texture, path, GL_SRGB8_ALPHA8, mipmap))
	{
		std::cout << "failed to load texture " << path << std::endl;
		throw std::exception();
	}

	if (key.empty()) {
		_map[path] = sample_texture;
	}
	else {
		_map[key] = sample_texture;
	}
}

void load_single_cubemap(const std::string& filepath, const std::string& key = "")
{

}

void texture_manager::clear()
{
	for (auto it = _map.begin(); it != _map.end(); ++it)
	{
		glDeleteTextures(1, &(it->second));
	}
}

void texture_manager::load_from_directory( const path & directory, bool recurse_into_subdirs )
{
	if( exists( directory ) )
	{
	directory_iterator end ;
	for( directory_iterator iter(directory) ; iter != end ; ++iter )
	{
		if ( is_directory( *iter ) )
		{
			if( recurse_into_subdirs ) load_from_directory(*iter) ;
		}
		else {
			//we have a file!
			const std::string ext = iter->path().extension().generic_string();
			if ( ext == ".tga" || ext == ".TGA") {
				string stem = iter->path().stem().generic_string();
				load_single_texture(iter->path().generic_string(), stem);
				std::cout << "texture_manager: loaded texture " << stem << std::endl; 
			}
		}
	}
	}
	else {
		cout << "directory " << directory << " does not exist\n";
		throw std::exception();
	}
}

bool texture_manager::load_tga_texture(GLuint * texture_handle, const std::string& filepath, GLint color_format, bool mipmap, GLint texture_format)
{
	bool status = false;
	tga::Texture tex;

	if (tga::LoadTGA(&tex, filepath.c_str()))
	{
		status = true;

		glGenTextures(1, texture_handle);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(texture_format, *texture_handle);

		if(mipmap)
		{
			glTexImage2D(texture_format, 0, color_format, tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);
			if (anisotropic) {
				GLfloat max_anisotropic;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropic);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropic);
			}
			glTexParameterf(texture_format, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtering
			glTexParameterf(texture_format, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
			glTexParameterf(texture_format, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameterf(texture_format, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);	//generate the mipmaps
		}
		else
		{
		// Generate The Texture
			glTexImage2D(texture_format, 0, color_format, tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);
			glTexParameteri(texture_format, GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
			glTexParameteri(texture_format, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
			glTexParameteri(texture_format, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameteri(texture_format, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	}
	else {
		//reading the file failed
		return false;
	}

	if (tex.imageData)					// If Texture Image Exists
	{
		free(tex.imageData);				// Free The Texture Image Memory
	}

	return status;
}