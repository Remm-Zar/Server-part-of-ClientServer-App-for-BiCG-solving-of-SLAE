#include "solver.h"
#include "SlauSolver.h"
#include <QFile>
#include<iomanip>
#include <QList>
#include <cmath>
#include <QDebug>
#include <omp.h>

typedef vector<double> vect_double;

Solver::Solver(const QString& matrixFile,const QString& vectorFile)
{
#pragma omp parallel num_threads(2)
{
        if(omp_get_thread_num()==0)
        {
            QFile f(matrixFile);
            m_matrix.IA.pop_back();
            if (!f.open(QIODevice::ReadOnly))
            {
                cout<<"This file hasn't been found"<<endl;
            }
            else
            {
                long long i_=1,i=1,j=0,nnz=0,dnnz=0;
                QList<QString> nums;
                QString s;
                while(!f.atEnd())
                {
                    s=f.readLine();
                    nums=s.split(" ",QString::SkipEmptyParts);
                    //
                    j=nums.at(0).toInt()-1;
                    i=nums.at(1).toInt();
                    if (i-1==j)
                    {
                        ++dnnz;
                        m_matrix.diag.push_back(1.0/nums.at(2).toDouble());
                        m_matrix.Idiag.push_back(dnnz);
                        m_matrix.Jdiag.push_back(nums.at(0).toInt()-1);
                    }
                    m_matrix.A.push_back(nums.at(2).toDouble());
                    m_matrix.JA.push_back(nums.at(0).toInt()-1);

                    if(i_!=i){
                        m_matrix.IA.push_back(nnz);
                    }
                    ++nnz;
                    i_=i;
                    s.clear();
                }
                m_matrix.IA.push_back(nnz);
                m_matrix.n= m_matrix.IA.size()-1;
                m_precond=CSR(m_matrix.diag,m_matrix.Idiag,m_matrix.Jdiag);
            }
            f.close();
        }
            if(omp_get_thread_num()==1)
            {
                m_vec.m_v.pop_back();
                QFile f1(vectorFile);
                if (!f1.open(QIODevice::ReadOnly))
                {
                    cout<<"This file hasn't been found"<<endl;
                }
                else
                {
                    QString s=f1.readLine();
                    QList<QString> nums=s.split(" ",QString::SkipEmptyParts);
                    QList<QString>::Iterator it=nums.begin();
                    while (it!=nums.end())
                    {
                        m_vec.m_v.push_back(it->toDouble());
                        ++it;
                    }
                    m_vec.m_n=m_vec.m_v.size();
                }
                f1.close();
            }

}
    cout<<"End of data reading\n";
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


AlgVect Solver::solve(int threads,double &time)
{

    ////////create preconditional system
    qDebug()<<"Preparing preconditional system...\n";
   CSR A=m_precond.Multiply(m_matrix);
   AlgVect b=m_precond.Multiply(m_vec);
   qDebug()<<"System is created\n";

    CRSMatrix mat={};
    mat.val=A.get_A();
    mat.colIndex=A.get_JA();
    mat.rowPtr=A.get_IA();
    mat.n=A.get_n();
    mat.m=mat.n;
    mat.nz=*(A.get_IA().end()-1);


    unsigned int size=b.size();
    double *res=new double[size];

    double start;     
    long count=0;
    CSlauSolver solver(threads);
    start=omp_get_wtime();
    solver.SLE_Solver_CRS_BICG(mat,b.data().data(),0.00000000001,50000,res,count);
    double end=omp_get_wtime();
    AlgVect check(res,size);
    qDebug()<<!(m_matrix.Multiply(check)-m_vec).continueProc(0.0000000001,m_vec.eNorm());
    time=end-start;
    delete[] res;

    return check.cleanNum();
}
