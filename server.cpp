#include "server.h"
#include <QDataStream>
#include <QTime>
#include <QSignalMapper>

Server::Server(int port):m_port(port),m_nextBlockSize(0){}

void Server::startServer()
{
    if (this->listen(QHostAddress::Any,m_port))
    {
        qDebug()<<"Listening...";
        m_start=QTime::currentTime();
        m_timer=new QTimer;
        connect(m_timer,SIGNAL(timeout()),this,SLOT(sendToClient()));
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
    QSignalMapper mapper(this);
    connect(&mapper,SIGNAL(mapped(int)),this,SLOT(sockDisc(int)));
    int disc=m_socket->socketDescriptor();
    SClients.insert(disc,m_socket);
   // connect(SClients[disc],SIGNAL(readyRead()),this,SLOT(sockReady()));
   //connect(SClients[disc],SIGNAL(disconnected()),this,SLOT(sockDisc()));
    connect(SClients[disc],SIGNAL(disconnected()),&mapper,SLOT(map()));
    mapper.setMapping(SClients[disc],disc);
  //  connect(m_timer,SIGNAL(timeout()),this,SLOT(sendToClient()));
  //  m_timer->start(2000);

    qDebug()<<disc<<" Client connected";
}


void Server::sendToClient()
{
    foreach(int i,SClients.keys())
    {
        if (!SClients[i]->isOpen())
        {
            SClients.remove(i);
            --numClients;
        }
        else
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
                qint64 y=SClients[i]->write(arr);
                x+=y;
            }
            m_nextBlockSize=0;
            qDebug()<<"Done\nListening...";
           // pSocket->close();
            //emit SClients[i]->disconnected();
          //  SClients[i]->close();
           //        //emit SClients[i]->disconnected();
                   //SClients.remove(i);
        }

    }

}

void Server::sockDisc(int key)
{
  //  QTcpSocket *clientSocket=(QTcpSocket*)sender();
    qDebug()<<"\nClient is disconnected";
    SClients[key]->close();
    SClients.remove(key);
    //SClients[key]->deleteLater();
}

void Server::serverClose()
{

}
