#include "SqlDatabase.h"
#include "SqlCondition.h"
#include "SqlConditionBuilder.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QsqlRecord>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QtSql/QSqlDatabase>

SqlDatabase::SqlDatabase(QObject *parent) : QObject(parent) {

}

QString buildWhere(const Condition &c, QList<QVariant> &binds) {
    switch (c.type) {
        case Condition::Type::Expr:
            if (c.op == "BETWEEN") {
                binds << c.value << c.value2;
                return QString("%1 BETWEEN ? AND ?").arg(c.field);
            }
            binds << c.value;
            return QString("%1 %2 ?").arg(c.field, c.op);
        case Condition::Type::And: {
            QStringList parts;
            for (auto &child : c.children) 
                parts << buildWhere(child, binds);
            return "(" + parts.join(" AND ") + ")";
        }
        case Condition::Type::Or: {
            QStringList parts;
            for (auto &child : c.children)
                parts << buildWhere(child, binds);
            return "(" + parts.join(" OR ") + ")";
        }
        case Condition::Type::Not:
            return "(NOT " + buildWhere(c.children.first(), binds) + ")";
    }

    return {};
}

bool SqlDatabase::open(const QString &path) {
    const QString connectionName = "QSQLITE";

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

bool SqlDatabase::insert(const QString &table, const QVariantMap &data, const QString &caller) {
    if (!db.isOpen() || data.isEmpty()) {
        qWarning() << "[SqlDatabase] Data is empty or db is not open," << table << data;
        return false;
    }

    qDebug() << "[SqlDatabase] Inserting...";

    if (caller == "OpeningAnimation") {
        QSqlQuery setupTable(db);
        setupTable.exec(QString("CREATE TABLE IF NOT EXISTS \"%1\" ("
            "id INTEGER PRIMARY KEY,"
            "username TEXT NOT NULL,"
            "storageLink TEXT NOT NULL,"
            "loggedIn INTEGER NOT NULL CHECK (loggedIn IN (0, 1))"
            ");").arg(table));
    }

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

QVariantList SqlDatabase::selectWhere(const QString &table, const Condition &where) {
    QVariantList result;
    QList<QVariant> binds;

    if (!db.isOpen()) {
        qWarning() << "[SqlDatabase] Database is not open";
        return result;
    }

    QString whereSql = buildWhere(where, binds);
    QString sql = QString("SELECT * FROM %1 WHERE %2").arg(table, whereSql);

    QSqlQuery query(db);
    query.prepare(sql);

    for (auto &v : binds) {
        query.addBindValue(v);
    }   

    if (!query.exec()) {
        qWarning() << "[SqlDatabase] SELECT WHERE failed," << query.lastError().text();
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

    QString sql = QString("UPDATE %1 SET %2 WHERE %3").arg(table, setParts.join(", "), whereParts.join(" AND "));

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