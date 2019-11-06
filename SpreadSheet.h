#pragma once

#include <QTableWidget>

#include "ArithmeticParser/OperationTable.h"

class Cell;
class SpreadsheetCompare;
class TableParser;

class Spreadsheet : public QTableWidget {
    Q_OBJECT
public:
    Spreadsheet(QWidget *parent = nullptr);

    bool autoRecalculate() const { return autoRecalc; }

    QString currentLocation() const;
    QString currentFormula() const;
    void setCurrentFormula(const QString& str) { setFormula(currentRow(), currentColumn(), str); }
    QTableWidgetSelectionRange selectedRange() const;

    void clear();
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    void sort(const SpreadsheetCompare &compare);

    TableParser* parser() const { return tabParser; }

    static const int CheckValue = 0x123FBC78;

public slots:

    void slotAddRow(int before);
    void slotAddColumn(int before);

    void slotDeleteRow(int before);
    void slotDeleteColumn(int before);

    void cut();
    void copy();
    void paste();
    void del();
    void selectCurrentRow();
    void selectCurrentColumn();
    void recalculate();
    void setAutoRecalculate(bool recalc);
    void findNext(const QString &str, Qt::CaseSensitivity cs);
    void findPrevious(const QString &str, Qt::CaseSensitivity cs);

signals:
    void modified();

private slots:
    void slotSpreadsheetChanged();

private:

    Cell *cell(int row, int column) const;
    QString text(int row, int column) const;
    QString formula(int row, int column) const;
    void setFormula(int row, int column, const QString &formula);

    QString columnToString(int) const;
    void setHorizontalHeader(int);
    OperationTable createOperationTable();


    int rCount;
    int colCount;
    QPair<int, int> start;
    QPair<int, int> end;
    QVector<QVector<QString>> bufferData;

    OperationTable operations;
    TableParser* tabParser;
    bool autoRecalc;


};


