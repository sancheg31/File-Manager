#pragma once

#include <QString>
#include <QVariant>

class IOperation {
public:

    enum class OperationType {
        UnaryOperation,
        BinaryOperation
    };

    virtual ~IOperation();
    virtual QString notation() const = 0;
    virtual OperationType type() const = 0;

};


