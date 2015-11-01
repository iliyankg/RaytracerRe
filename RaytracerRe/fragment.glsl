
#version 430 core

smooth in vec2 texCoord;

out vec4 color;

uniform sampler2D tex;

void main(void)
{
	color = texture(tex, texCoord);
}
