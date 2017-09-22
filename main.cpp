#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font;
    font.setFamily("楷体");
    font.setPixelSize(12);

    a.setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}
