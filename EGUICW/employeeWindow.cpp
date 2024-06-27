#include "employeeWindow.h"
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRelationalTableModel>
#include <QDateEdit>
#include <QCheckBox>
#include <QLabel>
#include <QSqlRecord>

EmployeeWindow::EmployeeWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Employee Window");

    window = new QWidget(this);
    setCentralWidget(window);

    setMinimumSize(500, 400);

    createLayout();
    createModel();

    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    configureWidgets();
    createMappings();
    tableView->setCurrentIndex(model->index(0, 0));
    tableView->selectRow(0);
}

void EmployeeWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
}

void EmployeeWindow::createLayout()
{
    tableView = new QTableView(window);
    gridLayout = new QGridLayout(window);

    QLabel *nameLabel = new QLabel(tr("Name:"), window);
    nameLineEdit = new QLineEdit(window);

    QLabel *dateLabel = new QLabel(tr("Date:"), window);
    dateEdit = new QDateEdit(window);
    dateEdit->setDisplayFormat("dd/MM/yyyy");

    QLabel *isAvailableLabel = new QLabel(tr("Is Available:"), window);
    isAvailableCheckBox = new QCheckBox(window);

    QLabel *hourlyRateLabel = new QLabel(tr("Hourly Rate:"), window);
    hourlyRateSpinBox = new QSpinBox(window);

    editEmployeeButton = new QPushButton(tr("Edit Employee"), window);
    deleteEmployeeButton = new QPushButton(tr("Delete Employee"), window);

    gridLayout->addWidget(nameLabel, 0, 0, Qt::AlignRight);
    gridLayout->addWidget(nameLineEdit, 0, 1, 1, 3);
    gridLayout->addWidget(dateLabel, 1, 0, Qt::AlignRight);
    gridLayout->addWidget(dateEdit, 1, 1, 1, 3);
    gridLayout->addWidget(isAvailableLabel, 2, 0, Qt::AlignRight);
    gridLayout->addWidget(isAvailableCheckBox, 2, 1, 1, 3);
    gridLayout->addWidget(hourlyRateLabel, 3, 0, Qt::AlignRight);
    gridLayout->addWidget(hourlyRateSpinBox, 3, 1, 1, 3);
    gridLayout->addWidget(editEmployeeButton, 4, 0, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(deleteEmployeeButton, 4, 2, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(tableView, 5, 0, 1, 4, Qt::AlignCenter);

    gridLayout->setColumnStretch(1, 1000);
    gridLayout->setColumnStretch(3, 1000);
    gridLayout->setContentsMargins(18, 18, 18, 18);
    gridLayout->setSpacing(18);
    gridLayout->setAlignment(Qt::AlignHCenter);
}


void EmployeeWindow::createModel()
{
    model = new QSqlRelationalTableModel(tableView);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("employees");

    model->setHeaderData(model->fieldIndex("name"), Qt::Horizontal, tr("Name"));
    model->setHeaderData(model->fieldIndex("date"), Qt::Horizontal, tr("Date"));
    model->setHeaderData(model->fieldIndex("is_available"), Qt::Horizontal, tr("Is Available"));
    model->setHeaderData(model->fieldIndex("hourly_rate"), Qt::Horizontal, tr("Hourly Rate"));
}

void EmployeeWindow::configureWidgets()
{
    tableView->setModel(model);
    tableView->setColumnHidden(model->fieldIndex("id"), true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void EmployeeWindow::createMappings()
{
    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(nameLineEdit, model->fieldIndex("name"));
    mapper->addMapping(dateEdit, model->fieldIndex("date"));
    mapper->addMapping(isAvailableCheckBox, model->fieldIndex("is_available"));
    mapper->addMapping(hourlyRateSpinBox, model->fieldIndex("hourly_rate"));
    //connect(tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper, &QDataWidgetMapper::setCurrentModelIndex);
    connect(editEmployeeButton, &QPushButton::clicked, this, &EmployeeWindow::editEmployee);
    connect(deleteEmployeeButton, &QPushButton::clicked, this, &EmployeeWindow::deleteEmployee);
}


void EmployeeWindow::editEmployee()
{
    QModelIndexList selection = tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select an employee to edit."));
        return;
    }

    QModelIndex index = selection.at(0);
    int row = index.row();

    QSqlRecord record = model->record(row);
    record.setValue("name", nameLineEdit->text());
    record.setValue("date", dateEdit->date());
    record.setValue("is_available", isAvailableCheckBox->isChecked());
    record.setValue("hourly_rate", hourlyRateSpinBox->value());

    if (model->setRecord(row, record)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Employee Updated"), tr("Employee successfully updated."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to update employee: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to set record for editing."));
    }
}

void EmployeeWindow::deleteEmployee()
{
    QModelIndexList selection = tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select an employee to delete."));
        return;
    }

    if (QMessageBox::question(this, tr("Confirm Delete"), tr("Are you sure you want to delete this employee?"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    QModelIndex index = selection.at(0);
    int row = index.row();

    if (model->removeRow(row)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Employee Deleted"), tr("Employee successfully deleted."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to delete employee: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to remove row from model."));
    }
}

