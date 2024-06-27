#include "createEmployeeWindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlError>
#include <QGridLayout>

CreateEmployeeWindow::CreateEmployeeWindow(QWidget *parent)
    : QMainWindow(parent), gridLayout(nullptr)
{
    setWindowTitle("Create Employee");

    window = new QWidget(this);
    setCentralWidget(window);

    setMinimumSize(500, 400);

    createLayout();
    createModel();
    configureWidgets();
    resetFields();
}

void CreateEmployeeWindow::createLayout()
{
    if (gridLayout) {
        QLayoutItem* item;
        while ((item = gridLayout->takeAt(0)) != nullptr)
        {
            delete item->widget();
        }
        delete gridLayout;
        gridLayout=nullptr;
    }

    // Utilisez window correctement défini comme parent pour le layout
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
    hourlyRateSpinBox->setMaximum(1000);

    createButton = new QPushButton(tr("Create Employee"), window);

    gridLayout->addWidget(nameLabel, 0, 0, Qt::AlignRight);
    gridLayout->addWidget(nameLineEdit, 0, 1, 1, 3);
    gridLayout->addWidget(dateLabel, 1, 0, Qt::AlignRight);
    gridLayout->addWidget(dateEdit, 1, 1, 1, 3);
    gridLayout->addWidget(isAvailableLabel, 2, 0, Qt::AlignRight);
    gridLayout->addWidget(isAvailableCheckBox, 2, 1, 1, 3);
    gridLayout->addWidget(hourlyRateLabel, 3, 0, Qt::AlignRight);
    gridLayout->addWidget(hourlyRateSpinBox, 3, 1, 1, 3);
    gridLayout->addWidget(createButton, 4, 0, 1, 4, Qt::AlignCenter);

    gridLayout->setColumnStretch(1, 1000);
    gridLayout->setColumnStretch(3, 1000);
    gridLayout->setContentsMargins(18, 18, 18, 18);
    gridLayout->setSpacing(18);
    gridLayout->setAlignment(Qt::AlignHCenter);

    connect(createButton, &QPushButton::clicked, this, &CreateEmployeeWindow::createEmployee);
}

void CreateEmployeeWindow::createModel()
{
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    model->setTable("employees");

    // Set the localized header captions
    model->setHeaderData(model->fieldIndex("name"), Qt::Horizontal, tr("Name"));
    model->setHeaderData(model->fieldIndex("date"), Qt::Horizontal, tr("Date"));
    model->setHeaderData(model->fieldIndex("is_available"), Qt::Horizontal, tr("Is Available"));
    model->setHeaderData(model->fieldIndex("hourly_rate"), Qt::Horizontal, tr("Hourly Rate"));
}

void CreateEmployeeWindow::configureWidgets()
{
}

void CreateEmployeeWindow::createEmployee()
{
    QSqlRecord record = model->record();
    record.setValue("name", nameLineEdit->text());
    record.setValue("date", dateEdit->date().toString(Qt::ISODate));
    record.setValue("is_available", isAvailableCheckBox->isChecked());
    record.setValue("hourly_rate", hourlyRateSpinBox->value());

    if (model->insertRecord(-1, record)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Employee Created"), tr("Employee successfully created."));
            close();
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to create employee: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to insert record into model."));
    }
}

void CreateEmployeeWindow::resetFields()
{
    nameLineEdit->clear();
    dateEdit->setDate(QDate::currentDate());
    isAvailableCheckBox->setChecked(false);
    hourlyRateSpinBox->setValue(0);
}
