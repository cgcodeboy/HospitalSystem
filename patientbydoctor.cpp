#include "patientbydoctor.h"
#include "ui_patientbydoctor.h"

//处理返回病人信息
QStringList *PatientByDoctor::getInfoFromReturnString(QString returnStr)
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

QStandardItemModel* PatientByDoctor::getModel(QString returnData,QStringList *list)
{
    QStandardItemModel * model = new QStandardItemModel;
    model->setColumnCount(list->length());
    for(int i = 0;i<list->length();i++)
    {
        model->setHorizontalHeaderItem(i,new QStandardItem(list->at(i)));
    }
    QString str2(returnData);
    int index_1 = 0;
    int rowCount = 0,columnCount = 0;

    while(!returnData.isEmpty())
    {
        index_1 = returnData.indexOf('@');
        QString str_1 = returnData.left(index_1);
        int index_2 =0;
        while(!str_1.isEmpty())
        {
            index_2 = str_1.indexOf('#');
            str_1.remove(0,index_2+1);
            columnCount++;
        }
        returnData.remove(0,index_1+1);
        rowCount++;
    }
    columnCount = columnCount/rowCount;
    model->setColumnCount(columnCount);
    model->setRowCount(rowCount);

    index_1 = 0;
    int i = 0,j = 0;
    while(!str2.isEmpty())
    {
        index_1 = str2.indexOf('@');
        QString str_1 = str2.left(index_1);
        int index_2 =0;
        while(!str_1.isEmpty())
        {
            index_2 = str_1.indexOf('#');
            QString cur = str_1.left(index_2);
            QStandardItem *itemID = new QStandardItem(cur);
            model->setItem(i,j,itemID);
            str_1.remove(0,index_2+1);
            j++;
        }
        j = 0;
        str2.remove(0,index_1+1);
        i++;
    }
    return model;
}

PatientByDoctor::PatientByDoctor(QWidget *parent, MyTcpsocket *socket, QString patientName, QString patientAge, QString patientSex, QString doctorName) :
    QWidget(parent),
    ui(new Ui::PatientByDoctor)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    m_socket = socket;
    this->inner_Doctor = doctorName;

    m_socket->writeData(QString("select nurseName from nurse_Table where nurseNum < 9"));
    m_socket->waitData();
    QString  returnStr = m_socket->getData();
    QStringList *returnList = this->getInfoFromReturnString(returnStr);
    ui->nurseName_CB->addItems(*returnList);

    m_socket->writeData(QString("select patientNurseName,patientIllness,patientMedicine from patient_Table where patientName = '%1' and patientAge = %2 and patientSex = '%3'")\
                        .arg(patientName).arg(patientAge).arg(patientSex));
    m_socket->waitData();
    returnStr = m_socket->getData();
    returnList = this->getInfoFromReturnString(returnStr);

    ui->patientName_LB->setText(patientName);
    ui->patientAge_LB->setText(patientAge);
    ui->patientSex_LB->setText(patientSex);
    if(QString(returnList->at(0)) == "")
    {
        qDebug()<<"isnull";
        ui->nurseName_CB->addItem("未分配");
        ui->nurseName_CB->setCurrentText("未分配");
    }
    else
    {
        ui->nurseName_CB->setCurrentText(returnList->at(0));
    }
    ui->patientIllness_LB->setText(returnList->at(1));
    ui->patientMedicine_LB->setText(returnList->at(2));

    m_socket->writeData(QString("select distinct medicineFirst from medicine_Table"));
    m_socket->waitData();
    returnStr = m_socket->getData();
    returnList = getInfoFromReturnString(returnStr);
    ui->firstLevel_CB->addItems(*returnList);

    m_socket->writeData(QString("select distinct medicineSecond from medicine_Table"));
    m_socket->waitData();
    returnStr = m_socket->getData();
    returnList = getInfoFromReturnString(returnStr);
    ui->secondLevel_CB->addItems(*returnList);
}

PatientByDoctor::~PatientByDoctor()
{
    delete ui;
}

void PatientByDoctor::on_confirm_BT_clicked()
{
    QString medicineName = ui->medicine_View->model()->index(ui->medicine_View->currentIndex().row(),0).data().toString();

    QString nurseName = ui->nurseName_CB->currentText();
    if(nurseName == "未分配")
    {
        nurseName = "";
    }
    m_socket->writeData(QString("update patient_Table set patientNurseName = '%1',patientMedicine = '%2' where patientName = '%3' and patientSex = '%4' and patientAge = '%5'")\
                        .arg(nurseName).arg(medicineName).arg(ui->patientName_LB->text())\
                        .arg(ui->patientSex_LB->text()).arg(ui->patientAge_LB->text()));
    m_socket->waitData();
    if(medicineName != ui->patientMedicine_LB->text())
    {
        m_socket->writeData(QString("insert into medicineRequest_Table values(Null,'%1','%2','%3','%4','false')")\
                            .arg(this->inner_Doctor).arg(QDate::currentDate().toString("yyyy.MM.dd")).arg(medicineName).arg(ui->patientName_LB->text()));
        m_socket->waitData();
    }
    this->close();
}

void PatientByDoctor::on_firstLevel_CB_currentTextChanged(const QString &arg1)
{
    ui->secondLevel_CB->clear();
    m_socket->writeData(QString("select distinct medicineSecond from medicine_Table where medicineFirst = '%1' and medicineRemain > 0")\
                        .arg(ui->firstLevel_CB->currentText()));
    m_socket->waitData();
    QString returnData = m_socket->getData();
    QStringList *returnList = getInfoFromReturnString(returnData);
    ui->secondLevel_CB->addItems(*returnList);
}

void PatientByDoctor::on_secondLevel_CB_currentTextChanged(const QString &arg1)
{
    ui->medicine_View->resizeColumnsToContents();
    ui->medicine_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->medicine_View->verticalHeader()->setVisible(false);
    ui->medicine_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("药品");
    strList->push_back("剩余");
    m_socket->writeData(QString("select distinct medicineName,medicineRemain from medicine_Table where medicineFirst = '%1' and medicineSecond = '%2'")\
                        .arg(ui->firstLevel_CB->currentText()).arg(ui->secondLevel_CB->currentText()));
    m_socket->waitData();

    QString returnStr = m_socket->getData();

    if(!returnStr.isEmpty())
    {
        QStandardItemModel * medicineModel = getModel(returnStr,strList);
        ui->medicine_View->setModel(medicineModel);
    }
}
