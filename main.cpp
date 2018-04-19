#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    RInside R(argc, argv);  		// create an embedded R instance

    QApplication a(argc, argv);
    MainWindow w(R);		// pass R inst. by reference;
    w.show();

    return a.exec();
}
