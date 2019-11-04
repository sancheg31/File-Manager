#pragma once

#include <QSet>
#include <QHash>
#include <initializer_list>
#include <type_traits>

#include "ArithmeticParser/Interfaces/IOperation.h"

class OperationContainer {
public:

    using iterator = QHash<QString, IOperation*>::iterator;
    using const_iterator = QHash<QString, IOperation*>::const_iterator;

    OperationContainer();
    OperationContainer(const OperationContainer&);
    OperationContainer(std::initializer_list<IOperation*>& list);

    iterator insert(IOperation*);
    int remove(const QString&);
    iterator find(const QString&);

    QList<IOperation*> toSortedList() const;
    bool contains(const QString&) const;
    int size() const;
    bool isEmpty() const;

    iterator begin() { return oper.begin(); }
    const_iterator begin() const { return oper.begin(); }
    iterator end() { return oper.end(); }
    const_iterator end() const { return oper.end(); }

    const_iterator cbegin() const { return oper.cbegin(); }
    const_iterator cend() const { return oper.cend(); }

private:
    QHash<QString, IOperation*> oper;
};
