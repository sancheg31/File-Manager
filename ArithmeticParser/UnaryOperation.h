#pragma once

#include "ArithmeticParser/Interfaces/IOperation.h"

class UnaryOperation: public IOperation {
public:
    UnaryOperation(std::function<QVariant(QVariant)>, const QString&);
    UnaryOperation(const UnaryOperation&);
    UnaryOperation& operator=(const UnaryOperation&);

    virtual ~UnaryOperation() override;
    virtual OperationType type() const override { return OperationType::UnaryOperation; }
    virtual QVariant operator()(QVariant v) const { return op(v); }
    virtual QString notation() const override { return opNotation; }

private:
    std::function<QVariant(QVariant)> op;
    QString opNotation;
};
