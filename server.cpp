#include "server.h"

Server::Server(){}

void Server::startServer()
{
    if (this->listen(QHostAddress::Any,5555))
    {
        qDebug()<<"Listening";
    }
    else
    {
        qDebug()<<"Not Listening";
    }
}

void Server::incomingConnection(int socketDescriptor)
{
    socket=new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
   // QTcpSocket *sock=this->nextPendingConnection();
   // connect(sock,SIGNAL(readyRead()),this,SLOT(sockReady()));
   // connect(sock,SIGNAL(disconnected()),this,SLOT(sockDisc()));
   // sockets.enqueue(sock);

    connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));

    qDebug()<<socketDescriptor<<" Client connected";

    socket->write("You are connected");

    qDebug()<<"Send to client: status: connected";

}

void Server::sockReady()
{
    /*QTcpSocket *curr=sockets.dequeue();
    data=curr->readAll();
    qDebug()<<curr->socketDescriptor()<<data;
    curr->write("Ok");
    emit curr->disconnected();*/
}

void Server::sockDisc()
{
    qDebug()<<"Client is disconnected";
    socket->deleteLater();
}
