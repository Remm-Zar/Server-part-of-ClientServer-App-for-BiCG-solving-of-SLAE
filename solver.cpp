#include "solver.h"
#include <cmath>

typedef vector<double> vect_double;

Solver::Solver(const CSR &M,const AlgVect &v)
{
    m_matrix=M;
    m_vec=v;
    m_precond=CSR(M.diag,M.Idiag,M.Jdiag);

}

Solver::Solver(CSR &&M,AlgVect &&v)
{
    m_matrix=M;
    m_vec=v;
    M.A.clear();
    M.IA.clear();
    M.JA.clear();
    v.m_v.clear();
    v.m_n=0;
}

void Solver::setPrecision(double prec)
{
    eps=prec;
}

AlgVect Solver::solve()
{
    ////////create preconditional system
    CSR p_matrix=m_precond.Multiply(m_matrix);
    AlgVect p_vect=m_precond.Multiply(m_vec.data());

  /*  cout<<"p_matrix:"<<endl;
    p_matrix.printMatrix();
    cout<<"p_vect="<<p_vect;*/

    AlgVect x0=m_vec,x_prev;
    AlgVect r0=m_precond.Multiply((m_vec-(m_matrix.Multiply(x0.data()))).data());
    AlgVect p0=r0,z0=r0,s0=r0,p1,r1;
    double alpha,beta;

    do
    {
        x_prev=x0;
        alpha=(p0*r0)/(s0*(AlgVect(p_matrix.Multiply(z0.data()))));
        x0=x0+alpha*z0;
        r1=r0-alpha*AlgVect(p_matrix.Multiply(z0.data()));
        p1=p0-alpha*AlgVect((m_matrix.T().Multiply(m_precond.T()).Multiply(s0.data())));
        beta=(p1*r1)/(p0*r0);
        z0=r1+beta*z0;
        s0=p1+beta*s0;
        r0=r1;
        p0=p1;
    }
    while ((x0-x_prev).continueProc(eps));
    return x0.cleanNum();
}
