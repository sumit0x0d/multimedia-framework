#version 320 es

precision mediump float;

uniform sampler2D uTex;
layout (location = 0) out vec4 color;
uniform vec4 uColor;
in vec2 vTexCoord;

void main()
{
	color = texture(uTex, vTexCoord);
};
