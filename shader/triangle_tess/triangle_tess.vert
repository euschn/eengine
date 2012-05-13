#version 400

// Vertex

in vec4 in_position;
out vec3 v_position;

void main()
{
	v_position = in_position.xyz;
}