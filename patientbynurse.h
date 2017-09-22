#ifndef PATIENTBYNURSE_H
#define PATIENTBYNURSE_H

#include <QWidget>
#include <QMessageBox>
#include "myTcpsocket.h"

namespace Ui {
class PatientByNurse;
}

class PatientByNurse : public QWidget
{
    Q_OBJECT

public:
    explicit PatientByNurse(QWidget *parent = 0,MyTcpsocket *socket = 0,QString patientName = 0,QString patientAge = 0,QString patientSex = 0);
    ~PatientByNurse();

private slots:
    void on_confirm_BT_clicked();

private:
    Ui::PatientByNurse *ui;

    MyTcpsocket *m_socket;

    QStringList *getInfoFromReturnString(QString returnStr);
};

#endif // PATIENTBYNURSE_H
