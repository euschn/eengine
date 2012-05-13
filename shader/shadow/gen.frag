// deferred shading - shadowmap generation

#version 150 core

//uniform vec2 near_far;

in vec4  ex_position;

out vec4 out_color;

// Where to split the value. 8 bits works well for most situations.  
float g_DistributeFactor = 256;  
vec4 DistributePrecision(vec2 Moments)  
{  
	float FactorInv = 1 / g_DistributeFactor;  
	// Split precision  
	vec2 IntPart;  
	vec2 FracPart = modf(Moments * g_DistributeFactor, IntPart);  
	// Compose outputs to make reconstruction cheap.  
	return vec4(IntPart * FactorInv, FracPart);  
}  

void main(void)
{
	float depth = ex_position.z / ex_position.w;

	//move from unit cube [-1,1] to [0,1]
	depth = depth * 0.5 + 0.5;

	//compute partial derivatives of depth
	float dx = dFdx(depth);
	float dy = dFdy(depth);

	//anti biasing
	float m2 = depth*depth + 0.25*(dx*dx + dy*dy);
	vec2 moments = vec2(depth, m2);
	// bias for more precision
	moments -= vec2(0.5, 0.5);
	//split precision
	out_color = DistributePrecision(moments);
	//out_color = vec4(moments, 0, 1);
}
