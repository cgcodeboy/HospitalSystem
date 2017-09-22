#ifndef SEARCHOUTPUT_H
#define SEARCHOUTPUT_H

#include <QWidget>
#include <QRegExp>
#include "myTcpsocket.h"

namespace Ui {
class SearchOutput;
}

class SearchOutput : public QWidget
{
    Q_OBJECT

public:
    explicit SearchOutput(QWidget *parent = 0,MyTcpsocket *socket = 0,QString str = 0);
    ~SearchOutput();

private:
    Ui::SearchOutput *ui;
    MyTcpsocket *m_socket;

    QStandardItemModel *getModel(QString returnData,QStringList *list);
};

#endif // SEARCHOUTPUT_H
