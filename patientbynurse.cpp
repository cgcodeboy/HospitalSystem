#include "patientbynurse.h"
#include "ui_patientbynurse.h"

//处理返回病人信息
QStringList *PatientByNurse::getInfoFromReturnString(QString returnStr)
{
    QStringList *returnList = new QStringList;
    returnStr = returnStr.replace("@","");
    int index = 0;
    while(!returnStr.isEmpty())
    {
        index = returnStr.indexOf('#');
        returnList->push_back(returnStr.left(index));
        returnStr.remove(0,index+1);
    }
    return returnList;
}

PatientByNurse::PatientByNurse(QWidget *parent, MyTcpsocket *socket, QString patientName, QString patientAge, QString patientSex) :
    QWidget(parent),
    ui(new Ui::PatientByNurse)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    m_socket = socket;

    socket->writeData(QString("select patientIllness,patientMedicine from patient_Table where patientName = '%1' and patientAge = %2 and patientSex = '%3'")\
                      .arg(patientName).arg(patientAge).arg(patientSex));

    m_socket->waitData();
    QString returnStr = m_socket->getData();
    QStringList *returnList = this->getInfoFromReturnString(returnStr);

    ui->patientName_LB->setText(patientName);
    ui->patientAge_LB->setText(patientAge);
    ui->patientSex_LB->setText(patientSex);

    ui->patientIllness_LB->setText(returnList->at(0));

    if(QString(returnList->at(1)) == "")
    {
        qDebug()<<"isnull";
        ui->patientMedicine_LB->setText("未分配");
    }
    else
    {
        ui->patientMedicine_LB->setText(returnList->at(1));
    }
}

PatientByNurse::~PatientByNurse()
{
    delete ui;
}

void PatientByNurse::on_confirm_BT_clicked()
{
    if(!ui->patientMedicine_LB->text().isEmpty())
    {
        if(QMessageBox::question(this,"确认信息","确认使用?") == QMessageBox::Yes)
        {
            this->m_socket->writeData(QString("update medicine_Table set medicineRemain = medicineRemain - 1 where madicineName = '%1'")\
                                      .arg(ui->patientMedicine_LB->text()));
        }
    }
}
