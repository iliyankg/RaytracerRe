#pragma once
#include <glm.hpp>

using namespace glm;

class Light
{
private:
	float _epsilon = 0.001f;

public:
	vec3 posAndSize[2];
	vec3 lightPos;
	int resolution = 4;
	float ambientIntensity = 0.2f;
	float intensity = 0.5f;
	float specIntensity = 1.0f;


	Light() {}

	Light(vec3 coords[])
	{
		for (int i = 0; i < 2; ++i)
		{
			posAndSize[i] = coords[i];
		}
		lightPos = posAndSize[0];
	}
	~Light() {}

	bool intersection(vec3 rayOrigin, vec3 rayDirection)
	{
		float Ts[6];
		if (rayDirection.x)
		{
			Ts[0] = posAndSize[0].x - rayOrigin.x / rayDirection.x;
			Ts[1] = (posAndSize[0] + posAndSize[1]).x - rayOrigin.x / rayDirection.x;
		}
		if (rayDirection.y)
		{
			Ts[2] = posAndSize[0].y - rayOrigin.y / rayDirection.y;
			Ts[3] = (posAndSize[0] + posAndSize[1]).y - rayOrigin.y / rayDirection.y;
		}
		if (rayDirection.z)
		{
			Ts[4] = posAndSize[0].x - rayOrigin.z / rayDirection.z;
			Ts[5] = (posAndSize[0] + posAndSize[1]).z - rayOrigin.z / rayDirection.z;
		}

		vec3 Ps[6];
		for (int i = 0; i < 6; ++i)
		{
			Ps[i] = rayOrigin + rayDirection * Ts[i];
		}

		for (int i = 0; i < 6; ++i)
		{
			if (Ps[i].x > posAndSize[0].x - _epsilon && Ps[i].x < posAndSize[0].x + posAndSize[1].x + _epsilon &&
				Ps[i].y > posAndSize[0].y - _epsilon && Ps[i].y < posAndSize[0].y + posAndSize[1].y + _epsilon &&
				Ps[i].x > posAndSize[0].z - _epsilon && Ps[i].z < posAndSize[0].z + posAndSize[1].z + _epsilon)
				return true;
		}

		return false;
	}
};
