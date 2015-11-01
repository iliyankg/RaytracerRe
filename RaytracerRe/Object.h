#pragma once
#include <glm.hpp>

#include "Intersection.h"

using namespace glm;

/** \class Object
* \brief A base class from which all other objects in the scene are derived.
*/
class Object
{
protected:
	int _id;

public:
	virtual ~Object() {};

	/** \brief isIntersecting - A pure virtual method invoked when checking intersections.
	* \param origin vec3 Origin of the fired ray.
	* \param direction vec3 Direction of the fired ray.
	* \param hit Intersection* In the event of a hit is populated with the relevant information.
	*
	* \return bool
	*/
	virtual bool isIntersecting(vec3 origin, vec3 direction, Intersection* hit) = 0;

	int getId()
	{
		return _id;
	}
};