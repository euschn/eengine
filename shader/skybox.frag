#version 150 core

uniform samplerCube in_texture;
//uniform vec3 light_position;

//in  vec3 ex_normal;
in vec3 ex_eye;

out vec4 out_color;
out vec4 out_normal;
out vec4 out_position;

void main(void)
{
	//vec3 light = normalize(light_position);

	//float diffuse_intensity = clamp(max( dot(ex_normal, light), 0.0), 0.0, 1.0);
	float diffuse_intensity = 0.8f;

	//vec3 reflection = reflect(ex_eye, ex_normal);

	vec4 texel_color = texture(in_texture, ex_eye);

	out_color = vec4((texel_color.xyz * diffuse_intensity), 1); //diffuse
	out_normal = vec4(0,0,0,0);
	out_position = vec4(500000, 500000, 500000, 1);
}
