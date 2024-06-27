#include "repairPartsWindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QMenuBar>
#include <QAction>
#include <QApplication>
#include <QDataWidgetMapper>
#include <QDebug>
#include <QSqlQuery>

RepairPartsWindow::RepairPartsWindow(QWidget *parent)
    : QMainWindow(parent), gridLayout(nullptr)
{
    setWindowTitle("Repair Parts Window");

    window = new QWidget(this);
    setCentralWidget(window);

    setMinimumSize(500, 400);

    createLayout();
    createModel();

    // Populate the model
    if (!model->select()) {
        QMessageBox::critical(this, "Unable to initialize model", "Error initializing model: " + model->lastError().text());
        return;
    }
    configureWidgets();
    createMappings();
    tableView->setCurrentIndex(model->index(0, 0));
    tableView->selectRow(0);
}

void RepairPartsWindow::createLayout()
{
    tableView = new QTableView;
    gridLayout = new QGridLayout;

    descriptionLabel = new QLabel(tr("Description:"), window);
    descriptionLineEdit = new QLineEdit(window);
    quantityLabel = new QLabel(tr("Quantity:"), window);
    quantityLineEdit = new QLineEdit(window);
    unitPriceLabel = new QLabel(tr("Unit Price:"), window);
    unitPriceLineEdit = new QLineEdit(window);
    totalPriceLabel = new QLabel(tr("Total Price:"), window);
    totalPriceLineEdit = new QLineEdit(window);
    totalPriceLineEdit->setReadOnly(true);
    editButton = new QPushButton(tr("Edit Repair Part"), window);
    deleteButton = new QPushButton(tr("Delete Repair Part"), window);

    gridLayout->addWidget(descriptionLabel, 1, 0, Qt::AlignRight);
    gridLayout->addWidget(descriptionLineEdit, 1, 1, 1, 3);
    gridLayout->addWidget(quantityLabel, 2, 0, Qt::AlignRight);
    gridLayout->addWidget(quantityLineEdit, 2, 1, 1, 3);
    gridLayout->addWidget(unitPriceLabel, 3, 0, Qt::AlignRight);
    gridLayout->addWidget(unitPriceLineEdit, 3, 1, 1, 3);
    gridLayout->addWidget(totalPriceLabel, 4, 0, Qt::AlignRight);
    gridLayout->addWidget(totalPriceLineEdit, 4, 1, 1, 3);
    gridLayout->addWidget(editButton, 5, 0, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(deleteButton, 5, 2, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(tableView, 6, 0, 1, 4);

    gridLayout->setColumnStretch(1, 1000);
    gridLayout->setColumnStretch(3, 1000);
    gridLayout->setContentsMargins(18, 18, 18, 18);
    gridLayout->setSpacing(18);

    window->setLayout(gridLayout);
}

void RepairPartsWindow::createModel()
{
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    model->setTable("parts");

    // Set header captions
    model->setHeaderData(model->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    model->setHeaderData(model->fieldIndex("amount"), Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(model->fieldIndex("unit_price"), Qt::Horizontal, tr("Unit Price"));
    model->setHeaderData(model->fieldIndex("total_price"), Qt::Horizontal, tr("Total Price"));


    tableView->setModel(model);
    tableView->hideColumn(model->fieldIndex("id"));
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void RepairPartsWindow::configureWidgets()
{
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->verticalHeader()->setVisible(false);

    // Hide totalPriceLineEdit initially
    totalPriceLabel->hide();
    totalPriceLineEdit->hide();
}

void RepairPartsWindow::createMappings()
{
    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->addMapping(descriptionLineEdit, model->fieldIndex("description"));
    mapper->addMapping(quantityLineEdit, model->fieldIndex("amount"));
    mapper->addMapping(unitPriceLineEdit, model->fieldIndex("unit_price"));

    //connect(tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper, &QDataWidgetMapper::setCurrentModelIndex);

    connect(editButton, &QPushButton::clicked, this, &RepairPartsWindow::editRepairPart);
    connect(deleteButton, &QPushButton::clicked, this, &RepairPartsWindow::deleteRepairPart);
}

void RepairPartsWindow::editRepairPart()
{
    QModelIndexList selection = tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a repair part to edit."));
        return;
    }

    QModelIndex index = selection.at(0);
    int row = index.row();

    QSqlRecord record = model->record(row);
    QString oldDescription = record.value("description").toString();
    double oldAmount = record.value("amount").toDouble();
    double oldUnitPrice = record.value("unit_price").toDouble();
    double oldTotalPrice = record.value("total_price").toDouble();

    QString newDescription = descriptionLineEdit->text();
    double newAmount = quantityLineEdit->text().toDouble();
    double newUnitPrice = unitPriceLineEdit->text().toDouble();
    double newTotalPrice = newAmount * newUnitPrice;

    // Update the record in the database
    record.setValue("description", newDescription);
    record.setValue("amount", newAmount);
    record.setValue("unit_price", newUnitPrice);
    record.setValue("total_price", newTotalPrice);

    if (model->setRecord(row, record)) {
        if (model->submitAll()) {
            // Calculate the difference in total price
            double totalPriceDifference = newTotalPrice - oldTotalPrice;

            // Get the ticket ID associated with this repair part
            int ticketId = record.value("ticket_id").toInt();

            // Update the ticket total price
            updateTicketTotalPrice(ticketId, totalPriceDifference);

            QMessageBox::information(this, tr("Repair Part Updated"), tr("Repair part successfully updated."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to update repair part: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to set record for editing."));
    }
}


void RepairPartsWindow::deleteRepairPart()
{
    QModelIndexList selection = tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a repair part to delete."));
        return;
    }

    if (QMessageBox::question(this, tr("Confirm Delete"), tr("Are you sure you want to delete this repair part?"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    QModelIndex index = selection.at(0);
    int row = index.row();

    if (model->removeRow(row)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Repair Part Deleted"), tr("Repair part successfully deleted."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to delete repair part: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to remove row from model."));
    }
}


void RepairPartsWindow::updateTicketTotalPrice(int ticketId, double newRepairPartTotal)
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



void RepairPartsWindow::refreshModel()
{
    model->select();
    tableView->reset();
}
