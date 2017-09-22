#include "logwidget.h"
#include "ui_logwidget.h"

LogWidget::LogWidget(QWidget *parent, QString name, QString password, QString job) :
    QWidget(parent),
    ui(new Ui::LogWidget)
{
    ui->setupUi(this);

    ui->name_LE->setText(name);
    ui->password_LE->setText(password);

    if(job == "doctor")
    {
        ui->job_CB->setCurrentIndex(0);
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/doctor.png)");
    }
    else if(job == "nurse")
    {
        ui->job_CB->setCurrentIndex(1);
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/nurse.png)");
    }
    else if(job == "medicine")
    {
        ui->job_CB->setCurrentIndex(2);
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/medicine.png)");
    }
    else if(job == "financial")
    {
        ui->job_CB->setCurrentIndex(3);
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/financial.png)");
    }
    else if(job == "manager")
    {
        ui->job_CB->setCurrentIndex(4);
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/manager.png)");
    }
    else
    {
        ui->job_CB->setCurrentIndex(0);
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/doctor.png)");
    }
    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
}

LogWidget::~LogWidget()
{
    delete ui;
}

void LogWidget::on_job_CB_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/doctor.png)");
    }
    else if(index == 1)
    {
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/nurse.png)");
    }
    else if(index == 2)
    {
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/medicine.png)");
    }
    else if(index == 3)
    {
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/financial.png)");
    }
    else
    {
        ui->job_widget->setStyleSheet("border-image: url(:/image/image/manager.png)");
    }
}

void LogWidget::on_login_BT_clicked()
{
    QString job;
    if(ui->job_CB->currentIndex() == 0)
        job = "doctor";
    else if(ui->job_CB->currentIndex() == 1)
        job = "nurse";
    else if(ui->job_CB->currentIndex() == 2)
        job = "medicine";
    else if(ui->job_CB->currentIndex() == 3)
        job = "financial";
    else if(ui->job_CB->currentIndex() == 4)
        job = "manager";
    emit infoSignal(ui->name_LE->text(),ui->password_LE->text(),job);
    this->close();
}
