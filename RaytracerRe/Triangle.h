#pragma once
#include <glm.hpp>

#include "Object.h"
#include "Material.h"

using namespace glm;

class Triangle :public Object
{
private:
	vec3 _coordinates[3];
	vec3 _normal;

	Material _objMat;
public:
	Triangle() {}

	Triangle(vec3 coords[], vec3 dCol, vec3 sCol, float shine)
	{
		_id = 2;

		for (int i = 0; i < 3; ++i)
		{
			_coordinates[i] = coords[i];
		}

		_normal = normalize(cross(_coordinates[0] - _coordinates[1], _coordinates[0] - _coordinates[2]));

		_objMat = Material(dCol, sCol, shine);
	}

	~Triangle() {}

	bool isIntersecting(vec3 rayOrigin, vec3 rayDirection, Intersection* hit)
	{
		vec3 one = _coordinates[1] - _coordinates[0];
		vec3 two = _coordinates[2] - _coordinates[0];

		double u = dot((rayOrigin - _coordinates[0]), cross(rayDirection, two)) / dot(one, cross(rayDirection, two));
		double v = dot(rayDirection, cross((rayOrigin - _coordinates[0]), one)) / dot(one, cross(rayDirection, two));

		if (u < 0 || u > 1)
		{
			return false;
		}
		else if (v < 0 || u + v > 1)
		{
			return false;
		}
		else
		{
			double t = dot(two, cross((rayOrigin - _coordinates[0]), one)) / dot(one, cross(rayDirection, two));

			if (t >= 0.0f)
			{
				*hit = Intersection(t, _normal, rayOrigin, rayDirection, _objMat);
				return true;
			}
		}
		return false;
	}
};