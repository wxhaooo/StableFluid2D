#ifndef SOLVER_H
#define SOLVER_H

#include<cstdlib>
#include"SolverParms.h"
#include<GL\glew.h>
#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include<iostream>
#include<random>

class Solver
{
public:
	//显示模式
	static int displayMode;

	//窗口大小
	int _width;
	int _height;
	
	//网格的参数
	int rowCellNum_Grid;
	int colCellNum_Grid;
	int totalCellNum_Grid;

	//存储速度水平分量的网格大小
	int rowCellNum_VelocityX;
	int colCellNum_VelocityX;
	int totalCelNum_VelocityX;

	//存储速度垂直分量的网格大小
	int rowCellNum_VelocityY;
	int colCellNum_VelocityY;
	int totalCelNum_VelocityY;

	float deltaTime;			//时间的步长
	float diffuseFactor;		//速度的粘性因子
	float viscosityFactor;	//浓度的粘性因子

	float* velocity_X;			//存储速度水平分量的网格指针
	float* velocity_Y;			//存储速度垂直分量的网格指针

	float* velocity_X_Temp;		//用于记录临时速度水平增量的网格指针
	float* velocity_Y_Temp;

	float* div;							//散度网格的指针
	float* p;							// Helmholtz-Hodge分解中的grad p

	float* density;					//浓度网格的指针
	float* density_Temp;		//浓度增量的网格指针

	Solver() {}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	static bool leftMouseDown;
	static bool rightMouseDown;

	static double leftMousePosX;		//左键在哪里
	static double leftMousePosY;

	static double rightMousePosX;		//右键位置
	static double rightMousePosY;

	//用一维数组表示二维数组则需要索引
	int VxIndex(int i, int j) { return (i + j * rowCellNum_VelocityX); }
	int VyIndex(int i, int j) { return (i + j * rowCellNum_VelocityY); }
	int GridIndex(int i, int j) { return (i + j * rowCellNum_Grid); }

	void init(int rowCellNum,int colCellNum,int win_width,int win_height,SolverParms* parms);
	void Reset();
	void ClearBuffer();
	void Projection();
	void AdvectVelocity();
	void AdvectDensity();
	void DiffuseVelocity();
	void DiffuseDensity();

	void Update();
	void UpdateVelocityX();
	void UpdateVelocityY();
	void SolveVelocity();
	void SolveDensity();

	void SetOtherBoundary(float* grid);
	void SetVelocityBoundaryX();
	void SetVelocityBoundaryY();

	float GetVelocityX(int i, int j) { return velocity_X[VxIndex(i, j)]; }
	float GetVelocityY(int i, int j) { return velocity_Y[VyIndex(i, j)]; }
	float GetDensity(int i, int j) { return density[GridIndex(i, j)]; }

	float GetRandomVelocityX()
	{
		//std::normal_distribution<float> e(70, 2.5);
		std::uniform_real_distribution<float> e(-70.0f, 110.0f);
		std::default_random_engine re;
		return e(re);
	}

	float GetRandomVelocityY()
	{
		//std::normal_distribution<float> e(100, 1.5);
		std::uniform_real_distribution<float> e(-100.0f, 80.0f);
		std::default_random_engine re;
		return e(re);
	}

	void AddVelocity(int i, int j, float vx0, float vy0);
	void AddDensity(int i, int j, float d0);

	void UpdateDensity();
};
#endif
