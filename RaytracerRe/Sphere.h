#pragma once
#include <glm.hpp>

#include "Object.h"
#include "Material.h"

using namespace glm;

/** \class Sphere
* \brief Extends the Object data type and is used to describe an infinite plane
*/
class Sphere : public Object
{
private:
	double _radius;
	vec3 _position;

	Material _mat;
public:
	Sphere() {}

	Sphere(double rad, vec3 pos, vec3 dCol, vec3 sCol, float shine) : _radius(rad), _position(pos)
	{
		_mat = Material(dCol, sCol, shine);
	}
	~Sphere() {}

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
		vec3 posToOrigin = _position - origin;

		float cpa = dot(posToOrigin, direction);

		if (cpa < 0)
			return false;
		
		float cpaDist = dot(posToOrigin, posToOrigin) - pow(cpa, 2);

		if (cpaDist > pow(_radius, 2))
			return false;

		float thc = sqrt(pow(_radius, 2) - cpaDist);

		*hit = Intersection(cpa - thc,
							(origin + direction * (cpa - thc)) - _position,
							origin,
							direction,
							_mat);

		return true;
	}

	Material* getMat()
	{
		return &_mat;
	}
};