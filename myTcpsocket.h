#ifndef MY_TCPSOCKET_H
#define MY_TCPSOCKET_H

#include <QTcpSocket>
#include <QDebug>
#include <QStandardItemModel>
#include <QHostAddress>
#include <QThread>

class MyTcpsocket:public QObject{
    Q_OBJECT
public:
    explicit MyTcpsocket();

    ~MyTcpsocket();

    void writeData(QString message);

    QString getData();

    void waitData();

    bool isconnected();

private slots:

    void errorSlot(QAbstractSocket::SocketError error);

    void receiveData();

    void connectSlot();

signals:
    void dataComming(QString);
    void doctorFeedbackComming(QString);
    void miFeedbackComming(QString);

private:
    QTcpSocket *m_socket;
    QString str;
    QString returnStr;
    bool conState;
};

#endif
