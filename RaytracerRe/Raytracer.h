#pragma once
#include <glm.hpp>
#include <vector>

#include "Object.h"
#include "Light.h"
#include "Intersection.h"

using namespace std;
using namespace glm;

//Namespace used to seperate the CPU implementation from other sides of the program.
namespace Raytracer
{
	int index; //Used for selfshadow check

	Intersection hitData; //Stores current hit data
	vector<Intersection> hitMats; //Stores list of all hits for recursive reflections.
	
	/** \brief calcDiffuse - Calculates the diffuse modifier
	* \param light Light*
	* \param hit Intersection* The hit data for which the modifier is being calculated
	*
	* \return float
	*/
	float _calcDiffuse(Light* light, Intersection* hit)
	{
		return light->intensity * fmax(0, dot(normalize(light->lightPos - hit->hitPos), hit->hitNormal));
	}
	/** \brief _calcSpec - Calculates specular modifier
	* \param light Light*
	* \param hit Intersection* The hit data for which the modifier is being calculated
	* \param rayDirection vec3 Direction from hit to origin; Used when calculating angles.
	*
	* \return float
	*/
	float _calcSpec(Light* light, Intersection* hit, vec3 rayDirection)
	{
		vec3 hitToLightDirection = normalize(hit->hitPos - light->lightPos);
		vec3 reflected = 2 * dot(hitToLightDirection, hit->hitNormal) * hit->hitNormal - hitToLightDirection;

		return light->specIntensity * pow(fmax(0, dot(reflected, rayDirection)), hit->hitMat.getShine());
	}
	/** \brief calcFinalColor - Returns the final color by utelizing the _calcDiffuse() and _calcSpec() functions.
	* \param light Light*
	* \param hit Intersection* The hit data for which the color is being calculated.
	* \param direction vec3 Ray origin to ray hit direction vector.
	*
	* \return vec3
	*/
	vec3 calcFinalColor(Light* light, Intersection* hit, vec3 direction)
	{
		vec3 toReturn;
		toReturn.r = hit->hitMat.getDiffuse().r * light->ambientIntensity + hit->hitMat.getDiffuse().r * _calcDiffuse(light, hit) + hit->hitMat.getSpecular().r* _calcSpec(light, hit, direction);
		toReturn.g = hit->hitMat.getDiffuse().g * light->ambientIntensity + hit->hitMat.getDiffuse().g * _calcDiffuse(light, hit) + hit->hitMat.getSpecular().g * _calcSpec(light, hit, direction);
		toReturn.b = hit->hitMat.getDiffuse().b * light->ambientIntensity + hit->hitMat.getDiffuse().b * _calcDiffuse(light, hit) + hit->hitMat.getSpecular().b * _calcSpec(light, hit, direction);
		return toReturn;
	}

	/** \brief _trace - Uses ray start and direction to check for intersections with objects.
	* \details This function returns a 'bool' value and stores the hit data from the performed trace in
	* the 'hitData' variable as well as storing the index of the hit to eliminate future self shadowing issues.
	*
	* \param origin vec3
	* \param direction vec3
	* \param objs vector<Object*> List of all objects in the scene stored as pointers to their base class.
	*
	* \return bool
	*/
	bool _trace(vec3 origin, vec3 direction, vector<Object*> objs)
	{
		bool toRenturn = false;

		int maxDist = 10000000000;
		for (int i = 0; i < objs.size(); ++i)
		{
			//Basic depth test.
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
	/** \brief _shadowAmmount - Returns a number between 0.0 and 1.0 to outline the percentage of shadow the current pixel is in.
	*
	* \param index int Used to eliminate self shadowing.
	* \param hit vec3 Exact position of the hit for which to evaluate the shadow value. 
	* \param light Light* 
	* \param objs vector<Object*> List of all objects in the scene stored as pointers to their base class.
	*
	* \return float
	*/
	float _shadowAmmount(int index, vec3 hit, Light* light, vector<Object*> objs)
	{
		float fullShadow = pow(light->resolution, 2);
		float shadowHits = 0.0f;

		Intersection temp; //Used to avoid having to overload the isIntersecting() method
		//Iterates through the objects
		for (int k = 0; k < objs.size(); ++k)
		{
			for (int i = 0; i <= light->resolution; ++i)
			{
				for (int j = 0; j <= light->resolution; ++j)
				{
					//Calculates the precise ray direction to the light sub-target.
					vec3 currentLightTarget = vec3(light->lightPos.x + (light->posAndSize[1].x / light->resolution) * i,
						light->lightPos.y,
						light->lightPos.z + (light->posAndSize[1].z / light->resolution) * j);

					//Self shadow test
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

	/** \brief recursiveTrace - Returns the color of the pixel for which the method is called.
	*
	* \param origin vec3 Ray origin.
	* \param direction vec3 Ray direction.
	* \param objs vector<Object*> List of all objects in the scene stored as pointers to their base class.
	* \param light Light*
	* \param counter int Counter used to stop recursion
	*
	* \return vec3
	*/
	vec3 recursiveTrace(vec3 origin, vec3 direction, vector<Object*> objs, Light* light, int counter)
	{
		if (counter == 2)
		{
			return vec3(0.0, 0.0, 0.0);//mix(calcFinalColor(light, &hitMats.back(), direction), vec3(0.0, 0.0, 0.0), _shadowAmmount(index, hitMats.back().hitPos, light, objs));
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

			vec3 reflectedDirection = direction - 2 * dot(direction, hitMats[counter].hitNormal) * hitMats[counter].hitNormal;
			vec3 reflectedOrigin = hitMats[counter].hitPos + hitMats[counter].hitNormal * float(1e-4);

			counter++;
			return mix(calcFinalColor(light, &hitMats[counter-1], direction), vec3(0.0, 0.0, 0.0), _shadowAmmount(index, hitMats[counter-1].hitPos, light, objs)) + hitMats[counter - 1].hitMat.getShine() * recursiveTrace(reflectedOrigin, reflectedDirection, objs, light, counter);
		}
	}
}
