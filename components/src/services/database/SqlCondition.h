#pragma once
#include <QString>
#include <QList>
#include <QVariant>

struct Condition {
    enum class Type {
        Expr,
        And,
        Or,
        Not
    };

    Type type;

    QString field;
    QString op;
    QVariant value;
    QVariant value2;

    QList<Condition> children;
};