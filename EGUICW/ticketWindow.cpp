#include "ticketWindow.h"
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRelationalDelegate>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlRecord>

TicketWindow::TicketWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Ticket Window");

    // Create the central widget for the window
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

    setupEmployeeComboBox();
    configureWidgets();
    createMappings();
    tableView->setCurrentIndex(model->index(0, 0));
    tableView->selectRow(0);
}

void TicketWindow::createLayout()
{
    tableView = new QTableView(window);
    gridLayout = new QGridLayout(window);

    QLabel *brandLabel = new QLabel(tr("Brand:"), window);
    brandLineEdit = new QLineEdit(window);
    QLabel *modelLabel = new QLabel(tr("Model:"), window);
    modelLineEdit = new QLineEdit(window);
    QLabel *registrationIdLabel = new QLabel(tr("Registration ID:"), window);
    registrationIdLineEdit = new QLineEdit(window);
    QLabel *descriptionLabel = new QLabel(tr("Description:"), window);
    descriptionLineEdit = new QLineEdit(window);
    QLabel *employeeIdLabel = new QLabel(tr("Employee:"), window);
    employeeIdComboBox = new QComboBox(window);
    QLabel *dateLabel = new QLabel(tr("Date:"), window);
    dateEdit = new QDateEdit(window);
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    QLabel *stateLabel = new QLabel(tr("State:"), window);
    stateComboBox = new QComboBox(window); // Utilisation de QComboBox
    stateComboBox->addItems({"created", "in progress", "done", "closed"});
    editTicketButton = new QPushButton(tr("Edit Ticket"), window);
    deleteTicketButton = new QPushButton(tr("Delete Ticket"), window);

    gridLayout->addWidget(brandLabel, 0, 0, Qt::AlignRight);
    gridLayout->addWidget(brandLineEdit, 0, 1, 1, 3);
    gridLayout->addWidget(modelLabel, 1, 0, Qt::AlignRight);
    gridLayout->addWidget(modelLineEdit, 1, 1, 1, 3);
    gridLayout->addWidget(registrationIdLabel, 2, 0, Qt::AlignRight);
    gridLayout->addWidget(registrationIdLineEdit, 2, 1, 1, 3);
    gridLayout->addWidget(descriptionLabel, 3, 0, Qt::AlignRight);
    gridLayout->addWidget(descriptionLineEdit, 3, 1, 1, 3);
    gridLayout->addWidget(employeeIdLabel, 4, 0, Qt::AlignRight);
    gridLayout->addWidget(employeeIdComboBox, 4, 1, 1, 3);
    gridLayout->addWidget(dateLabel, 5, 0, Qt::AlignRight);
    gridLayout->addWidget(dateEdit, 5, 1, 1, 3);
    gridLayout->addWidget(stateLabel, 6, 0, Qt::AlignRight);
    gridLayout->addWidget(stateComboBox, 6, 1, 1, 3);
    gridLayout->addWidget(editTicketButton, 7, 0, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(deleteTicketButton, 7, 2, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(tableView, 8, 0, 1, 4, Qt::AlignCenter);

    gridLayout->setColumnStretch(1, 1000);
    gridLayout->setColumnStretch(3, 1000);
    gridLayout->setContentsMargins(18, 18, 18, 18);
    gridLayout->setSpacing(18);
    gridLayout->setAlignment(Qt::AlignHCenter);
}

void TicketWindow::createModel()
{
    model = new QSqlRelationalTableModel(tableView);
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    model->setTable("tickets");

    // Set the relations to the other database tables
    model->setRelation(model->fieldIndex("employee_id"), QSqlRelation("employees", "id", "name"));

    // Set the localised header captions
    model->setHeaderData(model->fieldIndex("brand"), Qt::Horizontal, tr("Brand"));
    model->setHeaderData(model->fieldIndex("model"), Qt::Horizontal, tr("Model"));
    model->setHeaderData(model->fieldIndex("registration_id"), Qt::Horizontal, tr("Registration ID"));
    model->setHeaderData(model->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    model->setHeaderData(model->fieldIndex("employee_id"), Qt::Horizontal, tr("Employee"));
    model->setHeaderData(model->fieldIndex("time_slots"), Qt::Horizontal, tr("Time Slots"));
    model->setHeaderData(model->fieldIndex("state"), Qt::Horizontal, tr("State"));



}

void TicketWindow::configureWidgets()
{
    tableView->setModel(model);
    tableView->setColumnHidden(model->fieldIndex("id"), true);
    tableView->verticalHeader()->setVisible(false);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void TicketWindow::createMappings()
{
    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(brandLineEdit, model->fieldIndex("brand"));
    mapper->addMapping(modelLineEdit, model->fieldIndex("model"));
    mapper->addMapping(registrationIdLineEdit, model->fieldIndex("registration_id"));
    mapper->addMapping(descriptionLineEdit, model->fieldIndex("description"));
    mapper->addMapping(employeeIdComboBox, model->fieldIndex("employee_id"));
    mapper->addMapping(dateEdit, model->fieldIndex("time_slots"));
    mapper->addMapping(stateComboBox, model->fieldIndex("state"));

    //connect(tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper, &QDataWidgetMapper::setCurrentModelIndex);

    connect(editTicketButton, &QPushButton::clicked, this, &TicketWindow::editTicket);
    connect(deleteTicketButton, &QPushButton::clicked, this, &TicketWindow::deleteTicket);
}

void TicketWindow::setupEmployeeComboBox()
{
    QSqlTableModel *employeeModel = new QSqlTableModel(this);
    employeeModel->setTable("employees");
    employeeModel->select();

    employeeIdComboBox->setModel(employeeModel);
    employeeIdComboBox->setModelColumn(employeeModel->fieldIndex("name"));
}

void TicketWindow::editTicket()
{
    QModelIndexList selection = tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a ticket to edit."));
        return;
    }

    QModelIndex index = selection.at(0);
    int row = index.row();

    QSqlRecord record = model->record(row);
    record.setValue("brand", brandLineEdit->text());
    record.setValue("model", modelLineEdit->text());
    record.setValue("registration_id", registrationIdLineEdit->text());
    record.setValue("description", descriptionLineEdit->text());
    record.setValue("employee_id", employeeIdComboBox->currentData());
    record.setValue("time_slots", dateEdit->text());
    record.setValue("state", stateComboBox->currentText());

    if (model->setRecord(row, record)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Ticket Updated"), tr("Ticket successfully updated."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to update ticket: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to set record for editing."));
    }
}

void TicketWindow::deleteTicket()
{
    QModelIndexList selection = tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a ticket to delete."));
        return;
    }

    if (QMessageBox::question(this, tr("Confirm Delete"), tr("Are you sure you want to delete this ticket?"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    QModelIndex index = selection.at(0);
    int row = index.row();

    if (model->removeRow(row)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Ticket Deleted"), tr("Ticket successfully deleted."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to delete ticket: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to remove row from model."));
    }
}



void TicketWindow::refreshModel()
{
    model->select();
    tableView->reset();
}
