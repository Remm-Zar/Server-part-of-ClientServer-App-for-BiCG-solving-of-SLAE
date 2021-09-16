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
typedef vector<int> vect;
typedef vector<vector<double>> mat;

class CSR {
    /*==== http://window.edu.ru/resource/183/41183/files/nstu144.pdf (6 page) ====*/

    vect_double A={};//non-zero elements vector
    vect IA = {0}; //an array denoting the number of non-zero elements above the i-th line
    vect JA={}; //an array of j-indexes corresponding to elements from the array A
   // int* n=nullptr; //the horizontal length of the matrix (a special pointer to get rid of any data when deconstructing the CSR matrix)
    int n=0;
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
    CSR(const QString& matrixFile);
    //THIS IS ONLY FOR SOLVER USE!!! (костыль)
    CSR(const vect_double& out_A={}, const vect &out_IA={}, const vect &out_JA={},const  vect_double& out_d={},const  vect &out_Id={},const  vect &out_Jd={});
    ~CSR();

    /*====GETTERS=====*/
    vect_double get_A();
    vect get_IA();
    vect get_JA();
    double get(int x, int y); //геттер для координаты(работает достаточно быстро)
    CSR T()const;


    void printMatrix(); //печать матрицы по массивам A,IA,JA и в виде простой матрицы

    //Multiply functions
    vect_double Multiply(const vect_double& x);
    CSR Multiply(const CSR& M);

    CSR operator+(CSR& other);

    //Assignment operators(for convenience)
    CSR& operator=(const CSR& other);
    CSR& operator=(const CSR&& other);
    friend class Solver;
};
