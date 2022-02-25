#include <vector>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <time.h>
#include <string>
#include <random>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "omp.h"
#include "shaders.h"
#include "raytracer.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glut32.lib")

using namespace std;

//shader program ID
GLuint shaderProgram;

//the main window size
GLint wWindow = 800;
GLint hWindow = 800;

GLuint quadVao = -1;
GLuint frameTex = -1;
GLubyte* texData = 0;

RayTracer raytracer;
const std::string sceneFile = "cornell.txt";

bool shouldRedisplay = false;
bool shouldExit = false;

/*********************************
Some OpenGL-related functions
**********************************/

//called when a window is reshaped
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
	//remember the settings for the camera
	//wWindow = w;
	//hWindow = h;
}

void Idle(void)
{
	if (shouldRedisplay)
	{
		//cout << "new frame" << endl;
		glBindTexture(GL_TEXTURE_2D, frameTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wWindow, hWindow, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		glBindTexture(GL_TEXTURE_2D, 0);
		glutPostRedisplay();
		shouldRedisplay = false;
	}
}

//the main rendering function
void Display(void)
{
	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameTex);
	int fbo_tex_loc = glGetUniformLocation(shaderProgram, "tex");
	glUniform1i(fbo_tex_loc, 0); // we bound our texture to texture unit 1
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // draw quad
	glutSwapBuffers();
}

//keyboard callback
void Kbd(unsigned char a, int x, int y)
{
	switch(a)
	{
	  case 27 : exit(0);break;
	  case ' ':
	  {
		  break;
	  }
	}
	glutPostRedisplay();
}

//special keyboard callback
void SpecKbdPress(int a, int x, int y)
{
	switch (a)
	{
	case GLUT_KEY_LEFT:
	{
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		break;
	}
	case GLUT_KEY_DOWN:
	{
		break;
	}
	case GLUT_KEY_UP:
	{
		break;
	}
	}
	glutPostRedisplay();
}

//called when a special key is released
void SpecKbdRelease(int a, int x, int y)
{
	switch (a)
	{
	case GLUT_KEY_LEFT:
	{
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		break;
	}
	case GLUT_KEY_DOWN:
	{
		break;
	}
	case GLUT_KEY_UP:
	{
		break;
	}
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	//cout << "Location is " << "[" << x << "'" << y << "]" << endl;
}

void InitializeProgram(GLuint* program)
{
	std::vector<GLuint> shaderList;

	//load and compile shaders 	
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, LoadShader("shaders/phong.vert")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("shaders/phong.frag")));

	//create the shader program and attach the shaders to it
	*program = CreateProgram(shaderList);

	//delete shaders (they are on the GPU now)
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	glGenVertexArrays(1, &quadVao);
}

void OnExit()
{
	shouldExit = true;
	//_sleep(1 * 1000);
	if (texData)
	{
		delete texData;
		texData = 0;
	}
}

void InitializeGL(int argc, char** argv)
{
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("GL Output");
	GLenum err = glewInit();
	if (GLEW_OK != err)
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Kbd);
	glutSpecialUpFunc(SpecKbdRelease); //smooth motion
	glutSpecialFunc(SpecKbdPress);
	atexit(OnExit);
	InitializeProgram(&shaderProgram);
}

void InitializeRayTracer()
{
	raytracer.LoadScene(sceneFile);
	glm::ivec2 res = raytracer.GetResolution();
	wWindow = res.x;
	hWindow = res.y;
}

void InitializeFrame()
{
	glGenTextures(1, &frameTex);
	glBindTexture(GL_TEXTURE_2D, frameTex);
	texData = new GLubyte[wWindow * hWindow * 3];
	for (int i = 0; i < wWindow * hWindow * 3; i++)
		texData[i] = 0;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wWindow, hWindow, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	raytracer.SetOutImage(texData);
}

void RTRenderLoop()
{
	while (!shouldExit)
	{
		raytracer.RenderFrame();
		//_sleep(1 * 1000);
		shouldRedisplay = true;
	}
}

int main(int argc, char** argv)
{
	InitializeRayTracer();
	InitializeGL(argc, argv);
	InitializeFrame();

	omp_set_nested(1);
	#pragma omp parallel sections num_threads(2)
	{
		#pragma omp section
		glutMainLoop();
		#pragma omp section
		RTRenderLoop();
	}

	return 0;
}
