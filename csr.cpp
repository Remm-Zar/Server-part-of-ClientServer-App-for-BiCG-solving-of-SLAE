#include <vector>
#include <QFile>
#include <algorithm>
#include <iostream>
#include "csr.h"
#include <iomanip>
#include <QList>
#include <omp.h>
#include <parallel/algorithm>

using namespace std;

typedef vector<double> vect_double;
typedef vector<long> vect;
typedef vector<vector<double>> mat;

CSR::CSR(const QString& input)
{
    QFile f(input);
    CSR res;
    if (!f.open(QIODevice::ReadOnly))
    {
        cout<<"This file hasn't been found"<<endl;
    }
    else
    {
        long i_=1,i=1,j=0,nnz=0,dnnz=0;
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
                diag.push_back(1.0/nums.at(2).toDouble());
                Idiag.push_back(dnnz);
                Jdiag.push_back(nums.at(0).toInt()-1);
            }
            A.push_back(nums.at(2).toDouble());
            JA.push_back(nums.at(0).toInt()-1);

            if(i_!=i){
                IA.push_back(nnz);
            }
            ++nnz;
            i_=i;
            s.clear();
        }
        IA.push_back(nnz);
        n=IA.size()-1;
    }
    f.close();
}

CSR::CSR(const mat& M,bool forPrecond)  {
    long m = M.size(), i, j;
    m==0?n=0:n = M[0].size();
    long NNZ = 0;
    //double start=omp_get_wtime();
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            if (i==j && forPrecond)
            {
                diag.push_back(1/M[i][j]);
                Idiag.push_back(i+1);
                Jdiag.push_back(j);
            }
            if (M[i][j] != 0) {
                A.push_back(M[i][j]);
                JA.push_back(j);
                ++NNZ;
            }
        }
        IA.push_back(NNZ);
    }
    //double end=omp_get_wtime();
  //  cout<<" Time in csr constructor="<<end-start<<endl;
}

CSR::CSR(mat&& M,bool forPrecond)//necessity
{
    long m = M.size(), i, j;
    m==0?n=0:n = M[0].size();
    long NNZ = 0;

    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            if (i==j && forPrecond)
            {
                diag.push_back(1/M[i][j]);
                Idiag.push_back(i+1);
                Jdiag.push_back(j);
            }
            if (M[i][j] != 0) {
                A.push_back(M[i][j]);
                JA.push_back(j);
               ++NNZ;
            }
        }
        IA.push_back(NNZ);
    }
    this->m=m;
    nz=NNZ;
    M.clear();
    M.shrink_to_fit();
}

CSR::CSR(const CSR& M)/*: A(M.A), IA(M.IA), JA(M.JA),n(M.n),diag(M.diag),Idiag(M.Idiag),Jdiag(M.Jdiag)*/
{
    A=M.A;
    IA=M.IA;
    JA=M.JA;
    n=M.n;
    diag=M.diag;
    Idiag=M.Idiag;
    Jdiag=M.Jdiag;
}

CSR::CSR(CSR&& M): A(M.A), IA(M.IA), JA(M.JA),n(M.n),diag(M.diag),Idiag(M.Idiag),Jdiag(M.Jdiag)
{
    M.A.clear();
    M.IA.clear();
    M.JA.clear();
    M.diag.clear();
    M.Idiag.clear();
    M.Jdiag.clear();
    M.A.shrink_to_fit();
    M.IA.shrink_to_fit();
    M.JA.shrink_to_fit();
    M.diag.shrink_to_fit();
    M.Idiag.shrink_to_fit();
    M.Jdiag.shrink_to_fit();

}
CSR::CSR(const initializer_list<vector<double>>& list):CSR(mat(list),true)
{}

CSR::CSR(const vect_double& out_A,const vect &out_IA,const vect &out_JA,const vect_double& out_d, const vect &out_Id, const vect &out_Jd)
{
    n=out_IA.size()-1;
    A=out_A;
    IA=out_IA;
    JA=out_JA;
    diag=out_d;
    Idiag=out_Id;
    Jdiag=out_Jd;
}

CSR::~CSR()
{
    A.clear();
    IA.clear();
    JA.clear();
    A.shrink_to_fit();
    IA.shrink_to_fit();
    JA.shrink_to_fit();
}

vect_double CSR::get_A()
{
    return A;
}

vect CSR::get_IA()
{
    return IA;
}

vect CSR::get_JA()
{
    return JA;
}

double CSR::get(long x, long y)
{
    double res = 0;
    long i = IA[x];
    while (i < IA[x + 1] && JA[i] <= y)
    {
        if (JA[i] == y) res = A[i];
        i++;
    }
    return res;
}

// Useful function for printing full information about matrixs
void printVector(const vect& V, char* msg)
{
    cout << msg << "[ ";
    for_each(V.begin(), V.end(), [](int a) {cout << a << " "; });
    cout << "]" << endl;
}
void printVector(const vect_double& V, char* msg)
{
    cout << msg << "[ ";
    for_each(V.begin(), V.end(), [](int a) {cout << a << " "; });
    cout << "]" << endl;
}

void CSR::printMatrix() {
    cout << "Print Matrix:" << endl;
    printVector(A, (char*)"A = ");
    printVector(JA, (char*)"JA = ");
    printVector(IA, (char*)"IA = ");
    printVector(diag, (char*)"diag = ");
    printVector(Idiag, (char*)"Idiag = ");
    printVector(Jdiag, (char*)"Jdiag = ");
    cout<<"n="<<n<<endl;


    long size = IA.size() - 1;
    if (size>0)
    {
        double** ar = NULL;
        ar = new double* [size];
        for (long i = 0; i < size; i++)
        {
            ar[i] = new double[n];
            for (long j = 0; j < n; j++)
                ar[i][j] = 0;
        }


        long j_point = 0;
        for (long i = 0; i < size; i++)
        {
            long n = 0;
            for (long j = 0; j < IA[i + 1] - IA[i]; j++)
            {
                n++;
                ar[i][JA[j_point + j]] = A[j_point + j];
            }
            j_point += n;
        }
        for (long i = 0; i < size; i++) {
            cout<<"|";
            for (long j = 0; j < n; j++) {
                cout.width(5);
                cout <<setprecision(3)<< *(*(ar + i) + j) << " ";
            }
            cout << "|"<<endl;
        }

      //  double* cur = ar[0];
        for (long i = 0; i < size; i++)
        {
            delete[] ar[i];
        }
        delete[] ar;
    }

    cout << "=====================" << endl;
}

CSR CSR::T()const
{
    CSR transp(*this);
    long point=0;
    mat A_t;
    vect IA_t;
    IA_t.reserve(IA.size()-1);
#pragma omp parallel for schedule(runtime)
    for (unsigned long i=0;i<IA.size()-1;++i)
    {
        IA_t[i]=IA[i+1]-IA[i];
    }

    for (unsigned long i=0;i<A.size();++i)
    {
        A_t.push_back({A[i]});
    }

    for (unsigned long i=0;i<IA.size()-1;++i)
    {
        for (long j=0;j<IA[i+1]-IA[i];++j)
        {
          //  A_t[point][1]=i;
            --IA_t[i];
            ++IA_t[JA[point]];
            A_t[point].push_back(JA[point]);
            A_t[point].push_back(i);
            ++point;
        }
    }   
    __gnu_parallel::sort(A_t.begin(),A_t.end(),[](vect_double a,vect_double b){return a[1]<b[1];});
    __gnu_parallel::sort(A_t.begin(),A_t.end(),[](vect_double a,vect_double b){return (a[1]==b[1])&&(a[2]<b[2]);});
#pragma omp parallel for schedule(runtime)
    for (unsigned long i=0;i<A.size();++i)
    {
       transp.A[i]=A_t[i][0];
       transp.JA[i]=A_t[i][2];
    }

    long sum=0;
    for (unsigned long i=0;i<IA.size()-1;++i)
    {
        sum+=IA_t[i];
        transp.IA[i+1]=sum;
    }
    return transp;

}
AlgVect CSR::Multiply(const AlgVect& x)
{
    long m = IA.size() - 1;
    vect_double res(m,0);

#pragma omp parallel for schedule(runtime)
    for (long i = 0; i < m; i++)
    {
     for (long j = IA[i]; j < IA[i + 1]; j++)
        {
         //if (omp_get_thread_num()==0)cout<<"nult j v: "<<j<<"\n";
            res[i] = res[i] + x.data()[JA[j]] * A[j];
        }
    }
    AlgVect ans(res);
    return ans;
}

CSR CSR::Multiply(const CSR& M)
{
    CSR copy(M);
    long x = M.n, y = IA.size() - 1;
    mat ar(y,vect_double());
#pragma omp parallel for schedule(runtime)
    for (long i = 0; i < y; i++)
    {
        vect_double cur(x,0);
        for (long j = 0; j < x; j++)
        {
           // if (omp_get_thread_num()==0)cout<<"nult j m: "<<j<<"\n";
            for(long k=0;k<y;k++)
                cur[j] += get(i,k) * copy.get(k,j);
        }
        ar[i]=move(cur);
    }
    return CSR(ar,false);
}

CSR& CSR::operator=(const CSR& other)
{
    if (&other == this) {
        return *this;
    }
    A.clear();
    IA.clear();
    JA.clear();

    unsigned long i=0;
    for ( ; i < other.A.size(); ++i) {
        A.push_back(other.A[i]);
        JA.push_back(other.JA[i]);
    }
    for ( i=0; i < other.IA.size(); ++i) {
        IA.push_back(other.IA[i]);
    }
    n = other.n;
    return *this;
}

CSR& CSR::operator=(const CSR&& other)
{
    if (&other == this) {
        return *this;
    }
    A.clear();
    IA.clear();
    JA.clear();

    A = other.A;
    IA = other.IA;
    JA = other.JA;

    n = other.n;
    return *this;
}

CSR CSR::operator+(CSR& other)
{
    mat res_mat;
    vect_double line={};
    for(long i=0; i<n;i++){
        line = {};
        for(long j=0; j<n; j++){
            double t = this->get(i,j);
            double o = other.get(i,j);
            if(i==j){
                line.push_back(t);
            }
            else line.push_back(t+o);
        }
        res_mat.push_back(line);
    }
    return CSR(res_mat,0);
}


