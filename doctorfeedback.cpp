#include "doctorfeedback.h"
#include "ui_doctorfeedback.h"

QStringList *DoctorFeedback::getInfoFromReturnString(QString returnStr)
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

QStandardItemModel* DoctorFeedback::getModel(QString returnData,QStringList *list)
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

DoctorFeedback::DoctorFeedback(QWidget *parent, QString feedbackInfo, MyTcpsocket *socket) :
    QWidget(parent),
    ui(new Ui::DoctorFeedback)
{
    ui->setupUi(this);

    m_socket = socket;

    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    ui->feedback_View->resizeColumnsToContents();
    ui->feedback_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->feedback_View->verticalHeader()->setVisible(false);
    ui->feedback_View->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList *strList = new QStringList;
    strList->push_back("病人");
    strList->push_back("医生");
    strList->push_back("时间");
    strList->push_back("反馈内容");

    QStandardItemModel *feedbackModel = getModel(feedbackInfo,strList);
    ui->feedback_View->setModel(feedbackModel);
}

DoctorFeedback::~DoctorFeedback()
{
    delete ui;
}

void DoctorFeedback::on_confirm_BT_clicked()
{
    QString doctorName = ui->feedback_View->model()->index(0,1).data().toString();
    m_socket->writeData(QString("update feedback_Table set feedbackHandle = 'true' where feedbackDoctor = '%1'")\
                        .arg(doctorName));
    m_socket->waitData();
    this->close();
}
