#ifndef CSR_H
#define CSR_H

#endif // CSR_H
#pragma once
#include <vector>
#include <iostream>
#include <QString>
#include "algvect.h"
#include <initializer_list>

using namespace std;

typedef vector<double> vect_double;
typedef vector<long> vect;
typedef vector<vector<double>> mat;

class CSR {
    /*==== http://window.edu.ru/resource/183/41183/files/nstu144.pdf (6 page) ====*/

    vect_double A={};//non-zero elements vector
    vect IA = {0}; //an array denoting the number of non-zero elements above the i-th line
    vect JA={}; //an array of j-indexes corresponding to elements from the array A
    long n=0;//amount of lines
    long m=0;//amount of columns
    long nz=0;//amount of non-zero elements
    //it's needed for precondition

public:
    //CSR():A({}),IA({0}),JA({}),n(0){};
    CSR(const mat& M,bool forPrecond);
    vect_double diag={};//diagonal of reversed elements
    vect Idiag={0};
    vect Jdiag={};

    CSR(mat&& M,bool forPrecond);
    CSR(const CSR& M);
    CSR(CSR&& M);
    CSR(const initializer_list<vect_double>& list);
    CSR(const QString& input);
    //THIS IS ONLY FOR SOLVER USE!!! (костыль)
    CSR(const vect_double& out_A={}, const vect &out_IA={}, const vect &out_JA={},const  vect_double& out_d={},const  vect &out_Id={},const  vect &out_Jd={});
    ~CSR();

    /*====GETTERS=====*/
    vect_double get_A();
    vect get_IA();
    vect get_JA();
    long get_n()
    {
        return n;
    }
    long get_m()
    {
        return m;
    }
    long get_nz()
    {
        return nz;
    }
    double get(long x, long y); //геттер для координаты(работает достаточно быстро)
    CSR T()const;


    void printMatrix(); //печать матрицы по массивам A,IA,JA и в виде простой матрицы

    //Multiply functions
    AlgVect Multiply(const AlgVect& x);
    CSR Multiply(const CSR& M);

    CSR operator+(CSR& other);

    //Assignment operators(for convenience)
    CSR& operator=(const CSR& other);
    CSR& operator=(const CSR&& other);
    friend class Solver;
};
