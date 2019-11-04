#pragma once

#include <QVector>

#include "ArithmeticParser/OperationContainer.h"
#include "ArithmeticParser/UnaryOperation.h"
#include "ArithmeticParser/BinaryOperation.h"

class OperationTable {
public:
    OperationTable();

    void add(IOperation*, int);
    int remove(const QString&);

    bool contains(const QString&) const;
    bool isEmpty() const;
    int columnCount() const;
    int column(const QString&) const;

    QPair<IOperation*, int> nearestOperation(const QString&, int pos, int priority) const;
    IOperation* currentOperation(const QString&, int pos, int priority) const;

    OperationContainer& operator[](int i) { return operations[i]; }
    const OperationContainer& operator[](int i) const { return operations[i]; }
private:
    QVector<OperationContainer> operations;
};
