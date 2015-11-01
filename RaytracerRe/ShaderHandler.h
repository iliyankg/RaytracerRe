#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm.hpp>
#include <vector>

#include <fstream>
#include <string>

#include <iostream>

//Namespace used to seperate the GPU implementation from other sides of the program.
namespace Shaderhandler
{
	std::string readFile(const char* filePath)
	{
		std::string data;
		std::ifstream file(filePath, std::ios::in);

		//Error handle for not open file
		if (!file.is_open())
		{
			std::cout << "FILE " << filePath << " NOT OPEN" << std::endl;
			return "";
		}

		//Reads each line and appends it to the string to be returned
		std::string line = "";
		while (!file.eof())
		{
			std::getline(file, line);
			data.append(line + "\n");
		}

		file.close();
		return data;
	}
	
	int nextPowerOfTwo(int x)
	{
		x--;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		x++;
		return x;
	}

	void shaderCompileTest(GLuint shader)
	{
		GLint result = GL_FALSE;
		int logLength;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
		std::cout << &vertShaderError[0] << std::endl;
	}

	GLuint vao;
	GLuint vbo;
	GLuint texture;
	GLuint quadProgram;

	GLint workGroupSizeX;
	GLint workGroupSizeY;
	GLuint tracerProgram;

	void createQuadBuffers()
	{
		//Creates the vertex array and vertex buffer objects 
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		//Makes the active
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//Vertices for the screen quad in screen space. 
		GLfloat screenQuad[] =
		{
			-1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,

			1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f
		};

		//Binds the screen quad to the buffer objects. 
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad), screenQuad, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	}

	void createQuadProgram()
	{
		quadProgram = glCreateProgram();

		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vertStr = readFile("vertex.glsl");
		std::string fragStr = readFile("fragment.glsl");
		const char* vertSource = vertStr.c_str();
		const char* fragSource = fragStr.c_str();

		glShaderSource(vertex, 1, &vertSource, NULL);
		glCompileShader(vertex);

		std::cout << "VERTEX::" << std::endl;
		shaderCompileTest(vertex);

		glShaderSource(fragment, 1, &fragSource, NULL);
		glCompileShader(fragment);

		std::cout << "FRAGMENT::" << std::endl;
		shaderCompileTest(vertex);

		glAttachShader(quadProgram, vertex);
		glAttachShader(quadProgram, fragment);
		glLinkProgram(quadProgram);

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	void createTexture()
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 640, 480, 0, GL_RGBA, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void createTracerProgram()
	{
		tracerProgram = glCreateProgram();
		GLuint compute = glCreateShader(GL_COMPUTE_SHADER);

		std::string computeStr = readFile("compute.glsl");
		const char* computeSource = computeStr.c_str();


		glShaderSource(compute, 1, &computeSource, NULL);
		glCompileShader(compute);

		std::cout << "COMPUTE::" << std::endl;
		shaderCompileTest(compute);

		glAttachShader(tracerProgram, compute);
		glLinkProgram(tracerProgram);

	}

	void initQuadProg()
	{

		glUseProgram(quadProgram);
		GLint texUniform = glGetUniformLocation(quadProgram, "tex");
		glUniform1i(texUniform, 0);
		glUseProgram(0);
	}
	void initRaytracer()
	{

		glUseProgram(tracerProgram);
		GLint workGroupSize[3];
		glGetProgramiv(tracerProgram, GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
		workGroupSizeX = workGroupSize[0];
		workGroupSizeY = workGroupSize[1];

		glUseProgram(0);
	}

	void trace()
	{
		glUseProgram(tracerProgram);

		glBindImageTexture(0, texture, 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);

		GLint worksizeX = nextPowerOfTwo(640);
		GLint worksizeY = nextPowerOfTwo(480);

		glDispatchCompute(worksizeX / workGroupSizeX, worksizeY / workGroupSizeY, 1);

		glBindImageTexture(0, 0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glUseProgram(0);

		glUseProgram(quadProgram);
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);

	}
}
