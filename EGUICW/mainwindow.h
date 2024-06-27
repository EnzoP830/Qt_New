#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include "employeeWindow.h"
#include "ticketWindow.h"
#include "repairPartsWindow.h"
#include "createEmployeeWindow.h"
#include "createTicketWindow.h"
#include "createRepairPartsWindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void showEmployeeWindow();
    void showTicketWindow();
    void showRepairPartsWindow();
    void showCreateEmployeeWindow();
    void showCreateTicketWindow();
    void showCreateRepairPartsWindow();

private:
    void createMenuBar();
    void setupWelcomeLabel();

    EmployeeWindow *employeeWindow;
    TicketWindow *ticketWindow;
    RepairPartsWindow *repairPartsWindow;
    CreateEmployeeWindow *createEmployeeWindow;
    CreateTicketWindow *createTicketWindow;
    CreateRepairPartsWindow *createRepairPartsWindow;

    QLabel *welcomeLabel;

};

#endif // MAINWINDOW_H
