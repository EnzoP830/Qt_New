// repairpartwindow.h
#ifndef REPAIRPARTWINDOW_H
#define REPAIRPARTWINDOW_H

#include <QWidget>
#include <QtSql>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class RepairPartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RepairPartWindow(QWidget *parent = nullptr);

private slots:
    void addRepairPart();

private:
    QComboBox *registrationIdComboBox;
    QLineEdit *descriptionLineEdit;
    QSpinBox *amountSpinBox;
    QDoubleSpinBox *unitPriceSpinBox;
    QDoubleSpinBox *totalPriceSpinBox;
    QPushButton *addButton;

    QSqlTableModel *ticketModel;
};

#endif // REPAIRPARTWINDOW_H
