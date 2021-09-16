#ifndef ALGVECT_H
#define ALGVECT_H
#include <vector>
#include <initializer_list>
#include <iostream>
#include <QString>


using namespace std;
typedef vector<double> vect_double;

class AlgVect
{
private:
    unsigned int m_n=0;
    vect_double m_v={};

public:
    AlgVect(){}
    AlgVect(const vect_double &other):m_n(other.size()),m_v(other){}
    AlgVect(vect_double &&other);
    AlgVect(const AlgVect &other):m_n{other.m_n},m_v{other.m_v}{}
    AlgVect(const QString &input);
    AlgVect(AlgVect &&other):m_n{other.m_n},m_v{other.m_v}
    {
        other.m_n=0;
        other.m_v.clear();
        other.m_v.shrink_to_fit();
    }
    AlgVect(const initializer_list<double>& list):m_n(list.size())
    {
        vect_double temp(list);
        m_v=temp;
        m_n=temp.size();
    }

    ////////////////////////////////////////////////////////////////
    vect_double data()const;//getter
    unsigned int size()const;//to get size of vector
    void set(double num,unsigned int idx);//to set an element
    void changeCapacity(int capacity);//to increase the volume of the vector
    vect_double::iterator begin();
    vect_double::iterator end();
    bool continueProc(double eps);
    AlgVect cleanNum();
    /////////OPERATORS///////////////////////////////////////////////
    AlgVect& operator=(const AlgVect &v);
    AlgVect& operator=(AlgVect &&v);
    double operator*(const AlgVect &v)const;//scalar multipication
    double operator*(AlgVect &&v)const;//scalar multipication
    AlgVect operator+(const AlgVect &other);//summ in Decart's basis
    AlgVect operator+(AlgVect &&other);//summ in Decart's basis
    AlgVect operator-(const AlgVect &other);//subtraction
    AlgVect operator-(AlgVect &&other);//subtraction
    friend AlgVect operator*(double num,AlgVect v);//num multiplication
    friend AlgVect operator*(AlgVect v,double num);//the same
    friend ostream& operator<<(ostream &out, AlgVect v);
   // AlgVect operator-();
    //void print(const AlgVect& V);

    friend class Solver;
};

#endif // ALGVECT_H
