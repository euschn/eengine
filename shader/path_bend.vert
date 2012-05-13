// vertex path bending
// courtesy of prideout.net

#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;
uniform mat4 normal_matrix;
uniform sampler2D path_texture;
uniform float inverseWidth;
uniform float inverseHeight;
uniform float pathOffset;
uniform float pathScale;

#define TextureHeight 128

in  vec3 in_position;
in  vec3 in_normal;
in  vec2 in_texcoord;

out vec4 ex_position;
out vec3 ex_normal;
out vec2 fragment_texcoord;

void main(void)
{
	float id = 0;
	float xstep = inverseWidth;
	float ystep = inverseHeight;
	float xoffset = 0.5 * xstep;
	float yoffset = 0.5 * ystep;

	//look up the current and previous centerline positions
	vec2 texCoord;
	texCoord.x = pathScale * in_position.x + pathOffset + xoffset;
	texCoord.y = 2.0f * id / TextureHeight + yoffset;

	vec3 currentCenter = texture(path_texture, texCoord).rgb;
	vec3 previousCenter = texture(path_texture, texCoord -vec2(xstep, 0)).rgb;
	vec3 pathDirection = normalize(currentCenter - previousCenter);

	//loop up the current orientation vector
	texCoord.x = pathOffset + xoffset;
	texCoord.y = texCoord.y + ystep;
	vec3 pathNormal = texture(path_texture, texCoord).rgb;

	//form the change-of-basis matrix
	vec3 a = pathDirection;
	vec3 b = pathNormal;
	vec3 c = cross(a,b);
	mat3 basis = mat3(a,b,c);

	//transform the normal vector and positions
	vec3 normal = basis * in_normal;
	vec3 position = currentCenter + (basis * vec3(0, in_position.yz));

	//vec3 position = in_position;
	//position.x += pathOffset;
	//vec3 normal = in_normal;

	//this is the regular gbuffer creation part
	fragment_texcoord = in_texcoord;
	ex_normal = (normal_matrix * vec4(normal, 0.0)).xyz;

	ex_position = modelview_matrix * vec4(position, 1);
	gl_Position = perspective_matrix * ex_position;
}
