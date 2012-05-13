//simple raytracing
#version 150 core

uniform mat4      modelview_matrix;
uniform mat4      inverse_modelview_matrix;
uniform sampler2D in_texture;
uniform float	  time;
uniform samplerCube background_texture;

in vec2 ex_texcoord;

out vec4 out_color;

// refractive indices
const float AIR = 1.0;
const float GLASS = 1.51714;


// --------------------
// objects in our scene
// --------------------
vec4 sph1 = vec4(1, 2, 1, 1);
vec4 sph2 = vec4(-2, 2, 1, 1);
vec4 sph3 = vec4(10, 5, -5, 3);
vec4 sph4 = vec4(2, 5, 1, 1);
//plane
vec4 plane_normal = vec4(0, 1, 0, 0);
float plane_d = 0;
//ray origin
vec4 ray_origin = vec4(0.0, 0.5, 4.0, 1);

// lights
// ------
//light vector
vec3 light = normalize (vec3(0.57703, 0.57703, 0.57703) );

//area light
const float area_size = 2.0;
const float area_height = 10.0;
const float soft_shadow_quality = 50;
vec3 area_light_1 = vec3(-area_size, area_height, -area_size);
vec3 area_light_2 = vec3(-area_size, area_height, area_size);
vec3 area_light_3 = vec3(area_size, area_height, area_size);
vec3 area_light_4 = vec3(area_size, area_height, -area_size);


// ----------------------
// intersection functions
// ----------------------
float intersect_sphere(in vec3 ro, in vec3 rd, in vec4 sph, const float front = 1.0)
{
	// so a sphere centered at the origin has equation |xyz| = r
	// meaning, |xyz|^2 = r^2, meaning <yxz, xyz> = r^2
	// new xyz = ro + t*rd therefore |ro|^2 + t^2 + 2<ro,rd>t -r^2 = 0
	//which is a quadratic equation
	//vec3 oc = ro - sph.xyz;
	//float b = 2.0*dot(oc, rd);
	//float c = dot(oc,oc) - sph.w*sph.w;
	//float h = b*b - 4.0*c;
	//if (h < 0.0) { return -1.0; }
	//float t = (-b - sqrt(h))/2.0f;
	//return t;

	vec3 o_minus_c = ro - sph.xyz;
	float A = dot(rd,rd);
	float B = 2 * dot(o_minus_c, rd);
	float C = dot(o_minus_c, o_minus_c) - sph.w*sph.w;
	float sqrt_expression = B*B - 4*A*C;
	if (sqrt_expression < 0) {
		return -1;
	}
	else {
		float result = (-B - sqrt(sqrt_expression) * front) / (2*A);
		return result;
		//return t;
	}
}

// returns 0 if there is no intersection, else 1
float intersect_sphere_fast(in vec3 ro, in vec3 rd, in vec4 sph)
{
	vec3 o_minus_c = ro - sph.xyz;
	float A = dot(rd,rd);
	float B = 2 * dot(o_minus_c, rd);
	float C = dot(o_minus_c, o_minus_c) - sph.w*sph.w;
	float sqrt_expression = B*B - 4*A*C;
	return step(0, sqrt_expression);
}

// return the sphere normal at a position on the sphere
vec3 sphere_get_normal(in vec3 pos, in vec4 sph)
{
	return (pos - sph.xyz)/sph.w;
} 

vec3 plane_get_normal(in vec3 pos)
{
	return plane_normal.xyz;
}

float intersect_plane(in vec3 ro, in vec3 rd)
{
	float t = -(dot(ro, plane_normal.xyz) + plane_d) / dot(rd, plane_normal.xyz);
	return t;
}

// intersect a ray with all objects in our scene
float intersect_scene(in vec3 ro, in vec3 rd, out float resT)
{
	resT = 1000.0;
	float id = -1.0;
	float tsph = intersect_sphere(ro, rd, sph1); //intersect with sphere
	float tmirr = intersect_sphere(ro, rd, sph2); //intersect with mirror sphere
	float tsph3 = intersect_sphere(ro, rd, sph3); //intersect with 3rd sphere
	float tglass = intersect_sphere(ro, rd, sph4); //intersect with glass sphere
	float tpla = intersect_plane(ro, rd); //intersect with a plane
	if (tsph > 0.0)
	{
		id = 1.0;
		resT = tsph;
	}
	if(tpla > 0.0 && tpla < resT) 
	{
		id = 2.0;
		resT = tpla;
	}
	if(tmirr > 0.0 && tmirr < resT) 
	{
		id = 3.0;
		resT = tmirr;
	}
	if(tsph3 > 0.0 && tsph3 < resT) 
	{
		id = 4.0;
		resT = tsph3;
	}
	if(tglass > 0.0 && tglass < resT) 
	{
		id = 5.0;
		resT = tglass;
	}

	return id;
}


// intersect a ray with all objects in our scene
// we are just interested in whether there is a hit or not (shadows)
bool intersect_scene_bool(in vec3 ro, in vec3 rd)
{
	bool result = false;
	float id = -1.0;
	float tsph = intersect_sphere_fast(ro, rd, sph1); //intersect with sphere
	float tmirr = intersect_sphere_fast(ro, rd, sph2); //intersect with mirror sphere
	float tsph3 = intersect_sphere_fast(ro, rd, sph3); //intersect with 3rd sphere
	float tglass = intersect_sphere_fast(ro, rd, sph4); //intersect with glass sphere
	float tpla = intersect_plane(ro, rd); //intersect with a plane

	result = result || (tsph > 0.0) || (tmirr > 0.0)|| (tsph3 > 0.0) || (tglass > 0.0) || (tpla > 0.0);

	return result;
}

// shadow test
bool intersect_shadow(in vec3 ro, in vec3 rd)
{
		float shadow = -1.0;
		float shadow_t = -1.0;
		shadow_t = intersect_scene(ro, rd, shadow); 
		if (shadow_t > 0.0) {
			return true;
		}
		return false;
}

//soft shadow test
float intersect_soft_shadow(in vec3 ro, const float x_tests, const float y_tests) {
		float result = 0.0;
		const float num_tests = x_tests * y_tests;
		const float increment = 1.0 / num_tests;

		//soft shadow test loop
		//for (float i = 0; i < num_tests; i++ ) {
			////float shadow = -1.0;
			////float shadow_t = -1.0;
			////get the directions
			//vec3 light = mix(area_light_1, area_light_2, i / num_tests);
			//vec3 dir = normalize(light - ro);
//
			////shadow_t = intersect_scene(ro, dir, shadow); 
			////if (shadow_t < 0.0) {
			//if ( !intersect_scene_bool(ro, dir) ) {
				//result +=increment;
			//}
		//}
		for (float i = 0; i < x_tests; i++) {
			vec3 x1 = mix(area_light_1, area_light_2, i / x_tests);
			vec3 x2 = mix(area_light_4, area_light_3, i / x_tests);

			for (float j = 0; j < y_tests; j++) {
				vec3 light = mix(x1, x2, j / y_tests);
				vec3 dir = normalize(light - ro);

				if ( !intersect_scene_bool(ro, dir) ) {
					result +=increment;
				}
			}
		}


		//return the shadow value
		return result;
}

// -----------------
// main
// ----------------
void main(void) {

	//move objects according to time
	sph1.x += cos(time);
	sph1.z += sin(time);
	sph2.y += cos(time*2);
	//sph4.y += 3 * cos(time);

	//apply transformation matrix to scene
	sph1.xyz = (modelview_matrix * vec4(sph1.xyz, 1)).xyz;
	sph2.xyz = (modelview_matrix * vec4(sph2.xyz, 1)).xyz;
	sph3.xyz = (modelview_matrix * vec4(sph3.xyz, 1)).xyz;
	sph4.xyz = (modelview_matrix * vec4(sph4.xyz, 1)).xyz;
	plane_normal = modelview_matrix * plane_normal;
	plane_d = -dot(plane_normal.xyz, modelview_matrix[3].xyz );
	//transform lights
	light = (modelview_matrix * vec4(light, 0)).xyz;
	area_light_1 = (modelview_matrix * vec4(area_light_1, 1)).xyz;
	area_light_2 = (modelview_matrix * vec4(area_light_2, 1)).xyz;
	area_light_3 = (modelview_matrix * vec4(area_light_3, 1)).xyz;
	area_light_4 = (modelview_matrix * vec4(area_light_4, 1)).xyz;

	//color components 
	float ambient = 0.05;
	float diffuse_component = 0.95;
	float diffuse_intensity = 0;
	vec4 refract_color = vec4(1,1,1,1);

	//uv are pixel coords
	vec2 uv = (ex_texcoord);

	//we generate a ray at origin ro and direction rd
	vec3 ro = ray_origin.xyz;
	vec3 rd = normalize(vec3( (-1.0+2.0*uv)*vec2(1.7777777777777778, 1), -5.0));

	//we intersect the ray with the 3d scene
	float t = -1;
	float id = -1; //intersect_scene(ro, rd, t);
	float dif_reflect = 1;

	// exit condition for loop
	bool hit = false;

	//default color is pink
	vec3 col = vec3(255, 20, 147);
	
	//vec4 cm_tex = inverse_modelview_matrix * vec4( rd, 0 );
	//vec3 col = texture(background_texture, cm_tex.xyz).xyz;

	// initial position
	vec3 pos = ro;

	/************************
	// start of RT loop
	*************************/
	while (!hit) {
		// intersect with the scene
		t = -1; // reset t
		id = intersect_scene(ro, rd, t);
		if (id > 0) { //we hit something
			// green sphere
			if (id > 0.5 && id < 1.5)
			{
				hit = true;
				// if we hit the sphere
				pos = ro + t*rd;
				vec3 nor = sphere_get_normal (pos, sph1);
				diffuse_intensity = clamp(dot(nor, light), 0 , 1) * dif_reflect;
				//add shadow
				if (intersect_shadow(pos + nor * 0.0001, light)) {
					diffuse_intensity=0;
				}
				col = vec3(0.2, 0.9, 0.1)*diffuse_intensity*diffuse_component + vec3(0.5, 0.2, 0.4)*ambient;
			}
			// floor
			else if (id > 1.5 && id < 2.5)
			{
				hit = true;
				// we hit the plane
				pos = ro + t*rd;
				vec3 nor = plane_get_normal( pos );
				diffuse_intensity = clamp(dot(nor, light), 0 , 1 ) * dif_reflect;
				//add shadow
				if (intersect_shadow(pos + nor * 0.0001, light)) {
					diffuse_intensity=0;
				}
				//diffuse_intensity = intersect_soft_shadow(pos + nor * 0.0001, 7, 7) * dif_reflect; //soft shadow test

				col = vec3(0.5) * diffuse_intensity * diffuse_component + vec3(0.5) * ambient;
			}
			// reflecting sphere intersection
			if (id > 2.5 && id < 3.5)
			{
				// if we hit the sphere
				// reflect by sending a new ray from the intersection point
				pos = ro + t*rd;
				vec3 nor = sphere_get_normal(pos, sph2);
				rd = reflect(rd, nor); // reset ray direction
				//cm_tex = inverse_modelview_matrix * vec4(rd, 0);
				//col = texture(background_texture, cm_tex.xyz).xyz;
				ro = pos; // reset ray origin
				dif_reflect = 0.89 * clamp(dot(nor, light), 0 , 1) + 0.1;		
				// add shadow
				if (intersect_shadow(pos + nor * 0.0001, light)) {
					dif_reflect = 0.0;
				}
			}
			// yellow sphere
			else if (id > 3.5 && id < 4.5)
			{
				hit = true;
				// if we hit the sphere
				pos = ro + t*rd;
				vec3 nor = sphere_get_normal (pos, sph3);
				diffuse_intensity = clamp(dot(nor, light), 0 , 1) * dif_reflect;
				//add shadow
				if (intersect_shadow(pos + nor * 0.0001, light)) {
					diffuse_intensity=0;
				}
				col = vec3(1, 1, 0) * diffuse_intensity * diffuse_component + vec3(1, 1, 0)*ambient;
			}
			// refracting sphere
			else if (id > 4.5 && id < 5.5)
			{
				//hit = true;
				// if we hit the sphere
				pos = ro + t*rd;
				vec3 nor = sphere_get_normal(pos, sph4);
				float NdotL = dot(nor, light);
				dif_reflect = 0.89 * clamp(NdotL, 0 , 1) + 0.1;
				diffuse_intensity = clamp(NdotL, 0 , 1) * dif_reflect;
				
				ro = pos;
				// only do one direction of refraction for testing
				float eta = AIR / GLASS;
				rd = refract(normalize(rd), normalize(nor), eta);
				ro += rd * 0.0001;

				// now we get the intersection when exiting the reflecting sphere
				t = intersect_sphere(ro, rd, sph4, -1);
				pos = ro + t*rd;
				eta = GLASS / AIR;
				rd = refract(normalize(-rd), normalize(nor), eta);
				rd.y *= -1;
				ro += rd * 0.0001;
				//refract_color = vec4(1,0,0,1);


				pos = ro;

				//id = intersect_scene(ro, rd, t);
				//add shadow
				if (intersect_shadow(pos + nor * 0.0001, light)) {
					diffuse_intensity=0;
				}
				//col = vec3(1, 0, 0)*diffuse_intensity*diffuse_component + vec3(1, 0, 0)*ambient;
			}
			else {
				hit = true; // exit condition
			}
		}
		else {
			hit = true; // we hit the background
			vec4 cm_tex = inverse_modelview_matrix * vec4( rd, 0 );
			col = texture(background_texture, cm_tex.xyz).xyz;
		}
	}
	//}
	////slightly darker background reflect
	//else {
		//col *= dif_reflect;
	//}
//

	// were done, output the color
	out_color = vec4(col, 1.0) * refract_color;
}