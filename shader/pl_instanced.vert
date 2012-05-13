// Vertex Shader – file "pointlight.vert"
// instanced drawing of pointlights

#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;
uniform mat4 light_matrix;
uniform sampler1D pl_positions;
uniform float num_instances;
uniform float light_radius;

in  vec3 in_position;

out vec3 light_position;
out vec4 vs_viewspace_position;

void main(void)
{
	vec3 instance_offset = texelFetch(pl_positions, gl_InstanceID, 0).xyz;
	instance_offset /= light_radius; //this seems to get positions right
	vec4 pos = vec4(in_position + instance_offset, 1);
	vec4 world_pos = modelview_matrix * pos;
	gl_Position = perspective_matrix * world_pos;
	light_position = (modelview_matrix * vec4(instance_offset, 1) ).xyz;
	vs_viewspace_position = gl_Position;
}
