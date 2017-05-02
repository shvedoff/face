#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    try {

    w.show();
    }
    catch (std::exception &e){

        return 1;
    }

    return a.exec();
}
