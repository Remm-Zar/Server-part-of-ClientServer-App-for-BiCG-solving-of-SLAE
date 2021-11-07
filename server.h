#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QTime>

class Server: public QTcpServer
{
    Q_OBJECT
public:

    QMap<int,QTcpSocket*> SClients;
    int m_port;
    qint64 m_nextBlockSize;

    Server(int port);
    void startServer();

private:

    QTimer *m_timer;
    QTime m_start;
    int numClients=0;

public slots:
    void sendToClient();
    void slotNewConnection();
    void sockDisc(int key);
    void serverClose();

};

#endif // SERVER_H
