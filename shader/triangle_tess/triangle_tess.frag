#version 400

// Frag

in  vec3 g_facetNormal;
in	vec3 g_triDistance;
in	vec3 g_patchDistance;
in float diffuse_intensity;
//in float g_primitive; //??
out vec4 out_color;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main()
{
	vec3 color = vec3(0.4, 0.8, 0.8) * (0.2 + 0.8 * diffuse_intensity);

	float d1 = min(min(g_triDistance.x, g_triDistance.y), g_triDistance.z);
	float d2 = min(min(g_patchDistance.x, g_patchDistance.y), g_patchDistance.z);
	color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;

	out_color = vec4(color, 1.0);
}