#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QMessageBox::critical(nullptr, "Unable to load database", "This demo needs the SQLITE driver");
        return -1;
    }

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
