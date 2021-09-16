#include "server.h"
#include "solver.h"
#include "algvect.h"
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

}

void Server::sockReady()
{
    qDebug()<<"Status: reading of data";;
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
    qint64 matSize;
    in>>matSize;
    m_mat=m_socket->read(matSize);
    m_vec=m_socket->readAll();
    QFile m("C:/Users/Hp/Desktop/QT projects/ServerApp/Mat.txt"),v("C:/Users/Hp/Desktop/QT projects/ServerApp/Vec.txt");
    if (!m.open(QIODevice::WriteOnly)||!v.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Can't open file";
    }
    else
    {
        m.write(m_mat);
        m.close();
        v.write(m_vec);
        v.close();
        sendToClient(m_socket);
    }

}

void Server::sendToClient(QTcpSocket *pSocket)
{
    pSocket=m_socket;
    QString fNameMat="C:/Users/Hp/Desktop/QT projects/ServerApp/Mat.txt",fNameVec="C:/Users/Hp/Desktop/QT projects/ServerApp/Vec.txt";
    QByteArray arr,q;
    CSR A(fNameMat);
    AlgVect v(fNameVec),res;
    qDebug()<<"Status: solving the task...";
    Solver s(A,v);
    QTime time=QTime::currentTime();
    res=s.solve();
    QTime diff=QTime::currentTime().addMSecs(-time.msec());
    for (unsigned int i=0;i<res.size();++i)
    {
        q.append(QString::number(res.data().at(i)).toStdString().c_str());
        q.append(" ");
    }
    q.append("\n");
    qDebug()<<"Sending the answer...";
    QDataStream out(&arr,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out<<quint64{0}<<diff;
    arr.append(q);
    out.device()->seek(0);
    out<<quint64(arr.size()-sizeof(quint64));
    qint64 x=0;
    while (x<arr.size())
    {
        qint64 y=m_socket->write(arr);
        x+=y;
    }

    qDebug()<<"Done\nListening...";
}

void Server::sockDisc()
{
    qDebug()<<"Client "<<m_socket->socketDescriptor()<<" is disconnected";
    m_socket->deleteLater();
}
