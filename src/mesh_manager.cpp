// class mesh_manager
// singleton


#include "mesh_manager.h"
#include "texture_manager.h"
#include "config_manager.h"
#include "tga.h"
#include <openctm.h>

//assimp
#include <assimp.hpp>      // C++ importer interface
#include <aiPostProcess.h> // Post processing flags


using namespace boost::filesystem;
using namespace std;
using namespace std::tr1;

mesh_manager* mesh_manager::inst()
{
	static mesh_manager the_instance;
	return &the_instance;
}

mesh_ptr mesh_manager::value(const std::string& key)
{
	auto it = _map.find(key);
	if (it == _map.end()) {
		std::cerr << "mesh_manager: error: cannot find mesh " << key << std::endl;
		_map[key] = 0;
		return 0;
	}

	return it->second;
}

void mesh_manager::load_single_ctm_mesh(const std::string& path, shader_ptr shader, const glm::mat4& matrix, const GLuint tex_id, const std::string& key)
{
	try
	{
	// Create a new OpenCTM importer object
	CTMimporter ctm;
	// Load the OpenCTM file
	ctm.Load(path.c_str());
	// Access the mesh data
	CTMuint vertCount = ctm.GetInteger(CTM_VERTEX_COUNT);
	const CTMfloat * vertices = ctm.GetFloatArray(CTM_VERTICES);
	const CTMfloat * normals = ctm.GetFloatArray(CTM_NORMALS);
	CTMuint triCount = ctm.GetInteger(CTM_TRIANGLE_COUNT);
	const CTMuint * indices = ctm.GetIntegerArray(CTM_INDICES);

	// Deal with the mesh (e.g. transcode it to our
	// internal representation)

	//-------------------------
	//build vaos and vbos here
	//mesh_ptr new_mesh = build_mesh_node(shader, matrix, tex_id, scene->mMeshes[0]);
	//--------------------------------

	//generate the vao
	GLuint assimp_vao_id;
	glGenVertexArrays(1, &assimp_vao_id);

	//bind the vao
	glBindVertexArray(assimp_vao_id);

	//gererate the vbos
	std::vector<GLuint> mesh_vbos;
	GLuint position_vbo;
	glGenBuffers(1, &position_vbo);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glBufferData(GL_ARRAY_BUFFER, 
		3 * vertCount * sizeof(GLfloat),
		vertices, GL_STATIC_DRAW);
	shader->set_attrib_location("in_position", 3);
	mesh_vbos.push_back(position_vbo);

	//if (ctm.GetInteger(CTM_HAS_NORMALS) == CTM_TRUE) {
	GLuint normal_vbo;
	glGenBuffers(1, &normal_vbo);
	//normals
	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertCount * sizeof(GLfloat),
		normals, GL_STATIC_DRAW);
	shader->set_attrib_location("in_normal", 3);
	mesh_vbos.push_back(normal_vbo);

	//}
		
	//uv coords
	if (ctm.GetInteger(CTM_UV_MAP_COUNT) > 0) {
		//just get first uv coords for now
		const CTMfloat * uvcoords = ctm.GetFloatArray(CTM_UV_MAP_1);
		GLuint texcoord_vbo;
		glGenBuffers(1, &texcoord_vbo);
		//texture
		glBindBuffer(GL_ARRAY_BUFFER, texcoord_vbo);
		glBufferData(GL_ARRAY_BUFFER, 2 * vertCount * sizeof(GLfloat),
			uvcoords, GL_STATIC_DRAW);
		shader->set_attrib_location("in_texcoord", 2);
		mesh_vbos.push_back(texcoord_vbo);

	}

	//dont think ctm supports tangents?
	//if (aimesh->HasTangentsAndBitangents() && config_manager::get_bool("load_tangents")) {
	//	GLuint tangent_vbo;
	//	glGenBuffers(1, &tangent_vbo);
	//	//tangents
	//	glBindBuffer(GL_ARRAY_BUFFER, tangent_vbo);
	//	glBufferData(GL_ARRAY_BUFFER, 3 * aimesh->mNumVertices * sizeof(GLfloat),
	//		aimesh->mTangents, GL_STATIC_DRAW);
	//	shader->set_attrib_location("in_tangent", 3);
	//	mesh_vbos.push_back(tangent_vbo);
	//}

	get_errors();

	GLuint index_vbo;
	glGenBuffers(1, &index_vbo);
	//std::vector<GLuint> index_vector;
	//int num_indices = 0;
	////get all the indices
	//for (unsigned int idx=0; idx < triCount * 3; idx++) {
	//	//for (unsigned int idx=0; idx < aimesh->mFaces[face].mNumIndices; idx++) {
	//		index_vector.push_back( indices[idx] );
	//		num_indices++;
	//	//}
	//}

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triCount * sizeof(GLuint),
		indices, GL_STATIC_DRAW);
	mesh_vbos.push_back(index_vbo);
	
	// ------------------------
	//finally, do the map entry
	// ------------------------
	mesh_ptr new_mesh(new mesh_node(matrix, assimp_vao_id, triCount * 3) );
	new_mesh->vao_id = assimp_vao_id;
	new_mesh->vbos = mesh_vbos;
	new_mesh->set_texture_id(tex_id);

	// build mesh end
	//---------------
	// get the key for the mesh
	string mesh_key = key;
	if (key.empty()) {
			//take stem as key
			boost::filesystem::path p(path);
			mesh_key = p.stem().generic_string();
	}
	// do we have only one mesh
	// add the mesh node with the filename as the key
	_map[mesh_key] = new_mesh;
	//unbind vao and vbos
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if(!(glGetError() == GL_NO_ERROR))
	{
		cout << "mesh_manager: OGL error building node for CTM mesh " << 0 << " of " << path << endl;
	}



	}
	catch(exception &e)
	{
		cout << "Error: mesh manager CTM: " << e.what() << endl;
	}
}

void mesh_manager::load_single_mesh(const std::string& path, shader_ptr shader, const glm::mat4& matrix, const GLuint tex_id, const std::string& key)
{
	//TODO check for ctm
	size_t path_length = path.length();
	const string filetype = path.substr(path_length - 3, 3);
	if (filetype.compare("ctm") == 0) {
		//we have a ctm file
		std::cout << path << " has type " << filetype << std::endl;
		load_single_ctm_mesh(path, shader, matrix, tex_id, key);
		return;
	}

	//TODO load here
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile( path, 
		aiProcess_CalcTangentSpace       |
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType			 |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_OptimizeMeshes			|
		aiProcess_OptimizeGraph
		);
  
	// If the import failed, report it
	if( !scene)
	{
		std::cout << "mesh_manager: error: " <<  importer.GetErrorString() << "(assimp error message)\n";
		throw std::exception();
	}

	// Now we can access the file's contents. 
	// TODO access the file here
	std::cout << "mesh_manager: building mesh from " << path << std::endl;
	
	if (scene->HasMeshes()) {
		//we have meshes, load them
		//TODO: for now we assume only one mesh per file

		//TODO: do we have to bind shader?
		//shader->bind();
		std::cout << "mesh_manager:: number of meshes in " << path << " " << scene->mNumMeshes << std::endl;

		//build vaos and vbos
		mesh_ptr new_mesh = build_mesh_node(shader, matrix, tex_id, scene->mMeshes[0]);
		// get the key for the mesh
		string mesh_key = key;
		if (key.empty()) {
				//take stem as key
				boost::filesystem::path p(path);
				mesh_key = p.stem().generic_string();
		}
		// do we have only one mesh
		// add the mesh node with the filename as the key
		if (scene->mNumMeshes == 1) {
			_map[mesh_key] = new_mesh;
			//unbind vao and vbos
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			if(!(glGetError() == GL_NO_ERROR))
			{
				cout << "mesh_manager: OGL error building node for mesh " << 0 << " of " << path << endl;
			}

		}
		//we have more than one mesh
		else {
			// build a dummy node that is the root for all mesh nodes
			//mesh_ptr root_node(new mesh_node(matrix, 0, 0) );
			mesh_ptr dummy_mesh( new mesh_node(matrix, 0, 0, SH_NONE) );
			//add the first loaded mesh
			new_mesh->transformation_matrix = glm::mat4(1.0f);

			//get material for loaded mesh
			if (scene->HasMaterials()) {
					GLuint mesh_texture_id = tex_id;
					unsigned int mat_index = scene->mMeshes[0]->mMaterialIndex;
					aiMaterial* mat = scene->mMaterials[mat_index];
					aiString aipath;
					mat->GetTexture(aiTextureType_DIFFUSE, 0, &aipath);
					std::string texture_path(aipath.data);
					//std::cout << "multimesh: texture path is " << texture_path << std::endl;//take stem as key
					boost::filesystem::path relative(path);
					relative = relative.parent_path();
					string full = relative.generic_string() + "/" + texture_path;
					//std::cout << "full tex path: " << full << std::endl;
					boost::filesystem::path p(texture_path);
					std::string tex_key = p.stem().generic_string();
					//do we already have this texture?
					GLuint tex = txm::get(tex_key);
					if (tex != 0) {
						tex = txm::get(full);
					}
					if (tex != 0) {
						//we do not need to load this texture
						mesh_texture_id = tex;
					}
					else {
						//load the texture
						txm::load_single(full);
						mesh_texture_id = txm::get(full);
					}
				new_mesh->set_texture_id(mesh_texture_id);
			}

			// add first mesh to dummy root
			dummy_mesh->add_child(new_mesh);
			//add all other meshes
			unsigned int max_meshes = std::min<int>(scene->mNumMeshes, config_manager::get_int("max_meshes"));
			for (unsigned int i = 1; i < max_meshes; ++i) {
				//TODO: find material properties, load textures if needed
				GLuint mesh_texture_id = tex_id;

				if (scene->HasMaterials()) {
					unsigned int mat_index = scene->mMeshes[i]->mMaterialIndex;
					aiMaterial* mat = scene->mMaterials[mat_index];
					aiString aipath;
					mat->GetTexture(aiTextureType_DIFFUSE, 0, &aipath);
					std::string texture_path(aipath.data);
					boost::filesystem::path p(texture_path);
					std::string tex_key = p.stem().generic_string();
					GLuint tex = txm::get(tex_key);
					if (tex > 0) {
						mesh_texture_id = tex;
					}
					//else {
					//	//load the texture
					//	boost::filesystem::path relative(path);
					//	relative = relative.parent_path();
					//	string s = relative.generic_string() + "/" + texture_path;
					//	txm::load_single(s);
					//	mesh_texture_id = txm::get(s);
					//}
				}

				//load mesh i
				mesh_ptr next_mesh = build_mesh_node(shader, glm::mat4(1.0f), mesh_texture_id, scene->mMeshes[i]);
				//add it as a child of the dummy node
				dummy_mesh->add_child(next_mesh);
				//TODO add mesh to map with a new key
			}

			//finally, add the root dummy mesh to the list
			_map[mesh_key] = dummy_mesh;
			//unbind vao and vbos
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			if(!(glGetError() == GL_NO_ERROR))
			{
				cout << "mesh_manager: OGL error building node for mesh " << 0 << " of " << path << endl;
			}
			// add the 1st, already build mesh


		}
	}
	else {
		std::cout << "mesh_manager: error: cannot find meshes in file " << path << std::endl;
		throw std::exception();
	}
}

mesh_ptr mesh_manager::build_mesh_node(shader_ptr shader, const glm::mat4& matrix, const GLuint tex_id, aiMesh * aimesh)
{
		//generate the vao
		GLuint assimp_vao_id;
		glGenVertexArrays(1, &assimp_vao_id);

		//bind the vao
		glBindVertexArray(assimp_vao_id);

		//gererate the vbos
		std::vector<GLuint> mesh_vbos;
		GLuint position_vbo;
		glGenBuffers(1, &position_vbo);

		//vertices
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		glBufferData(GL_ARRAY_BUFFER, 
			3 * aimesh->mNumVertices * sizeof(GLfloat),
			aimesh->mVertices, GL_STATIC_DRAW);
		shader->set_attrib_location("in_position", 3);
		mesh_vbos.push_back(position_vbo);

		if (aimesh->HasNormals()) {
			GLuint normal_vbo;
			glGenBuffers(1, &normal_vbo);
			//normals
			glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
			glBufferData(GL_ARRAY_BUFFER, 3 * aimesh->mNumVertices* sizeof(GLfloat),
				aimesh->mNormals, GL_STATIC_DRAW);
			shader->set_attrib_location("in_normal", 3);
			mesh_vbos.push_back(normal_vbo);

		}
		
		if (aimesh->HasTextureCoords(0)) {
			GLuint texcoord_vbo;
			glGenBuffers(1, &texcoord_vbo);
			//texture
			glBindBuffer(GL_ARRAY_BUFFER, texcoord_vbo);
			glBufferData(GL_ARRAY_BUFFER, 3 * aimesh->mNumVertices * sizeof(GLfloat),
				aimesh->mTextureCoords[0], GL_STATIC_DRAW);
			shader->set_attrib_location("in_texcoord", 3);
			mesh_vbos.push_back(texcoord_vbo);

		}

		if (aimesh->HasTangentsAndBitangents() && config_manager::get_bool("load_tangents")) {
			GLuint tangent_vbo;
			glGenBuffers(1, &tangent_vbo);
			//tangents
			glBindBuffer(GL_ARRAY_BUFFER, tangent_vbo);
			glBufferData(GL_ARRAY_BUFFER, 3 * aimesh->mNumVertices * sizeof(GLfloat),
				aimesh->mTangents, GL_STATIC_DRAW);
			shader->set_attrib_location("in_tangent", 3);
			mesh_vbos.push_back(tangent_vbo);
		}

		get_errors();

		GLuint index_vbo;
		glGenBuffers(1, &index_vbo);
		std::vector<GLuint> index_vector;
		int num_indices = 0;
		//get all the indices
		for (unsigned int face=0; face < aimesh->mNumFaces; face++) {
			for (unsigned int idx=0; idx < aimesh->mFaces[face].mNumIndices; idx++) {
				index_vector.push_back( aimesh->mFaces[face].mIndices[idx]);
				num_indices++;
			}
		}

		//indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint),
			&index_vector[0], GL_STATIC_DRAW);
		mesh_vbos.push_back(index_vbo);
	
		// ------------------------
		//finally, do the map entry
		// ------------------------
		mesh_ptr new_mesh(new mesh_node(matrix, assimp_vao_id, num_indices) );
		new_mesh->vao_id = assimp_vao_id;
		new_mesh->vbos = mesh_vbos;
		new_mesh->set_texture_id(tex_id);

		return new_mesh;
}

void mesh_manager::clear()
{
}

void mesh_manager::load_from_directory( const path & directory, bool recurse_into_subdirs )
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
			//TODO load here
		}
	}
	}
	else {
		cout << "directory " << directory << " does not exist\n";
		throw std::exception();
	}
}


//TODO implement
void mesh_manager::clone(const std::string& key, const std::string& new_key)
{}

//texture and matrix setter/getter
GLuint mesh_manager::get_texture(const std::string& key)
{
	auto it = inst()->_map.find(key);
	if (it == inst()->_map.end()) {
		std::cerr << "mesh_manager: error: cannot find mesh " << key << std::endl;
		return 0;
	}

	return it->second->get_texture_id();
}

void mesh_manager::set_texture(const std::string& key, const GLuint tex_id)
{
	auto it = inst()->_map.find(key);
	if (it == inst()->_map.end()) {
		std::cerr << "mesh_manager: error: cannot find mesh " << key << std::endl;
		return;
	}

	it->second->set_texture_id(tex_id);
}

glm::mat4 mesh_manager::get_matrix(const std::string& key)
{
	auto it = inst()->_map.find(key);
	if (it == inst()->_map.end()) {
		std::cerr << "mesh_manager: error: cannot find mesh " << key << std::endl;
		return glm::mat4(1.0f);
	}

	return it->second->transformation_matrix;
}

void mesh_manager::set_matrix(const std::string& key, const glm::mat4& matrix)
{
	auto it = inst()->_map.find(key);
	if (it == inst()->_map.end()) {
		std::cerr << "mesh_manager: error: cannot find mesh " << key << std::endl;
		return;
	}

	it->second->transformation_matrix = matrix;
}

int mesh_manager::get_id(const std::string& key)
{
	auto it = inst()->_map.find(key);
	if (it == inst()->_map.end()) {
		std::cerr << "mesh_manager: error: cannot find mesh " << key << std::endl;
		return 0;
	}

	return it->second->shader_type();
}

void mesh_manager::set_id(const std::string& key, const int shader_id)
{
	auto it = inst()->_map.find(key);
	if (it == inst()->_map.end()) {
		std::cerr << "mesh_manager: error: cannot find mesh " << key << std::endl;
		return;
	}

	it->second->set_shader_type(shader_id);
}