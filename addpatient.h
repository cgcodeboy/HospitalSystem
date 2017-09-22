#ifndef ADDPATIENT_H
#define ADDPATIENT_H

#include <QWidget>

namespace Ui {
class AddPatient;
}

class AddPatient : public QWidget
{
    Q_OBJECT

public:
    explicit AddPatient(QWidget *parent = 0);
    ~AddPatient();

private slots:
    void on_addPatient_BT_clicked();

private:
    Ui::AddPatient *ui;

signals:
    void addPatientSignal(QString patientName,QString patientSex,QString patientAge,QString patientIll);
};

#endif // ADDPATIENT_H
