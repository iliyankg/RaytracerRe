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
	/**\brief readFile - Basic file loader; 
	* \details	Nexcius.net, (2012). How to load a GLSL shader in OpenGL using C++. [online]
	*			Available at: http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/ [Accessed 1 Nov. 2015].
	* \param filePath const char* 
	* \return string
	*/
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
	
	/** \brief nextPowerOfTwo - Finds the nearest power of two of the supplied number.
	* \details	Used in the compute shader invocation, the ammount of times the shader is called must be a power of two, 
	*			where the framebuffer size does not.
	*			
	*			Burjack, K. (2015). 2.6.1. Ray tracing with OpenGL Compute Shaders (Part I). [online] GitHub.
	*			Available at: https://github.com/LWJGL/lwjgl3-wiki/wiki/2.6.1.-Ray-tracing-with-OpenGL-Compute-Shaders-(Part-I) [Accessed 1 Nov. 2015].
	* 
	* \param x int
	* \return int
	*/
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

	/**\brief shaderCompileTest - Prints compilation errors (line and row numbers) for the given shader.
	* \details	Nexcius.net, (2012). How to load a GLSL shader in OpenGL using C++. [online]
	*			Available at: http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/ [Accessed 1 Nov. 2015].
	* \param shader GLuint 
	* \return void
	*/
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

	GLuint vao; // Vertex Array Object (VAO)
	GLuint vbo; // Vertex Buffer Object (VBO)
	GLuint texture; // Texture to be used on the final screen-size quad
	GLuint quadProgram; // Basic vertex - fragment shader pipeline

	GLint workGroupSizeX; // Used when invoking the compute shader
	GLint workGroupSizeY; // Used when invoking the compute shader
	GLuint tracerProgram; // Holds the compiled compute shader.

	/** \brief createQuadBuffers - Defines and binds the VAO and VBO used later to render the screen size quad.
	* \return void
	*/
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
	/** \brief createQuadProgram - Creates the program used to render the raytraced image.
	* \return void
	*/
	void createQuadProgram()
	{
		quadProgram = glCreateProgram();

		GLuint vertex = glCreateShader(GL_VERTEX_SHADER); //Defines temporary vertex shader
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER); //Defines temporary fragment shader
		//Loads the source code for the shaders.
		std::string vertStr = readFile("vertex.glsl");
		std::string fragStr = readFile("fragment.glsl");
		const char* vertSource = vertStr.c_str();
		const char* fragSource = fragStr.c_str();

		//Compiles the vertex shader
		glShaderSource(vertex, 1, &vertSource, NULL);
		glCompileShader(vertex);
		//Test for vertex shader compilation errors
		std::cout << "VERTEX::" << std::endl;
		shaderCompileTest(vertex);
		//Compiles fragment shader
		glShaderSource(fragment, 1, &fragSource, NULL);
		glCompileShader(fragment);
		//Test for fragment shader compilation errors
		std::cout << "FRAGMENT::" << std::endl;
		shaderCompileTest(vertex);
		//Attaches the vertex and fragment shader to the program and links them
		glAttachShader(quadProgram, vertex);
		glAttachShader(quadProgram, fragment);
		glLinkProgram(quadProgram);
		//Garbage collection
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	/** \brief createTeture - Binds the 'GLuint texture' variables as a GL_TEXTURE_2D, sets basic default settings and makes it currently active
	* \return void
	*/
	void createTexture()
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 640, 480, 0, GL_RGBA, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	/** \brief createTracerProgram - Creates the program used to performe the raytracing.
	* \return void
	*/
	void createTracerProgram()
	{
		tracerProgram = glCreateProgram(); 
		GLuint compute = glCreateShader(GL_COMPUTE_SHADER); //Compute shader available in OpenGL 4.3 and higher

		//Loads the source code
		std::string computeStr = readFile("compute.glsl");
		const char* computeSource = computeStr.c_str();
		//Binds the source and compiles it
		glShaderSource(compute, 1, &computeSource, NULL);
		glCompileShader(compute);
		//Checks for compilation errors
		std::cout << "COMPUTE::" << std::endl;
		shaderCompileTest(compute);
		//Attaches the compiled shader to the program and links it
		glAttachShader(tracerProgram, compute);
		glLinkProgram(tracerProgram);
	}
	/** \brief initQuadProg - Sets the 'tex' parameter as an input used to bind the texture to the fragment shader.
	* \return void
	*/
	void initQuadProg()
	{
		glUseProgram(quadProgram); //Sets 'quadProgram' as active one
		GLint texUniform = glGetUniformLocation(quadProgram, "tex"); //Gets the pointer to the location of the input defined by "tex"
		glUniform1i(texUniform, 0); //Sets it as the 0th uniform integer input.
		glUseProgram(0); //Stops 'quadProgram' being the current active one.
	}
	/** \brief createTracerProgram - Gets the the work group size used to run the compute shader
	* in the appropriate dimension.
	* \return void
	*/
	void initRaytracer()
	{
		glUseProgram(tracerProgram); //Sets 'tracerProgram' as current active.
		GLint workGroupSize[3]; //Temporary array used to store the pointers
		glGetProgramiv(tracerProgram, GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize); 
		workGroupSizeX = workGroupSize[0];
		workGroupSizeY = workGroupSize[1];
		glUseProgram(0); //Stops 'traceProgram' being the current active one.
	}

	/** \brief trace - Runs the entire program from raytracing to rendering the textured quad.
	* \details Swapping between the thwo shader programs binding and un-binding the 'texture' variaiable 
	* where needed. 
	* \return void
	*/
	void trace()
	{
		//COMPUTE SHADER
		glUseProgram(tracerProgram); //Sets 'tracerProgram' as the current active shader program

		glBindImageTexture(0, texture, 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F); //Binds texture to the 0th binding in the compute shader.
		//Sets the workgroup size.
		GLint worksizeX = nextPowerOfTwo(640);
		GLint worksizeY = nextPowerOfTwo(480);
		//Runs the compute shader in the appropriate dimension 
		glDispatchCompute(worksizeX / workGroupSizeX, worksizeY / workGroupSizeY, 1);
		
		glBindImageTexture(0, 0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);//Unbinds 'texture'
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//Blocks altering the image any further
		glUseProgram(0); //Stops 'tracerProgram' from being used.

		//QUAD SHADER
		glUseProgram(quadProgram); //Sets 'quadProgram' as curently active shader program 
		glBindVertexArray(vao); //Binds the VAO to the program
		glBindTexture(GL_TEXTURE_2D, texture); //Binds the texture to the program
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draws triangles based on the VAO/VBO vertices
		glBindTexture(GL_TEXTURE_2D, 0); //Unbinds the texture
		glBindVertexArray(0); //Unbinds the VAO 
		glUseProgram(0); //Stops 'quadProgram' from being used

	}
}
