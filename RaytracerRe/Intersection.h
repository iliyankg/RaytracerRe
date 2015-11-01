#pragma once
#include <glm.hpp>

#include "Material.h"

using namespace glm;

class Intersection
{
public:
	Intersection() { isHit = false; };

	bool isHit;

	float hitDistance;

	double shadowPercentage;

	vec3 hitPos;
	vec3 hitNormal;
	Material hitMat;

	Intersection(float hitDist, vec3 hitNorm, vec3 origin, vec3 direction, Material mat)
	{
		hitDistance = hitDist;
		hitNormal = hitNorm;
		hitMat = mat;
		isHit = true;
		hitPos = origin + direction * hitDist;
	}
};