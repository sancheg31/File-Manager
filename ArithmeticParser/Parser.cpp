#include "ArithmeticParser/Parser.h"


Parser::Parser() { }
#include <QDebug>

Parser::Parser(const OperationTable& t): table(t) { }

Parser::~Parser() { }

QVariant Parser::parse(const QString & str) const {
    QString temp = str;
    temp = temp.mid(1);
    temp.push_front("((");
    temp.push_back("))");
    temp.append(QChar::Null);
    temp = removeSpaces(temp);
    int pos = 0;
    return evalExpression(temp, pos);
}

QVariant Parser::getFactor(const QString &str, int &pos) const {
    QVariant result = 0.0;
    QRegExp regExpColumn(QString("[A-Za-z]{1,") + QString("1") + QString("}"));
    QRegExp regExpRow(QString("[1-9][0-9]{0,") + QString("}"));
    QString column;
    while (pos < str.size() && str[pos].isLetter()) {
        column += str[pos];
        ++pos;
    }
    QString row;
    while (pos < str.size() && (str[pos].isNumber() || str[pos] == '.')) {
        row += str[pos];
        ++pos;
    }
    bool ok;
    result = (column+row).toDouble(&ok);
    if (ok)
        qDebug() << "Parser::getFactor(): factor found " << result;
    if (!ok) {
        if (regExpColumn.exactMatch(column) && regExpRow.exactMatch(row)) {
                //Get Cell Value From TableWidget!!!!!!!!!!!!!!!!!!
        } else {
            result = QVariant{};
        }
    }
    return result;
}

QString Parser::removeSpaces(const QString& str) const {
    QString temp = str;
    temp.replace(" ", "");
    return temp;
}

QVariant Parser::evalExpression(const QString& str, int & pos) const {
    QVariant result = evalTerm(str, pos, table.columnCount()-2);
    while (str[pos] != QChar::Null) {
        BinaryOperation* curOperation = dynamic_cast<BinaryOperation*>(table.currentOperation(str, pos, table.columnCount()-1));
        if (curOperation == nullptr)
            return result;
        //qDebug() << "Parser::evalExpression(): first term is: " << result.toString();
        //qDebug() << "Parser::evalExpression(): current operation is: " << curOperation->notation();
        pos += curOperation->notation().size();
        QVariant term = evalTerm(str, pos, table.columnCount()-2);
        //qDebug() << "Parser::evalExpression(): second term is: " << term.toString();
        result = curOperation->operator()(result, term);
    }
    return result;
}

QVariant Parser::evalTerm(const QString& str, int & pos, int priority) const {
    QVariant result = (priority) ? evalTerm(str, pos, priority-1) : evalFactor(str, pos);
    while (str[pos] != QChar::Null) {
        BinaryOperation* curOperation = dynamic_cast<BinaryOperation*>(
                    table.currentOperation(str, pos, priority));
        if (curOperation == nullptr)
            return result;
        pos += curOperation->notation().size();
        //qDebug() << "Parser::evalTerm(): first term is: " << result.toString();
        //qDebug() << "Parser::evalTerm(): current operation is: " << curOperation->notation() << "on pr " << priority;
        QVariant term = (priority) ? evalTerm(str, pos, priority-1) : evalFactor(str, pos);
        //qDebug() << "Parser::evalTerm(): second term is: " << term.toString();
        result = curOperation->operator()(result, term);
    }
    return result;
}

QVariant Parser::evalFactor(const QString& str, int& pos) const {
    QVariant result;
    UnaryOperation* curOperation = nullptr;
    if (str[pos] == '(') {
        //qDebug() << "Parser::evalFactor(): open bracket found on " << pos << " position";
        ++pos;
        result = evalExpression(str, pos);
        if (str[pos] != ')')
            result = QVariant{};
        else {
            //qDebug() << "Parser::evalFactor(): close bracket found on " << pos << " position";
            ++pos;
        }
    } else {

        curOperation = dynamic_cast<UnaryOperation*>(table.currentOperation(str, pos, 0));
        if (curOperation) {
            pos += curOperation->notation().size();
            //qDebug() << "Parser::evalFactor(): current unary operation is: " << curOperation->notation();
        }
        if (str[pos] == '(') {
            //qDebug() << "Parser::evalFactor(): open bracket found on 2" << pos << " position";
            ++pos;
            result = evalExpression(str, pos);
            if (str[pos] != ')')
                result = QVariant{};
            ++pos;
        } else {
            result = getFactor(str, pos);
        }
        if (curOperation) {
            result = curOperation->operator()(result);
        }
    }
    return result;
}
