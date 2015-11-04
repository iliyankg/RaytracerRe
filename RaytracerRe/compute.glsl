#version 430 core

vec3 cameraPos = vec3(0.0, 0.0, 0.0);
float fovAngle = 0.523599;

struct Material
{
	vec3 _diffuseColor;
	vec3 _specColor;

	float _shine;
};

struct Light
{
	float _intensity;
	float _specIntensity;
	float _ambientIntensity;
	vec3 _lightPos;
};

struct Intersection
{
	bool _isHit;

	vec3 _closeHitPos;
	vec3 _farHitPos;

	float _closeHitDistance;
	float _farHitDistance;

	vec3 _closeHitNormal;
	vec3 _farHitNormal;
};

struct Sphere
{
	float _radius;
	vec3 _position;
};

struct Plane
{
	vec3 _vertOne;
	vec3 _vertTwo;
	vec3 _vertThree;
	vec3 _vertFour;

	vec3 _normal;
};

//Array of matterials whos indecies match the order of created objects
const Material mats[] =
{
	{vec3(1.0, 0.32, 0.36), vec3(1.0, 1.0, 1.0), 10.0},
	{vec3(0.9, 0.76, 0.46), vec3(1.0, 1.0, 1.0), 5.0},
	{vec3(0.65, 0.77, 0.97), vec3(1.0, 1.0, 1.0), 5.0},
	{vec3(0.9, 0.9, 0.9), vec3(1.0, 1.0, 1.0), 5.0},
	{vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), 5.0}
};

//Creates the spheres
int NUM_SPHERES = 4;
const Sphere spheres[] =
{
	{10.0, vec3(0.0, 0.0, -40)},
	{2.0, vec3(5.0, -1.0, -30)},
	{3.0, vec3(5.0, 0.0, -50)},
	{3.0, vec3(-5.5, 0.0, -30)}
};
int NUM_PLANES = 1;
const Plane planes[] =
{
	{vec3(-5.0, -4.0, 30.0), vec3(-5.0, -4.0, -30.0), vec3(5.0, -4.0, -30.0), vec3(5.0, -4.0, -30.0), vec3(0.0, 1.0, 0.0)},
	{vec3(-5.0, -4.0, 30.0), vec3(-5.0, -4.0, -30.0), vec3(5.0, -4.0, -30.0), vec3(5.0, -4.0, -30.0), vec3(0.0, 1.0, 0.0)}
};

//Calculates diffuse
float calcDiffuse(Light localLight, Intersection hit)
{
	return localLight._intensity * max(0, dot(normalize(localLight._lightPos - hit._closeHitPos), hit._closeHitNormal));
}

//Calculates specular
float calcSpec(Light light, Intersection hit, vec3 rayDirection, float shine)
{
	vec3 hitToCamDirection = -rayDirection;
	vec3 hitToLightDirection = normalize(light._lightPos - hit._closeHitPos);
	vec3 reflected = -hitToLightDirection - 2 * dot( -hitToLightDirection, hit._closeHitNormal) * hit._closeHitNormal;
	return light._specIntensity * pow(max(0, dot(reflected, hitToCamDirection)), shine);
}

//Checks for intersection with a specific sphere
bool sphereIntersection(vec3 rayGin, vec3 rayDir, const Sphere target, out Intersection hit)
{
	vec3 L = target._position - rayGin;

	float Tca = dot(L, rayDir);
	if(Tca < 0.0)
	{
		return false;
	}
	else
	{
		float S = dot(L,L) - pow(Tca, 2.0);

		if(pow(S, 2) > pow(target._radius,2))
		{ 
			return false;
		}
		else
		{
			float Thc = sqrt(pow(target._radius, 2.0) - S);

			hit._isHit = true;
			hit._closeHitDistance = Tca - Thc;
			hit._farHitDistance = Tca + Thc;
			
			hit._closeHitNormal = normalize((rayGin + rayDir * (Tca - Thc)) - target._position);
			hit._farHitNormal = normalize((rayGin + rayDir *(Tca + Thc)) - target._position);
			
			hit._closeHitPos = rayGin + rayDir * hit._closeHitDistance;
			hit._farHitPos = rayGin + rayDir * hit._farHitDistance;
			return true;
		}		
	}
}

//Checks for intersection with a specific plane
bool planeIntersection(vec3 rayGin, vec3 rayDir, const Plane target, out Intersection hit)
{
	float devideBy = dot(rayDir, target._normal);

	if(devideBy == 0.0f)
	{
		return false;
	}
	else
	{
		float distanceHit = dot((target._vertOne - rayGin), target._normal) / devideBy;

		if(distanceHit >= 0.0)
		{
			hit._isHit = true;
			hit._closeHitDistance = distanceHit;
			hit._farHitDistance = distanceHit;
			
			hit._closeHitNormal = target._normal;
			hit._farHitNormal = target._normal;
			
			hit._closeHitPos = rayGin + rayDir * hit._closeHitDistance;
			hit._farHitPos = rayGin + rayDir * hit._farHitDistance;
			return true;
		}
		return false;
	}
}

//External binding for the image to be rendered to
layout(binding = 0, rgba32f) uniform image2D framebuffer;

//Initial work group sizes
layout(local_size_x = 16, local_size_y = 8) in;

void main(void)
{
	Light light;

	bool isIntersecting = false;
	bool inShadow = false;

	light._lightPos = vec3(0.0, 15.0, -10.0);
	light._ambientIntensity = 0.2f;
	light._intensity = 0.5f;
	light._specIntensity = 1.0f;

	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(framebuffer);

	float pixelNormalizedx = (pix.x + 0.5) / size.x;
	float pixelNormalizedy = (pix.y + 0.5) / size.y;

	float aspectRatio = size.x / size.y;
	float pixelRemappedx = (2 * pixelNormalizedx - 1) * aspectRatio;
	float pixelRemappedy = 2 * pixelNormalizedy - 1;

	float pixelCamerax = pixelRemappedx * tan(fovAngle / 2);
	float pixelCameray = pixelRemappedy * tan(fovAngle / 2);

	vec3 point = vec3(pixelCamerax, pixelCameray, -1);
	vec3 rayDirection = normalize(point - cameraPos);

	float maxDist = 100000000000000.0;
	vec4 finalColor;
	
	Intersection hit;
	Intersection shadowHit;

	int hitId;

	//Due to limitations two for loops are needed to itterate through our objects. (one for each object type)
	//Sphere
	for(int i = 0; i < NUM_SPHERES; i++)
	{
		Intersection tempHit;

		sphereIntersection(cameraPos, rayDirection, spheres[i], tempHit);
		
		if(tempHit._isHit == true && tempHit._closeHitDistance < maxDist && tempHit._closeHitDistance > 0.0)
		{
			maxDist = hit._closeHitDistance;

			hit = tempHit;

			hitId = i;
		}
	}
	//Plane
	for(int i = 0; i < NUM_PLANES; i++)
	{
		Intersection tempHit;

		planeIntersection(cameraPos, rayDirection, planes[i], tempHit);

		if(tempHit._isHit == true && tempHit._closeHitDistance < maxDist && tempHit._closeHitDistance > 0.0)
		{
			maxDist = hit._closeHitDistance;

			hit = tempHit;

			hitId = i + NUM_SPHERES;
		}
	}

	//As with the primary ray the secondary shadow ray also requires two for loops for each object type.
	//Sphere
	for(int i = 0; i < NUM_SPHERES; i++)
	{
		Intersection shadowTempHit;
		if(hitId == i)
		{
		}
		else
		{
			sphereIntersection(hit._closeHitPos, normalize(light._lightPos - hit._closeHitPos), spheres[i], shadowTempHit);
			shadowHit = shadowTempHit;
		}
	}
	//Plane
	for(int i = 0; i < NUM_PLANES; i++)
	{
		Intersection shadowTempHit;

		if(hitId - NUM_SPHERES == i)
		{
		}
		else
		{
			planeIntersection(hit._closeHitPos, normalize(light._lightPos - hit._closeHitPos), planes[i], shadowTempHit);
			shadowHit = shadowTempHit;
		}
	}

	if(hit._isHit)
	{
		if(shadowHit._isHit)
		{
			//Shadow areas are set to the ambient color
			finalColor.r = mats[hitId]._diffuseColor.r * light._ambientIntensity;
			finalColor.g = mats[hitId]._diffuseColor.g * light._ambientIntensity;
			finalColor.b = mats[hitId]._diffuseColor.b * light._ambientIntensity;
		}
		else
		{
			//No shadow areas are set to the diffuse calculation based on the matterial array listed above. 
			//hitId is calculated on numbers of objects in the lits and is accumolative. (max value of hitId is spheres.lenght + planes.lenght)
			finalColor.r = mats[hitId]._diffuseColor.r * light._ambientIntensity + mats[hitId]._diffuseColor.r * calcDiffuse(light, hit) + mats[hitId]._diffuseColor.r * calcSpec(light, hit, rayDirection, mats[hitId]._shine);
			finalColor.g = mats[hitId]._diffuseColor.g * light._ambientIntensity + mats[hitId]._diffuseColor.g * calcDiffuse(light, hit) + mats[hitId]._diffuseColor.g * calcSpec(light, hit, rayDirection, mats[hitId]._shine);
			finalColor.b = mats[hitId]._diffuseColor.b * light._ambientIntensity + mats[hitId]._diffuseColor.b * calcDiffuse(light, hit) + mats[hitId]._diffuseColor.b * calcSpec(light, hit, rayDirection, mats[hitId]._shine);
		}
	}
	else
	{
		//No hit is set to black
		finalColor.r = 0.0;
		finalColor.g = 0.0;
		finalColor.b = 0.0;
	}
	

	vec4 color = vec4(finalColor);

	//vec4 color = vec4(1.0, 1.0, 0.0, 1.0);
	
	//The pixel just calculated is set with the color variable onto the output image (framebuffer)
	imageStore(framebuffer, pix, color);
}