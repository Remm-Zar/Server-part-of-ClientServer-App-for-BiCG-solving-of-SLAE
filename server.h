#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>

class Server: public QTcpServer
{
    Q_OBJECT
public:

    QMap<int,QTcpSocket*> SClients;
    int m_port;
 ///   QQueue<QTcpSocket*> sockets;
    QByteArray m_mat;
    QByteArray m_vec;
    qint64 m_nextBlockSize;

    Server(int port);
    void startServer();

private:
    void sendToClient(QTcpSocket *pSocket);

public slots:

    void slotNewConnection();
    void sockReady();
    void sockDisc(int key);

};

#endif // SERVER_H
