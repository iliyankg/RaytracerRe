#pragma once
#include "glm.hpp"

using namespace glm;

/** \class Material
* \brief A datatype used to describe the visual properties of an object
*/
class Material
{
private:
	vec3 _diffuse;
	vec3 _specular;
	
	float _shine;

public:
	Material()
	{
		_diffuse = vec3(0.0, 0.0, 0.0);
		_specular = vec3(0.0, 0.0, 0.0);
		_shine = 0.0f;
	}
	Material(vec3 dCol, vec3 sCol, float shine) : _diffuse(dCol), _specular(sCol), _shine(shine) {}
	~Material() {}
	//Getters
	vec3 getDiffuse()
	{
		return _diffuse;
	}
	vec3 getSpecular()
	{
		return _specular;
	}
	float getShine()
	{
		return _shine;
	}
};