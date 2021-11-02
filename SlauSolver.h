#include <vector>
using namespace std;

struct CRSMatrix
{
    long n; // Число строк в матрице
    long m; // Число столбцов в матрице
    long nz; // Число ненулевых элементов в разреженной матрице
	vector<double> val; // Массив значений матрицы по строкам 
    vector<long> colIndex; // Массив номеров столбцов
    vector<long> rowPtr; // Массив индексов начала строк
};

class CSlauSolver {
public:
    CSlauSolver(int threads)
    {
        numThreads=threads;
    }
    int numThreads=1;
    double SLE_Solver_CRS_BICG(CRSMatrix & A, double * b, double eps, long max_iter, double * x, long & count);

    double Dot(double *a, double *b, long n);

	// Сумма векторов с записью в первый вектор
    void Sum(double *a, double *b, long n, double alfa = 1);
	
	// Сумма векторов с записью в другой вектор
    void Sum(double *a, double *b, double * res,  long n, double alfa = 1);

	// Разность векторов с записью в первый вектор
    void Diff(double *a, double *b, long n);

	void Mult(CRSMatrix & A, double * b, double * res);

	// Решение выражения вида Rk = Bk + alfa*A*Zk-1
	void SolveR(CRSMatrix & A, double * z, double * b, double * r, double alfa = 1);
	
	void SolveRWithResolveX(CRSMatrix & A, double * z, double * b, double * r, double * x, double * p, double alfa = 1);

	// Решение выражения вида Rk = Bk + alfa*A_transp*Zk-1
	void SolveRT(CRSMatrix & A, double * z, double * b, double * r, double alfa = 1);
	
	// Нахождение номера строки по индексу элемента в массиве val
    int GetRowIndex(CRSMatrix & A, long index);

    double GetAlfaAndCopyPredArrays(CRSMatrix & A, double * r, double * r_sop, double * p, double * p_sop, double * temp, double * predR, double * predR_sop, long n);

    double GetBetta(double * r, double * r_sop, double * predR, double * predR_sop, long n);

    void ResolvePandPSop(double * p, double * p_sop, double * r, double * r_sop, long n, double betta);

private: 
    void GenerateSolution(double * x, long n);
    void Copy(double * a, double * copyA, long n);
    bool IsEnd(double * x0, double * x, long n, double eps);
    bool IsEnd(double * x, long n, double eps);
};

