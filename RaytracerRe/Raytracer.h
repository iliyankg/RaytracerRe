#pragma once
#include <glm.hpp>
#include <vector>

#include "Object.h"
#include "Light.h"
#include "Intersection.h"

using namespace std;
using namespace glm;

namespace Raytracer
{
	int index;

	Intersection hitData;
	vector<Intersection> hitMats;

	float _calcDiffuse(Light* light, Intersection* hit)
	{
		return light->intensity * fmax(0, dot(normalize(light->lightPos - hit->hitPos), hit->hitNormal));
	}
	float _calcSpec(Light* light, Intersection* hit, vec3 rayDirection)
	{
		vec3 hitToCamDirection = -rayDirection;
		vec3 hitToLightDirection = normalize(light->lightPos - hit->hitPos);
		vec3 reflected = -2 * dot(-hitToLightDirection, hit->hitNormal) * hit->hitNormal - hitToLightDirection;

		return light->specIntensity * pow(fmax(0, dot(reflected, hitToCamDirection)), hit->hitMat.getShine());
	}
	vec3 calcFinalColor(Light* light, Intersection* hit, vec3 direction)
	{
		vec3 toReturn;
		toReturn.r = hit->hitMat.getDiffuse().r * light->ambientIntensity + hit->hitMat.getDiffuse().r * _calcDiffuse(light, hit) + hit->hitMat.getSpecular().r * _calcSpec(light, hit, direction);
		toReturn.g = hit->hitMat.getDiffuse().g * light->ambientIntensity + hit->hitMat.getDiffuse().g * _calcDiffuse(light, hit) + hit->hitMat.getSpecular().g * _calcSpec(light, hit, direction);
		toReturn.b = hit->hitMat.getDiffuse().b * light->ambientIntensity + hit->hitMat.getDiffuse().b * _calcDiffuse(light, hit) + hit->hitMat.getSpecular().b * _calcSpec(light, hit, direction);
		return toReturn;
	}

	bool _trace(vec3 origin, vec3 direction, vector<Object*> objs)
	{
		bool toRenturn = false;

		int maxDist = 10000000000;
		for (int i = 0; i < objs.size(); ++i)
		{
			if (objs[i]->isIntersecting(origin, direction, &hitData) && hitData.hitDistance < maxDist && hitData.hitDistance > 0.0)
			{
				toRenturn = true;
				maxDist = hitData.hitDistance;
				index = i;
			}
		}

		if (toRenturn)
		{
			objs[index]->isIntersecting(origin, direction, &hitData);
			return true;
		}
		else
		{
			return false;
		}
	}
	float _shadowAmmount(int index, vec3 hit, Light* light, vector<Object*> objs)
	{
		float fullShadow = pow(light->resolution, 2);
		float shadowHits = 0.0f;

		Intersection temp;
		for (int k = 0; k < objs.size(); ++k)
		{
			for (int i = 0; i <= light->resolution; ++i)
			{
				for (int j = 0; j <= light->resolution; ++j)
				{
					vec3 currentLightTarget = vec3(light->lightPos.x + (light->posAndSize[1].x / light->resolution) * i,
						light->lightPos.y,
						light->lightPos.z + (light->posAndSize[1].z / light->resolution) * j);

					if (index != k)
					{
						if (objs[k]->isIntersecting(hit, normalize(currentLightTarget - hit), &temp))
							shadowHits++;
					}
				}
			}
		}

		return shadowHits / fullShadow;
	}

	vec3 recursiveTrace(vec3 origin, vec3 direction, vector<Object*> objs, Light* light, int counter)
	{
		if (counter == 0)
		{
			vec4 temp = mix(vec4(calcFinalColor(light, &hitMats[0], direction), 0), vec4(0.0, 0.0, 0.0, 1.0), _shadowAmmount(index, hitMats[0].hitPos, light, objs));
			return vec3(temp.r, temp.g, temp.b);

		}
		else
		{
			if (_trace(origin, direction, objs))
			{
				hitMats.push_back(hitData);
			}
			else
			{
				hitMats.push_back(Intersection());
			}
			counter--;
			return recursiveTrace(origin, direction, objs, light, counter);
		}
	}
}
