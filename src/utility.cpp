// by RTR team, www.cg.tuwien.ac.at
// 2010
// modified by Eugen Jiresch

#include "common.hpp"

#include <fstream>
#include <vector>

bool file_exists(const string &filename)
{
	std::ifstream ifile(filename.c_str());

	return ifile;
}

string read_file(const string &filename)
{
	std::ifstream ifile(filename.c_str());

	return string(std::istreambuf_iterator<char>(ifile),
		std::istreambuf_iterator<char>());
}

void get_errors(void)
{
	GLenum error = glGetError();

	if (error != GL_NO_ERROR) {
		switch (error) {
		case GL_INVALID_ENUM:
			cerr << "GL: enum argument out of range." << endl;
			break;
		case GL_INVALID_VALUE:
			cerr << "GL: Numeric argument out of range." << endl;
			break;
		case GL_INVALID_OPERATION:
			cerr << "GL: Operation illegal in current state." << endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cerr << "GL: Framebuffer object is not complete." << endl;
			break;
		case GL_OUT_OF_MEMORY:
			cerr << "GL: Not enough memory left to execute command." << endl;
			break;
		default:
			cerr << "GL: Unknown error." << endl;
		}
	}
}

//read vertices, normals and vertex indices from CG1 atoff files
bool read_atoff_object(string& filename, std::vector<float>& vertices_out, std::vector<float>& normals_out, std::vector<unsigned int>& indices_out)
{
	std::ifstream fp_in;
	fp_in.open(filename.c_str(), std::ios::in);

	string header;
	fp_in >> header; //"ATOFF"

	int num_vertices, num_normals, num_colors, num_edges, num_faces;
	fp_in >> num_vertices >> num_normals >> num_colors >> num_edges >> num_faces;

	//setup normals_out to be the same size as indices_out
	normals_out = std::vector<float>(num_vertices*4);
	std::vector<float> normals_temp;

	//read vertices
	float x,y,z;
	for (int i=0; i < num_vertices; i++)
	{
		fp_in >> x >> y >> z;
		vertices_out.push_back(x);
		vertices_out.push_back(y);
		vertices_out.push_back(z);
		vertices_out.push_back(1.0f); //homogenous coordinate
	}

	//read normals (into temporary vector)
	for (int i=0; i < num_normals; i++)
	{
		fp_in >> x >> y >> z;
		normals_temp.push_back(x);
		normals_temp.push_back(y);
		normals_temp.push_back(z);
	}

	//read colors (and discard them)
	int discard;
	for (int i=0; i < num_colors*3; i++) {
		fp_in >> discard;
	}

	//read faces, i.e., indices
	int face;
	//for each face
	for (face=0; face < num_faces;face++)
	{
		int verts_per_face;
		fp_in >> verts_per_face;
		std::vector<int> v_index(4);
		for (int vert=0; vert < verts_per_face; vert++)
		{
			fp_in >> v_index[vert];
		}
		//read normal indices and fill normals_out to match vertex indices
		std::vector<int> n_index(4);
		if (num_normals > 0) 
		{
			for (int norm=0; norm < verts_per_face; norm++)
			{
				fp_in >> n_index[norm];
			}
			for (int norm=0; norm < verts_per_face; norm++)
			{
				//put the normals at the same index as the corresponding vertex
				normals_out[3*v_index[norm]] = normals_temp[3*n_index[norm]];
				normals_out[3*v_index[norm]+1] = normals_temp[3*n_index[norm]+1];
				normals_out[3*v_index[norm]+2] = normals_temp[3*n_index[norm]+2];
			}
		}
		else 
		{
			fp_in >> discard; //discard "-1"
		}
		//discard color index
		fp_in >> discard;

		//append indices to output
		if (verts_per_face == 3)
		{
			indices_out.push_back(v_index[0]);
			indices_out.push_back(v_index[1]);
			indices_out.push_back(v_index[2]);
		}
		//split quads into 2 triangles
		else if (verts_per_face == 4)
		{
			//triangle 1
			indices_out.push_back(v_index[0]);
			indices_out.push_back(v_index[1]);
			indices_out.push_back(v_index[2]);
			//triangle 2
			indices_out.push_back(v_index[2]);
			indices_out.push_back(v_index[3]);
			indices_out.push_back(v_index[0]);
		}
		else { std::cout << "error: file contains faces > QUAD!\n";}
	}

	//std::cout << header << "\nvertices:" << (vertices_out.size() / 4) << "\nnormals:" << (normals_out.size() / 3) << "\nfaces read correctly:" << (face == num_faces) << std::endl;

	fp_in.close();
	return true;
}
