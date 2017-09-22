#ifndef BUYMEDICINE_H
#define BUYMEDICINE_H

#include <QWidget>

namespace Ui {
class BuyMedicine;
}

class BuyMedicine : public QWidget
{
    Q_OBJECT

public:
    explicit BuyMedicine(QWidget *parent = 0,QString medicineName = 0);
    ~BuyMedicine();

private slots:
    void on_confirm_BT_clicked();

private:
    Ui::BuyMedicine *ui;

signals:
    void buyNumSignal(int num);
};

#endif // BUYMEDICINE_H
