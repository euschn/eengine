// wrapper class for skybox

#include "skybox.h"
#include "tga.h"
#include "shader_manager.h"
#include <glm/gtc/type_ptr.hpp> 

skybox::skybox()
{}

skybox::~skybox()
{
	glDeleteTextures(1, &skybox_texture);
	glDeleteVertexArrays(1, &skybox_vao);
	glDeleteBuffers(3, skybox_vbo);
}

void skybox::init(const std::string& path)
{
	//shader init
	shader_manager::load_single("../shader/skybox");

	//texture setup
	glGenTextures(1, &skybox_texture);
	get_errors();

	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	tga::Texture tex;

	string filepaths[] = {
		path + "_right.tga",
		path + "_left.tga",
		path + "_top.tga",
		path + "_bottom.tga",
		path + "_front.tga",
		path + "_back.tga",
	};

	//string filepaths[] = {
	//	"../textures/skybox/alpine_right.tga",
	//	"../textures/skybox/alpine_left.tga",
	//	"../textures/skybox/alpine_top.tga",
	//	"../textures/skybox/alpine_bottom.tga",
	//	"../textures/skybox/alpine_front.tga",
	//	"../textures/skybox/alpine_back.tga",
	//};

	for(int side = 0; side < 6; side++) {

		if (tga::LoadTGA(&tex, filepaths[side].c_str()))
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, GL_SRGB8_ALPHA8 , tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);

		
			if (tex.imageData)					// If Texture Image Exists
			{
				free(tex.imageData);				// Free The Texture Image Memory
			}
		}
		else {
			std::cout << "skybox: loading " << filepaths[side] << "failed\n";
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//vao/vbo setup
	// Points of a cube.
	GLfloat points[] = {  -2.0f, -2.0f, -2.0f, 1.0f,
						  -2.0f, -2.0f, +2.0f, 1.0f,
						  +2.0f, -2.0f, +2.0f, 1.0f,
						  +2.0f, -2.0f, -2.0f, 1.0f,
						  -2.0f, +2.0f, -2.0f, 1.0f,
						  -2.0f, +2.0f, +2.0f, 1.0f,
						  +2.0f, +2.0f, +2.0f, 1.0f,
						  +2.0f, +2.0f, -2.0f, 1.0f,
						  -2.0f, -2.0f, -2.0f, 1.0f,
						  -2.0f, +2.0f, -2.0f, 1.0f,
						  +2.0f, +2.0f, -2.0f, 1.0f,
						  +2.0f, -2.0f, -2.0f, 1.0f,
						  -2.0f, -2.0f, +2.0f, 1.0f,
						  -2.0f, +2.0f, +2.0f, 1.0f,
						  +2.0f, +2.0f, +2.0f, 1.0f,
						  +2.0f, -2.0f, +2.0f, 1.0f,
						  -2.0f, -2.0f, -2.0f, 1.0f,
						  -2.0f, -2.0f, +2.0f, 1.0f,
						  -2.0f, +2.0f, +2.0f, 1.0f,
						  -2.0f, +2.0f, -2.0f, 1.0f,
						  +2.0f, -2.0f, -2.0f, 1.0f,
						  +2.0f, -2.0f, +2.0f, 1.0f,
						  +2.0f, +2.0f, +2.0f, 1.0f,
						  +2.0f, +2.0f, -2.0f, 1.0f
						};

	// Normals of a cube.
	GLfloat normals[] = { +0.0f, -1.0f, +0.0f,
						  +0.0f, -1.0f, +0.0f,
						  +0.0f, -1.0f, +0.0f,
						  +0.0f, -1.0f, +0.0f,
						  +0.0f, +1.0f, +0.0f,
						  +0.0f, +1.0f, +0.0f,
						  +0.0f, +1.0f, +0.0f,
						  +0.0f, +1.0f, +0.0f,
						  +0.0f, +0.0f, -1.0f,
						  +0.0f, +0.0f, -1.0f,
						  +0.0f, +0.0f, -1.0f,
						  +0.0f, +0.0f, -1.0f,
						  +0.0f, +0.0f, +1.0f,
						  +0.0f, +0.0f, +1.0f,
						  +0.0f, +0.0f, +1.0f,
						  +0.0f, +0.0f, +1.0f,
						  -1.0f, +0.0f, +0.0f,
						  -1.0f, +0.0f, +0.0f,
						  -1.0f, +0.0f, +0.0f,
						  -1.0f, +0.0f, +0.0f,
						  +1.0f, +0.0f, +0.0f,
						  +1.0f, +0.0f, +0.0f,
						  +1.0f, +0.0f, +0.0f,
						  +1.0f, +0.0f, +0.0f
						};

	// The associated indices.
	GLuint indices[] = {		0, 2, 1,
								0, 3, 2, 
								4, 5, 6,
								4, 6, 7,
								8, 9, 10,
								8, 10, 11, 
								12, 15, 14,
								12, 14, 13, 
								16, 17, 18,
								16, 18, 19, 
								20, 23, 22,
								20, 22, 21
						};

	glGenVertexArrays(1, &skybox_vao);
	glBindVertexArray(skybox_vao);

	// Allocate and assign two Vertex Buffer Objects (VBOs) to our handle
	glGenBuffers(3, skybox_vbo);

	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)	
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	get_errors();

	shader_ptr texture_cm = shm::get("skybox");

	texture_cm->set_attrib_location("in_position", 4, GL_FLOAT);


	//vbo for texcoords
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	std::cout << "skybox: init successful\n";
}

void skybox::draw(const glm::mat4& perspective_matrix, const  glm::mat4& modelview_matrix)
{
	
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	shader_ptr texture_cm = shm::get("skybox");
	texture_cm->bind();

	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
		
	GLint tex_location = texture_cm->get_uniform_location("in_texture");
	glUniform1i(tex_location, 0);
		
	texture_cm->set_uniform("perspective_matrix", perspective_matrix);
	texture_cm->set_uniform("modelview_matrix", modelview_matrix);
		
	glBindVertexArray(skybox_vao);
	// Draw triangle in VAO
	glDrawElements(GL_TRIANGLES, 6*2*3, GL_UNSIGNED_INT, 0);
		
	get_errors();
	glBindVertexArray(0);
		
	get_errors();
	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	texture_cm->unbind();
	
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
}