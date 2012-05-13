// deferred shading - gbuffer generation

#version 150 core

uniform sampler2D in_texture;
//uniform vec3 light_position;

in vec4  ex_position;
in vec3  ex_normal;
in vec2  fragment_texcoord;

out vec4 out_color;
out vec4 out_normal;
out vec4 out_position;

void main(void)
{
	//vec3 light = normalize(light_position);
	vec3 normal = normalize(ex_normal);

	vec4 texel_color = texture(in_texture, fragment_texcoord);
	out_color = texel_color;
	out_normal = vec4(normal, 0.0);
	//out_color = texel_color * 0.05 + texel_color * diffuse_intensity * 0.95; //ambient + diffuse
	out_position = ex_position;
}
