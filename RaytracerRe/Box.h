#pragma once
#include <glm.hpp>

#include "Object.h"
#include "Material.h"

using namespace glm;

class Box : public Object
{
private:
	float _epsilon = 0.00f;
	vec3 _posAndSize[2];
	vec3 _position;

	Material _mat;
public:
	Box() {}
	Box(vec3 coords[])
	{
		for (int i = 0; i < 2; ++i)
		{
			_posAndSize[i] = coords[i];
		}
		_position = _posAndSize[0];
	}

	bool isIntersection(vec3 rayOrigin, vec3 rayDirection, Intersection* hit)
	{
		float Ts[6];
		if (rayDirection.x)
		{
			Ts[0] = _posAndSize[0].x - rayOrigin.x / rayDirection.x;
			Ts[1] = (_posAndSize[0] + _posAndSize[1]).x - rayOrigin.x / rayDirection.x;
		}
		if (rayDirection.y)
		{
			Ts[2] = _posAndSize[0].y - rayOrigin.y / rayDirection.y;
			Ts[3] = (_posAndSize[0] + _posAndSize[1]).y - rayOrigin.y / rayDirection.y;
		}
		if (rayDirection.z)
		{
			Ts[4] = _posAndSize[0].x - rayOrigin.z / rayDirection.z;
			Ts[5] = (_posAndSize[0] + _posAndSize[1]).z - rayOrigin.z / rayDirection.z;
		}

		vec3 Ps[6];
		for (int i = 0; i < 6; ++i)
		{
			Ps[i] = rayOrigin + rayDirection * Ts[i];
		}

		for (int i = 0; i < 6; ++i)
		{
			if (Ps[i].x > _posAndSize[0].x - _epsilon && Ps[i].x < _posAndSize[0].x + _posAndSize[1].x + _epsilon &&
				Ps[i].y > _posAndSize[0].y - _epsilon && Ps[i].y < _posAndSize[0].y + _posAndSize[1].y + _epsilon &&
				Ps[i].x > _posAndSize[0].z - _epsilon && Ps[i].z < _posAndSize[0].z + _posAndSize[1].z + _epsilon)
				
				
				return true;
		}

		return false;
	}
};
