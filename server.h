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

    QTcpSocket *m_socket;
    int m_port;
 ///   QQueue<QTcpSocket*> sockets;
    QByteArray m_data;
    quint16 m_nextBlockSize;

    Server(int port);
    void startServer();

private:
    void sendToClient(QTcpSocket *pSocket,const QString str);

public slots:

    void slotNewConnection();
    void sockReady();
    void sockDisc();

};

#endif // SERVER_H
