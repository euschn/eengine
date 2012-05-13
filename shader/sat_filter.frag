//post-processing that does nothing!
#version 150 core

uniform sampler2D in_texture;
uniform vec2 dimension; //dimension of the texture
uniform float filter_width;

in vec2 ex_texcoord;

out vec4 out_color;


void main(void) {
	float filter_size = filter_width;
	float num_pixels = (1 + filter_size*2) * (1 + filter_size*2);
	//determine pixel size
	float pixel_x = 1.0 / dimension.x;
	float pixel_y = 1.0 / dimension.y;
	//get max and min vectors
	vec2 smax = ex_texcoord + vec2(pixel_x * filter_size, pixel_y * filter_size);
	vec2 smin = ex_texcoord - vec2(pixel_x * filter_size + pixel_x, pixel_y * filter_size + pixel_y);

	//filter: t(xmax, ymax) - t(xmax, xmin) - t(xmin, ymax) + t(xmin, ymin)
	vec4 color = texture(in_texture, smax)
				- texture(in_texture, vec2(smax.x, smin.y))
				- texture(in_texture, vec2(smin.x, smax.y))
				+ texture(in_texture, smin);
	color /= num_pixels;

	//vec3 color = texture(in_texture, ex_texcoord).xyz;
	out_color = color;
}