// mesh manager
// singleton

#pragma once

#include "common.hpp"
#include "mesh_node.h"
#include "shader.hpp"
#include <aiScene.h>       // Output data structure

#include <unordered_map>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std::tr1;

class mesh_manager
{
public:
	//access the instance
	static mesh_manager* inst();
	static mesh_ptr			get(const std::string& key) { return inst()->value(key); }
	// load a single mesh, init with shader and matrix/texture id
	static void load_single(const std::string& path, shader_ptr shader, 
		const glm::mat4& matrix = glm::mat4(1.0f),
		const GLuint tex_id = 0,
		const std::string& key = "") { inst()->load_single_mesh(path, shader, matrix, tex_id, key); }
	static void load_single(const std::string& path, shader_ptr shader, 
		const std::string& key,
		const glm::mat4& matrix = glm::mat4(1.0f),
		const GLuint tex_id = 0) { inst()->load_single_mesh(path, shader, matrix, tex_id, key); }
	static void load_single(const std::string& path, shader_ptr shader, 
		const GLuint tex_id,
		const glm::mat4& matrix = glm::mat4(1.0f),
		const std::string& key = "") { inst()->load_single_mesh(path, shader, matrix, tex_id, key); }
	//clone: add a new mesh with an existing vao
	static void clone(const std::string& key, const std::string& new_key);
	static void add(mesh_ptr mesh, const std::string& key) { inst()->add_mesh(mesh, key); }


	void load_from_directory( const boost::filesystem::path& directory, bool recurse_into_subdirs = false );
	static void render(const std::string& key) { return inst()->value(key)->render(); }
	static void pre_render(const std::string& key) { return inst()->value(key)->pre_render(); }

	//texture and matrix setter/getter
	static GLuint get_texture(const std::string& key);
	static void set_texture(const std::string& key, const GLuint tex_id);
	static glm::mat4 get_matrix(const std::string& key);
	static void set_matrix(const std::string& key, const glm::mat4& matrix);
	static int get_id(const std::string& key);
	static void set_id(const std::string& key, const int shader_id);

	mesh_ptr value(const std::string& key);

private:
	mesh_manager() {}
	virtual ~mesh_manager() { clear(); }
	//functions
	void load_single_mesh(const std::string& path, shader_ptr shader, const glm::mat4& matrix, const GLuint tex_id, const std::string& key = "");
	void load_single_ctm_mesh(const std::string& path, shader_ptr shader, const glm::mat4& matrix, const GLuint tex_id, const std::string& key = "");
	mesh_ptr build_mesh_node(shader_ptr shader, const glm::mat4& matrix, const GLuint tex_id, aiMesh * aimesh);
	void add_mesh(mesh_ptr mesh, const std::string& key) { _map[key] = mesh; }
	void clear();

	//members
	unordered_map<string, mesh_ptr> _map;

};

typedef mesh_manager mm;