#ifndef CANVAS_H
#define CANVAS_H
#include<iostream>

#define GLEW_STATIC
#include<GL\glew.h>
#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include"Solver.h"

using namespace glm;

class Canvas
{
public:

	void init(int width, int height, Solver* solver);
	void render();
	void Draw2D();
	void DrawGrid2D();
	void DrawDensity2D();
	void DrawVelocity2D();

	int _width;
	int _height;
	GLFWwindow * _window;
	Solver* _solver;

	Canvas() {}

	
};

#endif // !CANVAS_H

