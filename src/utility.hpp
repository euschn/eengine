#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include "common.hpp"
#include <vector>

bool file_exists(const string &filename);

string read_file(const string &filename);

// Query OpenGL errors and print error messages to STDERR.
void get_errors(void);

bool read_atoff_object(string& filename, std::vector<float>& vertices_out, std::vector<float>& normals_out, std::vector<unsigned int>& indices_out);

#endif //#ifndef _UTILITY_HPP_
