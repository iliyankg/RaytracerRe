#version 430 core

in vec2 vertex;

smooth out vec2 texCoord;

void main(void)
{
	gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0);
	texCoord = vertex * 0.5 + vec2(0.5, 0.5);
}
