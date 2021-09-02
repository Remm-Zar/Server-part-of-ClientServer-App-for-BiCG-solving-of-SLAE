#include "server.h"
#include <QDataStream>
#include <QTime>
#include <QFile>

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

    qDebug()<<"Send to client: status: connected";

}

void Server::sockReady()
{
    qDebug()<<"Status: readyRead";
    QTcpSocket *clientSocket=m_socket;/*(QTcpSocket*)sender();*/
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_9);
    if (!m_nextBlockSize)
    {
        if (m_socket->bytesAvailable()<sizeof(quint16))
        {
            qDebug()<<"1:"<<m_socket->bytesAvailable();
            return;
        }
        in>>m_nextBlockSize;
    }
    if (m_socket->bytesAvailable()<m_nextBlockSize)
    {
        qDebug()<<"2";
        return;
    }
    m_data=m_socket->readAll();

    double *N;
    N=new double[m_data.size()/sizeof (double)];
    memcpy(N,m_data.data(),m_data.size());
    for (unsigned int i=0;i<m_data.size()/sizeof (double);++i)
    {
        qDebug()<<N[i]<<" ";
    }
    delete N;
    sendToClient(m_socket);

}

void Server::sendToClient(QTcpSocket *pSocket)
{
    pSocket=m_socket;
    qDebug()<<"Status: sendToClient: "<<m_data.toStdString().c_str();
    QString fileName="sys3.txt";
    QFile file("C:/Users/Hp/Desktop/QT projects/"+fileName);
    if (!file.open(QIODevice::ReadWrite))
    {
        qDebug()<<"File error";
        return;
    }
    file.write(m_data);
    file.seek(0);
    QByteArray arr;
    QDataStream out(&arr,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out<<quint16{0};
    QByteArray q=file.readAll();
    file.close();
    arr.append(q);
    out.device()->seek(0);
    out<<quint16(arr.size()-sizeof(quint16));
    qint64 x=0;
    while (x<arr.size())
    {
        qint64 y=m_socket->write(arr);
        x+=y;
    }
}

void Server::sockDisc()
{
    qDebug()<<"Client is disconnected";
    m_socket->deleteLater();
}
