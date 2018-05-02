#ifndef STABLEFLUIDSOLVER_H
#define STABLEFLUIDSOLVER_H

#include"SolverParms.h"
#include"Canvas.h"
#include"Solver.h"


class StableFluidSolver
{
public:
	Canvas* canvas;
	Solver* solver;
	StableFluidSolver() {}

	void Start(int width,int height,SolverParms* parms);

};
#endif // !STABLEFLUIDSOLVER_H

