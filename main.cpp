#include <QCoreApplication>
#include "server.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server s(5555,&a);
    s.startServer();
    return a.exec();
}
