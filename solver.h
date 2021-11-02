#ifndef SOLVER_H
#define SOLVER_H
#include "csr.h"
#include "algvect.h"
#include <iostream>

using namespace std;

typedef vector<double> vect_double;
typedef vector<long> vect;
typedef vector<vector<double>> mat;


class Solver
{
private:
    CSR m_matrix={{}};
    CSR m_precond={{}};//reversed-diagonal matrix
    AlgVect m_vec={{}};
public:
    Solver(){};
    Solver(const QString& matrixFile,const QString& vectorFile);
    Solver(CSR &&M,AlgVect &&v);

    AlgVect solve(int threads,double &time);
};

#endif // SOLVER_H
