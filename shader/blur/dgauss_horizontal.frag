#version 150 core

uniform sampler2D in_texture;
uniform float width;

in vec2 ex_texcoord;

out vec4 out_color;

//number of neighbours considered
//5x5 = 3
//7x7 = 4
const int blur_size = 3;

//original weigths
float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

//5x5 weights
//float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
//float weight[3] = float[]( 0.383, 0.242, 0.061 );

//7x7 weights
//float offset[5] = float[]( 0.00, 1.00, 2.00, 3.00, 4.00 );
//float weight[5] = float[]( 0.16, 0.15, 0.12, 0.09, 0.05 );

//horizontal gauss filter
vec4 post_process(void) {
	vec4 tc = texture(in_texture, ex_texcoord) * weight[0];
	
	//blur it
	for (int i=1; i<blur_size; i++)
	{
		tc += texture(in_texture, ex_texcoord + vec2(offset[i], 0.0)/width) * weight[i];
		tc += texture(in_texture, ex_texcoord - vec2(offset[i], 0.0)/width) * weight[i];
	}
	return tc;
}

void main(void) {
	out_color = post_process();
}