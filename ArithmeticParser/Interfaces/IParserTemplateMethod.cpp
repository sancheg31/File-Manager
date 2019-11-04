#include "ArithmeticParser/Interfaces/IParserTemplateMethod.h"

IParserTemplateMethod::~IParserTemplateMethod() {}

QVariant IParserTemplateMethod::parse(const QString& str) const {
    QString temp = str;
    temp = temp.mid(1);
    temp.push_front("((");
    temp.push_back("))");
    temp.append(QChar::Null);
    temp = removeSpaces(temp);
    int pos = 0;
    QVariant result = evalExpression(temp, pos);
    return result;
}

