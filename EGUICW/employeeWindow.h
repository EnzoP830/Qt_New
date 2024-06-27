#ifndef EMPLOYEEWINDOW_H
#define EMPLOYEEWINDOW_H

#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QDataWidgetMapper>
#include <QSqlError>
#include <QPushButton>

class EmployeeWindow : public QMainWindow
{
    Q_OBJECT

public:
    EmployeeWindow(QWidget *parent = nullptr);

private slots:
    void showError(const QSqlError &err);
    void about();
    //void openCreateEmployeeWindow();


private:
    void createLayout();
    void createModel();
    void configureWidgets();
    void createMappings();
    void editEmployee();
    void deleteEmployee();

    QWidget *window;
    QTableView *tableView;
    QGridLayout *gridLayout;
    QLineEdit *nameLineEdit;
    QDateEdit *dateEdit;
    QCheckBox *isAvailableCheckBox;
    QSpinBox *hourlyRateSpinBox;
    QPushButton *editEmployeeButton;
    QPushButton *deleteEmployeeButton;
    QSqlRelationalTableModel *model;
};

#endif // EMPLOYEEWINDOW_H
