#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include "myTcpsocket.h"
#include "searchoutput.h"

namespace Ui {
class SearchWidget;
}

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = 0,MyTcpsocket *socket = 0);
    ~SearchWidget();

private slots:
    void on_search_BT_clicked();

private:
    Ui::SearchWidget *ui;
    MyTcpsocket *m_socket;
};

#endif // SEARCHWIDGET_H
