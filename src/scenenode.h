/*
 * scenenode.h
 * base class for scenegraph nodes
 *
 */

#ifndef SCENENODE_H_
#define SCENENODE_H_

#include <vector>
#include "glm/glm.hpp"

using namespace std;
using namespace std::tr1;

// enum to denote the shader used to render a node
// non-renderable nodes (e.g., transformation) get SH_NONE
enum shader_type {SH_NONE, SH_MINIMAL, SH_SMOOTH, SH_TEXTURE, SH_PARTICLE, SH_SNOW, SH_NORMTEXTURE, SH_TERRAIN, SH_POINTLIGHT};

class scene_node {

private:

protected:
	int _shader_type;

public:
	//members
	glm::mat4 transformation_matrix;
	vector< shared_ptr<scene_node> > children;

	//functions
	scene_node() { _shader_type = SH_NONE; transformation_matrix = glm::mat4(1.0f); }
	virtual ~scene_node() {}
	//prepare for rendering
	virtual void pre_render() {}
	//get the normal information set up
	virtual void pre_normal() {}
	//render the object associated with the node
	virtual void render() {}
	//instanced rendering
	virtual void render_instanced(const int num_instances = 1) { render(); }
	//do stuff after rendering
	virtual void post_render() {}
	//access shader type (cannot be changed in base scenenode)
	int shader_type() const { return _shader_type; }
	//add a child node
	virtual void add_child(shared_ptr<scene_node> mp) {
		children.push_back(mp);
	}
	//dimension of the node, used for bounding sphere
	//returns (0,0,0) for non-draw nodes
	virtual glm::vec3 get_dimension() const {return glm::vec3(0.0f);}
	//return a scalar as dimension
	virtual float get_scalar_dimension() const {return 0.0f;}

};
typedef std::tr1::shared_ptr<scene_node> node_ptr;

#endif /* SCENENODE_H_ */
