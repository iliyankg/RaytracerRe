#pragma once
#include <glm.hpp>

#include "Intersection.h"

using namespace glm;

class Object
{
protected:
	int _id;

public:
	virtual ~Object() {};

	virtual bool isIntersecting(vec3 origin, vec3 direction, Intersection* hit) = 0;

	int getId()
	{
		return _id;
	}
};