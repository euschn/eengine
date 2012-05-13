//cgse - shader that generates the circle of confusion
#version 150 core

uniform sampler2D in_texture; //position texture
uniform vec2 near_far;

in vec2 ex_texcoord;

out vec4 out_color;

void main(void) {
	float threshold = 0.01;
	//get the position
	vec3 position = texture(in_texture, ex_texcoord).xyz;
	//linearize the z value
	float depth = -position.z / near_far.y;
	//hack to account for skybox
	if (position.z > near_far.y) {
		depth = 1;
	}

	vec4 color = vec4(0,0,0,1);
	// if the depth is close, we have a high coc
	color.x = 1 - smoothstep(threshold - 0.01, threshold, depth);

	out_color = color;
}