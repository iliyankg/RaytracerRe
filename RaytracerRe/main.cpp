#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm.hpp>
#include <vector>

#include <time.h>
#include <iostream>

#include "Raytracer.h"

#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"

using namespace std;
using namespace glm;

#define WIDTH 640
#define HEIGHT 480
#define FOV 0.523599

const double ASPECT_RATIO = double(WIDTH) / double(HEIGHT);

vec3 cameraPos(0.0, 0.0, 0.0);

GLuint texture;
float image[HEIGHT * WIDTH * 3];

Light* light;
vector<Object*> objects;

clock_t t;

void initialize();
void render();
void changeViewPort(int w, int h);

int main(int argc, char* argv[])
{
	//General GLUT setup
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Raytracer");
	glutDisplayFunc(render);
	glutReshapeFunc(changeViewPort);
	glewInit();


	initialize();


	glutMainLoop();
	return 0;
}

void initialize()
{
	glGenTextures(1, &texture);
	
	vec3 posAndSize[] =
	{
		vec3(-4.5, 20, -4.5),
		vec3(9.0, 0.1, 9.0)
	};
	light = new Light(posAndSize);

	vec3 coords[4];
	coords[0] = vec3(-5.0, -4.0, 30.0);
	coords[1] = vec3(-5.0, -4.0, -30.0);
	coords[2] = vec3(5.0, -4.0, 30.0);
	coords[3] = vec3(5.0, -4.0, -30.0);

	objects.push_back(new Plane(normalize(vec3(0.0f, 1.0f, 0.0f)), coords, vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), 10.0f));

	objects.push_back(new Sphere(1.0, vec3(6.0, -2.0, -32), vec3(1.0, 0.32, 0.36), vec3(1.0, 1.0, 1.0), 5.0f));
	objects.push_back(new Sphere(1.0, vec3(7.0, -2.0, -30), vec3(0.9, 0.76, 0.46), vec3(1.0, 1.0, 1.0), 5.0f));
	objects.push_back(new Sphere(1.0, vec3(5.0, -2.0, -34), vec3(0.65, 0.77, 0.97), vec3(1.0, 1.0, 1.0), 5.0f));
	
	objects.push_back(new Sphere(1.0, vec3(6.0, 0.5, -32), vec3(0.9, 0.7, 0.2), vec3(1.0, 1.0, 1.0), 5.0f));
	objects.push_back(new Sphere(1.0, vec3(7.0, 0.5, -30), vec3(0.3, 0.5, 0.9), vec3(1.0, 1.0, 1.0), 5.0f));
	objects.push_back(new Sphere(1.0, vec3(5.0, 0.5, -34), vec3(0.9, 0.9, 0.9), vec3(1.0, 1.0, 1.0), 5.0f));

	objects.push_back(new Sphere(1.0, vec3(6.0, 3.0, -32), vec3(0.9, 0.7, 0.2), vec3(1.0, 1.0, 1.0), 5.0f));
	objects.push_back(new Sphere(1.0, vec3(7.0, 3.0, -30), vec3(0.1, 0.6, 0.9), vec3(1.0, 1.0, 1.0), 5.0f));
	objects.push_back(new Sphere(1.0, vec3(5.0, 3.0, -34), vec3(0.1, 0.9, 0.2), vec3(1.0, 1.0, 1.0), 5.0f));

	vec3 coordsTwo[3];
	coordsTwo[0] = vec3(-6.0, 2.0, -32.0);
	coordsTwo[1] = vec3(-5.0, -3.0, -30.0);
	coordsTwo[2] = vec3(-5.0, -3.0, -34.0);
	objects.push_back(new Triangle(coordsTwo, vec3(1.0f, 0.0f, 0.0), vec3(1.0, 1.0, 1.0), 10.0f));

	coordsTwo[0] = vec3(-6.0, 2.0, -32.0);
	coordsTwo[1] = vec3(-5.0, -3.0, -30.0);
	coordsTwo[2] = vec3(-9.0, -3.0, -30.0);
	objects.push_back(new Triangle(coordsTwo, vec3(1.0f, 0.0f, 0.0), vec3(1.0, 1.0, 1.0), 10.0f));

	coordsTwo[0] = vec3(-6.0, 2.0, -32.0);
	coordsTwo[1] = vec3(-5.0, -3.0, -34.0);
	coordsTwo[2] = vec3(-9.0, -3.0, -34.0);
	objects.push_back(new Triangle(coordsTwo, vec3(1.0f, 0.0f, 0.0), vec3(1.0, 1.0, 1.0), 10.0f));

	coordsTwo[0] = vec3(-6.0, 2.0, -32.0);
	coordsTwo[1] = vec3(-9.0, -3.0, -30.0);
	coordsTwo[2] = vec3(-9.0, -3.0, -34.0);
	objects.push_back(new Triangle(coordsTwo, vec3(1.0f, 0.0f, 0.0), vec3(1.0, 1.0, 1.0), 10.0f));

	//Sets up the plane and texture we will render to.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);

	//Binds the texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	t = clock();

	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int j = 0; j < WIDTH; ++j)
		{
			double pixelNormalizedx = (double(j) + 0.5) / double(WIDTH);
			double pixelNormalizedy = (double(i) + 0.5) / double(HEIGHT);

			double pixelRemappedx = (2.0 * pixelNormalizedx - 1.0) * ASPECT_RATIO;
			double pixelRemappedy = 1.0 - 2.0 * pixelNormalizedy;

			double pixelCamerax = pixelRemappedx * glm::tan(0.523599 / 2.0);
			double pixelCameray = pixelRemappedy * glm::tan(0.523599 / 2.0);

			vec3 point(pixelCamerax, pixelCameray, -1);
			vec3 rayDirection = point - cameraPos;
			rayDirection = glm::normalize(rayDirection);

			Raytracer::hitMats.clear();
			if (Raytracer::_trace(cameraPos, rayDirection, objects))
			{
				int a = 5;
			}
			vec3 finalColor = Raytracer::recursiveTrace(cameraPos, rayDirection, objects, light, 1);

			image[(i * WIDTH + j) * 3] = finalColor.r;
			image[(i * WIDTH + j) * 3 + 1] = finalColor.g;
			image[(i * WIDTH + j) * 3 + 2] = finalColor.b;
		}
	}

	//Binds the image pixel array to the active texture buffer object.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, image);

	//Renders the quad with thetexture
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3f(0, glutGet(GLUT_WINDOW_HEIGHT), 0);
	glTexCoord2f(0, 0); glVertex3f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
	glTexCoord2f(0, 1); glVertex3f(glutGet(GLUT_WINDOW_WIDTH), 0, 0);
	glEnd();

	t = clock() - t;
	std::cout << "Frame took: " << (float)t / CLOCKS_PER_SEC << " seconds to render." << std::endl;

	glutSwapBuffers();
}
void changeViewPort(int w, int h)
{
	glViewport(0, 0, w, h);
}