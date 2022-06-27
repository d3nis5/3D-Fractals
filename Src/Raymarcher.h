/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Raymarcher.h
 *
 */

#pragma once

#ifndef RAYMARCHER_H
#define RAYMARCHER_H

#include "Camera.h"

typedef struct fractal
{
	float power;
	int iterations;
} Fractal;

typedef struct rendering
{
	int maxSteps;
	float detail;
	float detailPower;
	bool shadows;
	float shadowSoftness;
	int antialiasing;
	glm::vec3 lightPosition;
} Rendering;

typedef struct ray
{
	glm::vec3 origin;
	glm::vec3 dir;
} Ray;

#define FAR_PLANE 15.0f		// far plane distance
#define NEAR_PLANE 0.0f		// near plane distance

class Raymarcher
{
public:
	Raymarcher(glm::vec2 screenSize, Camera* camera, Fractal* fractalInfo, Rendering* renderingInfo);

	/**
	 * @brief Computes color of a given pixel
	 */
	glm::vec3 getColor(glm::vec2 pixelCoords);

private:
	glm::vec2 screenSize;
	Fractal* fractal;		// fractal info
	Rendering* rendering;	// rendering info
	Camera* camera;

    /**
     * @brief Returns direction of a ray going through given pixel
     */
    glm::vec3 rayDirection(glm::vec2 pixelCoord);

	/**
	 * @brief Signed distance function of a scene
	 * @param point Point for which to calculate SDF
	 */
	float sceneSDF(glm::vec3 point);

	float mandelbulbSDF(glm::vec3 point);

	float sphereSDF(glm::vec3 sphereCenter, float sphereRadius, glm::vec3 point);

	glm::vec3 estimateNormal(glm::vec3 p, float dist, float epsilon);

	glm::vec3 trace(Ray r);

	glm::vec3 shade(glm::vec3 point, glm::vec3 viewDirection, glm::vec3 color, float dist, float epsilon);
};

#endif