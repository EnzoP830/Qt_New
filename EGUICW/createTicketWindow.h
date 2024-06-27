#ifndef CREATETICKETWINDOW_H
#define CREATETICKETWINDOW_H

#include "ticketWindow.h"
#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QComboBox>

class CreateTicketWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreateTicketWindow(TicketWindow *parentWindow, QWidget *parent = nullptr);

private:
    QWidget *window;
    QSqlRelationalTableModel *model;
    QGridLayout *gridLayout;
    QLineEdit *brandLineEdit;
    QLineEdit *modelLineEdit;
    QLineEdit *registrationIdLineEdit;
    QLineEdit *descriptionLineEdit;
    QComboBox *employeeIdComboBox;
    QDateEdit *dateEdit;
    QComboBox *stateComboBox;
    QPushButton *createButton;
    TicketWindow *parentWindow;
    QSqlTableModel *employeeModel;

    void createLayout();
    void createModel();
    void configureWidgets();
    void createTicket();
    void setupEmployeeComboBox();
    void updateEmployeeComboBox(const QDate &selectedDate);
    void resetFields();

};

#endif // CREATETICKETWINDOW_H
