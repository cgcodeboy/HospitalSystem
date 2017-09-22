#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QDate>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <iostream>
#include <complex>

#include "logwidget.h"
#include "addpatient.h"
#include "myTcpsocket.h"
#include "patientbydoctor.h"
#include "patientbynurse.h"
#include "searchwidget.h"
#include "buymedicine.h"
#include "doctorfeedback.h"
#include "mifeedback.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_finacial_BT_clicked();

    void on_drug_BT_clicked();

    void on_care_BT_clicked();

    void on_news_BT_clicked();

    void on_homepage_BT_clicked();

    void on_report_BT_clicked();

    void on_interact_BT_clicked();

    void on_log_BT_clicked();

    void on_work_BT_clicked();

    void on_nurse_View_BT_clicked();

    void on_doctorInfoChange_BT_clicked();

    void on_doctorInfoChangeComplete_BT_clicked();

    void on_addPatient_BT_clicked();

    void on_doctorInfo_Manager_BT_clicked();

    void on_nurseInfo_Manager_BT_clicked();

    void on_patientInfo_Manager_BT_clicked();

    void on_medicineInfo_Manager_BT_clicked();

    void on_medInspectInfo_Manager_BT_clicked();

    void on_financialInfo_Manager_BT_clicked();

    void on_financierInfo_Manager_BT_clicked();

    void on_personerInfo_Manager_BT_clicked();

    void on_feedback_Commit_BT_clicked();

    void on_homeLogin_BT_clicked();

    void on_homeWorkcenter_BT_clicked();

    void on_homeNews_BT_clicked();

    void on_doctorPersonInfo_BT_clicked();

    void on_nursePersonInfo_Nurse_BT_clicked();

    void on_patient_Doctor_View_BT_clicked();

    void on_nurse_Doctor_View_BT_clicked();

    void on_doctorView_Nurse_BT_clicked();

    void on_patientView_Nurse_BT_clicked();

    void on_financialCheck_View_doubleClicked(const QModelIndex &index);

    void on_patient_Doctor_View_doubleClicked(const QModelIndex &index);

    void on_removePatient_BT_clicked();

    void on_medicine_TW_itemClicked(QTreeWidgetItem *item, int column);

    void on_homeSearch_BT_clicked();

    void on_nurseInfoChange_BT_clicked();

    void on_nurseInfoChangeComplete_BT_clicked();

    void on_patient_Nurse_View_doubleClicked(const QModelIndex &index);

    void on_medicineIns_TW_itemClicked(QTreeWidgetItem *item, int column);

    void on_urgentMedicine_BT_clicked();

    void on_buyMedicine_BT_clicked();

    //自定义槽
    void loginInfoSlot(QString name,QString password,QString job);

    void financialRequest_View_Change_Slot();

    void addPatientSlot(QString patientName,QString patientSex,QString patientAge,QString patientIll);

    void on_buyNumSlot(int num);

    void doctorFeedbackHanleSlot(QString feedbackInfo);
    void miFeedbackHanleSlot(QString feedbackInfo);

    void timeChangeSlot();


private:
    Ui::MainWindow *ui;
    MyTcpsocket *m_socket;

    QString inner_name;
    QString inner_password;
    QString inner_job;
    QString doctorFeedbackMsg;
    QString miFeedbackMsg;
    QTimer timer;

signals:
    void financialRequestChange();
};

#endif // MAINWINDOW_H
