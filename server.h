#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QQueue>

class Server: public QTcpServer
{
    Q_OBJECT
public:

    QTcpSocket *socket;
 //   QQueue<QTcpSocket*> sockets;
    QByteArray data;

    Server();
public slots:
    void startServer();
    void incomingConnection(int socketDescriptor);
    void sockReady();
    void sockDisc();

};

#endif // SERVER_H
