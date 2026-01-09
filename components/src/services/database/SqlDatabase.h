#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include "SqlCondition.h"


class SqlDatabase : public QObject {
    Q_OBJECT
public:
    explicit SqlDatabase(QObject *parent = nullptr);

    bool open(const QString &path);
    void close();
    bool isOpen();
    bool insert(const QString &table, const QVariantMap &data);
    QVariantList selectAll(const QString &table);
    QVariantList selectWhere(const QString &table, const Condition &where);
    bool update(const QString &table, const QVariantMap &data, const QVariantMap &where);
    bool remove(const QString &table, const QVariantMap &where);
private:
    QSqlDatabase db;
};

#endif //SQLDATABASE_H