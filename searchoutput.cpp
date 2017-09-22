#include "searchoutput.h"
#include "ui_searchoutput.h"

//根据特定得返回字符串得到model
QStandardItemModel *SearchOutput::getModel(QString returnData,QStringList *list)
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

SearchOutput::SearchOutput(QWidget *parent, MyTcpsocket *socket, QString str) :
    QWidget(parent),
    ui(new Ui::SearchOutput)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
    this->m_socket = socket;

    ui->output_View->resizeColumnsToContents();
    ui->output_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->output_View->verticalHeader()->setVisible(false);
    ui->output_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;

    QRegExp *rex = new QRegExp("[0123456789]{9}");

    if(rex->exactMatch(str))
    {
        qDebug()<<"match 9";
        strList->push_back("姓名");
        strList->push_back("性别");
        strList->push_back("年龄");
        m_socket->writeData(QString("select patientName,patientSex,patientAge from patient_Table where patientID = %1")\
                            .arg(str));

        m_socket->waitData();

        QString returnData = m_socket->getData();

        qDebug()<<returnData;

        QStandardItemModel *outputModel = getModel(returnData,strList);

        ui->output_View->setModel(outputModel);
    }

    rex = new QRegExp("[0123456789]{5}");

    if(rex->exactMatch(str))
    {
        qDebug()<<"match 5";

        m_socket->writeData(QString("select doctorName,doctorSex,doctorAge,doctorRoom,doctorPhone,doctorDis from doctor_Table where doctorID = %1")\
                            .arg(str));

        m_socket->waitData();
        QString returnData = m_socket->getData();

        if(!returnData.isEmpty())
        {
            strList->push_back("姓名");
            strList->push_back("性别");
            strList->push_back("年龄");
            strList->push_back("办公室");
            strList->push_back("电话");
            strList->push_back("主治");

            QStandardItemModel *outputModel = getModel(returnData,strList);

            ui->output_View->setModel(outputModel);
        }

        m_socket->writeData(QString("select nurseName,nurseSex,nurseAge,nurseRoom,nursePhone from nurse_Table where nurseID = %1")\
                            .arg(str));
        m_socket->waitData();
        returnData = m_socket->getData();

        if(!returnData.isEmpty())
        {
            strList->push_back("姓名");
            strList->push_back("性别");
            strList->push_back("年龄");
            strList->push_back("办公室");
            strList->push_back("电话");

            QStandardItemModel *outputModel = getModel(returnData,strList);

            ui->output_View->setModel(outputModel);
        }
    }

    rex = new QRegExp("[\u4e00-\u9fa5]{2,}");

    if(rex->exactMatch(str))
    {
        qDebug()<<"match 中文";

        QString returnData;

        m_socket->writeData(QString("select patientName,patientSex,patientAge from patient_Table where patientName = '%1'")\
                            .arg(str));
        m_socket->waitData();

        QString patientStr = m_socket->getData();

        if(!patientStr.isEmpty())
        {
            int index = patientStr.lastIndexOf("@");
            patientStr.insert(index,"患者#");
            returnData += patientStr;
        }

        m_socket->writeData(QString("select doctorName ,doctorSex,doctorAge from doctor_Table where doctorName = '%1'")\
                            .arg(str));
        m_socket->waitData();

        QString doctorStr = m_socket->getData();

        if(!doctorStr.isEmpty())
        {
            int index = doctorStr.lastIndexOf("@");
            doctorStr.insert(index,"医生#");
            returnData = returnData+doctorStr;
        }

        m_socket->writeData(QString("select nurseName ,nurseSex,nurseAge from nurse_Table where nurseName = '%1'")\
                            .arg(str));
        m_socket->waitData();
        QString nurseStr = m_socket->getData();

        if(!nurseStr.isEmpty())
        {
            int index = nurseStr.lastIndexOf("@");
            nurseStr.insert(index,"护士#");
            returnData = returnData + nurseStr;
        }
        if(!returnData.isEmpty())
        {
            strList->push_back("姓名");
            strList->push_back("性别");
            strList->push_back("年龄");
            strList->push_back("备注");

            QStandardItemModel *outputModel = getModel(returnData,strList);

            ui->output_View->setModel(outputModel);

            return;
        }
    }
    rex = new QRegExp("[\u4e00-\u9fa5]{2,}(.*)");

    if(rex->exactMatch(str))
    {
        qDebug()<<"匹配 药品";
        m_socket->writeData(QString("select medicineName,medicineRemain from medicine_Table where medicineName = '%1'")\
                            .arg(str));
        m_socket->waitData();
        QString returnData = m_socket->getData();
        if(!returnData.isEmpty())
        {
            strList->push_back("药品");
            strList->push_back("剩余量");

            QStandardItemModel *outputModel = getModel(returnData,strList);
            ui->output_View->setModel(outputModel);
        }
    }
}

SearchOutput::~SearchOutput()
{
    delete ui;
    this->deleteLater();
}
