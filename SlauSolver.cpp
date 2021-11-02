#include "SlauSolver.h"
#include <omp.h>
#include <cmath>
#include <QDebug>


double CSlauSolver::Dot(double *a, double *b, long n)
{
	double res = 0;
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for reduction(+:res)
    for (long i = 0; i < n; i++)
	{
		res += a[i] * b[i];
	}
}
	return res;

}

void CSlauSolver::Sum(double *a, double *b, long n, double alfa)
{
//#pragma omp parallel for
    for (long i = 0; i < n; i++)
	{
		a[i] += b[i] * alfa;
	}
}

void CSlauSolver::Sum(double *a, double *b, double * res, long n, double alfa)
{
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for
    for (long i = 0; i < n; i++)
	{
		res[i] = a[i] + b[i] * alfa;
	}
}
}


void CSlauSolver::Diff(double *a, double *b, long n)
{
    for (long i = 0; i < n; i++)
	{
		a[i] -= b[i];
	}
}

void CSlauSolver::Mult(CRSMatrix & A, double * b, double * res)
{
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for
    for (long i = 0; i < A.n; i++)
	{
        long elemCountInRow = A.rowPtr[i + 1] - A.rowPtr[i];
		res[i] = 0;

        for (long j = 0; j < elemCountInRow; j++)
		{
			res[i] += A.val[A.rowPtr[i] + j] * b[A.colIndex[A.rowPtr[i] + j]];
		}
	}
}
}

void CSlauSolver::SolveR(CRSMatrix & A, double * z, double * b, double * r, double alfa)
{
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for
    for (long i = 0; i < A.n; i++)
	{
        long elemCountInRow = A.rowPtr[i + 1] - A.rowPtr[i];
		double res = 0;

        for (long j = 0; j < elemCountInRow; j++)
		{
			res += A.val[A.rowPtr[i] + j] * z[A.colIndex[A.rowPtr[i] + j]] * alfa;
		}

		r[i] = b[i] + res;
	}
}
}

void CSlauSolver::SolveRWithResolveX(CRSMatrix & A, double * z, double * b, double * r, double * x, double * p, double alfa)
{
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for
    for (long i = 0; i < A.n; i++)
	{
        long elemCountInRow = A.rowPtr[i + 1] - A.rowPtr[i];
		double res = 0;

        for (long j = 0; j < elemCountInRow; j++)
		{
			res += A.val[A.rowPtr[i] + j] * z[A.colIndex[A.rowPtr[i] + j]] * alfa;
		}

		r[i] = b[i] + res;
		x[i] = x[i] - p[i] * alfa;
	}
}
}

void CSlauSolver::SolveRT(CRSMatrix & A, double * z, double * b, double * r, double alfa)
{
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for
    for (long i = 0; i < A.m; i++)
	{
		double res = 0;
        for (unsigned long j = 0; j < A.colIndex.size(); j++)
		{
			if (A.colIndex[j] == i)
			{
				res += alfa * A.val[j] * z[GetRowIndex(A, j)];
			}
		}

		r[i] = b[i] + res;
	}
}
}

int CSlauSolver::GetRowIndex(CRSMatrix & A, long index)
{
    unsigned long res;
    for (unsigned long i = 0; i < A.rowPtr.size() - 1; i++)
	{
		if (index >= A.rowPtr[i] && index < A.rowPtr[i + 1])
		{
			res = i;
			break;
		}
	}

	return res;
}

void  CSlauSolver::GenerateSolution(double * x, long n)
{
    for (long i = 0; i < n; i++)
	{
		x[i] = 1;
	}
}

void CSlauSolver::Copy(double * a, double * copyA, long n)
{
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for
    for (long i = 0; i < n; i++)
	{
		copyA[i] = a[i];
	}
}
}

bool CSlauSolver::IsEnd(double * x0, double * x, long n, double eps)
{
    for (long i = 0; i < n; i++)
	{
		if (abs(x0[i] - x[i]) > eps)
		{
			return false;
		}
	}

	return true;
}

bool CSlauSolver::IsEnd(double * x, long n, double eps)
{
	double norma = 0;
    for (long i = 0; i < n; i++)
	{
		norma += x[i] * x[i];
	}

	norma = sqrt(norma);
	return norma < eps;
}

double CSlauSolver::GetAlfaAndCopyPredArrays(CRSMatrix & A, double * r, double * r_sop, double * p, double * p_sop, double * temp, double * predR, double * predR_sop, long n)
{
	double alfa1 = 0;
	double alfa2 = 0;
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for reduction(+:alfa1, alfa2)
    for (long i = 0; i < A.n; i++)
	{
		predR[i] = r[i];
		predR_sop[i] = r_sop[i];

        long elemCountInRow = A.rowPtr[i + 1] - A.rowPtr[i];
		temp[i] = 0;
        for (long j = 0; j < elemCountInRow; j++)
		{
			temp[i] += A.val[A.rowPtr[i] + j] * p[A.colIndex[A.rowPtr[i] + j]];
		}

		alfa1 += (r[i] * r_sop[i]);
		alfa2 += (temp[i] * p_sop[i]);
	}
}

	return alfa1 / alfa2;
}

double CSlauSolver::GetBetta(double * r, double * r_sop, double * predR, double * predR_sop, long n)
{
	double betta1 = 0;
	double betta2 = 0;
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for reduction(+:betta1, betta2)
    for (long i = 0; i < n; i++)
	{
		betta1 += r[i] * r_sop[i];
		betta2 += predR[i] * predR_sop[i];
	}
}
	return betta1 / betta2;
}

void CSlauSolver::ResolvePandPSop(double * p, double * p_sop, double * r, double * r_sop, long n, double betta)
{
    omp_set_dynamic(0);
#pragma omp parallel num_threads(numThreads)
{
#pragma omp for
    for (long i = 0; i < n; i++)
	{
		p[i] = r[i] + p[i] * betta;
		p_sop[i] = r_sop[i] + p_sop[i] * betta;
	}

}
}

double CSlauSolver::SLE_Solver_CRS_BICG(CRSMatrix & A, double * b, double eps, long max_iter, double * x, long & count)
{
    long n = A.n;

	double * r = new double[n]();
	double * p = new double[n]();
	double * r_sop = new double[n]();
	double * p_sop = new double[n]();
	double * temp = new double[n]();
	double * predR_sop = new double[n]();
	double * predR = new double[n]();

	double alfa, betta;

	GenerateSolution(x, n); // начальное приближение
	SolveR(A, x, b, r, -1); // начальное r

	Copy(r, p, n);
	Copy(r, r_sop, n);
	Copy(r, p_sop, n);

	do
	{
        count++;
        qDebug()<<"Iter: "<<count;

		//Copy(r, predR, n);
		//Copy(r_sop, predR_sop, n);

		//Mult(A, p, temp);
		//alfa = Dot(r, r_sop, n) / Dot(temp, p_sop, n);

		alfa = GetAlfaAndCopyPredArrays(A, r, r_sop, p, p_sop, temp, predR, predR_sop, n);

		//Sum(x, p, x, n, alfa); // пересчт x

		//SolveR(A, p, r, r, -alfa);  // пересчт r
		SolveRWithResolveX(A, p, r, r, x, p, -alfa);  // пересчт r
		SolveRT(A, p_sop, r_sop, r_sop, -alfa);  // пересчт r_sop

		//betta = Dot(r, r_sop, n) / Dot(predR, predR_sop, n);
		betta = GetBetta(r, r_sop, predR, predR_sop, n);

        if (abs(betta) < 0.00000000001 || IsEnd(r, n, eps))
			break;

		if (count >= max_iter)
			break;

		//Sum(r, p, p, n, betta); // пересчт p
		//Sum(r_sop, p_sop, p_sop, n, betta); // пересчт p_sop

		ResolvePandPSop(p, p_sop, r, r_sop, n, betta); // пересчт p и p_sop

	} while (true);
    delete[] r;
    delete[] p;
    delete[] temp;
    delete[] predR;
    delete[] r_sop;
    delete[] p_sop;
    delete[] predR_sop;
}

