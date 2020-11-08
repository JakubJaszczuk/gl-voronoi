#version 450

#define FLT_MAX 3.402823466e+38

struct Point
{
	vec4 col;
	vec2 pos;
};

layout (binding = 0, std430) buffer Points
{
	Point points[];
};

layout (binding = 0, std140) uniform Resolution
{
	uint width;
	uint height;
} resolution;

out vec4 color;

void main(void)
{
	vec2 pos = vec2(gl_FragCoord.x / resolution.width, gl_FragCoord.y / resolution.height);
	uint ind = 0;
	float dist = FLT_MAX;
	for(uint i = 0; i < points.length(); ++i)
	{
		vec2 v = pos - points[i].pos;
		float d = dot(v, v);
		if(d < dist)
		{
			dist = d;
			ind = i;
		}
	}
	color = vec4(points[ind].col.xyz * (1 - sqrt(dist)), 1);
}

// Można robić kolor albo odległość :D
