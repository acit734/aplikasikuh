#pragma once 
#include "SqlCondition.h"

inline Condition expr(const QString &field, const QString &op, const QVariant &value) {
    return {Condition::Type::Expr, field, op, value};
}

inline Condition And(const std::initializer_list<Condition> list) {
    return {Condition::Type::And, "", "", {}, {}, QList<Condition>(list)};
}

inline Condition Or(const std::initializer_list<Condition> list) {
    return {Condition::Type::Or, "", "", {}, {}, QList<Condition>(list)};
}

inline Condition Not(const Condition &c) {
    return {Condition::Type::Not, "", "", {}, {}, {c}};
}