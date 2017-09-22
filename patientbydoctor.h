#ifndef PATIENTBYDOCTOR_H
#define PATIENTBYDOCTOR_H

#include <QWidget>
#include <QDate>
#include "myTcpsocket.h"

namespace Ui {
class PatientByDoctor;
}

class PatientByDoctor : public QWidget
{
    Q_OBJECT

public:
    explicit PatientByDoctor(QWidget *parent = 0,MyTcpsocket *socket = 0,QString patientName = 0,QString patientAge = 0,QString patientSex = 0,QString doctorName = 0);
    ~PatientByDoctor();

private slots:
    void on_confirm_BT_clicked();

    void on_firstLevel_CB_currentTextChanged(const QString &arg1);

    void on_secondLevel_CB_currentTextChanged(const QString &arg1);

private:
    Ui::PatientByDoctor *ui;
    MyTcpsocket *m_socket;
    QString inner_Doctor;

    QStringList *getInfoFromReturnString(QString returnStr);
    QStandardItemModel *getModel(QString returnData, QStringList *list);
};

#endif // PATIENTBYDOCTOR_H
