#ifndef CREATEREPAIRPartWINDOW_H
#define CREATEREPAIRPartWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QSqlRelationalTableModel>
#include "repairPartWindow.h"

class CreateRepairPartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreateRepairPartWindow(RepairPartWindow *parentWindow, QWidget *parent = nullptr);

private slots:
    void createRepairPart();

private:
    void createLayout();
    void createModel();
    void configureWidgets();
    void resetFields();
    void setupTicketComboBox();

    RepairPartWindow *parentWindow;
    QWidget *window;
    QGridLayout *gridLayout;

    QLineEdit *descriptionLineEdit;
    QLineEdit *amountLineEdit;
    QLineEdit *unitPriceLineEdit;
    QLineEdit *totalPriceLineEdit;
    QComboBox *ticketIdComboBox;
    QPushButton *createButton;

    QSqlRelationalTableModel *model;
};

#endif // CREATEREPAIRPartWINDOW_H
