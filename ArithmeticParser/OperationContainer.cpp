
#include "ArithmeticParser/OperationContainer.h"

OperationContainer::OperationContainer() {

}

OperationContainer::OperationContainer(const OperationContainer& operations) {
    for (const auto & it: operations)
        oper.insert(it->notation(), it);
}

OperationContainer::OperationContainer(std::initializer_list<IOperation*> & list) {
    for (const auto & it: list)
        oper.insert(it->notation(), it);
}

auto OperationContainer::insert(IOperation* op) -> iterator {
    return oper.insert(op->notation(), op);
}

auto OperationContainer::find(const QString& str) -> iterator {
    return oper.find(str);
}

QList<IOperation*> OperationContainer::toSortedList() const {
    QList<IOperation*> list;
    for (auto & x: oper)
        list.push_back(x);
    std::sort(list.begin(), list.end(), [](IOperation* op1, IOperation* op2) -> bool {
            return op1->notation().size() < op2->notation().size();
        });
    return list;
}

int OperationContainer::remove(const QString& str) {
    return oper.remove(str);
}

bool OperationContainer::contains(const QString& str) const {
    return oper.contains(str);
}

int OperationContainer::size() const {
    return oper.size();
}

bool OperationContainer::isEmpty() const {
    return oper.isEmpty();
}
