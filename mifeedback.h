#ifndef MIFEEDBACK_H
#define MIFEEDBACK_H

#include <QWidget>
#include <QMessageBox>

#include "myTcpsocket.h"

namespace Ui {
class MIfeedback;
}

class MIfeedback : public QWidget
{
    Q_OBJECT

public:
    explicit MIfeedback(QWidget *parent = 0,QString feedbackInfo = 0,MyTcpsocket *socket = 0);
    ~MIfeedback();

private slots:
    void on_feedback_View_doubleClicked(const QModelIndex &index);

private:
    Ui::MIfeedback *ui;
    QStandardItemModel *getModel(QString returnData, QStringList *list);
    MyTcpsocket *m_socket;
};

#endif // MIFEEDBACK_H
