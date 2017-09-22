#include "addpatient.h"
#include "ui_addpatient.h"

AddPatient::AddPatient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddPatient)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
}

AddPatient::~AddPatient()
{
    delete ui;
}

void AddPatient::on_addPatient_BT_clicked()
{
    emit addPatientSignal(ui->patientName_LE->text(),ui->patientSex_LE->text(),ui->patientAge_LE->text(),ui->patientIll_LE->text());
    this->close();
}
