
#include <QtMath>

#include "TableParser.h"
#include "Cell.h"

TableParser::TableParser(Spreadsheet* s): sp(s) {

    OperationTable operations;
    operations.add(new UnaryOperation([](QVariant v) -> QVariant {
                  return (v.type() == QVariant::Invalid) ? v : v.toDouble();
              }, "+"), 0);
    operations.add(new UnaryOperation([](QVariant v) -> QVariant {
                  return (v.type() == QVariant::Invalid) ? v : -v.toDouble();
              }, "-"), 0);
    operations.add(new UnaryOperation([](QVariant v) -> QVariant {
                  return (v.type() == QVariant::Invalid) ? v : v.toDouble()+1;
              }, "++"), 0);
    operations.add(new UnaryOperation([](QVariant v) -> QVariant {
                  return (v.type() == QVariant::Invalid) ? v : v.toDouble()-1;
              }, "--"), 0);
    operations.add(new BinaryOperation([](QVariant v1, QVariant v2) -> QVariant {
                  return (v1.type() == QVariant::Invalid || v2.type() == QVariant::Invalid) ?
                  QVariant{} : qPow(v1.toDouble(), v2.toDouble());
              }, "^"), 1);
    operations.add(new BinaryOperation([](QVariant v1, QVariant v2) -> QVariant {
                  return (v1.type() == QVariant::Invalid || v2.type() == QVariant::Invalid) ?
                  QVariant{} : v1.toDouble() * v2.toDouble();
              }, "*"), 2);
    operations.add(new BinaryOperation([](QVariant v1, QVariant v2) -> QVariant {
                  return (v1.type() == QVariant::Invalid || v2.type() == QVariant::Invalid) ?
                    QVariant{} : v1.toDouble() / v2.toDouble();
              }, "/"), 2);
    operations.add(new BinaryOperation([](QVariant v1, QVariant v2) -> QVariant {
                  return (v1.type() == QVariant::Invalid || v2.type() == QVariant::Invalid) ?
                  QVariant{} : v1.toDouble() + v2.toDouble();
              }, "+"), 3);
    operations.add(new BinaryOperation([](QVariant v1, QVariant v2) -> QVariant {
                  return (v1.type() == QVariant::Invalid || v2.type() == QVariant::Invalid) ?
                    QVariant{} : v1.toDouble() - v2.toDouble();
              }, "-"), 3);
    operations.add(new BinaryOperation([](QVariant v1, QVariant v2) -> QVariant {
                  return (v1.type() == QVariant::Invalid || v2.type() == QVariant::Invalid) ?
                    QVariant{} : v1.toDouble() || v2.toDouble();
              }, "||"), 4);
    operations.add(new BinaryOperation([](QVariant v1, QVariant v2) -> QVariant {
                  return (v1.type() == QVariant::Invalid || v2.type() == QVariant::Invalid) ?
                    QVariant{} : v1.toDouble() && v2.toDouble();
              }, "&&"), 4);
    table = operations;
}

TableParser::~TableParser() {}

#include <QDebug>

QVariant TableParser::getFactor(const QString &str, int &pos) const {
    QVariant result = 0.0;
    QRegExp regExpColumn(QString("[A-Za-z]{1,") + QString::number(sp->rowCount()/26 + 1) + QString("}"));
    QRegExp regExpRow(QString("[1-9][0-9]{0,") + QString::number(sp->columnCount()/10) + QString("}"));
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
        qDebug() << "TableParser::getFactor(): factor found " << result;
    if (!ok) {
        if (regExpColumn.exactMatch(column) && regExpRow.exactMatch(row)) {
            int tabColumn = 0;
            for (auto & x : column)
                tabColumn += x.toUpper().unicode() - 'A';
            int tabRow = row.toInt()-1;

            Cell *c = static_cast<Cell*>(sp->item(tabRow, tabColumn));
            if (c) {
                qDebug() << "TableParser::getFactor(): found value: " << c->value();
                result = c->value();
            } else {
                result = 0.0;
            }
        } else {
            result = QVariant{};
        }
    }
    return result;
}
