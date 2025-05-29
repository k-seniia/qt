#include <QApplication>
#include "mainwindow.h"

int main(
    int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Багатокритеріальна оптимізація");
    window.resize(1100, 400);
    window.show();

    return app.exec();
}
