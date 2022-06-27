#version 430

/**
 * PGR - Raytracing pomocou shaderov
 * VUT FIT, 2019/2020
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	quadShader.frag
 *
 */

in vec2 texPosition;
out vec4 fragmentColor;
layout (binding = 0) uniform sampler2D img;

void main()
{
	fragmentColor = texture(img, texPosition);
}