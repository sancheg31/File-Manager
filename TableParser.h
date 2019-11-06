#pragma once

#include <QTableWidget>

#include "ArithmeticParser/Parser.h"
#include "Spreadsheet.h"

class TableParser: public Parser {
public:
    TableParser(Spreadsheet*, const OperationTable&);
    virtual ~TableParser() override;

protected:
    QVariant getFactor(const QString& str, int & pos) const override;

    Spreadsheet* sp;
};
