#include "ArithmeticParser/UnaryOperation.h"

UnaryOperation::UnaryOperation(std::function<QVariant(QVariant)> operation, const QString & str):
    op(operation), opNotation(str)
{

}

UnaryOperation::UnaryOperation(const UnaryOperation & ob): op(ob.op), opNotation(ob.opNotation) {

}

UnaryOperation &UnaryOperation::operator=(const UnaryOperation & ob) {
    op = ob.op;
    opNotation = ob.opNotation;
    return *this;
}

UnaryOperation::~UnaryOperation() {}
