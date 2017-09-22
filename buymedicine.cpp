#include "buymedicine.h"
#include "ui_buymedicine.h"

BuyMedicine::BuyMedicine(QWidget *parent, QString medicineName) :
    QWidget(parent),
    ui(new Ui::BuyMedicine)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    ui->medicineName_LB->setText(medicineName);
}

BuyMedicine::~BuyMedicine()
{
    delete ui;
}

void BuyMedicine::on_confirm_BT_clicked()
{
    emit buyNumSignal(ui->num1_SB->text().toInt()*1000\
                      +ui->num2_SB->text().toInt()*100\
                      +ui->num3_SB->text().toInt()*10\
                      +ui->num4_SB->text().toInt());
    this->close();
}
