#pragma once
#include <glm.hpp>

#include "Object.h"
#include "Material.h"

using namespace glm;

/** \class Plane
* \brief Extends the Object data type and is used to describe an infinite plane
*/
class Plane : public Object
{
private:
	vec3 _coordinates[4];
	vec3 _normal;

	Material _objMat;
public:
	Plane() {}

	Plane(vec3 normal, vec3 coords[], vec3 dCol, vec3 sCol, float shine): _normal(normal)
	{
		for (int i = 0; i < 4; ++i)
		{
			_coordinates[i] = coords[i];
		}

		_objMat = Material(dCol, sCol, shine);
	}

	~Plane() {}
	/** \brief isIntersecting - An override of the Object::isIntersecting method
	* \details Based on the lecture slides and task sheets.
	*
	* \param origin vec3 Origin of the fired ray.
	* \param direction vec3 Direction of the fired ray.
	* \param hit Intersection* In the event of a hit is populated with the relevant information.
	*
	* \return bool
	*/
	bool isIntersecting(vec3 origin, vec3 direction, Intersection* hit)
	{
		float devideBy = dot(direction, _normal);

		if (devideBy != 0)
		{
			float distanceHit = dot((_coordinates[0] - origin), _normal) / devideBy;

			if (distanceHit >= 0.0f)
			{
				*hit = Intersection(distanceHit, _normal, origin, direction, _objMat);
				return true;
			}
			return false;
		}
		return false;
	}
};
