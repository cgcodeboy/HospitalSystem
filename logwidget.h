#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = 0, QString name = NULL, QString password = NULL, QString job = NULL);
    ~LogWidget();

private slots:

    void on_job_CB_currentIndexChanged(int index);

    void on_login_BT_clicked();

signals:
    void infoSignal(QString,QString,QString);

private:
    Ui::LogWidget *ui;
};

#endif // LOGWIDGET_H
