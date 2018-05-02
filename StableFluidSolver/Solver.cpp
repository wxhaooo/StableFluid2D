#include"stdafx.h"
#include"Solver.h"

bool Solver::leftMouseDown = false;
bool Solver::rightMouseDown = false;

double Solver::leftMousePosX = 0.0;
double Solver::leftMousePosY = 0.0;

double Solver::rightMousePosX = 0.0;
double Solver::rightMousePosY = 0.0;

int Solver::displayMode = 0;

void Solver::init(int rowCellNum,int colCellNum,int win_width,int win_height,SolverParms* parms)
{
	//网格参数初始化
	_width = win_width;
	_height = win_height;
	rowCellNum_Grid =rowCellNum;
	colCellNum_Grid = colCellNum;
	totalCellNum_Grid = rowCellNum_Grid * colCellNum_Grid;

	rowCellNum_VelocityX = rowCellNum_Grid + 1;
	colCellNum_VelocityX = colCellNum_Grid + 1;
	totalCelNum_VelocityX = rowCellNum_VelocityX * colCellNum_VelocityX;

	rowCellNum_VelocityY = rowCellNum_Grid + 1;
	colCellNum_VelocityY = colCellNum_Grid + 1;
	totalCelNum_VelocityY = rowCellNum_VelocityY * colCellNum_VelocityY;

	//解算器参数初始化
	deltaTime = parms->deltaTime;
	diffuseFactor = parms->diffuseFactor;
	viscosityFactor = parms->viscosityFactor;

	//内存分配
	velocity_X = new float[totalCelNum_VelocityX];
	velocity_X_Temp = new float[totalCelNum_VelocityX];
	velocity_Y = new float[totalCelNum_VelocityY];
	velocity_Y_Temp = new float[totalCelNum_VelocityY];

	density = new float[totalCellNum_Grid];
	density_Temp = new float[totalCellNum_Grid];

	div = new float[totalCellNum_Grid];
	p = new float[totalCellNum_Grid];

}

void Solver::Reset()
{
	//初始的速度场
	for (int i = 0; i < totalCelNum_VelocityX; i++) velocity_X[i] = 0.0f;
	for (int i = 0; i < totalCelNum_VelocityY; i++) velocity_Y[i] = 0.0f;
	//初始浓度场
	for (int i = 0; i < totalCellNum_Grid; i++) density[i] = 0.0f;
}

void Solver::ClearBuffer()
{
	//把辅助的速度网格Clear
	for (int i = 0; i < totalCelNum_VelocityX; i++) velocity_X_Temp[i] = 0.0f;
	for (int i = 0; i < totalCelNum_VelocityY; i++) velocity_Y_Temp[i] = 0.0f;
	//Clear辅助的浓度网格
	for (int i = 0; i < totalCellNum_Grid; i++) density_Temp[i] = 0.0f;
}

void Solver::AddVelocity(int i, int j, float vx0, float vy0)
{
	velocity_X_Temp[VxIndex(i, j)] += vx0;
	velocity_Y_Temp[VyIndex(i, j)] += vy0;
}

void Solver::AddDensity(int i, int j, float d0)
{
	density_Temp[GridIndex(i, j)] += d0;
}

void Solver::Update()
{
	int lxPos, lyPos, rxPos, ryPos;

	//std::cout << "forwardMousePosX:" << forwardMousePosX << " forwardMousePosY:"
	//	<< forwardMousePosY << std::endl;

	//std::cout << "currentMousePosX:" << currentMousePosX << " currentMousePosY:"
		//<< currentMousePosY << std::endl;

	//左键增加速度
	if (leftMouseDown) {
		//计算openGL风格的坐标系中的坐标
		lxPos = (int)(leftMousePosX /_width *rowCellNum_Grid);
		lyPos = (int)((float)(_height - this->leftMousePosY)
			/ _height*colCellNum_Grid);
		//std::cout << "左键按下" << std::endl;
		//std::cout << "lxPos" <<lxPos<<  "  lyPos" << lyPos<<std::endl;
		if (lxPos > 0 && lxPos < rowCellNum_Grid - 1 && lyPos>0 && lyPos < colCellNum_Grid - 1) {
			//std::cout << "增加速度" << std::endl;
			AddVelocity(lxPos, lyPos,
				GetRandomVelocityX(), GetRandomVelocityY());
		}

		//存储下这次的坐标位置用于下次计算速度向量，废弃
		//forwardMousePosX = currentMousePosX;
		//forwardMousePosY = currentMousePosY;

		UpdateVelocityX();
		UpdateVelocityY();
	}

	//右键增加浓度
	if (rightMouseDown) {
		rxPos = (int)(rightMousePosX / _width * rowCellNum_Grid);
		ryPos = (int)((float)(_height - this->rightMousePosY)
			/ _height * colCellNum_Grid);
		if (rxPos > 0 && rxPos < rowCellNum_Grid - 1 && ryPos>0 && ryPos < colCellNum_Grid - 1) {
			
			AddDensity(rxPos, ryPos, 100.0f);
		}

		UpdateDensity();
	}
}

void Solver::SetVelocityBoundaryX()
{
	//边界单元的填充
	for (int i = 1; i <= rowCellNum_VelocityX - 2; i++) {			//填充上下两行的边界
		velocity_X[VxIndex(i, 0)] = velocity_X[VxIndex(i, 1)];
		velocity_X[VxIndex(i, colCellNum_VelocityX - 1)] = velocity_X[VxIndex(i, colCellNum_VelocityX - 2)];
	}

	for (int j = 1; j <= colCellNum_VelocityX - 2; j++) {			//填充左右两行的边界
		velocity_X[VxIndex(0, j)] = velocity_X[VxIndex(1, j)];
		velocity_X[VxIndex(rowCellNum_VelocityX - 1, j)] = velocity_X[VxIndex(rowCellNum_VelocityX - 2, j)];
	}

	//填充四个角
	velocity_X[VxIndex(0, 0)] = (velocity_X[VxIndex(1, 0)] + velocity_X[VxIndex(0, 1)]) / 2;

	velocity_X[VxIndex(rowCellNum_VelocityX - 1, 0)] =
		(velocity_X[VxIndex(rowCellNum_VelocityX - 2, 0)]
			+ velocity_X[VxIndex(rowCellNum_VelocityX - 1, 1)]) / 2;

	velocity_X[VxIndex(0, colCellNum_VelocityX - 1)] =
		(velocity_X[VxIndex(1, colCellNum_VelocityX - 1)]
			+ velocity_X[VxIndex(0, colCellNum_VelocityX - 2)]) / 2;

	velocity_X[VxIndex(rowCellNum_VelocityX - 1, colCellNum_VelocityX - 1)] =
		(velocity_X[VxIndex(rowCellNum_VelocityX - 2, colCellNum_VelocityX - 1)]
			+ velocity_X[VxIndex(rowCellNum_VelocityX - 1, colCellNum_VelocityX - 2)]) / 2;
}

void Solver::SetVelocityBoundaryY()
{
	//边界单元的填充,为什么设置成负的
	for (int i = 1; i <= rowCellNum_VelocityY - 2; i++) {			//填充上下两行的边界
		velocity_Y[VyIndex(i, 0)] = -velocity_Y[VyIndex(i, 1)];
		velocity_Y[VyIndex(i, colCellNum_VelocityY - 1)] = -velocity_Y[VyIndex(i, colCellNum_VelocityY - 2)];
	}

	for (int j = 1; j <= colCellNum_VelocityY - 2; j++) {			//填充左右两行的边界
		velocity_Y[VyIndex(0, j)] = velocity_Y[VyIndex(1, j)];
		velocity_Y[VyIndex(rowCellNum_VelocityY - 1, j)] = velocity_Y[VyIndex(rowCellNum_VelocityY - 2, j)];
	}

	//填充四个角
	velocity_Y[VyIndex(0, 0)] = (velocity_Y[VyIndex(0, 1)] + velocity_Y[VyIndex(1, 0)]) / 2;

	velocity_Y[VyIndex(rowCellNum_VelocityY - 1, 0)] =
		(velocity_Y[VyIndex(rowCellNum_VelocityY - 2, 0)]
			+ velocity_Y[VyIndex(rowCellNum_VelocityY - 1, 1)]) / 2;

	velocity_Y[VyIndex(0, colCellNum_VelocityY - 1)] =
		(velocity_Y[VyIndex(1, colCellNum_VelocityY - 1)]
			+ velocity_Y[VyIndex(0, colCellNum_VelocityY - 2)]) / 2;

	velocity_Y[VyIndex(rowCellNum_VelocityY - 1, colCellNum_VelocityY - 1)] =
		(velocity_Y[VyIndex(rowCellNum_VelocityY - 2, colCellNum_VelocityY - 1)]
			+ velocity_Y[VyIndex(rowCellNum_VelocityY - 1, colCellNum_VelocityY - 2)]) / 2;
}

void Solver::UpdateVelocityX()
{
	//更新速度场的水平分量
	for (int i = 0; i < totalCelNum_VelocityX; i++) velocity_X[i] += velocity_X_Temp[i];

	SetVelocityBoundaryX();
}

void Solver::UpdateVelocityY()
{
	//更新速度场的垂直分量
	for (int i = 0; i < totalCelNum_VelocityY; i++) velocity_Y[i] += velocity_Y_Temp[i];

	SetVelocityBoundaryY();
	
}

void Solver::UpdateDensity()
{
	for (int i = 0; i < totalCellNum_Grid; i++) density[i] += density_Temp[i];

	SetOtherBoundary(density);
}

void Solver::SetOtherBoundary(float* grid)
{
	for (int i = 1; i <= rowCellNum_Grid - 2; i++)
	{
		grid[GridIndex(i, 0)] = grid[GridIndex(i, 1)];
		grid[GridIndex(i, colCellNum_Grid - 1)] = grid[GridIndex(i, colCellNum_Grid - 2)];
	}
	for (int j = 1; j <= colCellNum_Grid - 2; j++)
	{
		grid[GridIndex(0, j)] = grid[GridIndex(1, j)];
		grid[GridIndex(rowCellNum_Grid - 1, j)] = grid[GridIndex(rowCellNum_Grid - 2, j)];
	}

	//其他按临点取均值即可
	grid[GridIndex(0, 0)] = (grid[GridIndex(1, 0)] + grid[GridIndex(0, 1)]) / 2;

	grid[GridIndex(rowCellNum_Grid - 1, 0)] = (grid[GridIndex(rowCellNum_Grid - 2, 0)] + grid[GridIndex(rowCellNum_Grid - 1, 1)]) / 2;

	grid[GridIndex(0, colCellNum_Grid - 1)] = (grid[GridIndex(1, colCellNum_Grid - 1)] + grid[GridIndex(0, colCellNum_Grid - 2)]) / 2;

	grid[GridIndex(rowCellNum_Grid - 1, colCellNum_Grid - 1)] = (grid[GridIndex(rowCellNum_Grid - 1, colCellNum_Grid - 2)] +
																							grid[GridIndex(rowCellNum_Grid - 1, colCellNum_Grid - 2)]) / 2;
}

void Solver::Projection()
{
	for (int i = 1; i <=rowCellNum_Grid - 2; i++) {
		for (int j = 1; j <= colCellNum_Grid - 2; j++) {
			//计算出散度
			div[GridIndex(i, j)] = 0.5f*(velocity_X[VxIndex(i + 1, j)] - velocity_X[VxIndex(i - 1, j)]
				+ velocity_Y[VyIndex(i, j + 1)] - velocity_Y[VyIndex(i, j - 1)]);
			//把p先做个初始化
			p[GridIndex(i, j)] = 0.0f;
		}
	}
	//边界确定一下
	SetOtherBoundary(p);
	SetOtherBoundary(div);

	//解泊松方程
	for (int k = 0; k < 20; k++) {
		for (int i = 1; i <= rowCellNum_Grid - 2; i++) {
			for (int j = 1; j <= colCellNum_Grid - 2; j++) {
				p[GridIndex(i, j)] = (p[GridIndex(i + 1, j)] + p[GridIndex(i, j + 1)] 
					+ p[GridIndex(i - 1, j)] + p[GridIndex(i, j - 1)] - div[GridIndex(i, j)]) / 4.0f;
			}
		}
		SetOtherBoundary(p);
	}
	
	//减去梯度场，一个问题，使用二阶差分近似的时候会有些问题，不知道什么原因
	for (int i = 1; i <=rowCellNum_VelocityX - 2; i++) {
		for (int j = 1; j <= colCellNum_VelocityX - 2; j++) {
			//velocity_X[VxIndex(i, j)] -= (0.5f*(p[GridIndex(i + 1, j)] - p[GridIndex(i - 1, j)]));
			velocity_X[VxIndex(i, j)] -= (p[GridIndex(i, j)] - p[GridIndex(i - 1, j)]);
		}
	}

	for (int i = 1; i <= rowCellNum_VelocityY - 2; i++) {
		for (int j = 1; j <= colCellNum_VelocityY - 2; j++) {
			//velocity_Y[VyIndex(i, j)] -= (0.5f*(p[GridIndex(i, j + 1)] - p[GridIndex(i, j - 1)]));
			velocity_Y[VyIndex(i, j)] -= (p[GridIndex(i, j)] - p[GridIndex(i, j - 1)]);
		}
	}

	SetVelocityBoundaryX();
	SetVelocityBoundaryY();
	/*
	for (int i = 0; i < rowCellNum_VelocityX; i++) {
		for (int j = 0; j < colCellNum_VelocityX; j++) {
			std::cout << "(" << i << "," << j << ")" << velocity_X[VxIndex(i, j)] << std::endl;
		}
	}

	for (int i = 0; i < rowCellNum_VelocityY; i++) {
		for (int j = 0; j < colCellNum_VelocityY; j++) {
			std::cout << "(" << i << "," << j << ")" << velocity_Y[VyIndex(i, j)] << std::endl;
		}
	}
	*/
	//for (int i = 1; i <= rowCellNum_Grid; i++) {
	//	for (int j = 1; j <= colCellNum_Grid; j++) {
		//	std::cout << "(" << i << "," << j << ")" << div[GridIndex(i, j)] << std::endl;
	//	}
	//}
	//std::cout << div[GridIndex(0, 0)] << std::endl;
}

void Solver::DiffuseVelocity()
{
	for (int i = 0; i < totalCelNum_VelocityX; i++) velocity_X[i] = 0.0f;
	for (int i = 0; i < totalCelNum_VelocityY; i++) velocity_Y[i] = 0.0f;
	float factor = diffuseFactor * deltaTime;

	for (int k = 0; k < 20; k++) {
		//计算水平分量的扩散
		for (int i = 1; i <= rowCellNum_VelocityX-2; i++) {
			for (int j = 1; j <= colCellNum_VelocityX-2; j++) {
				velocity_X[VxIndex(i, j)] = (velocity_X_Temp[VxIndex(i, j)] +
					factor * (velocity_X[VxIndex(i + 1, j)] + velocity_X[VxIndex(i - 1, j)] +
						velocity_X[VxIndex(i, j + 1)] + velocity_X[VxIndex(i, j - 1)])) / (1.0f + 4.0f * factor);
			}
		}
		//计算垂直分量的扩散
		for (int i = 1; i <= rowCellNum_VelocityY-2; i++) {
			for (int j = 1; j <= colCellNum_VelocityY-2; j++) {
				velocity_Y[VyIndex(i, j)] = (velocity_Y_Temp[VyIndex(i, j)] +
					factor * (velocity_Y[VyIndex(i + 1, j)] + velocity_Y[VyIndex(i - 1, j)] +
						velocity_Y[VyIndex(i, j + 1)] + velocity_Y[VyIndex(i, j - 1)])) / (1.0f + 4.0f * factor);
			}
		}
		SetVelocityBoundaryX();
		SetVelocityBoundaryY();

	}
}

void Solver::AdvectVelocity()
{
	float maxRowNum = (float)(rowCellNum_Grid);
	float maxColNum = (float)(colCellNum_Grid);

	//速度的水平分量对流
	for (int i = 1; i <= rowCellNum_VelocityX - 2; i++) {
		for (int j = 1; j <= colCellNum_VelocityX - 2; j++) {
			//记录当前网格处的速度
			float vx = velocity_X_Temp[VxIndex(i, j)];
			float vy = velocity_Y_Temp[VyIndex(i, j)];
			
			//退回到delta前的坐标
			float oldX = (float)i - vx * deltaTime;
			float oldY = (float)j + 0.5f - vy * deltaTime;

			//参数修正
			if (oldX < 0.5f) oldX = 0.5f;
			if (oldX > maxRowNum - 0.5f) oldX = maxRowNum - 0.5f;
			if (oldY < 1.0f) oldY = 1.0f;
			if (oldY > maxColNum - 1.0f) oldY = maxColNum - 1.0f;

			int i0 = (int)oldX;
			int j0 = (int)(oldY - 0.5f);
			int i1 = i0 + 1;
			int j1 = j0 + 1;

			float s1 = (float)i1 - oldX;
			float s0 = 1.0f - s1;
			float t1 = (float)j1 + 0.5f - oldY;
			float t0 = 1.0f - t1;

			//std::cout << "s1:" << s1 << " s0:" << s0 << " t1:" << t1 << " t0:" << t0 << std::endl;
			//做双线性插值

			velocity_X[VxIndex(i, j)] = t1 * (s1*velocity_X_Temp[VxIndex(i0, j0)] + s0 * velocity_X_Temp[VxIndex(i1, j0)]) +
										  t0 * (s1*velocity_X_Temp[VxIndex(i0, j1)] + s0 * velocity_X_Temp[VxIndex(i1, j1)]);
		}
	}

	//速度的垂直分量对流
	for (int i = 1; i <= rowCellNum_VelocityY - 2; i++) {
		for (int j = 1; j <= colCellNum_VelocityY - 2; j++) {
			//记录当前网格处的速度
			float vx = velocity_X_Temp[VxIndex(i, j)];
			float vy = velocity_Y_Temp[VyIndex(i, j)];
			//退回到delta前的坐标
			float oldX = (float)i + 0.5f - vx * deltaTime;
			float oldY = (float)j  - vy * deltaTime;

			//参数修正
			if (oldX < 1.0f) oldX = 1.0f;
			if (oldX > maxRowNum -1.0f) oldX = maxRowNum - 1.0f;
			if (oldY < 0.5) oldY = 0.5f;
			if (oldY > maxColNum - 0.5f) oldY = maxColNum - 0.5f;

			int i0 = (int)(oldX - 0.5f);
			int j0 = (int)oldY;
			int i1 = i0 + 1;
			int j1 = j0 + 1;

			float s1 = (float)i1 + 0.5f - oldX;
			float s0 = 1.0f - s1;
			float t1 = (float)j1 - oldY;
			float t0 = 1.0f - t1;

			//std::cout << "s1:" << s1 << " s0:" << s0 << " t1:" << t1 << " t0:" << t0 << std::endl;
			//做双线性插值

			velocity_Y[VyIndex(i, j)] = t1 * (s1*velocity_Y_Temp[VyIndex(i0, j0)] + s0 * velocity_Y_Temp[VyIndex(i1, j0)]) +
												t0 * (s1*velocity_Y_Temp[VyIndex(i0, j1)] + s0 * velocity_Y_Temp[VyIndex(i1, j1)]);
		}
	}

	SetVelocityBoundaryX();
	SetVelocityBoundaryY();
	/*
	for (int i = 0; i < rowCellNum_VelocityX; i++) {
		for (int j = 0; j < colCellNum_VelocityX; j++) {
			std::cout << "(" << i << "," << j << ")" << velocity_X[VxIndex(i, j)] << std::endl;
		}
	}

	for (int i = 0; i < rowCellNum_VelocityY; i++) {
		for (int j = 0; j < colCellNum_VelocityY; j++) {
			std::cout << "(" << i << "," << j << ")" << velocity_Y[VyIndex(i, j)] << std::endl;
		}
	}
	*/
}

void Solver::SolveVelocity()
{
	Projection();
	if (diffuseFactor > 0.0f) {
		//把现有的网格让-Temp先拿着，方便计算diffuse
		std::swap(velocity_X, velocity_X_Temp);
		std::swap(velocity_Y, velocity_Y_Temp);
		DiffuseVelocity();
	}
	//把现有的网格让-Temp先拿着，方便计算advect
	std::swap(velocity_X, velocity_X_Temp);
	std::swap(velocity_Y, velocity_Y_Temp);
	AdvectVelocity();

	Projection();
}

void Solver::DiffuseDensity()
{
	for (int i = 0; i < totalCellNum_Grid; i++) density[i] = 0.0f;
	float factor = viscosityFactor * deltaTime;

	for (int k = 0; k < 20; k++) {
		for (int i = 1; i <= rowCellNum_Grid - 2; i++) {
			for (int j = 1; j <= colCellNum_Grid - 2; j++) {
				density[GridIndex(i, j)] = (density_Temp[GridIndex(i, j)] +
					factor * (density[GridIndex(i + 1, j)] + density[GridIndex(i - 1, j)] +
						density[GridIndex(i, j + 1)] + density[GridIndex(i, j - 1)])) / (1.0f + 4.0f*factor);
			}
		}
		SetOtherBoundary(density);
	}
}

void Solver::AdvectDensity()
{
	float maxRowNum = (float)(rowCellNum_Grid);
	float maxColNum = (float)(colCellNum_Grid);

	//对流浓度
	for (int i = 1; i <= rowCellNum_Grid - 2; i++) {
		for (int j = 1; j <=colCellNum_Grid - 2; j++) {
			//记录当前网格处的速度
			float vx = velocity_X_Temp[VxIndex(i, j)];
			float vy = velocity_Y_Temp[VyIndex(i, j)];

			//退回到delta前的坐标
			float oldX = (float)i + 0.5f - vx * deltaTime;
			float oldY = (float)j + 0.5f - vy * deltaTime;

			//参数修正
			if (oldX < 1.0f) oldX = 1.0f;
			if (oldX > maxRowNum - 1.0f) oldX = maxRowNum - 1.0f;
			if (oldY < 1.0f) oldY = 1.0f;
			if (oldY > maxColNum - 1.0f) oldY = maxColNum - 1.0f;

			int i0 = (int)(oldX - 0.5f);
			int j0 = (int)(oldY - 0.5f);
			int i1 = i0 + 1;
			int j1 = j0 + 1;

			float s1 = (float)i1 + 0.5f - oldX;
			float s0 = 1.0f - s1;
			float t1 = (float)j1 + 0.5f - oldY;
			float t0 = 1.0f - t1;

			density[GridIndex(i, j)] = t1 * (s1*density_Temp[GridIndex(i0, j0)] + s0 * density_Temp[GridIndex(i1, j0)]) +
				t0 * (s1*density_Temp[GridIndex(i0, j1)] + s0 * density_Temp[GridIndex(i1, j1)]);
		}
	}

	SetOtherBoundary(density);
}

void Solver::SolveDensity()
{
	if (viscosityFactor > 0.0f) {
		std::swap(density, density_Temp);
		DiffuseDensity();
	}
	std::swap(density, density_Temp);
	AdvectDensity();
}

void Solver::mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS)			//左键按下
			leftMouseDown = true;
		else if (action == GLFW_RELEASE)	//左键松开
			leftMouseDown = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)			//右键按下
			rightMouseDown = true;
		else if (action == GLFW_RELEASE)	//右键松开
			rightMouseDown = false;
	}

}

void Solver::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		displayMode++;
		displayMode %= 2;
	}
}

void Solver::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
		//记录运动时的鼠标位置
	if (leftMouseDown) {
		//std::cout << "左键按下..." << "位置为" << xpos << " " << ypos << std::endl;
		leftMousePosX = xpos;
		leftMousePosY = ypos;
	}
	if (rightMouseDown) {
		rightMousePosX = xpos;
		rightMousePosY = ypos;
	}
	//std::cout << "鼠标移动中..." << "位置为" << xpos << " " << ypos << std::endl;
	//currentMousePosX = xpos;
	//currentMousePosY = ypos;
}

