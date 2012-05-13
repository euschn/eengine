//deferred lighting phong shader
#version 150 core

uniform sampler2D in_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;
uniform float filter_width;

uniform vec3 directional_light;
//color mask: reduce light if needed
uniform vec4 color_mask;
uniform bool enable_shadowmap;

in vec2 ex_texcoord;

out vec4 out_color;

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
	texel_color.xyz = texel_color.xyz * 0.05 + //ambient
				texel_color.xyz * diffuse_intensity * 0.95 //diffuse
				//+ texel_color*specular_intensity;
				+ vec3(1,1,1)*specular_intensity;
	out_color = texel_color;
	//color mask for testing;
	out_color *= color_mask;
}