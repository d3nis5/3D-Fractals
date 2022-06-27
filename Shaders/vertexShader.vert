#version 430

/**
 * PGR - Raytracing pomocou shaderov
 * VUT FIT, 2019/2020
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	vertexShader.vert
 *
 */

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 texturePosition;

out vec2 texPosition;

void main()
{
	texPosition = texturePosition;
	gl_Position = vec4(vertexPosition, 0.0, 1.0);
}

