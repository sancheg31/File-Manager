
#include <QtMath>

#include "TableParser.h"
#include "Cell.h"

TableParser::TableParser(Spreadsheet* s, const OperationTable& operations): sp(s) {
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
    if (!ok) {
        if (regExpColumn.exactMatch(column) && regExpRow.exactMatch(row)) {
            int tabColumn = 0;
            for (auto & x : column)
                tabColumn += x.toUpper().unicode() - 'A';
            int tabRow = row.toInt()-1;

            Cell *c = static_cast<Cell*>(sp->item(tabRow, tabColumn));
            result = (c) ? c->value() : 0.0;
        } else {
            result = QVariant{};
        }
    }
    return result;
}
