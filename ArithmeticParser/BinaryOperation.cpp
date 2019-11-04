#include "ArithmeticParser/BinaryOperation.h"


BinaryOperation::BinaryOperation(std::function<QVariant (QVariant, QVariant)> operation, const QString & str):
    op(operation), opNotation(str)
{

}

BinaryOperation::BinaryOperation(const BinaryOperation & ob): op(ob.op), opNotation(ob.opNotation) {

}

BinaryOperation &BinaryOperation::operator=(const BinaryOperation & ob) {
    op = ob.op;
    opNotation = ob.opNotation;
    return *this;
}

BinaryOperation::~BinaryOperation() { }
