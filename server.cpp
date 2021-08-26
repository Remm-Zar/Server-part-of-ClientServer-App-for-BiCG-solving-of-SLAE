#include "server.h"
#include <QDataStream>
#include <QTime>

Server::Server(int port):m_port(port),m_nextBlockSize(0){}

void Server::startServer()
{
    if (this->listen(QHostAddress::Any,m_port))
    {
        qDebug()<<"Listening...";
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
    m_socket=this->nextPendingConnection();
   // m_socket=new QTcpSocket(this);
    //m_socket->setSocketDescriptor(socketDescriptor);
   // QTcpSocket *sock=this->nextPendingConnection();
   // connect(sock,SIGNAL(readyRead()),this,SLOT(sockReady()));
   // connect(sock,SIGNAL(disconnected()),this,SLOT(sockDisc()));
   // sockets.enqueue(sock);

    connect(m_socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));

    qDebug()<<m_socket->socketDescriptor()<<" Client connected";

    sendToClient(m_socket,"status: connected");

    qDebug()<<"Send to client: status: connected";

}

void Server::sockReady()
{
    qDebug()<<"Status: readyRead";
    QTcpSocket *clientSocket=m_socket;/*(QTcpSocket*)sender();*/
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_9);
    for (;;)
    {
        if (!m_nextBlockSize)
        {
            if  (clientSocket->bytesAvailable()<sizeof (quint16))
            {
               // qDebug()<<sizeof (quint16)<<" Status 1: too small message";
                break;
            }
            in>>m_nextBlockSize;
           // qDebug()<<sizeof (quint16)<<" "<<m_nextBlockSize;
        }
        if (clientSocket->bytesAvailable()<m_nextBlockSize)
        {
           // qDebug()<<sizeof (quint16)<<" "<<m_nextBlockSize<<" "<<clientSocket->bytesAvailable()<<"Status 2: too small message";
            break;
        }
        in>>m_data;
        qDebug()<<m_data;
        m_nextBlockSize=0;
        sendToClient(clientSocket,m_data);
    }
}

void Server::sendToClient(QTcpSocket *pSocket,const QString str)
{
    qDebug()<<"Status: sendToClient: "<<m_data.toStdString().c_str();
     QByteArray arr;
     QDataStream out(&arr,QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_5_9);
     out<<quint16{0}<<QTime::currentTime()<<": You sent: "<<m_data;
     out.device()->seek(0);

     out<<quint16(arr.size()-sizeof(quint16));
     qDebug()<<quint16(arr.size()-sizeof(quint16))<<" "<<arr;
     pSocket->write(arr);
}

void Server::sockDisc()
{
    qDebug()<<"Client is disconnected";
    m_socket->deleteLater();
}
