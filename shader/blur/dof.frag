//cgse - final depth of field shader
#version 150 core

uniform sampler2D in_texture; //position texture
uniform sampler2D blur_texture; //blurred scene
uniform sampler2D coc_texture; //coc

in vec2 ex_texcoord;

out vec4 out_color;

void main(void) {
	vec3 color = texture(in_texture, ex_texcoord).xyz;
	vec3 blur_color = texture(blur_texture, ex_texcoord).xyz;
	float coc = texture(coc_texture, ex_texcoord).x;

	//combine the two texels based on coc
	color = mix(color, blur_color, min(coc * 10, 1));
	//if (coc > 0) color = blur_color;
	out_color = vec4(color, 1);
}