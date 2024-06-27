// repairpartwindow.cpp
#include "repairpartwindow.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

RepairPartWindow::RepairPartWindow(QWidget *parent)
    : QWidget(parent),
    registrationIdComboBox(new QComboBox(this)),
    descriptionLineEdit(new QLineEdit(this)),
    amountSpinBox(new QSpinBox(this)),
    unitPriceSpinBox(new QDoubleSpinBox(this)),
    totalPriceSpinBox(new QDoubleSpinBox(this)),
    addButton(new QPushButton(tr("Add Repair Part"), this)),
    ticketModel(new QSqlTableModel(this))
{
    amountSpinBox->setMaximum(1000);
    unitPriceSpinBox->setMaximum(10000);
    totalPriceSpinBox->setMaximum(100000);

    ticketModel->setTable("tickets");
    ticketModel->select();

    for (int i = 0; i < ticketModel->rowCount(); ++i) {
        QString registrationId = ticketModel->record(i).value("registration_id").toString();
        registrationIdComboBox->addItem(registrationId, ticketModel->record(i).value("id"));
    }

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Registration ID"), registrationIdComboBox);
    formLayout->addRow(tr("Description"), descriptionLineEdit);
    formLayout->addRow(tr("Amount"), amountSpinBox);
    formLayout->addRow(tr("Unit Price"), unitPriceSpinBox);
    formLayout->addRow(tr("Total Price"), totalPriceSpinBox);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(addButton);

    setLayout(mainLayout);

    connect(addButton, &QPushButton::clicked, this, &RepairPartWindow::addRepairPart);
}

void RepairPartWindow::addRepairPart()
{
    int ticketId = registrationIdComboBox->currentData().toInt();
    QString description = descriptionLineEdit->text();
    int amount = amountSpinBox->value();
    double unitPrice = unitPriceSpinBox->value();
    double totalPrice = totalPriceSpinBox->value();

    QSqlQuery query;
    query.prepare("INSERT INTO parts (description, amount, unit_price, total_price, ticket_id) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(description);
    query.addBindValue(amount);
    query.addBindValue(unitPrice);
    query.addBindValue(totalPrice);
    query.addBindValue(ticketId);

    if (query.exec()) {
        QMessageBox::information(this, tr("Success"), tr("Repair part added successfully."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to add repair part: %1").arg(query.lastError().text()));
    }
}
