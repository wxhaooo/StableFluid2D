#include"stdafx.h"
#include"Canvas.h"

void Canvas::init(int gl_width,int gl_height,Solver* solver)
{
	_width = gl_width;
	_height = gl_height;
	_solver = solver;
	if (!glfwInit()) {
		std::cerr << "glfw init failed!" << std::endl;
		return ;
	}
	
	//新版本openGL还没掌握，先用旧版本的函数
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	_window = glfwCreateWindow(_width, _height, "Stable Fluid2D", nullptr, nullptr);
	if (_window == NULL) {
		std::cerr << "window init failed!" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(_window);
	
	glfwSetKeyCallback(_window, Solver::key_callback);
	glfwSetMouseButtonCallback(_window, Solver::mouse_button_callback);
	glfwSetCursorPosCallback(_window, Solver::cursor_position_callback);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cerr << "glew init failed" << std::endl;
	return;
	}

}

void Canvas::DrawGrid2D()
{
	//画网格
	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_LINES);
	for (float i = 0.0f; i <= (float)(_solver->rowCellNum_Grid + 1); i += 1.0f)
	{
		glVertex2f(i, 0.0f);
		glVertex2f(i, (float)(_solver->colCellNum_Grid + 1));
	}
	for (float i = 0.0f; i <= (float)(_solver->colCellNum_Grid + 1); i += 1.0f)
	{
		glVertex2f(0.0f, i);
		glVertex2f((float)(_solver->rowCellNum_Grid + 1), i);
	}
	glEnd();
}

void Canvas::DrawVelocity2D()
{
	glColor3f(0.0f, 0.5f, 1.0f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i<_solver->rowCellNum_Grid; i++)
	{
		for (int j = 0; j<_solver->colCellNum_Grid; j++)
		{
			glVertex2f((float)i + 0.5f, (float)j + 0.5f);
			glVertex2f((float)i + 0.5f + _solver->GetVelocityX(i, j)*10.0f,
				(float)j + 0.5f + _solver->GetVelocityY(i, j)*10.0f);
		}
	}
	glEnd();
	
} 

void Canvas::DrawDensity2D()
{
	float x, y;
	float d00, d01, d10, d11;

	int rowSize = _solver->rowCellNum_Grid;
	int colSize = _solver->colCellNum_Grid;

	glBegin(GL_QUADS);
	for (int i = 1; i <= rowSize - 2; i++)
	{
		x = (float)i;
		for (int j = 1; j <= colSize - 2; j++)
		{
			y = (float)j;

			d00 = _solver->GetDensity(i, j);
			d01 = _solver->GetDensity(i, j + 1);
			d10 = _solver->GetDensity(i + 1, j);
			d11 = _solver->GetDensity(i + 1, j + 1);

			//如果浓度为0，则显示白色，否则着色
			d00 > 0.0f ? glColor3f(1.0f-d00, 1.0f-d00, 1.0f) : glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x, y);

			d10 > 0.0f ? glColor3f(1.0f-d10, 1.0f-d10, 1.0f) : glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x+1.0f, y);

			d11 > 0.0f ? glColor3f(1.0f-d11, 1.0f-d11, 1.0f) : glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x + 1.0f, y + 1.0f);

			d01 > 0.0f ? glColor3f(1.0f-d01, 1.0f-d01, 1.0f) : glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x, y + 1.0f);
			
		}
	}
	glEnd();
}

void Canvas::Draw2D()
{
	glViewport(0, 0, _width, _height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//正交投影
	glOrtho(0.0f, (float)(_solver->rowCellNum_Grid),0.0f, (float)(_solver->colCellNum_Grid),-1.0,1.0);
	//清屏幕
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//画速度场
	if(_solver->displayMode==0)
		DrawVelocity2D();
	else if(_solver->displayMode==1)
		DrawDensity2D();

	//画网格
	DrawGrid2D();

	glfwSwapBuffers(_window);
}

void Canvas::render()
{
	_solver->Reset();
	while (!glfwWindowShouldClose(_window))
	{
		_solver->ClearBuffer();		//清理buffer
		
		glfwPollEvents();			//事件处理

		_solver->Update();		//由事件来更新buffer
		_solver->SolveVelocity();			//解算速度场
		_solver->SolveDensity();			//解算密度

		//渲染

		Draw2D();				
		
	}
	glfwTerminate();
}



