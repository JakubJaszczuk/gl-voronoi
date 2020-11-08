#version 450

void main(void)
{
	const vec4[] vertices = vec4[](
		vec4(-1, -1, 0, 1.0),
		vec4(1, -1, 0, 1.0),
		vec4(-1, 1, 0, 1.0),
		vec4(1, 1, 0, 1.0)
	);

	gl_Position = vertices[gl_VertexID];
}
