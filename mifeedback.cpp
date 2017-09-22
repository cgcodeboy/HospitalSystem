#include "mifeedback.h"
#include "ui_mifeedback.h"

QStandardItemModel* MIfeedback::getModel(QString returnData,QStringList *list)
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

MIfeedback::MIfeedback(QWidget *parent,QString feedbackInfo,MyTcpsocket *socket) :
    QWidget(parent),
    ui(new Ui::MIfeedback)
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
    strList->push_back("时间");
    strList->push_back("医生");
    strList->push_back("病人");
    strList->push_back("药品");

    QStandardItemModel *feedbackModel = getModel(feedbackInfo,strList);
    ui->feedback_View->setModel(feedbackModel);
}

MIfeedback::~MIfeedback()
{
    delete ui;
}

void MIfeedback::on_feedback_View_doubleClicked(const QModelIndex &index)
{
    if(QMessageBox::warning(this,"提示信息","同意该医生为病人的药品申请?") == QMessageBox::Ok)
    {
        qDebug()<<"in";
        int row = index.row();
        QString doctorName = ui->feedback_View->model()->index(row,1).data().toString();
        QString patientName = ui->feedback_View->model()->index(row,2).data().toString();
        QString medicineName = ui->feedback_View->model()->index(row,3).data().toString();
        qDebug()<<patientName;
        qDebug()<<doctorName;

        m_socket->writeData(QString("update medicineRequest_Table set requestHandle = 'true' where requestDoctor = '%1' and requestPatient = '%2' and requestMedicine = '%3'")\
                            .arg(doctorName).arg(patientName).arg(medicineName));
        m_socket->waitData();
        this->close();
    }

}
