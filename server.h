#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QTime>
#include <QCoreApplication>

class Server: public QTcpServer
{
    Q_OBJECT
public:

    QList<QTcpSocket*> SClients;
    int m_port;
    qint64 m_nextBlockSize;
    int timesWaitingConnection=0;
    QCoreApplication *m_app;
    Server(int port,QCoreApplication *app);
    void startServer();

private:

    QTimer *m_timer;
    QTime m_start;
    static int numClients;

signals:
    void closeConnection();

public slots:
    void sendToClient();
    void slotNewConnection();
    void sockDisc();
    void serverClose();

};

#endif // SERVER_H
