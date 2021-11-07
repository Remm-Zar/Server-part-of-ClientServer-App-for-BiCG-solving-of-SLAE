#include "server.h"
#include <QDataStream>
#include <QTime>
#include <QSignalMapper>
#include <algorithm>

int Server::numClients=0;
Server::Server(int port,QCoreApplication *app):m_port(port),m_nextBlockSize(0),m_app(app){}

void Server::startServer()
{
    if (this->listen(QHostAddress::Any,m_port))
    {
        qDebug()<<"Listening...";
        m_start=QTime::currentTime();
        m_timer=new QTimer;
        connect(m_timer,SIGNAL(timeout()),this,SLOT(sendToClient()));
       // connect(this,SIGNAL(closeConnection()),this,SLOT(sockDisc()));
        m_timer->start(2000);
    }
    else
    {
        qDebug()<<"Not Listening"+this->errorString();
        this->close();
        return;
    }
    connect(this,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
}

void Server::slotNewConnection()
{
    ++numClients;
    QTcpSocket *m_socket=new QTcpSocket;
    m_socket=this->nextPendingConnection();
    int disc=m_socket->socketDescriptor();
    SClients.push_back(m_socket);
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));

    qDebug()<<disc<<" Client connected";
}


void Server::sendToClient()
{
    qDebug()<<"Amount of clients: "<<numClients<<"\n";
    if (numClients==0)
    {
        ++timesWaitingConnection;
        if (timesWaitingConnection==10)
        {
            qDebug()<<"Timeout\nExit";
            m_app->exit();
        }
    }
   else
    {
        timesWaitingConnection=0;
        QList<QTcpSocket*>::iterator it=SClients.begin();
        while(it!=SClients.end())
        {
            QString str="LOL";
            QByteArray arr;
            //QTime time=QTime::currentTime();
            qDebug()<<"Sending the answer...";
            QDataStream out(&arr,QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_9);
            out<<quint64{0}<<str;
            out.device()->seek(0);
            out<<quint64(arr.size()-sizeof(quint64));
            qint64 x=0;
            while (x<arr.size())
            {
                    qint64 y=dynamic_cast<QTcpSocket*>(*it)->write(arr);
                    x+=y;
            }
            ++it;
            m_nextBlockSize=0;
            qDebug()<<"Done\nListening...";
        }
    }
}



void Server::sockDisc()
{
    QTcpSocket *s=(QTcpSocket*)QObject::sender();
    SClients.removeOne(s);
    --numClients;
    qDebug()<<"\nClient is disconnected";
}

void Server::serverClose()
{

}
