#include "SqlDatabase.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QsqlRecord>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QtSql/QSqlDatabase>

SqlDatabase::SqlDatabase(QObject *parent) : QObject(parent) {

}

bool SqlDatabase::open(const QString &path) {
    const QString connectionName = "app";

    if (QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::database(connectionName);
    } else {
        db = QSqlDatabase::addDatabase(connectionName);
    }

    db.setDatabaseName(path);

    if (!db.open()) {
        qDebug() << "[SqlDatabase] Database open failed with error: " << db.lastError().text();
        return false;
    }

    return true;
}

void SqlDatabase::close () {
    if (!db.isValid()) return;

    const QString connectionName = db.connectionName();

    if (db.isOpen()) db.close();

    db.QSqlDatabase::database(QString(), false);

    QSqlDatabase::removeDatabase(connectionName);

    return;
}

bool SqlDatabase::isOpen() {
    return db.isOpen();
}

bool SqlDatabase::insert(const QString &table, const QVariantMap &data) {
    if (!db.isOpen() || data.isEmpty()) return false;

    QStringList fields;
    QStringList placeholders;

    for (auto it = data.begin(); it != data.end(); ++it) {
        fields << it.key();
        placeholders << "?";
    }

    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
    .arg(
        table,
        fields.join(", "),
        placeholders.join(", ")
    );

    QSqlQuery query(db);
    query.prepare(sql);

    for (auto it = data.begin(); it != data.end(); ++it) query.addBindValue(it.value());

    if (!query.exec()) {
        qWarning() << "[SqlDatabase] Insert Failed, " << query.lastError().text();
        return false;
    }

    return true;
}

QVariantList SqlDatabase::selectAll(const QString &table) {
    QVariantList result;

    if (!db.isOpen()) {
        qWarning() << "[SqlDatabase] Database is not open";
        return result;
    }

    QSqlQuery query(db);
    QString sql = QString("SELECT * FROM %1").arg(table);

    if (!query.exec(sql)) {
        qWarning() << "[SqlDatabase] Database read error, " << query.lastError().text();
        return result;
    }

    while (query.next()) {
        QVariantMap row;
        QSqlRecord record = query.record();

        for (int i = 0; i < record.count(); ++i) {
            row.insert(
                record.fieldName(i),
                query.value(i)
            );
        }
        result.append(row);
    }

    return result;
}

bool SqlDatabase::update(const QString &table, const QVariantMap &data, const QVariantMap &where) {
    if (!db.isOpen()) {
        qWarning() << "[SqlDatabase] Database is not open";
        return false;
    }

    if (data.isEmpty() || where.isEmpty()) {
        qWarning() << "[SqlDatabase] UPDATE requires data and where clause";
        return false;
    }

    QStringList setParts;
    QStringList whereParts;

    for (auto it = data.begin(); it != data.end(); ++it) {
        setParts << QString("%1 = ?").arg(it.key());
    }

    for (auto it = where.begin(); it != where.end(); ++it) {
        whereParts << QString("%1 = ?").arg(it.key());
    }

    QString sql = QString("UPDATE %1 SET %2 WHERE $3").arg(table, setParts.join(", "), whereParts.join(" AND "));

    QSqlQuery query(db);
    query.prepare(sql);

    for (auto it = data.begin(); it != data.end(); ++it) {
        query.addBindValue(it.value());
    }

    for (auto it = where.begin(); it != where.end(); ++it) {
        query.addBindValue(it.value());
    }

    if (!query.exec()) {
        qWarning() << "[SqlDatabase] UPDATE failed, ";
        return false;
    }

    return true;
}

bool SqlDatabase::remove(const QString &table, const QVariantMap &where) {
    if (!db.isOpen()) {
        qWarning() << "[SqlDatabase] Database is not open";
        return false;
    }

    if (where.isEmpty()) {
        qWarning() << "[SqlDatabase] DELETE without WHERE is not allowed";
        return false;
    }

    QStringList whereParts;

    for (auto it = where.begin(); it != where.end(); ++it) {
        whereParts << QString("%1 = ?").arg(it.key());
    }

    QString sql = QString("DELETE %1 WHERE %2").arg(table, whereParts.join(" AND "));

    QSqlQuery query(db);
    query.prepare(sql);

    for (auto it = where.begin(); it != where.end(); ++it) {
        query.addBindValue(it.value());
    }

    if (!query.exec()) {
        qWarning() << "[SqlDatabase] DELETE failed, ";
        return false;
    }

    return true;
}