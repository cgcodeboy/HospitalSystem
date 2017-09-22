#include "mainwindow.h"
#include "ui_mainwindow.h"

//判断是否存在用户
bool exitUser(QString returndata,QString realPassword)
{
    QString curData = returndata.replace("#","").replace("@","");

    qDebug()<<"curdata "<<curData;

    if(curData == realPassword)
        return true;

    return false;
}

//由返回信息解析内部信息
QStringList *getInfoFromReturnString(QString returnStr)
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

//根据特定得返回字符串得到model
QStandardItemModel *getModel(QString returnData,QStringList *list)
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mainStack_Widget->setCurrentIndex(0);

    m_socket = new MyTcpsocket;

    ///////////////////////////////////////////////

    QFile file("medicine.txt");

    QTextStream stream(&file);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"open error";
        return;
    }

    int i = 0,index;

    QString str;

    QStringList *list = new QStringList;

    QStringList secondList;

    QTreeWidgetItem *central_firstLevel,*ins_firstLevel;

    while(!stream.atEnd())
    {
        str = stream.readLine();
        if(str.left(1) == "第")
        {
            index = str.indexOf(" ");
            QString cur = str.mid(index+1);
            QStringList *firstList = new QStringList;
            firstList->push_back(cur);
            central_firstLevel = new QTreeWidgetItem(*firstList);
            ins_firstLevel = new QTreeWidgetItem(*firstList);

            ui->medicine_TW->addTopLevelItem(central_firstLevel);
            ui->medicineIns_TW->addTopLevelItem(ins_firstLevel);
        }
        else if(str.mid(1,1) == "、")
        {
            index = str.indexOf("、");
            QString cur = str.mid(index+1);
            secondList.push_back(cur);
        }
        else if(str.isEmpty())
        {
            for(QStringList::iterator it = secondList.begin();it!=secondList.end();it++)
            {
                QStringList curList;
                curList.push_back(*it);
                QTreeWidgetItem *centralItem = new QTreeWidgetItem(curList);
                QTreeWidgetItem *insItem = new QTreeWidgetItem(curList);
                central_firstLevel->addChild(centralItem);
                ins_firstLevel->addChild(insItem);
            }
            secondList.clear();
        }
    }
    file.close();
    ///////////////////////////////////////////////

    //设置医生信息不可编辑
    ui->doctorInfoChangeComplete_BT->setVisible(false);
    ui->doctorAge_LE->setEnabled(false);
    ui->doctorDis_LE->setEnabled(false);
    ui->doctorExprience_TE->setEnabled(false);
    ui->doctorRoom_LE->setEnabled(false);
    ui->doctorName_LE->setEnabled(false);
    ui->doctorSex_LE->setEnabled(false);
    ui->doctorPhone_LE->setEnabled(false);
    ui->doctorPassword_LE->setEnabled(false);
    //设置护士信息不可编辑
    ui->nurseInfoChangeComplete_BT->setVisible(false);
    ui->nurseNum_LE->setEnabled(false);
    ui->nurseAge_LE->setEnabled(false);
    ui->nurseRoom_LE->setEnabled(false);
    ui->nurseName_LE->setEnabled(false);
    ui->nurseSex_LE->setEnabled(false);
    ui->nursePhone_LE->setEnabled(false);
    ui->nursePassword_LE->setEnabled(false);

    QDate date = QDate::currentDate();
    ui->news_Group->setTitle(date.toString("dd.MM.yyyy"));

    //设置医护人员widget种得doctorview表属性
    ui->doctorInfo_View->resizeColumnsToContents();
    ui->doctorInfo_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->doctorInfo_View->verticalHeader()->setVisible(false);
    ui->doctorInfo_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //设置医护人员widget中得nurseview表属性
    ui->nurseInfo_View->resizeColumnsToContents();
    ui->nurseInfo_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->nurseInfo_View->verticalHeader()->setVisible(false);
    ui->nurseInfo_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(this,SIGNAL(financialRequestChange()),this,SLOT(financialRequest_View_Change_Slot()));
    connect(m_socket,SIGNAL(doctorFeedbackComming(QString)),this,SLOT(doctorFeedbackHanleSlot(QString)));
    connect(m_socket,SIGNAL(miFeedbackComming(QString)),this,SLOT(miFeedbackHanleSlot(QString)));
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeChangeSlot()));
    timer.start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;

    if(this->inner_job == "doctor")
    {
        m_socket->writeData(QString("update doctor_Table set loginBool = 'false' where doctorName = '%1'")\
                            .arg(this->inner_name));
    }
    else if(this->inner_job == "nurse")
    {
        m_socket->writeData(QString("update nurse_Table set loginBool = 'false' where nurseName = '%1'")\
                            .arg(this->inner_name));
    }
    else if(this->inner_job == "medicine")
    {
        m_socket->writeData(QString("update medicineIns_Table set loginBool = 'false' where medicineInsName = '%1'")\
                            .arg(this->inner_name));
    }
    else if(this->inner_job == "manager")
    {
        m_socket->writeData(QString("update manager_Table set loginBool = 'false' where managerName = '%1'")\
                            .arg(this->inner_name));
    }
    else if(this->inner_job == "financial")
    {
        m_socket->writeData(QString("update financial_Table set loginBool = 'false' where financialName = '%1'")\
                            .arg(this->inner_name));
    }
}

//转到财务透明栏
void MainWindow::on_finacial_BT_clicked()
{
    ui->mainStack_Widget->setCurrentIndex(1);

    ui->financialOpen_View->resizeColumnsToContents();
    ui->financialOpen_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->financialOpen_View->verticalHeader()->setVisible(false);
    ui->financialOpen_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("金额");
    strList->push_back("时间");
    strList->push_back("出纳");

    if(!this->m_socket->isconnected())
    {
        return;
    }

    m_socket->writeData("select moneyNum,moneyTime,moneyFinancial from money_Table");
    m_socket->waitData();
    QString returnData = m_socket->getData();


    QStandardItemModel *financialOpenModel = getModel(returnData,strList);
    ui->financialOpen_View->setModel(financialOpenModel);
    //设置财务公开tableview属性
    //    QStandardItemModel *financialOpenModel = new QStandardItemModel;
    //    financialOpenModel->setColumnCount(3);
    //    financialOpenModel->setHorizontalHeaderItem(0,new QStandardItem("金额"));
    //    financialOpenModel->setHorizontalHeaderItem(1,new QStandardItem("时间"));
    //    financialOpenModel->setHorizontalHeaderItem(2,new QStandardItem("出纳"));
    //    ui->financialOpen_View->setModel(financialOpenModel);
    //    for(int i = 0;i<3;i++)
    //    {
    //        ui->financialOpen_View->setColumnWidth(i,ui->financialOpen_View->width()/3-1);
    //    }
}

//转到药品监督栏
void MainWindow::on_drug_BT_clicked()
{
    ui->mainStack_Widget->setCurrentIndex(2);
}

//转到医生和护理人员栏
void MainWindow::on_care_BT_clicked()
{
    ui->mainStack_Widget->setCurrentIndex(3);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("性别");
    strList->push_back("年龄");
    strList->push_back("办公室");
    strList->push_back("办公电话");
    strList->push_back("主治");


    //    for(int i = 0;i<strList->length();i++)
    //    {
    //        ui->doctorInfo_View->setColumnWidth(i,ui->doctorInfo_View->width()/strList->length());
    //    }
    if(!m_socket->isconnected())
    {
        return;
    }
    m_socket->writeData("select doctorName,doctorSex,doctorAge,doctorRoom,doctorPhone,doctorDis from doctor_Table");
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *model = getModel(returnData,strList);

    ui->doctorInfo_View->setModel(model);

    strList->clear();
    strList->push_back("姓名");
    strList->push_back("性别");
    strList->push_back("年龄");
    strList->push_back("电话");
    strList->push_back("办公地点");
    //    for(int i = 0;i<strList->length();i++)
    //    {
    //        ui->nurseInfo_View->setColumnWidth(i,ui->nurseInfo_View->width()/strList->length());
    //    }

    m_socket->writeData("select nurseName,nurseSex,nurseAge,nursePhone,nurseRoom from nurse_table");
    m_socket->waitData();
    returnData = m_socket->getData();
    qDebug()<<"before change:"<<returnData;
    qDebug()<<QString::fromLocal8Bit(returnData.toLocal8Bit());

    model = getModel(returnData,strList);

    ui->nurseInfo_View->setModel(model);

}

//转到医院近况栏
void MainWindow::on_news_BT_clicked()
{
    ui->mainStack_Widget->setCurrentIndex(4);
}

//回到主页
void MainWindow::on_homepage_BT_clicked()
{
    ui->mainStack_Widget->setCurrentIndex(0);
}

//统计报表
void MainWindow::on_report_BT_clicked()
{
    ui->mainStack_Widget->setCurrentIndex(5);

    m_socket->writeData(QString("select patientInNum,patientOutNum,medicineCost,medicineUse from report_Table where Time = '%1'")\
                        .arg(QDate::currentDate().toString("yyyy.MM.dd")));
    m_socket->waitData();
    QString returnData = m_socket->getData();
    if(!returnData.isEmpty())
    {
        QStringList *numList = getInfoFromReturnString(returnData);

        ui->todayInNum_LB->setText(numList->at(0));
        ui->todayOutNum_LB->setText(numList->at(1));
        QString cur = numList->at(2);
        int index = cur.indexOf(".");
        cur = cur.mid(0,index+3);
        ui->todayMedicineInCost_LB->setText(cur);
        cur = numList->at(3);
        index = cur.indexOf(".");
        cur = cur.mid(0,index+3);
        ui->todayMedicineOutCost_LB->setText(cur);
    }
    else
    {
        m_socket->writeData(QString("insert into report_Table values('%1',0,0,0,0)")\
                            .arg(QDate::currentDate().toString("yyyy.MM.dd")));
        m_socket->waitData();
    }

    QString yearMonth = QDate::currentDate().toString("yyyy.MM.dd").left(7);

    m_socket->writeData(QString("select sum(patientInNum),sum(patientOutNum),sum(medicineCost),sum(medicineUse) from report_Table where Time between '%1-01' and '%1-30'")\
                        .arg(yearMonth));

    m_socket->waitData();

    returnData = m_socket->getData();

    if(!returnData.isEmpty())
    {
        QStringList *sumList = getInfoFromReturnString(returnData);
        ui->monthInNum_LB->setText(sumList->at(0));
        ui->monthOutNum_LB->setText(sumList->at(1));
        QString cur = sumList->at(2);
        int index = cur.indexOf(".");
        cur = cur.mid(0,index+3);
        qDebug()<<cur;
        ui->monthMedicineInCost_LB->setText(cur);
        cur = sumList->at(3);
        index = cur.indexOf(".");
        cur = cur.mid(0,index+3);
        qDebug()<<cur;
        ui->monthMedicineOutCost_LB->setText(cur);
    }
}

//互动平台
void MainWindow::on_interact_BT_clicked()
{
    ui->mainStack_Widget->setCurrentIndex(6);
}

//登陆
void MainWindow::on_log_BT_clicked()
{
    LogWidget *logWidget = new LogWidget(this,this->inner_name,this->inner_password,this->inner_job);
    logWidget->show();
    connect(logWidget,SIGNAL(infoSignal(QString,QString,QString)),this,SLOT(loginInfoSlot(QString,QString,QString)));
}

//转向工作栏
void MainWindow::on_work_BT_clicked()
{
    if(this->inner_job == "doctor")
    {
        ui->mainStack_Widget->setCurrentIndex(7);
        this->on_doctorPersonInfo_BT_clicked();
    }
    else if(this->inner_job == "nurse")
    {
        ui->mainStack_Widget->setCurrentIndex(8);
        this->on_nursePersonInfo_Nurse_BT_clicked();
    }
    else if(this->inner_job == "medicine")
    {
        ui->mainStack_Widget->setCurrentIndex(9);
    }
    else if(this->inner_job == "manager")
    {
        ui->mainStack_Widget->setCurrentIndex(10);
        this->on_personerInfo_Manager_BT_clicked();
    }
    else if(this->inner_job == "financial")
    {
        ui->mainStack_Widget->setCurrentIndex(11);
        emit financialRequestChange();
    }
}



//医生查看护士信息
void MainWindow::on_nurse_View_BT_clicked()
{
    //    ui->doctor_SW->setCurrentIndex(2);
    //    QStandardItemModel *nurseModel = new QStandardItemModel;
    //    nurseModel->setColumnCount(5);
    //    nurseModel->setHorizontalHeaderItem(0,new QStandardItem("姓名"));
    //    nurseModel->setHorizontalHeaderItem(1,new QStandardItem("年龄"));
    //    nurseModel->setHorizontalHeaderItem(2,new QStandardItem("性别"));
    //    nurseModel->setHorizontalHeaderItem(3,new QStandardItem("护理人数"));
    //    nurseModel->setHorizontalHeaderItem(4,new QStandardItem(""));
    //    ui->nurse_Doctor_View->setModel(nurseModel);
    //    for(int j= 0;j<5;j++)
    //    {
    //        ui->nurse_Doctor_View->setColumnWidth(j,ui->nurse_Doctor_View->width()/5);
    //    }
}

void MainWindow::on_doctorInfoChange_BT_clicked()
{
    ui->doctorInfoChangeComplete_BT->setVisible(true);
    ui->doctorAge_LE->setEnabled(true);
    ui->doctorDis_LE->setEnabled(true);
    ui->doctorExprience_TE->setEnabled(true);
    ui->doctorRoom_LE->setEnabled(true);
    ui->doctorName_LE->setEnabled(true);
    ui->doctorSex_LE->setEnabled(true);
    ui->doctorPhone_LE->setEnabled(true);
    ui->doctorPassword_LE->setEnabled(true);
}

void MainWindow::on_doctorInfoChangeComplete_BT_clicked()
{
    //如果确认保存则保存并更新数据库，否则直接退出，并从新提取信息覆盖掉改动痕迹
    if(QMessageBox::question(this,"更改信息","确认更改?") == QMessageBox::Yes)
    {
        ui->doctorInfoChangeComplete_BT->setVisible(false);
        ui->doctorAge_LE->setEnabled(false);
        ui->doctorDis_LE->setEnabled(false);
        ui->doctorExprience_TE->setEnabled(false);
        ui->doctorRoom_LE->setEnabled(false);
        ui->doctorName_LE->setEnabled(false);
        ui->doctorSex_LE->setEnabled(false);
        ui->doctorPhone_LE->setEnabled(false);
        ui->doctorPassword_LE->setEnabled(false);


        m_socket->writeData(QString("update doctor_Table set doctorName = '%1',password = '%2',doctorSex = '%3',doctorAge = %4,doctorRoom  = %5,doctorPhone = %6,doctorDis = '%7',doctorExprience = '%8' where doctorName = '%9' and password = '%10'")\
                            .arg(ui->doctorName_LE->text())\
                            .arg(ui->doctorPassword_LE->text())\
                            .arg(ui->doctorSex_LE->text())\
                            .arg(ui->doctorAge_LE->text())\
                            .arg(ui->doctorRoom_LE->text())\
                            .arg(ui->doctorPhone_LE->text())\
                            .arg(ui->doctorDis_LE->text())\
                            .arg(ui->doctorExprience_TE->toPlainText())
                            .arg(this->inner_name)\
                            .arg(this->inner_password));
        m_socket->waitData();
        this->inner_name = ui->doctorName_LE->text();
        this->inner_password = ui->doctorPassword_LE->text();
    }
    else
    {
        this->on_doctorPersonInfo_BT_clicked();
    }
}

void MainWindow::on_addPatient_BT_clicked()
{
    AddPatient *addPatient = new AddPatient(this);
    addPatient->show();
    connect(addPatient,SIGNAL(addPatientSignal(QString,QString,QString,QString)),this,SLOT(addPatientSlot(QString,QString,QString,QString)));
}

//管理员查看医生信息
void MainWindow::on_doctorInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(0);

    ui->doctor_Manager_View->resizeColumnsToContents();
    ui->doctor_Manager_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->doctor_Manager_View->verticalHeader()->setVisible(false);
    ui->doctor_Manager_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("性别");
    strList->push_back("年龄");
    strList->push_back("办公室");
    strList->push_back("办公电话");
    strList->push_back("主治");
    strList->push_back("是否到职");

    m_socket->writeData(QString("select doctorName,doctorSex,doctorAge,doctorRoom,doctorPhone,doctorDis,loginBool from doctor_Table"));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *model = getModel(returnData,strList);
    ui->doctor_Manager_View->setModel(model);
}

//管理员查看护士信息
void MainWindow::on_nurseInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(1);

    ui->nurse_Manager_View->resizeColumnsToContents();
    ui->nurse_Manager_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->nurse_Manager_View->verticalHeader()->setVisible(false);
    ui->nurse_Manager_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("性别");
    strList->push_back("年龄");
    strList->push_back("办公室");
    strList->push_back("办公电话");
    strList->push_back("护理人数");
    strList->push_back("是否到职");

    m_socket->writeData(QString("select nurseName,nurseSex,nurseAge,nurseRoom,nursePhone,nurseNum,loginBool from nurse_Table"));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *model = getModel(returnData,strList);
    ui->nurse_Manager_View->setModel(model);
}

//管理员查看病人信息
void MainWindow::on_patientInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(2);

    ui->patient_Manager_View->resizeColumnsToContents();
    ui->patient_Manager_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->patient_Manager_View->verticalHeader()->setVisible(false);
    ui->patient_Manager_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("性别");
    strList->push_back("年龄");
    strList->push_back("办公室");
    strList->push_back("办公电话");
    strList->push_back("护理人数");
    strList->push_back("是否到职");

    m_socket->writeData(QString("select patientName,patientSex,patientAge,patientNurseName,patientDoctorName from patient_Table"));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *model = getModel(returnData,strList);
    ui->patient_Manager_View->setModel(model);
}

//管理员查看药品信息
void MainWindow::on_medicineInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(3);

    ui->medicine_Manager_View->resizeColumnsToContents();
    ui->medicine_Manager_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->medicine_Manager_View->verticalHeader()->setVisible(false);
    ui->medicine_Manager_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("药品编号");
    strList->push_back("药品名");
    strList->push_back("剩余量");

    m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineRemain < 100 "));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    if(!returnData.isEmpty())
    {
        QStandardItemModel *medicineModel = getModel(returnData,strList);
        ui->medicine_Manager_View->setModel(medicineModel);
        return;
    }
}

//管理员查看药监人员信息
void MainWindow::on_medInspectInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(4);

    ui->medicineIns_Manager_View->resizeColumnsToContents();
    ui->medicineIns_Manager_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->medicineIns_Manager_View->verticalHeader()->setVisible(false);
    ui->medicineIns_Manager_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("性别");
    strList->push_back("年龄");
    strList->push_back("是否到职");

    m_socket->writeData(QString("select medicineinsName,medicineinsSex,medicineinsAge,loginBool from medicineins_Table"));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *model = getModel(returnData,strList);
    ui->medicineIns_Manager_View->setModel(model);
}


//管理员查看财务信息
void MainWindow::on_financialInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(5);

    ui->money_Manager_View->resizeColumnsToContents();
    ui->money_Manager_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->money_Manager_View->verticalHeader()->setVisible(false);
    ui->money_Manager_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("金额");
    strList->push_back("时间");
    strList->push_back("出纳");

    m_socket->writeData(QString("select moneyNum,moneyTime,moneyFinancial from money_Table"));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *model = getModel(returnData,strList);
    ui->money_Manager_View->setModel(model);
}

//管理员查看财务人员信息
void MainWindow::on_financierInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(6);

    ui->financial_Manager_View->resizeColumnsToContents();
    ui->financial_Manager_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->financial_Manager_View->verticalHeader()->setVisible(false);
    ui->financial_Manager_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("性别");
    strList->push_back("年龄");
    strList->push_back("是否到职");

    m_socket->writeData(QString("select financialName,financialSex,financialAge,loginBool from financial_Table"));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *model = getModel(returnData,strList);
    ui->financial_Manager_View->setModel(model);
}

//管理员查看个人信息
void MainWindow::on_personerInfo_Manager_BT_clicked()
{
    ui->manager_SW->setCurrentIndex(7);

    m_socket->writeData(QString("select managerName,managerSex,managerAge from manager_Table where managerName = '%1'")\
                        .arg(this->inner_name));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStringList *infoList = getInfoFromReturnString(returnData);
    ui->managerName_LB->setText(infoList->at(0));
    ui->managerSex_LB->setText(infoList->at(1));
    ui->managerAge_LB->setText(infoList->at(2));
}

//反馈信息提交槽函数
void MainWindow::on_feedback_Commit_BT_clicked()
{
    QString patientName = ui->feedback_PatientName_LE->text();
    QString doctorName = ui->feedback_DoctorName_LE->text();
    int patientID = ui->feedback_PatientID_LE->text().toInt();

    m_socket->writeData(QString("select patientID from patient_Table where patientName = '%1'")\
                        .arg(patientName));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    if(returnData.isEmpty())
    {
        QMessageBox::warning(this,"提示信息","不存在该病人");
        return;
    }
    else
    {
        QString idInfo = getInfoFromReturnString(returnData)->at(0);
        if(idInfo.toInt() != patientID)
        {
            QMessageBox::warning(this,"提示信息","病号错误");
            return;
        }
        else
        {
            m_socket->writeData(QString("select patientDoctorName from patient_Table where patientID = %1")\
                                .arg(patientID));
            m_socket->waitData();
            returnData = m_socket->getData();
            if(getInfoFromReturnString(returnData)->at(0)!= doctorName)
            {
                QMessageBox::warning(this,"提示信息","该医生未为您就诊过！");
                return;
            }
            ui->news_TE->append("病人: "+patientName+" 向 "+doctorName+"医生反馈信息   "+QTime::currentTime().toString("hh.mm")+"  "+QDate::currentDate().toString("dd.MM.yyyy"));
            m_socket->writeData(QString("insert into feedback_Table values('%1','%2','%3','%4','false')")\
                                .arg(QDate::currentDate().toString("yyyy.MM.dd")).arg(patientName).arg(doctorName).arg(ui->feedback_TE->toPlainText()));
            m_socket->waitData();
        }
    }
}

//登陆信息槽函数
void MainWindow::loginInfoSlot(QString name, QString password, QString job)
{
    bool userExit;
    QString tableName;
    QString userName;
    if(job == "doctor")
    {
        m_socket->writeData(QString("select password from doctor_table where doctorName = '%1'").arg(name));
        m_socket->waitData();
        QString returnData = m_socket->getData();
        if(returnData != "ERROR")
        {
            userExit = exitUser(returnData,password);
            tableName = "doctor_Table";
            userName = "doctorName";
        }
    }
    else if(job == "nurse")
    {
        m_socket->writeData(QString("select password from nurse_table where nurseName = '%1'").arg(name));
        m_socket->waitData();
        QString returnData = m_socket->getData();
        if(returnData != "ERROR")
        {
            userExit = exitUser(returnData,password);
            tableName = "nurse_Table";
            userName = "nurseName";
        }
    }
    else if(job == "medicine")
    {
        m_socket->writeData(QString("select password from medicineins_table where medicineinsName = '%1'").arg(name));
        m_socket->waitData();
        QString returnData = m_socket->getData();
        if(returnData != "ERROR")
        {
            userExit = exitUser(returnData,password);
            tableName = "medicineIns_Table";
            userName = "medicineInsName";
        }
    }
    else if(job == "financial")
    {
        m_socket->writeData(QString("select password from financial_table where financialName = '%1'").arg(name));
        m_socket->waitData();
        QString returnData = m_socket->getData();
        if(returnData != "ERROR")
        {
            userExit = exitUser(returnData,password);
            tableName = "financial_Table";
            userName = "financialName";
        }
    }
    else if(job == "manager")
    {
        m_socket->writeData(QString("select password from manager_table where managerName = '%1'").arg(name));
        m_socket->waitData();
        QString returnData = m_socket->getData();
        if(returnData != "ERROR")
        {
            userExit = exitUser(returnData,password);
            tableName = "manager_Table";
            userName = "managerName";
        }
    }
    if(userExit)
    {
        ui->job_widget->setStyleSheet(QString("border-image: url(:/image/image/%1.png)").arg(job));
        ui->name_LB->setText(name);
        this->inner_name = name;
        ui->job_LB->setText(job);
        this->inner_job = job;
        this->inner_password = password;
        m_socket->writeData(QString("update %1 set loginBool = 'true' where %2 = '%3'")\
                            .arg(tableName).arg(userName).arg(this->inner_name));
        m_socket->waitData();
    }
}

//财务人员界面改动刷新槽函数
void MainWindow::financialRequest_View_Change_Slot()
{
    ui->financialCheck_View->resizeColumnsToContents();
    ui->financialCheck_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->financialCheck_View->verticalHeader()->setVisible(false);
    ui->financialCheck_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("金额");
    strList->push_back("时间");
    strList->push_back("申请人");
    strList->push_back("申请目的");
    strList->push_back("同意");

    m_socket->writeData("select requestMoney,requestTime,requestPerson,requestUse,requestBool from moneyRequest_Table");
    m_socket->waitData();
    QString returnData = m_socket->getData();

    QStandardItemModel *requestModel = getModel(returnData,strList);
    ui->financialCheck_View->setModel(requestModel);

    QString yearMonth = QDate::currentDate().toString("yyyy.MM.dd").left(7);

    m_socket->writeData(QString("select sum(moneyNum) from money_Table where moneyTime between '%1.01' and '%1.31'")\
                        .arg(yearMonth));

    m_socket->waitData();

    returnData = m_socket->getData().replace("@","").replace("#","");

    int cost = returnData.toInt();
    qDebug()<<cost;
    int left = 200000 - cost;
    ui->moneyCost_FN_LB->setText(returnData);
    ui->moneyleft_FN_LB->setText(QString::number(left));
}

//添加病人槽函数
void MainWindow::addPatientSlot(QString patientName, QString patientSex, QString patientAge, QString patientIll)
{
    m_socket->writeData(QString("insert into patient_Table(patientID,patientName,patientSex,patientAge,patientDoctorName,patientIllness) values(NULL,'%1','%2',%3,'%4','%5')")\
                        .arg(patientName).arg(patientSex).arg(patientAge).arg(ui->name_LB->text()).arg(patientIll));
    m_socket->waitData();

    this->on_patient_Doctor_View_BT_clicked();

    m_socket->writeData(QString("select patientInNum from report_Table where Time = '%1'")\
                        .arg(QDate::currentDate().toString("yyyy.MM.dd")));

    m_socket->waitData();

    QString returnData = m_socket->getData();
    if(!returnData.isEmpty())
    {
        //        returnData = QString::number(returnData.replace("@","").replace("#","").toInt() + 1);
        m_socket->writeData(QString("update report_Table set patientInNum = patientInNum + 1 where Time = '%1'")\
                            .arg(QDate::currentDate().toString("yyyy.MM.dd")));
        m_socket->waitData();
    }
}

//购买药品数目确定后槽函数
void MainWindow::on_buyNumSlot(int num)
{
    QModelIndex index = ui->medicine_Ins_View->currentIndex();

    int row = index.row();

    QString medicineID = ui->medicine_Ins_View->model()->index(row,0).data().toString();
    QString medicineName = ui->medicine_Ins_View->model()->index(row,1).data().toString();
    QString medicineRemain = ui->medicine_Ins_View->model()->index(row,2).data().toString();
    qDebug()<<medicineRemain;
    int remain = medicineRemain.toInt();
    if( (num + remain) > 9999)
    {
        QMessageBox::warning(this,"提示信息","购买量太多,超过存储能力!");
        return;
    }

    QString date = QDate::currentDate().toString("dd.MM.yyyy");
    m_socket->writeData(QString("update medicine_Table set medicineRemain = medicineRemain + %1 where medicineID = %2")\
                        .arg(num).arg(medicineID));
    m_socket->waitData();
    ui->news_TE->append("药品监管 "+ui->name_LB->text()+" 提出关于药品 "+medicineName+" 购买的经费申请");
    m_socket->writeData(QString("insert into moneyRequest_Table values(NULL, 5*%1,'%2','%3','医药','false')")\
                        .arg(num).arg(date).arg(ui->name_LB->text()));
    m_socket->waitData();
    this->on_urgentMedicine_BT_clicked();
}

//医生反馈信息处理
void MainWindow::doctorFeedbackHanleSlot(QString feedbackInfo)
{
    if(feedbackInfo == this->doctorFeedbackMsg||this->inner_name.isEmpty())
    {
        return;
    }
    this->doctorFeedbackMsg = feedbackInfo;
    int index = feedbackInfo.indexOf("@");
    QString checkStr = feedbackInfo.left(index);
    QStringList *checkList = getInfoFromReturnString(checkStr);
    if(checkList->at(1) == this->inner_name&&this->inner_job == "doctor")
    {
        DoctorFeedback *feedbackWidget = new DoctorFeedback(this,this->doctorFeedbackMsg,this->m_socket);
        feedbackWidget->show();
    }
}

//药监员反馈信息处理
void MainWindow::miFeedbackHanleSlot(QString feedbackInfo)
{
    if(feedbackInfo == this->miFeedbackMsg||this->inner_name.isEmpty())
    {
        return;
    }
    this->miFeedbackMsg = feedbackInfo;
    if(this->inner_job == "medicine")
    {
        MIfeedback *feedbackWidget = new MIfeedback(this,this->miFeedbackMsg,this->m_socket);
        feedbackWidget->show();
    }
}

//主页面时间变化槽函数
void MainWindow::timeChangeSlot()
{
    ui->time_TE->setText(QTime::currentTime().toString("hh:mm:ss"));
}

//主页上登陆按钮点击
void MainWindow::on_homeLogin_BT_clicked()
{
    this->on_log_BT_clicked();
}

//主页上工作中心点击
void MainWindow::on_homeWorkcenter_BT_clicked()
{
    this->on_work_BT_clicked();
}

//主页上动态按钮点击
void MainWindow::on_homeNews_BT_clicked()
{
    this->on_news_BT_clicked();
}

//医生个人信息点击
void MainWindow::on_doctorPersonInfo_BT_clicked()
{
    ui->doctor_SW->setCurrentIndex(0);
    m_socket->writeData(QString("select doctorSex,doctorAge,doctorRoom,doctorPhone,doctorDis,doctorExprience from doctor_Table where doctorName = '%1' and password = '%2'")\
                        .arg(this->inner_name)\
                        .arg(this->inner_password));
    m_socket->waitData();
    QString returnStr = m_socket->getData();
    QStringList *returnList = getInfoFromReturnString(returnStr);
    ui->doctorName_LE->setText(this->inner_name);
    ui->doctorPassword_LE->setText(this->inner_password);
    ui->doctorSex_LE->setText(returnList->at(0));
    ui->doctorAge_LE->setText(returnList->at(1));
    ui->doctorRoom_LE->setText(returnList->at(2));
    ui->doctorPhone_LE->setText(returnList->at(3));
    ui->doctorDis_LE->setText(returnList->at(4));
    ui->doctorExprience_TE->setText(returnList->at(5));
}

//医生查看病人信息view
void MainWindow::on_patient_Doctor_View_BT_clicked()
{
    ui->doctor_SW->setCurrentIndex(1);

    ui->patient_Doctor_View->resizeColumnsToContents();
    ui->patient_Doctor_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->patient_Doctor_View->verticalHeader()->setVisible(false);
    ui->patient_Doctor_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("年龄");
    strList->push_back("性别");
    strList->push_back("病症");
    strList->push_back("");

    //    QStandardItemModel *patientModel = new QStandardItemModel;
    //    patientModel->setColumnCount(5);
    //    patientModel->setHorizontalHeaderItem(0,new QStandardItem("姓名"));
    //    patientModel->setHorizontalHeaderItem(1,new QStandardItem("年龄"));
    //    patientModel->setHorizontalHeaderItem(2,new QStandardItem("性别"));
    //    patientModel->setHorizontalHeaderItem(3,new QStandardItem("护理人员"));
    //    patientModel->setHorizontalHeaderItem(4,new QStandardItem(""));
    //    ui->patient_Doctor_View->setModel(patientModel);
    //    for(int i = 0;i<5;i++)
    //    {
    //        ui->patient_Doctor_View->setColumnWidth(i,ui->patient_Doctor_View->width()/5);
    //    }

    m_socket->writeData(QString("select patientName,patientAge,patientSex,patientIllness from patient_table where patientDoctorName = '%1'").arg(this->inner_name));
    m_socket->waitData();
    QString returnData = m_socket->getData();
    QStandardItemModel *model = getModel(returnData,strList);
    ui->patient_Doctor_View->setModel(model);
}

//医生查看护士信息view
void MainWindow::on_nurse_Doctor_View_BT_clicked()
{
    ui->doctor_SW->setCurrentIndex(2);

    ui->nurse_Doctor_View->resizeColumnsToContents();
    ui->nurse_Doctor_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->nurse_Doctor_View->verticalHeader()->setVisible(false);
    ui->nurse_Doctor_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("年龄");
    strList->push_back("性别");
    strList->push_back("电话");
    strList->push_back("办公地点");
    strList->push_back("当前护理人数");

    m_socket->writeData(QString("select nurseName,nurseAge,nurseSex,nursePhone,nurseRoom,nurseNum from nurse_Table"));
    m_socket->waitData();
    QString returnStr = m_socket->getData();
    QStandardItemModel *nurseView = getModel(returnStr,strList);
    ui->nurse_Doctor_View->setModel(nurseView);
}

//护士个人信息点击
void MainWindow::on_nursePersonInfo_Nurse_BT_clicked()
{
    ui->nurse_SW->setCurrentIndex(0);
    m_socket->writeData(QString("select nurseSex,nurseAge,nurseRoom,nursePhone,nurseNum from nurse_Table where nurseName = '%1' and password = '%2'")\
                        .arg(this->inner_name)\
                        .arg(this->inner_password));
    m_socket->waitData();
    QString returnStr = m_socket->getData();
    QStringList *returnList = getInfoFromReturnString(returnStr);
    ui->nurseName_LE->setText(this->inner_name);
    ui->nursePassword_LE->setText(this->inner_password);
    ui->nurseSex_LE->setText(returnList->at(0));
    ui->nurseAge_LE->setText(returnList->at(1));
    ui->nurseRoom_LE->setText(returnList->at(2));
    ui->nursePhone_LE->setText(returnList->at(3));
    ui->nurseNum_LE->setText(returnList->at(4));
}

//护士查看医生信息view
void MainWindow::on_doctorView_Nurse_BT_clicked()
{
    ui->nurse_SW->setCurrentIndex(1);

    ui->doctor_Nurse_View->resizeColumnsToContents();
    ui->doctor_Nurse_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->doctor_Nurse_View->verticalHeader()->setVisible(false);
    ui->doctor_Nurse_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("年龄");
    strList->push_back("性别");
    strList->push_back("电话");
    strList->push_back("办公地点");
    strList->push_back("主治");

    m_socket->writeData(QString("select doctorName,doctorAge,doctorSex,doctorPhone,doctorRoom,doctorDis from doctor_Table"));
    m_socket->waitData();
    QString returnStr = m_socket->getData();
    QStandardItemModel *doctorView = getModel(returnStr,strList);
    ui->doctor_Nurse_View->setModel(doctorView);
}

//护士查看病人信息view
void MainWindow::on_patientView_Nurse_BT_clicked()
{
    ui->nurse_SW->setCurrentIndex(2);

    ui->patient_Nurse_View->resizeColumnsToContents();
    ui->patient_Nurse_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->patient_Nurse_View->verticalHeader()->setVisible(false);
    ui->patient_Nurse_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("姓名");
    strList->push_back("年龄");
    strList->push_back("性别");
    strList->push_back("病症");
    strList->push_back("");

    m_socket->writeData(QString("select patientName,patientAge,patientSex,patientIllness from patient_table where patientNurseName = '%1'").arg(this->inner_name));
    m_socket->waitData();
    QString returnData = m_socket->getData();
    QStandardItemModel *model = getModel(returnData,strList);
    ui->patient_Nurse_View->setModel(model);
}

//财务人员对申请处理的槽函数
void MainWindow::on_financialCheck_View_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(ui->financialCheck_View->model());
    QList<QStandardItem*> list =  model->takeRow(row);
    if(list.last()->text() == "true")
    {
        emit financialRequestChange();
        return;
    }
    if(QMessageBox::question(this,"同意信息","是否同意申请?",QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {

        //        qDebug()<<list.at(0)->text()<<" "<<list.at(1)->text();
        //        int column = model->columnCount(index.parent());
        //        QStandardItem *item = new QStandardItem("true");
        //        model->setItem(row,column-1,item);
        //        ui->financialCheck_View->setModel(model);

        m_socket->writeData(QString(\
                                "update moneyRequest_Table set requestBool = 'true' where requestMoney = %1 and requestTime = '%2' and requestPerson = '%3'")\
                            .arg(list.at(0)->text()).arg(list.at(1)->text()).arg(list.at(2)->text()));
        m_socket->waitData();
        emit financialRequestChange();
//        m_socket->waitData();
        m_socket->writeData(QString("insert into money_Table(moneyNum,moneyTime,moneyFinancial) values(%1,'%2','%3')")\
                            .arg(list.at(0)->text()).arg(list.at(1)->text()).arg(ui->name_LB->text()));

        ui->news_TE->append("财务员 "+ui->name_LB->text()+" 同意 "+list.at(2)->text()+" 关于 "+ list.at(3)->text()+"的财务申请");
    }

}

//医生双击查看病人具体信息并对其进行操作如配药、分配护士姐姐等
void MainWindow::on_patient_Doctor_View_doubleClicked(const QModelIndex &index)
{
    int row = ui->patient_Doctor_View->currentIndex().row();

    QString patientName = ui->patient_Doctor_View->model()->index(row,0).data().toString();
    QString patientAge = ui->patient_Doctor_View->model()->index(row,1).data().toString();
    QString patientSex = ui->patient_Doctor_View->model()->index(row,2).data().toString();


    PatientByDoctor *patientWidget = new PatientByDoctor(this,m_socket,patientName,patientAge,patientSex,this->inner_name);
    patientWidget->show();
}

//医生删除病人槽函数
void MainWindow::on_removePatient_BT_clicked()
{
    int row = ui->patient_Doctor_View->currentIndex().row();

    QString patientName = ui->patient_Doctor_View->model()->index(row,0).data().toString();
    QString patientAge = ui->patient_Doctor_View->model()->index(row,1).data().toString();
    QString patientSex = ui->patient_Doctor_View->model()->index(row,2).data().toString();

    m_socket->writeData(QString("delete from patient_Table where patientName = '%1' and patientAge = %2 and patientSex = '%3' ")\
                        .arg(patientName)\
                        .arg(patientAge)\
                        .arg(patientSex));
    m_socket->waitData();
    this->on_patient_Doctor_View_BT_clicked();
}

//药品类别点击后
void MainWindow::on_medicine_TW_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->medicine_View->resizeColumnsToContents();
    ui->medicine_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->medicine_View->verticalHeader()->setVisible(false);
    ui->medicine_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("药品编号");
    strList->push_back("药品名");
    strList->push_back("剩余量");

    QString text = item->text(0);

    m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineFirst = '%1'")\
                        .arg(text));
    m_socket->waitData();
    QString returnData = m_socket->getData();
    qDebug()<<"returndata"<<returnData;

    if(!returnData.isEmpty())
    {
        QStandardItemModel *medicineModel = getModel(returnData,strList);
        ui->medicine_View->setModel(medicineModel);
        return;
    }

    m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineSecond = '%1'")\
                        .arg(text));
    m_socket->waitData();
    returnData = m_socket->getData();

    if(!returnData.isEmpty())
    {
        QStandardItemModel *medicineModel = getModel(returnData,strList);
        ui->medicine_View->setModel(medicineModel);
        return;
    }

    //    int columnCount = ui->medicine_TW->topLevelItemCount();

    //    for(int i = 0;i<columnCount;i++)
    //    {
    //        QTreeWidgetItem *topItem =  ui->medicine_TW->topLevelItem(i);

    //        if(topItem->text(0) == text)
    //        {
    //            m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineFirst = %1")\
    //                                .arg(i+1));
    //            m_socket->waitData();
    //            QString returnData = m_socket->getData();
    //            QStandardItemModel *medicineModel = getModel(returnData,strList);
    //            ui->medicine_View->setModel(medicineModel);
    //        }
    //        else
    //        {
    //            int childCount = topItem->childCount();

    //            for(int j = 0;j<childCount;j++)
    //            {
    //                QTreeWidgetItem *childItem = topItem->child(j);
    //                if(childItem->text(0) == text)
    //                {
    //                    m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineFirst = %1 and medicineSecond = %2")\
    //                                        .arg(i+1).arg(j+1));
    //                    m_socket->waitData();
    //                    QString returnData = m_socket->getData();
    //                    QStandardItemModel *medicineModel = getModel(returnData,strList);
    //                    ui->medicine_View->setModel(medicineModel);
    //                }
    //            }
    //        }
    //    }
}

//主页点击搜索点击事件
void MainWindow::on_homeSearch_BT_clicked()
{
    SearchWidget *searchWidget = new SearchWidget(this,this->m_socket);

    searchWidget->show();
}

//护士小姐姐修改信息
void MainWindow::on_nurseInfoChange_BT_clicked()
{
    ui->nurseInfoChangeComplete_BT->setVisible(true);
    ui->nurseAge_LE->setEnabled(true);
    ui->nurseRoom_LE->setEnabled(true);
    ui->nurseName_LE->setEnabled(true);
    ui->nurseSex_LE->setEnabled(true);
    ui->nursePhone_LE->setEnabled(true);
    ui->nursePassword_LE->setEnabled(true);
}

//护士小姐姐修改信息完成啦
void MainWindow::on_nurseInfoChangeComplete_BT_clicked()
{
    if(QMessageBox::question(this,"更改信息","确认更改?") == QMessageBox::Yes)
    {
        ui->nurseInfoChangeComplete_BT->setVisible(false);
        ui->nurseAge_LE->setEnabled(false);
        ui->nurseRoom_LE->setEnabled(false);
        ui->nurseName_LE->setEnabled(false);
        ui->nurseSex_LE->setEnabled(false);
        ui->nursePhone_LE->setEnabled(false);
        ui->nursePassword_LE->setEnabled(false);


        m_socket->writeData(QString("update nurse_Table set nurseName = '%1',password = '%2',nurseSex = '%3',nurseAge = %4,nurseRoom  = %5,nursePhone = %6 where nurseName = '%8' and password = '%9'")\
                            .arg(ui->nurseName_LE->text())\
                            .arg(ui->nursePassword_LE->text())\
                            .arg(ui->nurseSex_LE->text())\
                            .arg(ui->nurseAge_LE->text())\
                            .arg(ui->nurseRoom_LE->text())\
                            .arg(ui->nursePhone_LE->text())\
                            .arg(this->inner_name)\
                            .arg(this->inner_password));
        m_socket->waitData();
        this->inner_name = ui->nurseName_LE->text();
        this->inner_password = ui->nursePassword_LE->text();
    }
    else
    {
        this->on_nursePersonInfo_Nurse_BT_clicked();
    }
}

//护士小姐姐双击病人view对其查看具体信息并确认使用药物
void MainWindow::on_patient_Nurse_View_doubleClicked(const QModelIndex &index)
{
    int row = ui->patient_Nurse_View->currentIndex().row();

    QString patientName = ui->patient_Nurse_View->model()->index(row,0).data().toString();
    QString patientAge = ui->patient_Nurse_View->model()->index(row,1).data().toString();
    QString patientSex = ui->patient_Nurse_View->model()->index(row,2).data().toString();


    PatientByNurse *patientWidget = new PatientByNurse(this,this->m_socket,patientName,patientAge,patientSex);
    patientWidget->show();
}

//药监员点击查看各类别的药品
void MainWindow::on_medicineIns_TW_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->medicine_Ins_View->resizeColumnsToContents();
    ui->medicine_Ins_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->medicine_Ins_View->verticalHeader()->setVisible(false);
    ui->medicine_Ins_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("药品编号");
    strList->push_back("药品名");
    strList->push_back("剩余量");

    QString text = item->text(0);

    m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineFirst = '%1'")\
                        .arg(text));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    if(!returnData.isEmpty())
    {
        QStandardItemModel *medicineModel = getModel(returnData,strList);
        ui->medicine_Ins_View->setModel(medicineModel);
        return;
    }

    m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineSecond = '%1'")\
                        .arg(text));
    m_socket->waitData();
    returnData = m_socket->getData();

    if(!returnData.isEmpty())
    {
        QStandardItemModel *medicineModel = getModel(returnData,strList);
        ui->medicine_Ins_View->setModel(medicineModel);
        return;
    }
}

void MainWindow::on_urgentMedicine_BT_clicked()
{
    ui->medicine_Ins_View->resizeColumnsToContents();
    ui->medicine_Ins_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->medicine_Ins_View->verticalHeader()->setVisible(false);
    ui->medicine_Ins_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("药品编号");
    strList->push_back("药品名");
    strList->push_back("剩余量");

    m_socket->writeData(QString("select medicineID,medicineName,medicineRemain from medicine_Table where medicineRemain < 100 "));
    m_socket->waitData();
    QString returnData = m_socket->getData();

    if(!returnData.isEmpty())
    {
        QStandardItemModel *medicineModel = getModel(returnData,strList);
        ui->medicine_Ins_View->setModel(medicineModel);
        return;
    }
}

//药监员进购药品槽函数
void MainWindow::on_buyMedicine_BT_clicked()
{
    QModelIndex index = ui->medicine_Ins_View->currentIndex();

    int row = index.row();

    QString medicineName = ui->medicine_Ins_View->model()->index(row,1).data().toString();

    BuyMedicine *buyWidget = new BuyMedicine(this,medicineName);
    buyWidget->show();
    connect(buyWidget,SIGNAL(buyNumSignal(int)),this,SLOT(on_buyNumSlot(int)));
}
