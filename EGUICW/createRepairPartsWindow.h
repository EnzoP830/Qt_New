#ifndef CREATEREPAIRPARTSWINDOW_H
#define CREATEREPAIRPARTSWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QSqlRelationalTableModel>

class CreateRepairPartsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreateRepairPartsWindow(QWidget *parent = nullptr);

private slots:
    void createRepairPart();
    void resetFields();

private:
    void createLayout();
    void createModel();
    void configureWidgets();
    void setupComboBoxes();
    void updateTicketTotalPrice(int ticketId, double newRepairPartTotal);

    QWidget *window;
    QGridLayout *gridLayout;
    QLabel *descriptionLabel;
    QLineEdit *descriptionLineEdit;
    QLabel *quantityLabel;
    QLineEdit *quantityLineEdit;
    QLabel *unitPriceLabel;
    QLineEdit *unitPriceLineEdit;
    QLabel *ticketLabel;
    QComboBox *ticketComboBox;
    QPushButton *createButton;
    QSqlRelationalTableModel *model;
};

#endif // CREATEREPAIRPARTSWINDOW_H
