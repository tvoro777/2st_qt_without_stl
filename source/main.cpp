#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include "source/gui/mainwindow.h"

int main(int argc, char *argv[])
{
    qDebug() << "Приложение запущено";

    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}
