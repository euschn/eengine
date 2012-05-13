// Fragment Shader – file "pointlight.frag"

#version 150 core

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;

uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_radius;

in  vec4 vs_viewspace_position;

out vec4 out_color;

void main(void)
{
	// gbuffer access vector
	vec2 fragpos = vs_viewspace_position.xy / vs_viewspace_position.w;
	vec2 texcoord = 0.5f * (fragpos + 1); //move from [-1,-1] to [0,1]

	//diffuse factor
	vec3 normal = normalize(texture(normal_texture, texcoord).xyz);
	vec3 pos = texture(position_texture, texcoord).xyz;
	vec3 light_vec = pos - light_position;
	vec3 L = normalize(light_vec);

	float diffuse = clamp(dot(normal, -L), 0.0, 1.0);

	//specular highlight
	float specular = 0;
	if (diffuse > 0) {
		vec3 eye = normalize(-pos);
		vec3 reflection = reflect(L, normal);

		specular = pow (clamp(dot(reflection, eye), 0.0, 1.0), 20);
	}

	//attenuation
	float distance_factor = length(light_vec) / light_radius;
	float attenuation = (1 - (distance_factor));

	//final color
	vec4 texel = texture(albedo_texture, texcoord);
	float pointlight_power = 1;
	out_color = (texel * diffuse * pointlight_power
				+ texel * specular)
				* attenuation;
	//apply light color
	out_color *= vec4(light_color, 1);
}
