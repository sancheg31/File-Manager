#include <QDebug>

#include "ArithmeticParser/OperationTable.h"

OperationTable::OperationTable() {
    operations.push_back(OperationContainer{});
}

void OperationTable::add(IOperation* op, int priority) {

    if (priority == 0 && op->type() == IOperation::OperationType::UnaryOperation) {
        operations[0].insert(dynamic_cast<UnaryOperation*>(op));
    }
    else if (op->type() == IOperation::OperationType::BinaryOperation) {
        if (priority > 0 && priority < operations.size()) {
            operations[priority].insert(dynamic_cast<BinaryOperation*>(op));
        }
        else if (priority >= operations.size()) {
            while (operations.size() <= priority)
            operations.push_back(OperationContainer{});
            operations[priority].insert(dynamic_cast<BinaryOperation*>(op));
        }
    }
}

int OperationTable::remove(const QString& str) {
    for (auto & x : operations) {
        int c = x.remove(str);
        if (c)
            return c;
    }
    return 0;
}

IOperation* OperationTable::currentOperation(const QString & str, int pos, int priority) const {
    auto list = operations[priority].toSortedList();
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        int curIndex = str.indexOf((*it)->notation(), pos);
        if (curIndex == pos)
            return (*it);
    }
    return nullptr;
}

QPair<IOperation*, int> OperationTable::nearestOperation(const QString & str, int pos, int priority) const {
    auto list = operations[priority].toSortedList();
    int minIndex = str.size();
    IOperation* result = nullptr;
    for (int i = list.size()-1; i > -1; --i) {
        int curIndex = str.indexOf(list[i]->notation(), pos);
        if (curIndex < minIndex && curIndex != -1) {
            minIndex = curIndex;
            result = list[i];
        }
    }
    return QPair{result, minIndex};
}

bool OperationTable::contains(const QString& str) const {
    for (auto & x: operations)
        if (x.contains(str))
            return true;
    return false;
}

bool OperationTable::isEmpty() const {
    return operations.isEmpty();
}
int OperationTable::columnCount() const {
    return operations.size();
}

int OperationTable::column(const QString& str) const {
    for (int i = 0; i < operations.size(); ++i)
        if (operations[i].contains(str))
            return i;
    return -1;
}





