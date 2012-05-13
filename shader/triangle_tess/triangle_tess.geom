 #version 400
 
 // Geometry

//uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;
uniform vec3 light_position;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in  vec3 te_position[3];
in	vec3 te_patchDistance[3];
out float diffuse_intensity; // flat per triangle lighting
out vec3 g_patchDistance;
out vec3 g_triDistance;

void main()
{
	vec3 A = te_position[1] - te_position[0];
	vec3 B = te_position[2] - te_position[0];
	vec3 g_facetNormal = (modelview_matrix * vec4(normalize(cross(A, B) ), 0 ) ).xyz;
	vec3 N = normalize(g_facetNormal);
	vec3 L = normalize(light_position);
	diffuse_intensity = clamp( dot(N,L), 0, 1);

	g_patchDistance = te_patchDistance[0];
	g_triDistance = vec3(1,0,0);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	g_patchDistance = te_patchDistance[1];
	g_triDistance = vec3(0,1,0);
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	g_patchDistance = te_patchDistance[2];
	g_triDistance = vec3(0,0,1);
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}