//deferred lighting phong shader
#version 150 core

uniform sampler2D in_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;
uniform sampler2D shadow_texture; //shadowmap
uniform vec2 shadow_dimension; //dimension of the texture
uniform float filter_width;

uniform mat4 shadow_matrix; //transform from view space to light space
uniform vec3 directional_light;
//color mask: reduce light if needed
uniform vec4 color_mask;
uniform bool enable_shadowmap;

in vec2 ex_texcoord;

out vec4 out_color;
	 
// Where to split the value. 8 bits works well for most situations.  
float g_DistributeFactor = 256;  
vec2 RecombinePrecision(vec4 Value)  
{
	float FactorInv = 1 / g_DistributeFactor;  
	return (Value.zw * FactorInv + Value.xy);  
}  

// variance shadow mapping
// shadow test using chebychev inequality
float shadow_test_cheby(vec3 pos)
{
	float p = 0.0;

	//transform pos to shadow space
	vec4 shadow_pos = shadow_matrix * vec4(pos, 1);
	shadow_pos = shadow_pos / shadow_pos.w;
	float current_depth = shadow_pos.z;
	//look up depth in texture
	vec4 shadow_texel = texture(shadow_texture, shadow_pos.xy);
	//recombine precision
	vec2 moments = RecombinePrecision(shadow_texel);
	// unbias for more precision
	moments += vec2(0.5, 0.5);
	//shadow test
	if (current_depth <= moments.x) {
		p = 1.0;
	}

	float variance = moments.y - (moments.x*moments.x);
	variance = max(variance, 0.00001);

	//probabilistic upper bound
	float d =  current_depth - moments.x;

	float pmax = variance / (variance + d*d);

	return max(p, pmax);
	//return result;
}


// variance shadow mapping
// shadow test using chebychev inequality
// filtered with SAT
float shadow_test_sat(vec3 pos)
{
	float p = 0.0;

	//transform pos to shadow space
	vec4 shadow_pos = shadow_matrix * vec4(pos, 1);
	shadow_pos = shadow_pos / shadow_pos.w;
	float current_depth = shadow_pos.z;
	//look up depth rectangle in sat texture
	float filter_size = filter_width;
	float num_pixels = (1 + filter_size*2) * (1 + filter_size*2);
	//determine pixel size
	float pixel_x = 1.0 / shadow_dimension.x;
	float pixel_y = 1.0 / shadow_dimension.y;
	//get max and min vectors
	vec2 smax = shadow_pos.xy + vec2(pixel_x * filter_size, pixel_y * filter_size);
	vec2 smin = shadow_pos.xy - vec2(pixel_x * filter_size + pixel_x, pixel_y * filter_size + pixel_y);

	//filter: t(xmax, ymax) - t(xmax, xmin) - t(xmin, ymax) + t(xmin, ymin)
	vec4 color = texture(shadow_texture, smax)
				- texture(shadow_texture, vec2(smax.x, smin.y))
				- texture(shadow_texture, vec2(smin.x, smax.y))
				+ texture(shadow_texture, smin);
	color /= num_pixels;

	//get moments from filtering SAT area
	vec2 moments = RecombinePrecision(color);
	//vec2 moments = color.xy;
	// unbias for more precision
	moments += vec2(0.5, 0.5);
	//vec2 moments = texture(shadow_texture, shadow_pos.xy).xy;
	//shadow test
	if (current_depth <= moments.x) {
		p = 1.0;
	}

	float variance = moments.y - (moments.x*moments.x);
	variance = max(variance, 0.00001);

	//probabilistic upper bound
	float d =  current_depth - moments.x;

	float pmax = variance / (variance + d*d);

	return max(p, pmax);
	//return result;
}

//light bleeding reduction
float reduce_light_bleeding(float pmax, float amount) {
	//remove the [0, amount] tail and linearly rescale (Amount,1]
	return smoothstep(amount, 1.0, pmax);

}


void main(void) {
	//simple lighting calculations
	vec3 light = normalize(directional_light);
	vec3 normal = texture(normal_texture, ex_texcoord).xyz;
	vec3 position = texture(position_texture, ex_texcoord).xyz;

	float diffuse_intensity = 1;
	float specular_intensity = 0;
	//hack to properly show skybox
	//skybox normals are set to null vector
	if (length(normal) > 0) {
		float shadow = 1;
		//shadowmapping
		shadow = shadow_test_cheby(position);
		//shadow = reduce_light_bleeding(shadow, 0.3);

		diffuse_intensity = clamp(max( dot(normal, light), 0.0), 0.0, 1.0) * shadow;

		//specular
		vec3 reflection = normalize(reflect(-light, normal));
		vec3 eye = -normalize(position);
		specular_intensity = pow( clamp(max( dot(reflection, eye), 0.0), 0.0, 1.0), 50.0);
		//shadow needs to remove specular
		if (diffuse_intensity == 0.0 || shadow < 1.0) {
			specular_intensity = 0;
		}
	}

	vec4 texel_color = texture(in_texture, ex_texcoord);
	out_color = texel_color * 0.05 + //ambient
				texel_color * diffuse_intensity * 0.95 //diffuse
				+ texel_color*specular_intensity;
	//color mask for testing;
	out_color *= color_mask;
}

//directional shadow test
float shadow_test(vec3 pos)
{
	float result = 0.0;

	//float current_depth = -pos.z;
	//current_depth = current_depth / 1000; //far plane division
	//transform pos to shadow space
	vec4 shadow_pos = shadow_matrix * vec4(pos, 1);
	shadow_pos /= shadow_pos.w;
	float current_depth = -shadow_pos.z * 0.5 + 0.5;
	//look up depth in texture
	vec2 moments = texture(shadow_texture, shadow_pos.xy).xy;
	//shadow test
	if (current_depth >= moments.x) {
		result = 1.0;
	}

	return result;
}