/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Raymarcher.h
 *
 */

#include "Raymarcher.h"


Raymarcher::Raymarcher(glm::vec2 screenSize, Camera* camera, Fractal* fractalInfo, Rendering* renderingInfo)
{
    this->screenSize = screenSize;
    this->camera = camera;
    this->fractal = fractalInfo;
    this->rendering = renderingInfo;
}

glm::vec3 Raymarcher::rayDirection(glm::vec2 pixelCoord) {
    glm::vec2 xy = pixelCoord - screenSize / 2.0f;
    float z = screenSize.y / camera->vFov;
    return glm::normalize(glm::vec3(xy, -z));
}

float Raymarcher::sphereSDF(glm::vec3 sphereCenter, float sphereRadius, glm::vec3 point)
{
    return glm::length(point - sphereCenter) - sphereRadius;
}

float Raymarcher::mandelbulbSDF(glm::vec3 p)
{
	int Iterations = fractal->iterations;
	float Power = fractal->power;

	glm::vec3 w = p;
	float m = glm::dot(w, w);

	float dz = 1.0;
	for (int i = 0; i < Iterations; i++)
	{
		dz = Power * pow(sqrt(m), Power - 1.0f) * dz + 1.0f;

		float r = glm::length(w);
		float b = Power * acos(w.y / r);
		float a = Power * atan2(w.x, w.z);
		w = p + pow(r, Power) * glm::vec3(sin(b) * sin(a), cos(b), sin(b) * cos(a));

		m = dot(w, w);
		if (m > 256.0)
			break;
	}

	return 0.25f * log(m) * sqrt(m) / dz;
}

float Raymarcher::sceneSDF(glm::vec3 point)
{
    //return sphereSDF(glm::vec3(0.0f), 1.0f, point);
	return mandelbulbSDF(point);
}

glm::vec3 Raymarcher::estimateNormal(glm::vec3 p, float dist, float epsilon) {
    glm::vec3 n;
    n.x = sceneSDF(p + glm::vec3(epsilon, 0.0, 0.0)) - dist;
    n.z = sceneSDF(p + glm::vec3(0.0, 0.0, epsilon)) - dist;
    n.y = sceneSDF(p + glm::vec3(0.0, epsilon, 0.0)) - dist;
    return normalize(n);
}

glm::vec3 Raymarcher::trace(Ray r)
{
	const glm::vec3 BgColor = glm::vec3(0.53, 0.8, 0.8);
	float MinDist = 1.0f / powf(10, rendering->detail);
	float DetailPower = rendering->detailPower;
	int MaxMarchingSteps = rendering->maxSteps;

	// background color
	glm::vec3 color = BgColor;

	float totalDist = NEAR_PLANE;
	int steps = 0;

	float epsilon = MinDist;
	float epsilonModified = MinDist;		// SDF minimal distance based on zoom level

	// bounding sphere
	float boundingSphere = sphereSDF(glm::vec3(0.0, 0.0, 0.0), 1.2f, r.origin);

	if (boundingSphere > 0.0)
	{
		totalDist += boundingSphere;
	}

	glm::vec3 col;

	for (steps = 0; steps < MaxMarchingSteps; steps++)
	{
		glm::vec3 samplePoint = r.origin + totalDist * r.dir;
		float dist = sceneSDF(samplePoint);

		// Move along the view ray
		totalDist += dist;

		epsilonModified = glm::clamp(epsilon * pow(totalDist, DetailPower), MinDist, FAR_PLANE);

		if (dist < epsilonModified)
		{
			// Ray is inside the scene surface
			totalDist -= (epsilonModified - dist);

			col = glm::vec3(0.334, 0.42, 0.184);
			col *= 0.5;

			color = shade(samplePoint, r.dir, col, dist, epsilonModified);

			// ambient occlusion based on number of marching steps
			//color *= glm::vec3(1-float(steps)/float(MaxMarchingSteps));

			break;
		}

		if (totalDist >= FAR_PLANE) {
			// Ray reached far plane
			break;
		}
	}

	if (steps == MaxMarchingSteps) color = glm::vec3(0.0);

	return color;
}

glm::vec3 Raymarcher::getColor(glm::vec2 pixelCoords)
{
	glm::vec3 direction = rayDirection(pixelCoords);
	glm::vec4 dir = camera->getViewMatrix() * glm::vec4(direction, 0.0);
	Ray r = { camera->position, glm::vec3(dir.x, dir.y, dir.z) };

	glm::vec3 color = trace(r);

	color = sqrt(color);
	return color;
}

glm::vec3 Raymarcher::shade(glm::vec3 point, glm::vec3 viewDirection, glm::vec3 color, float dist, float epsilon)
{
	const glm::vec3 ambientLight = glm::vec3(0.1f);
	const glm::vec3 light = glm::normalize(glm::vec3(0.0f, 1.4f, 1.7f));
	const glm::vec3 lightIntensity = glm::vec3(1.0f);

	// normal vector of a given surface point
	glm::vec3 N = estimateNormal(point, dist, epsilon);

	// specular exponent
	const float n = 10.0f;
	// specular component
	const float Ks = 0.08f;

	// compute diffuse component
	glm::vec3 diffuse = color * lightIntensity * glm::max(0.0f, glm::dot(light, N));

	// reflected light vector
	glm::vec3 R = glm::reflect(-light, N);

	// compute specular component
	glm::vec3 specular = glm::vec3(pow(glm::max(0.0f, dot(-viewDirection, R)), n));

	glm::vec3 result;
	glm::vec3 ambientColor = color * ambientLight;

	result = ambientColor + lightIntensity * (diffuse + Ks * specular);

	return glm::clamp(result, 0.0f, 1.0f);
}