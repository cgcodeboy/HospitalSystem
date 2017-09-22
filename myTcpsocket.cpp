#include "myTcpsocket.h"

MyTcpsocket::MyTcpsocket()
{
    this->str = "";
    this->m_socket = new QTcpSocket;
    m_socket->connectToHost(QHostAddress::LocalHost,5555);
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(disconnectSlot()));
    connect(m_socket,SIGNAL(readyRead()),this,SLOT(receiveData()));
    connect(m_socket,SIGNAL(QAbstractSocket::SocketError),this,SLOT(errorSlot(QAbstractSocket::SocketError)));
    connect(m_socket,SIGNAL(connected()),this,SLOT(connectSlot()));
    conState = false;
}

MyTcpsocket::~MyTcpsocket()
{

}

void MyTcpsocket::writeData(QString message)
{
    qDebug()<<message;
    message.toLocal8Bit();
    if(!m_socket->isValid())
    {
        qDebug()<<"invalid";
        return;
    }
    if(m_socket->isWritable())
        this->m_socket->write(message.toLocal8Bit());
}

QString MyTcpsocket::getData()
{
    return this->returnStr;
}

void MyTcpsocket::waitData()
{
    this->returnStr = "";
    m_socket->waitForReadyRead(100);
}

bool MyTcpsocket::isconnected()
{
    return this->conState;
}

void MyTcpsocket::receiveData()
{  
    str = QString::fromLocal8Bit(m_socket->readAll());
    if(str.left(1) != "$")
    {
        this->returnStr = str;
    }
    else
    {
        QString cur = str.remove(0,1);
        int index = cur.indexOf("#");
        QString checkStr = cur.left(index);
        qDebug()<<checkStr;
        QRegExp rexp("[0-9]{4}-[0-9]{2}-[0-9]{2}");
        if(rexp.exactMatch(checkStr))
        {
            qDebug()<<"match mi";
            emit miFeedbackComming(cur);
        }
        else
        {
            qDebug()<<"match doc";
            emit doctorFeedbackComming(cur);
        }
    }
}

void MyTcpsocket::connectSlot()
{
    qDebug()<<"connected!";
    this->conState = true;
}

void MyTcpsocket::errorSlot(QAbstractSocket::SocketError error)
{
    qDebug()<<error;
}
