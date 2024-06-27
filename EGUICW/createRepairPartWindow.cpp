#include "createRepairPartWindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlRelationalDelegate>
#include <QGridLayout>
#include <QDebug>

CreateRepairPartWindow::CreateRepairPartWindow(RepairPartWindow *parentWindow, QWidget *parent)
    : QMainWindow(parent), parentWindow(parentWindow), gridLayout(nullptr)
{
    setWindowTitle("Create Repair Part");

    window = new QWidget(this);
    setCentralWidget(window);

    createLayout();
    createModel();
    configureWidgets();
    resetFields();
}

void CreateRepairPartWindow::createLayout()
{
    if (gridLayout) {
        QLayoutItem* item;
        while ((item = gridLayout->takeAt(0)) != nullptr)
        {
            delete item->widget();
        }
        delete gridLayout;
        gridLayout = nullptr;
    }

    gridLayout = new QGridLayout(window);

    QLabel *descriptionLabel = new QLabel(tr("Description:"), window);
    descriptionLineEdit = new QLineEdit(window);
    QLabel *amountLabel = new QLabel(tr("Amount:"), window);
    amountLineEdit = new QLineEdit(window);
    QLabel *unitPriceLabel = new QLabel(tr("Unit Price:"), window);
    unitPriceLineEdit = new QLineEdit(window);
    QLabel *totalPriceLabel = new QLabel(tr("Total Price:"), window);
    totalPriceLineEdit = new QLineEdit(window);
    QLabel *ticketIdLabel = new QLabel(tr("Ticket ID:"), window);
    ticketIdComboBox = new QComboBox(window);

    createButton = new QPushButton(tr("Create Repair Part"), window);

    gridLayout->addWidget(descriptionLabel, 0, 0, Qt::AlignRight);
    gridLayout->addWidget(descriptionLineEdit, 0, 1, 1, 3);
    gridLayout->addWidget(amountLabel, 1, 0, Qt::AlignRight);
    gridLayout->addWidget(amountLineEdit, 1, 1, 1, 3);
    gridLayout->addWidget(unitPriceLabel, 2, 0, Qt::AlignRight);
    gridLayout->addWidget(unitPriceLineEdit, 2, 1, 1, 3);
    gridLayout->addWidget(totalPriceLabel, 3, 0, Qt::AlignRight);
    gridLayout->addWidget(totalPriceLineEdit, 3, 1, 1, 3);
    gridLayout->addWidget(ticketIdLabel, 4, 0, Qt::AlignRight);
    gridLayout->addWidget(ticketIdComboBox, 4, 1, 1, 3);
    gridLayout->addWidget(createButton, 5, 0, 1, 4, Qt::AlignCenter);

    gridLayout->setColumnStretch(1, 1000);
    gridLayout->setColumnStretch(3, 1000);
    gridLayout->setContentsMargins(18, 18, 18, 18);
    gridLayout->setSpacing(18);
    gridLayout->setAlignment(Qt::AlignHCenter);

    connect(createButton, &QPushButton::clicked, this, &CreateRepairPartWindow::createRepairPart);
}

void CreateRepairPartWindow::createModel()
{
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    model->setTable("Part");

    model->setHeaderData(model->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    model->setHeaderData(model->fieldIndex("amount"), Qt::Horizontal, tr("Amount"));
    model->setHeaderData(model->fieldIndex("unit_price"), Qt::Horizontal, tr("Unit Price"));
    model->setHeaderData(model->fieldIndex("total_price"), Qt::Horizontal, tr("Total Price"));
    model->setHeaderData(model->fieldIndex("ticket_id"), Qt::Horizontal, tr("Ticket ID"));
}

void CreateRepairPartWindow::configureWidgets()
{
    setupTicketComboBox();
}

void CreateRepairPartWindow::createRepairPart()
{
    int ticketIndex = ticketIdComboBox->currentIndex();
    if (ticketIndex == -1) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a ticket."));
        return;
    }

    QSqlTableModel *ticketModel = qobject_cast<QSqlTableModel*>(ticketIdComboBox->model());
    int ticketId = ticketModel->record(ticketIndex).value("id").toInt();

    QSqlRecord record = model->record();
    record.setValue("description", descriptionLineEdit->text());
    record.setValue("amount", amountLineEdit->text().toDouble());
    record.setValue("unit_price", unitPriceLineEdit->text().toDouble());
    record.setValue("total_price", totalPriceLineEdit->text().toDouble());
    record.setValue("ticket_id", ticketId);

    if (model->insertRecord(-1, record)) {
        if (model->submitAll()) {
            QMessageBox::information(this, tr("Repair Part Created"), tr("Repair part successfully created."));
            parentWindow->refreshModel();
            resetFields();
            close();
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to create repair part: %1").arg(model->lastError().text()));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to insert record into model."));
    }
}

void CreateRepairPartWindow::setupTicketComboBox()
{
    QSqlTableModel *ticketModel = new QSqlTableModel(this);
    ticketModel->setTable("tickets");
    ticketModel->select();

    ticketIdComboBox->setModel(ticketModel);
    ticketIdComboBox->setModelColumn(ticketModel->fieldIndex("registration_id"));
    ticketIdComboBox->setCurrentIndex(-1);
}

void CreateRepairPartWindow::resetFields()
{
    descriptionLineEdit->clear();
    amountLineEdit->clear();
    unitPriceLineEdit->clear();
    totalPriceLineEdit->clear();
    ticketIdComboBox->setCurrentIndex(-1);
}
