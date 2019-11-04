#include "ArithmeticParser/Parser.h"


Parser::Parser() { }
#include <QDebug>

Parser::Parser(const OperationTable& t, const QSet<QString>& cell): table(t), cellNames(cell) { }

Parser::~Parser() { }

QVariant Parser::parse(const QString & str) const {
    QString temp = str;
    temp = temp.mid(1);
    qDebug() << "Parser::parse: removed \'=\' " << temp;
    temp.push_front("((");
    temp.push_back("))");
    qDebug() << "Parser::parse: added brackets " << temp;
    temp.append(QChar::Null);
    temp = removeSpaces(temp);
    qDebug() << "Parser::parse: removed spaces " << temp;
    int pos = 0;
    qDebug() << "\n";
    QVariant result = evalExpression(temp, pos);
    return result;
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
        qDebug() << "Parser::evalExpression(): first term is: " << result.toString();
        qDebug() << "Parser::evalExpression(): current operation is: " << curOperation->notation();
        pos += curOperation->notation().size();
        QVariant term = evalTerm(str, pos, table.columnCount()-2);
        qDebug() << "Parser::evalExpression(): second term is: " << term.toString();
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
        qDebug() << "Parser::evalTerm(): first term is: " << result.toString();
        qDebug() << "Parser::evalTerm(): current operation is: " << curOperation->notation() << "on pr " << priority;
        QVariant term = (priority) ? evalTerm(str, pos, priority-1) : evalFactor(str, pos);
        qDebug() << "Parser::evalTerm(): second term is: " << term.toString();
        result = curOperation->operator()(result, term);
    }
    return result;
}

QVariant Parser::evalFactor(const QString& str, int& pos) const {
    QVariant result;
    UnaryOperation* curOperation = nullptr;
    if (str[pos] == '(') {
        qDebug() << "Parser::evalFactor(): open bracket found on " << pos << " position";
        ++pos;
        result = evalExpression(str, pos);
        if (str[pos] != ')')
            result = QVariant{};
        else {
            qDebug() << "Parser::evalFactor(): close bracket found on " << pos << " position";
            ++pos;
        }
    } else {

        curOperation = dynamic_cast<UnaryOperation*>(table.currentOperation(str, pos, 0));
        if (curOperation) {
            pos += curOperation->notation().size();
            qDebug() << "Parser::evalFactor(): current unary operation is: " << curOperation->notation();
        }
        if (str[pos] == '(') {
            qDebug() << "Parser::evalFactor(): open bracket found on 2" << pos << " position";
            ++pos;
            result = evalExpression(str, pos);
            if (str[pos] != ')')
                result = QVariant{};
            else
                qDebug() << "Parser::evalFactor(): close bracket found on 2" << pos << " position";
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
/*
QVariant evalExpression(const QString &str, int &pos) {

    QVariant result = evalTerm(str, pos);
    while (str[pos] != QChar::Null) {
        QChar op = str[pos];
        if (op != '+' && op != '-')
            return result;
        ++pos;

        QVariant term = evalTerm(str, pos);
        if (result.type() == QVariant::Double
                && term.type() == QVariant::Double) {
            if (op == '+') {
                result = result.toDouble() + term.toDouble();
            } else {
                result = result.toDouble() - term.toDouble();
            }
        } else {
            result = QVariant{};
        }
    }
    return result;
}

QVariant evalTerm(const QString &str, int & pos, int priority) {
    QVariant result = (priority) ? evalTerm(str, pos, priority-1) : evalFactor(str, pos);
    while (str[pos] != QChar::Null) {
        QChar op = str[pos];
        if (op != '*' && op != '/')
            return result;
        ++pos;

        QVariant factor = (priority) ? evalTerm(str, pos, priority-1) : evalFactor(str, pos);
        if (result.type() == QVariant::Double
                && factor.type() == QVariant::Double) {
            if (op == '*') {
                result = result.toDouble() * factor.toDouble();
            } else {
                if (factor.toDouble() == 0.0) {
                    result = QVariant{};
                } else {
                    result = result.toDouble() / factor.toDouble();
                }
            }
        } else {
            result = QVariant{};
        }
    }
    return result;
}

QVariant evalFactor(const QString &str, int & pos) {
    QVariant result;
    bool negative = false;

    if (str[pos] == '-') {
        negative = true;
        ++pos;
    }

    if (str[pos] == '(') {
        ++pos;
        result = evalExpression(str, pos);
        if (str[pos] != ')')
            result = QVariant{};
        ++pos;
    } else {
        QRegExp regExp("[A-Za-z][1-9][0-9]{0,2}");
        QString token;

        while (str[pos].isLetterOrNumber() || str[pos] == '.') {
            token += str[pos];
            ++pos;
        }

        if (regExp.exactMatch(token)) {
            int column = token[0].toUpper().unicode() - 'A';
            int row = token.mid(1).toInt() - 1;

            //Cell *c = static_cast<Cell *>(tableWidget()->item(row, column));
            if (c) {
                result = c->value();
            } else {
                result = 0.0;
            }
        } else {
            bool ok;
            result = token.toDouble(&ok);
            if (!ok)
                result = QVariant{};
        }
    }

    if (negative) {
        if (result.type() == QVariant::Double) {
            result = -result.toDouble();
        } else {
            result = QVariant::Invalid;
        }
    }
    return result;
}
*/
