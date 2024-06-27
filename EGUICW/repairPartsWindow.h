#ifndef REPAIRPARTSWINDOW_H
#define REPAIRPARTSWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTableView>
#include <QGridLayout>
#include <QMessageBox>
#include <QSqlRelationalTableModel>

class RepairPartsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RepairPartsWindow(QWidget *parent = nullptr);

private slots:
    void editRepairPart();
    void deleteRepairPart();
    void refreshModel();

private:
    void createLayout();
    void createModel();
    void configureWidgets();
    void createMappings();
    void updateTicketTotalPrice(int ticketId, double newRepairPartTotal);

    QTableView *tableView;
    QWidget *window;
    QGridLayout *gridLayout;
    QLabel *descriptionLabel;
    QLineEdit *descriptionLineEdit;
    QLabel *quantityLabel;
    QLineEdit *quantityLineEdit;
    QLabel *unitPriceLabel;
    QLineEdit *unitPriceLineEdit;
    QLabel *totalPriceLabel;
    QLineEdit *totalPriceLineEdit;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QSqlRelationalTableModel *model;
};

#endif // REPAIRPARTSWINDOW_H
