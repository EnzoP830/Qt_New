#include "createRepairPartsWindow.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>

CreateRepairPartsWindow::CreateRepairPartsWindow(QWidget *parent)
    : QMainWindow(parent), gridLayout(nullptr)
{
    setWindowTitle("Create Repair Part");

    window = new QWidget(this);
    setCentralWidget(window);

    setMinimumSize(500, 400);

    createLayout();
    createModel();
    configureWidgets();
    resetFields();
}

void CreateRepairPartsWindow::createLayout()
{
    gridLayout = new QGridLayout(window);

    descriptionLabel = new QLabel(tr("Description:"), window);
    descriptionLineEdit = new QLineEdit(window);
    quantityLabel = new QLabel(tr("Quantity:"), window);
    quantityLineEdit = new QLineEdit(window);
    unitPriceLabel = new QLabel(tr("Unit Price:"), window);
    unitPriceLineEdit = new QLineEdit(window);
    ticketLabel = new QLabel(tr("Ticket:"), window);
    ticketComboBox = new QComboBox(window);

    createButton = new QPushButton(tr("Create Repair Part"), window);

    gridLayout->addWidget(descriptionLabel, 1, 0, Qt::AlignRight);
    gridLayout->addWidget(descriptionLineEdit, 1, 1, 1, 3);
    gridLayout->addWidget(quantityLabel, 2, 0, Qt::AlignRight);
    gridLayout->addWidget(quantityLineEdit, 2, 1, 1, 3);
    gridLayout->addWidget(unitPriceLabel, 3, 0, Qt::AlignRight);
    gridLayout->addWidget(unitPriceLineEdit, 3, 1, 1, 3);
    gridLayout->addWidget(ticketLabel, 4, 0, Qt::AlignRight);
    gridLayout->addWidget(ticketComboBox, 4, 1, 1, 3);
    gridLayout->addWidget(createButton, 5, 0, 1, 4, Qt::AlignCenter);

    gridLayout->setColumnStretch(1, 1000);
    gridLayout->setColumnStretch(3, 1000);
    gridLayout->setContentsMargins(18, 18, 18, 18);
    gridLayout->setSpacing(18);
    gridLayout->setAlignment(Qt::AlignHCenter);

    connect(createButton, &QPushButton::clicked, this, &CreateRepairPartsWindow::createRepairPart);
}

void CreateRepairPartsWindow::createModel()
{
    model = new QSqlRelationalTableModel(this);
    model->setTable("parts");
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);

    model->setHeaderData(model->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    model->setHeaderData(model->fieldIndex("amount"), Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(model->fieldIndex("unit_price"), Qt::Horizontal, tr("Unit Price"));
    model->setHeaderData(model->fieldIndex("ticket_id"), Qt::Horizontal, tr("Ticket ID"));

    QSqlQuery query("SELECT id, brand, model FROM tickets", model->database());
    while (query.next()) {
        int ticketId = query.value(0).toInt();
        QString ticketLabel = QString("%1 - %2 %3").arg(ticketId).arg(query.value(1).toString()).arg(query.value(2).toString());
        ticketComboBox->addItem(ticketLabel, ticketId);
    }
}

void CreateRepairPartsWindow::configureWidgets()
{}

void CreateRepairPartsWindow::setupComboBoxes()
{}

void CreateRepairPartsWindow::createRepairPart()
{
    QString description = descriptionLineEdit->text();
    QString quantityStr = quantityLineEdit->text();
    QString unitPriceStr = unitPriceLineEdit->text();
    int ticketId = ticketComboBox->currentData().toInt();

    double unitPrice = unitPriceStr.toDouble();
    double quantity = quantityStr.toDouble();
    double total = quantity * unitPrice;

    QSqlRecord record = model->record();
    record.setValue("description", description);
    record.setValue("amount", quantity);
    record.setValue("unit_price", unitPrice);
    record.setValue("ticket_id", ticketId);
    record.setValue("total_price", total);

    if (model->insertRecord(-1, record)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Repair Part Created"), tr("Repair part successfully created."));

            double newRepairPartTotal = total;
            updateTicketTotalPrice(ticketId, newRepairPartTotal);

            resetFields();
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to create repair part: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to insert record into model."));
    }
}

void CreateRepairPartsWindow::updateTicketTotalPrice(int ticketId, double newRepairPartTotal)
{
    QSqlQuery query(model->database());
    query.prepare("SELECT total_price FROM tickets WHERE id = :ticketId");
    query.bindValue(":ticketId", ticketId);

    if (query.exec() && query.next()) {
        double currentTotalPrice = query.value(0).toDouble();
        double updatedTotalPrice = currentTotalPrice + newRepairPartTotal;

        QSqlQuery updateQuery(model->database());
        updateQuery.prepare("UPDATE tickets SET total_price = :updatedTotalPrice WHERE id = :ticketId");
        updateQuery.bindValue(":updatedTotalPrice", updatedTotalPrice);
        updateQuery.bindValue(":ticketId", ticketId);

        if (updateQuery.exec()) {
            QMessageBox::information(this, tr("Ticket Total Price Updated"), tr("Ticket total price updated successfully."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to update ticket total price: %1").arg(updateQuery.lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to fetch current ticket total price: %1").arg(query.lastError().text()));
    }
}


void CreateRepairPartsWindow::resetFields()
{
    descriptionLineEdit->clear();
    quantityLineEdit->clear();
    ticketComboBox->setCurrentIndex(0);
}
