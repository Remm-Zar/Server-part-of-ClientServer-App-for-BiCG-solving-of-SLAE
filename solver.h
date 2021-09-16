#ifndef SOLVER_H
#define SOLVER_H
#include "csr.h"
#include "algvect.h"
#include <iostream>

using namespace std;

typedef vector<double> vect_double;
typedef vector<int> vect;
typedef vector<vector<double>> mat;


class Solver
{
private:
    CSR m_matrix={{}};
    CSR m_precond={{}};//reversed-diagonal matrix
    AlgVect m_vec={{}};
    double eps=0.001;
public:
    Solver(){};
    Solver(const CSR &M,const AlgVect &v);
    Solver(CSR &&M,AlgVect &&v);

    void setPrecision(double prec);

    AlgVect solve();
};

#endif // SOLVER_H
