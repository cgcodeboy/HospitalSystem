#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->m_server = new QTcpServer;
    this->m_socketMap = new QMap<QString,QTcpSocket*>;
    connect(m_server,SIGNAL(newConnection()),this,SLOT(newConnectSlot()));
    connect(&timer,SIGNAL(timeout()),this,SLOT(checkInformation()));
    timer.start(2000);
    currentSocket = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
    m_server->deleteLater();
}

void MainWindow::on_listen_BT_clicked()
{
    if(!m_server->isListening())
    {
        m_server->listen(QHostAddress::Any,5555);
    }
}

void MainWindow::on_close_BT_clicked()
{
    m_server->close();
    QMap<QString,QTcpSocket*>::iterator it = m_socketMap->begin();
    while(it!=m_socketMap->end())
    {
        qDebug()<<it.key();
        it.value()->close();
        it++;
    }
}

void MainWindow::newConnectSlot()
{
    QColor color(50,200,50);
    ui->info_Text->setTextColor(color);
    currentSocket = m_server->nextPendingConnection();
    QString currentAdr = currentSocket->localAddress().toString();
    m_socketMap->insert(currentAdr,currentSocket);
    ui->info_Text->append("新的连接已建立！"+currentAdr);
    connect(currentSocket,SIGNAL(readyRead()),this,SLOT(dataComingSlot()));
    connect(currentSocket,SIGNAL(disconnected()),this,SLOT(disconnectSlot()));
    connect(currentSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorSlot(QAbstractSocket::SocketError)));
}

void MainWindow::dataComingSlot()
{
    if(currentSocket->bytesAvailable())
    {
        QString str = QString::fromLocal8Bit(currentSocket->readAll());
        qDebug()<<str;
        QString client_name = currentSocket->localAddress().toString();

        QSqlQuery query;
        bool ok = query.exec(str);
        if(!ok)
        {
            currentSocket->write(QString("ERROR").toLocal8Bit());
            return;
        }
        QString returnData;
        int count = query.record().count();
        while(query.next())
        {
            for(int i = 0;i<count;i++)
            {
                returnData.append(query.value(i).toString()+"#");
            }
            returnData.append("@");
        }
        qDebug()<<returnData;

        currentSocket->write(returnData.toLocal8Bit());

        QSqlQueryModel *model = new QSqlQueryModel;
        model->setQuery(query);
        QTableView *view = ui->model_VIEW;
        view->setModel(model);
        view->show();
    }
}

void MainWindow::disconnectSlot()
{
    if(!m_server->isListening())
    {
        QMessageBox::information(this,"连接信息","停止监听，所有连接已断开");
        return;
    }
    QMessageBox::warning(this,"连接信息","有连接已断开！");
}

void MainWindow::on_connect_BT_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName(ui->dbname_LE->text().toStdString().c_str());
    db.setUserName(ui->user_LE->text().toStdString().c_str());
    db.setPassword(ui->password_LE->text().toStdString().c_str());
    bool ok = db.open();
    if(!ok)
    {
        QMessageBox::warning(this,"登陆信息","连接错误或者用户名、密码错误！");
        return;
    }
    QColor color(200,50,50);
    ui->info_Text->setTextColor(color);
    ui->info_Text->append("数据库已连接！");
}

void MainWindow::errorSlot(QAbstractSocket::SocketError error)
{
    qDebug()<<error;
}

//检测信息，并发送给客户端
void MainWindow::checkInformation()
{
    if(currentSocket)
        this->currentSocket->waitForBytesWritten(20);
    QSqlQuery query;
    query.exec("select feedbackPatient,feedbackDoctor,feedbackTime,feedbackContent from feedback_Table where feedbackHandle = 'false'");
    QString returnData;
    int count = query.record().count();
    if(query.size()!=0)
    {
        returnData.append("$");
    }
    while(query.next())
    {
        for(int i = 0;i<count;i++)
        {
            returnData.append(query.value(i).toString()+"#");
        }
        returnData.append("@");
    }
    if(!returnData.isEmpty())
    {
        qDebug()<<returnData;
        if(currentSocket&&currentSocket->isWritable())
        {
            currentSocket->write(returnData.toLocal8Bit());
        }
    }
    if(currentSocket)
        this->currentSocket->waitForBytesWritten(20);
    returnData.clear();

    query.exec("select requestTime,requestDoctor,requestPatient,requestMedicine from medicineRequest_Table where requestHandle = 'false'");
    returnData;
    count = query.record().count();
    if(query.size()!=0)
    {
        returnData.append("$");
    }
    while(query.next())
    {
        for(int i = 0;i<count;i++)
        {
            returnData.append(query.value(i).toString()+"#");
        }
        returnData.append("@");
    }
    if(!returnData.isEmpty())
    {
        qDebug()<<returnData;
        if(currentSocket&&currentSocket->isWritable())
        {
            currentSocket->write(returnData.toLocal8Bit());
        }
    }
}
