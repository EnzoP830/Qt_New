#include "createTicketWindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlRelationalDelegate>
#include <QGridLayout>
#include <QDebug>

CreateTicketWindow::CreateTicketWindow(TicketWindow *parentWindow, QWidget *parent)
    : QMainWindow(parent), parentWindow(parentWindow), gridLayout(nullptr)
{
    setWindowTitle("Create Ticket");

    window = new QWidget(this);
    setCentralWidget(window);

    setMinimumSize(500, 400);

    createLayout();
    createModel();
    configureWidgets();
    resetFields();
}

void CreateTicketWindow::createLayout()
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
    stateComboBox = new QComboBox(window);
    stateComboBox->addItems({"created", "in progress", "done", "closed"});

    createButton = new QPushButton(tr("Create Ticket"), window);

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
    gridLayout->addWidget(createButton, 7, 0, 1, 4, Qt::AlignCenter);

    gridLayout->setColumnStretch(1, 1000);
    gridLayout->setColumnStretch(3, 1000);
    gridLayout->setContentsMargins(18, 18, 18, 18);
    gridLayout->setSpacing(18);
    gridLayout->setAlignment(Qt::AlignHCenter);

    connect(createButton, &QPushButton::clicked, this, &CreateTicketWindow::createTicket);
}

void CreateTicketWindow::createModel()
{
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    model->setTable("tickets");

    // Set the relations to the other database tables if necessary
    model->setRelation(model->fieldIndex("employee_id"), QSqlRelation("employees", "id", "name"));

    // Set the localized header captions
    model->setHeaderData(model->fieldIndex("brand"), Qt::Horizontal, tr("Brand"));
    model->setHeaderData(model->fieldIndex("model"), Qt::Horizontal, tr("Model"));
    model->setHeaderData(model->fieldIndex("registration_id"), Qt::Horizontal, tr("Registration ID"));
    model->setHeaderData(model->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    model->setHeaderData(model->fieldIndex("employee_id"), Qt::Horizontal, tr("Employee"));
    model->setHeaderData(model->fieldIndex("date"), Qt::Horizontal, tr("Date"));
    model->setHeaderData(model->fieldIndex("state"), Qt::Horizontal, tr("State"));
}

void CreateTicketWindow::configureWidgets()
{
    setupEmployeeComboBox();
}

void CreateTicketWindow::createTicket()
{
    int employeeIndex = employeeIdComboBox->currentIndex();
    if (employeeIndex == -1) {
        QMessageBox::warning(this, tr("Error"), tr("Please select an employee."));
        return;
    }

    QSqlTableModel *employeeModel = qobject_cast<QSqlTableModel*>(employeeIdComboBox->model());
    int employeeId = employeeModel->record(employeeIndex).value("id").toInt();

    QDate selectedDate = dateEdit->date();

    qDebug() << "Date:" << selectedDate.toString(Qt::ISODate);

    QSqlRecord record = model->record();
    record.setValue("brand", brandLineEdit->text());
    record.setValue("model", modelLineEdit->text());
    record.setValue("registration_id", registrationIdLineEdit->text());
    record.setValue("description", descriptionLineEdit->text());
    record.setValue("employee_id", employeeId);
    record.setValue("time_slots", selectedDate.toString(Qt::ISODate));
    record.setValue("state", stateComboBox->currentText());

    if (model->insertRecord(-1, record)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Ticket Created"), tr("Ticket successfully created."));
            parentWindow->refreshModel();
            resetFields();
            close();
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to create ticket: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to insert record into model."));
    }
}


void CreateTicketWindow::setupEmployeeComboBox()
{
    employeeModel = new QSqlTableModel(this);
    employeeModel->setTable("employees");

    employeeModel->select();
    employeeIdComboBox->setModel(employeeModel);
    employeeIdComboBox->setModelColumn(employeeModel->fieldIndex("name"));
    employeeIdComboBox->setCurrentIndex(-1);

    connect(dateEdit, &QDateEdit::dateChanged, this, [this](const QDate &selectedDate){
        updateEmployeeComboBox(selectedDate);
    });

    updateEmployeeComboBox(dateEdit->date());
}

void CreateTicketWindow::updateEmployeeComboBox(const QDate &selectedDate)
{
    QString selectedDateString = selectedDate.toString(Qt::ISODate);

    QString query = QString("SELECT id, name FROM employees WHERE "
                            "id IN (SELECT employee_id FROM tickets WHERE date = '%1') "
                            "AND is_available = 1").arg(selectedDateString);


    // Exécuter la requête et mettre à jour le modèle des employés
    if (employeeModel) {
        employeeModel->setQuery(query);
        employeeIdComboBox->setCurrentIndex(-1); // Réinitialiser la sélection
    }
}

void CreateTicketWindow::resetFields()
{
    brandLineEdit->clear();
    modelLineEdit->clear();
    registrationIdLineEdit->clear();
    descriptionLineEdit->clear();
    employeeIdComboBox->setCurrentIndex(-1);
    dateEdit->setDate(QDate::currentDate());
    stateComboBox->setCurrentIndex(0);
}

