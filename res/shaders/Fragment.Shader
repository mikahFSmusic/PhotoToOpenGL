#version 330 core

in vec4 our_Color;

out vec4 color;

void main()
{
	color = vec4(our_Color);
}