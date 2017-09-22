#ifndef DOCTOR_FEEDBACK_H
#define DOCTOR_FEEDBACK_H

#include <QWidget>
#include <QStandardItemModel>

#include "myTcpsocket.h"

namespace Ui {
class DoctorFeedback;
}

class DoctorFeedback : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorFeedback(QWidget *parent = 0,QString feedbackInfo = 0,MyTcpsocket *socket = 0);
    ~DoctorFeedback();

private slots:
    void on_confirm_BT_clicked();

private:
    Ui::DoctorFeedback *ui;
    QStringList *getInfoFromReturnString(QString returnStr);
    QStandardItemModel *getModel(QString returnData, QStringList *list);
    MyTcpsocket *m_socket;
};

#endif // DoctorFeedback
