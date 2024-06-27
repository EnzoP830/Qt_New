#include "mainwindow.h"
#include "employeeWindow.h"
#include "ticketWindow.h"
#include "repairPartsWindow.h"
#include "createEmployeeWindow.h"
#include "createTicketWindow.h"
#include "createRepairPartsWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include "initdb.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Main Window");

    // Initialize the database:
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QMessageBox::critical(this, "Unable to load database", "This demo needs the SQLITE driver");
        return;
    }

    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
        QMessageBox::critical(this, "Unable to initialize Database", "Error initializing database: " + err.text());
        return;
    }

    createMenuBar();
    setupWelcomeLabel();
}

void MainWindow::createMenuBar()
{
    QMenu *createMenu = menuBar()->addMenu(tr("&Create"));
    QAction *employeeCreate = createMenu->addAction(tr("&Employee"));
    connect(employeeCreate, &QAction::triggered, this, &MainWindow::showCreateEmployeeWindow);

    QAction *ticketCreate = createMenu->addAction(tr("&Ticket"));
    connect(ticketCreate, &QAction::triggered, this, &MainWindow::showCreateTicketWindow);

    QAction *repairPartsCreate = createMenu->addAction(tr("&Repair Parts"));
    connect(repairPartsCreate, &QAction::triggered, this, &MainWindow::showCreateRepairPartsWindow);

    QMenu *viewMenu = menuBar()->addMenu(tr("&Edit"));
    QAction *employeeAction = viewMenu->addAction(tr("&Employee"));
    connect(employeeAction, &QAction::triggered, this, &MainWindow::showEmployeeWindow);

    QAction *ticketAction = viewMenu->addAction(tr("&Ticket"));
    connect(ticketAction, &QAction::triggered, this, &MainWindow::showTicketWindow);

    QAction *repairPartsAction = viewMenu->addAction(tr("&Repair Parts"));
    connect(repairPartsAction, &QAction::triggered, this, &MainWindow::showRepairPartsWindow);

    QMenu *fileMenu = menuBar()->addMenu(tr("&Quit"));
    QAction *quitAction = fileMenu->addAction(tr("&Quit"));
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::setupWelcomeLabel()
{
    welcomeLabel = new QLabel("Welcome to our Car Workshop!", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    setMinimumSize(500, 400);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(welcomeLabel);

    centralWidget->setLayout(layout);
}

void MainWindow::showEmployeeWindow()
{
    EmployeeWindow *employeeWindow = new EmployeeWindow(this);
    employeeWindow->show();
    employeeWindow->raise();
    employeeWindow->activateWindow();
}

void MainWindow::showTicketWindow()
{
    TicketWindow *ticketWindow = new TicketWindow(this);
    ticketWindow->show();
    ticketWindow->raise();
    ticketWindow->activateWindow();
}

void MainWindow::showCreateEmployeeWindow()
{
    CreateEmployeeWindow *createEmployeeWindow = new CreateEmployeeWindow(this);
    createEmployeeWindow->show();
    createEmployeeWindow->raise();
    createEmployeeWindow->activateWindow();
}

void MainWindow::showCreateTicketWindow()
{
    TicketWindow *ticketWindow = new TicketWindow(this);
    CreateTicketWindow *createTicketWindow = new CreateTicketWindow(ticketWindow);

    createTicketWindow->show();
    createTicketWindow->raise();
    createTicketWindow->activateWindow();
}

void MainWindow::showRepairPartsWindow()
{
    RepairPartsWindow *repairPartsWindow = new RepairPartsWindow(this);
    repairPartsWindow->show();
    repairPartsWindow->raise();
    repairPartsWindow->activateWindow();
}

void MainWindow::showCreateRepairPartsWindow()
{
    CreateRepairPartsWindow *createRepairPartsWindow = new CreateRepairPartsWindow(this);
    createRepairPartsWindow->show();
    createRepairPartsWindow->raise();
    createRepairPartsWindow->activateWindow();
}
