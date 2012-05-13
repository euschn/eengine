#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;

in  vec4 in_position;
//in  vec3 in_normal;

//out vec3 ex_normal;
out vec3 ex_eye;

void main(void)
{

/*
	ex_normal = (modelview_matrix * vec4(in_normal, 0.0)).xyz;

	ex_eye = normalize((modelview_matrix * in_position).xyz);

	gl_Position = perspective_matrix * modelview_matrix * in_position;
	*/

	ex_eye = in_position.xyz;

	gl_Position = perspective_matrix * ((modelview_matrix * in_position)-vec4(vec3(modelview_matrix[3].xyz), 0.0));

}
