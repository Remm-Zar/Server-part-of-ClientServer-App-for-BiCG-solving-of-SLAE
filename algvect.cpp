#include "algvect.h"
#include <iostream>
#include <QFile>
#include <cmath>
#include <parallel/algorithm>
//#include <thread>


using namespace std;

typedef vector<double> vect_double;
AlgVect::AlgVect(const QString &input)
{
    QFile f(input);
    if (!f.open(QIODevice::ReadOnly))
    {
        cout<<"This file hasn't been found"<<endl;
    }
    else
    {
        QString s=f.readLine();
        QList<QString> nums=s.split(" ",QString::SkipEmptyParts);
        QList<QString>::Iterator it=nums.begin();
        while (it!=nums.end())
        {
            m_v.push_back(it->toDouble());
            ++it;
        }
        m_n=m_v.size();
    }
    f.close();
}

AlgVect::AlgVect(vect_double &&other)
{
  m_n=other.size();
  m_v=other;
  other.clear();
}
////////////////////////////////////////////////////////////////
vect_double AlgVect::data()const//getter
{
    return m_v;
}
unsigned int AlgVect::size()const//to get size
{
    return m_n;
}
void AlgVect::set(double num,unsigned int idx)//to set an element
{
    m_v[idx]=num;
}
void AlgVect::changeCapacity(int capacity)//increase the volume of the vector
{
    m_v.reserve(capacity);
}

vect_double::iterator AlgVect::begin()
{
    return m_v.begin();
}
vect_double::iterator AlgVect::end()
{
    return m_v.end();
}

bool AlgVect::continueProc(double eps)
{
    bool con_proc=true;
    vect_double::iterator it=m_v.begin();
    while (it!=m_v.end())
    {
        if (abs(*it)<eps)
        {
            con_proc=false;
            break;
        }
        ++it;
    }
    return con_proc;
}
AlgVect AlgVect::cleanNum()
{
    AlgVect copy(*this);
    vect_double::iterator it=copy.m_v.begin();
    while (it!=copy.m_v.end())
    {
        if (abs(*it)<0.00001)
        {
           *it=0;
        }
        ++it;
    }
    return copy;
}
/////////OPERATORS///////////////////////////////////////////////
AlgVect& AlgVect::operator=(const AlgVect &v)
{
    m_v=v.m_v;
    m_n=v.size();
    return *this;
}
AlgVect& AlgVect::operator=(AlgVect &&v)
{
    m_v=v.m_v;
    m_n=v.size();
    v.m_v.clear();
    v.m_v.shrink_to_fit();
    return *this;
}
double AlgVect::operator*(const AlgVect &v) const//scalar multipication
{
    unsigned int i=0;
    double res=0;
    while (i<m_n)
    {
        res+=v.m_v[i]*this->m_v[i];
        ++i;
    }
    return res;
}
double AlgVect::operator*(AlgVect &&v) const//scalar multipication
{
    unsigned int i=0;
    double res=0;
    while (i<m_n)
    {
        res+=v.m_v[i]*this->m_v[i];
        ++i;
    }
    return res;
}

/* AlgVect AlgVect::operator-()
{
    AlgVect temp;
    for (unsigned int i=0;i<m_n;++i)
    {
        temp.m_v[i]=-(this->m_v[i]);
    }
    temp.m_n=this->m_n;
    return temp;
}*/
AlgVect AlgVect::operator+(const AlgVect &other)//summ in Decart's basis
{
    AlgVect temp;
    temp.m_n=this->m_n;
    for (unsigned int i=0;i<m_n;++i)
    {
        temp.m_v.push_back(this->m_v[i]+other.m_v[i]);
    }
    return temp;
}
AlgVect AlgVect::operator+(AlgVect &&other)//summ in Decart's basis
{
    AlgVect temp;
    temp.m_n=this->m_n;
    for (unsigned int i=0;i<m_n;++i)
    {
        temp.m_v.push_back(this->m_v[i]+other.m_v[i]);
    }
    return temp;
}
AlgVect AlgVect::operator-(const AlgVect &other)//subtraction
{
    AlgVect temp;
    temp.m_n=this->m_n;
    for (unsigned int i=0;i<m_n;++i)
    {
        temp.m_v.push_back(this->m_v[i]-other.m_v[i]);
    }
    return temp;
}

AlgVect AlgVect::operator-(AlgVect &&other)//subtraction
{
    AlgVect temp;
    temp.m_n=this->m_n;
    for (unsigned int i=0;i<m_n;++i)
    {
        temp.m_v.push_back(this->m_v[i]-other.m_v[i]);
    }
    return temp;
}

AlgVect operator*(double num,AlgVect v)
{
    AlgVect temp(v);
    for (unsigned int i=0;i<v.size();++i)
    {
        temp.set(v.data()[i]*num,i);
    }
    return temp;
}
AlgVect operator*(AlgVect v,double num)
{
    return num*v;
}

ostream& operator<<(ostream &out,AlgVect v)
{
    vect_double::iterator it=v.begin();
    cout << "{";
    while (it!=v.end())
    {
        cout<<" "<<*it;
        ++it;
    }
    cout << "}" << endl;
    return out;
}

