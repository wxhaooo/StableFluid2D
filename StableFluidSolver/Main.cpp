#include"stdafx.h"
#include"SolverParms.h"
#include"StableFluidSolver.h"
int main()
{
	//解算器参数设置
	SolverParms* parms = new SolverParms;
	parms->deltaTime = 1.0f;
	parms->diffuseFactor = 0.2f;
	parms->viscosityFactor = 0.3f;

	StableFluidSolver* stableFluidSolver = new StableFluidSolver();
	stableFluidSolver->Start(1024,768,parms);
	return 0;
}


