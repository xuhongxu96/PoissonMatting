#include "mainwindow.h"
#include <QTranslator>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("poisson.qm");
    a.installTranslator(&qtTranslator);
    MainWindow w;
    w.show();

    return a.exec();
}
