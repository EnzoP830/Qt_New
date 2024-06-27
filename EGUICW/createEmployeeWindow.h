#ifndef CREATEEMPLOYEEWINDOW_H
#define CREATEEMPLOYEEWINDOW_H

#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QLineEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>

class CreateEmployeeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreateEmployeeWindow(QWidget *parent = nullptr);

private slots:
    void createEmployee();

private:
    QWidget *window;
    QSqlRelationalTableModel *model;
    QLineEdit *nameLineEdit;
    QDateEdit *dateEdit;
    QCheckBox *isAvailableCheckBox;
    QSpinBox *hourlyRateSpinBox;
    QPushButton *createButton;
    QGridLayout *gridLayout;

    void createLayout();
    void createModel();
    void configureWidgets();
    void resetFields();

};

#endif // CREATEEMPLOYEEWINDOW_H
