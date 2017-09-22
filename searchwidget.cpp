#include "searchwidget.h"
#include "ui_searchwidget.h"

SearchWidget::SearchWidget(QWidget *parent, MyTcpsocket *socket) :
    QWidget(parent),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    this->m_socket = socket;
}

SearchWidget::~SearchWidget()
{
    delete ui;
}

void SearchWidget::on_search_BT_clicked()
{
    SearchOutput *output = new SearchOutput(this->parentWidget(),m_socket,ui->searchContent_LE->text());
    output->show();
    this->close();
}
