#include <QCoreApplication>
#include "server.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server s;
    s.startServer();
   /* QQueue<int> q;
    for (int i=0;i<5;++i)
    {
        q.push_back(i);
    }
    for (int i=0;i<5;++i)
    {
        cout<<q[i]<<endl;
    }
    cout<<endl;
    int b=q.dequeue();
    cout<<b<<endl;
    for (int i=0;i<4;++i)
    {
        cout<<q[i];
    }*/
    return a.exec();
}
