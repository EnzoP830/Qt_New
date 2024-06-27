#ifndef INITDB_H
#define INITDB_H

#include <QtSql>
#include<QDebug>

void addEmployee(QSqlQuery &q, const QString &name, const QDate &date, bool isAvailable, double hourlyRate)
{
    q.addBindValue(name);
    q.addBindValue(date.toString(Qt::ISODate));
    q.addBindValue(isAvailable);
    q.addBindValue(hourlyRate);
    q.exec();
}


void addTicket(QSqlQuery &q, const QString &brand, const QString &model, const QString &registrationId,
               const QString &description, const QVariant &employeeId, const QDate &timeSlots, const QString &state)
{
    q.addBindValue(brand);
    q.addBindValue(model);
    q.addBindValue(registrationId);
    q.addBindValue(description);
    q.addBindValue(employeeId);
    q.addBindValue(timeSlots.toString(Qt::ISODate));
    q.addBindValue(state);
    q.exec();
}

void addPart(QSqlQuery &q, const QString &description, double amount, double unitPrice, double totalPrice, const QVariant &ticketId)
{
    q.addBindValue(description);
    q.addBindValue(amount);
    q.addBindValue(unitPrice);
    q.addBindValue(totalPrice);
    q.addBindValue(ticketId);
    q.exec();
}

const auto EMPLOYEES_SQL = QLatin1String(R"(
    create table employees(id integer primary key, name varchar, date date, is_available boolean, hourly_rate real)
    )");

const auto TICKETS_SQL = QLatin1String(R"(
    create table tickets(id integer primary key, brand varchar, model varchar, registration_id varchar,
                         description text, employee_id integer, time_slots date, state varchar,
                         total_price real default 0, foreign key(employee_id) references employees(id))
    )");

const auto PARTS_SQL = QLatin1String(R"(
    create table parts(id integer primary key, description varchar, amount real, unit_price real,
                       total_price real, ticket_id integer, foreign key(ticket_id) references tickets(id))
    )");

const auto INSERT_EMPLOYEE_SQL = QLatin1String(R"(
    insert into employees(name, date, is_available, hourly_rate) values(?, ?, ?, ?)
    )");

const auto INSERT_TICKET_SQL = QLatin1String(R"(
    insert into tickets(brand, model, registration_id, description, employee_id, time_slots, state)
                        values(?, ?, ?, ?, ?, ?, ?)
    )");

const auto INSERT_PART_SQL = QLatin1String(R"(
    insert into parts(description, amount, unit_price, total_price, ticket_id) values(?, ?, ?, ?, ?)
    )");


QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("employees", Qt::CaseInsensitive)
        && tables.contains("tickets", Qt::CaseInsensitive)
        && tables.contains("parts", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(EMPLOYEES_SQL))
        return q.lastError();
    if (!q.exec(TICKETS_SQL))
        return q.lastError();
    if (!q.exec(PARTS_SQL))
        return q.lastError();

    if (!q.prepare(INSERT_EMPLOYEE_SQL))
        return q.lastError();
    addEmployee(q, QLatin1String("John Doe"), QDate::currentDate(), true, 50.0);
    addEmployee(q, QLatin1String("Jane Smith"), QDate::currentDate(), false, 60.0);

    if (!q.prepare(INSERT_TICKET_SQL))
        return q.lastError();
    addTicket(q, QLatin1String("Toyota"), QLatin1String("Camry"), QLatin1String("ABC1234"),
              QLatin1String("Brake issue"), 1, QDate::currentDate(), QLatin1String("created"));

    if (!q.prepare(INSERT_PART_SQL))
        return q.lastError();
    addPart(q, QLatin1String("Oil"), 3.5, 10.56, 36.96, 1);

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE tickets SET total_price = 36.96 WHERE id = 1");
    if (!updateQuery.exec())
        return updateQuery.lastError();
    ;

    return QSqlError();
}

#endif
