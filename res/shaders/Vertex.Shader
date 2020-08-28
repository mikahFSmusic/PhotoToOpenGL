#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 u_MVP;
uniform mat4 color_Mod;

out vec4 our_Color;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);
	our_Color = color * color_Mod;
}