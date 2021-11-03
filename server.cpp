#include "server.h"
#include "solver.h"
#include "algvect.h"
#include <QDataStream>
#include <QTime>
#include <QFile>
#include <QSignalMapper>

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
    QTcpSocket *m_socket=new QTcpSocket;
    m_socket=this->nextPendingConnection();
    QSignalMapper mapper(this);
    connect(&mapper,SIGNAL(mapped(int)),this,SLOT(sockDisc(int)));
    int disc=m_socket->socketDescriptor();
    SClients.insert(disc,m_socket);
    connect(SClients[disc],SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(SClients[disc],SIGNAL(disconnected()),&mapper,SLOT(map()));
    mapper.setMapping(SClients[disc],disc);


    qDebug()<<disc<<" Client connected";
}

void Server::sockReady()
{
    qDebug()<<"Status: reading of data";;
  // QTcpSocket *clientSocket=m_socket;/*(QTcpSocket*)sender();*/
    foreach(int i,SClients.keys())
    {
        QDataStream in(SClients[i]);
        in.setVersion(QDataStream::Qt_5_9);
        qDebug()<<"recieved data: "<<SClients[i]->size()<<" bytes";
        if (!m_nextBlockSize)
        {
            if (SClients[i]->bytesAvailable()<sizeof(quint64))
            {
                qDebug()<<"1:"<<SClients[i]->bytesAvailable();
                return;
            }
            in>>m_nextBlockSize;
        }
        if (SClients[i]->bytesAvailable()<m_nextBlockSize)
        {
            qDebug()<<"2";
            return;
        }
        qint64 matSize;
        in>>matSize;
        m_mat=SClients[i]->read(matSize);
        m_vec=SClients[i]->readAll();
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
            sendToClient(SClients[i]);
        }
        m_mat.clear();
        m_vec.clear();
//       SClients[i]->close();
//        //emit SClients[i]->disconnected();
        //SClients.remove(i);
    }

}

void Server::sendToClient(QTcpSocket *pSocket)
{

    QString fNameMat="C:/Users/Hp/Desktop/QT projects/ServerApp/Mat.txt",fNameVec="C:/Users/Hp/Desktop/QT projects/ServerApp/Vec.txt";
    QByteArray arr,q;
    qDebug()<<"Status: solving the task...";
    Solver s(fNameMat,fNameVec);
    double t=-1;
    QTime start=QTime::currentTime();
    AlgVect res=s.solve(4,t);
    QTime end=QTime::currentTime(),diff=end.addMSecs(-start.msec());
    for (unsigned int i=0;i<res.size();++i)
    {
        q.append(QString::number(res.data().at(i)).toStdString().c_str());
        q.append("  ");
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
        qint64 y=pSocket->write(arr);
        x+=y;
    }
    m_nextBlockSize=0;
    qDebug()<<"Done\nListening...";
   // pSocket->close();
    //emit SClients[i]->disconnected();
}

void Server::sockDisc(int key)
{
    qDebug()<<"\nClient "<<key<<" is disconnected";
    SClients[key]->close();
    //SClients[key]->deleteLater();
}
