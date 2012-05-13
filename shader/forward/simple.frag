// Fragment Shader – file "minimal.frag"
#version 150 core

uniform vec3 light_position;
uniform sampler2D in_texture;

in vec3 ex_normal;
in vec2 ex_texcoord;

out vec4 out_color;

void main(void)
{
	vec4 color = texture(in_texture, ex_texcoord); //vec4(1,0,0,1.0);

	//lighting
	float ambient = 0.1;
	float diffuse_intensity = 1;//clamp(dot(normalize(ex_normal), light_position), 0.0, 1.0);
	out_color = color * ambient +
				color * diffuse_intensity * 0.9;
}
