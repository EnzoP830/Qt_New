#ifndef TICKETWINDOW_H
#define TICKETWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDateEdit>
#include <QPushButton>

class TicketWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TicketWindow(QWidget *parent = nullptr);
    void refreshModel();

private:
    void createLayout();
    void createModel();
    void configureWidgets();
    void createMappings();
    void showError(const QSqlError &err);
    void setupEmployeeComboBox();
    void editTicket();
    void deleteTicket();

    QWidget *window;
    QTableView *tableView;
    QSqlRelationalTableModel *model;
    QGridLayout *gridLayout;
    QLineEdit *brandLineEdit;
    QLineEdit *modelLineEdit;
    QLineEdit *registrationIdLineEdit;
    QLineEdit *descriptionLineEdit;
    QComboBox *employeeIdComboBox;
    QDateEdit *dateEdit;
    QComboBox *stateComboBox;
    QPushButton *editTicketButton;
    QPushButton *deleteTicketButton;

};

#endif // TICKETWINDOW_H
