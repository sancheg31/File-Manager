
#include <QFile>
#include <QMessageBox>
#include <QtWidgets>

#include "Cell.h"
#include "SpreadSheet.h"
#include "SpreadSheetCompare.h"
#include "TableParser.h"
#include "Styles/SpreadsheetStyle.h"

Spreadsheet::Spreadsheet(QWidget* parent): QTableWidget(parent), autoRecalc(true),
    rCount(30), colCount(26), bufferData{}, operations{}, tabParser(new TableParser(this, createOperationTable())) {
    setItemPrototype(new Cell);
    setSelectionMode(SelectionMode::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
    setStyle(new SpreadsheetStyle());
    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotSpreadsheetChanged()));
    clear();
}

void Spreadsheet::clear() {

   setRowCount(0);
   setColumnCount(0);
   setRowCount(rCount);
   setColumnCount(colCount);

   for (int i = 0; i < colCount; ++i) {
       QString str{};
       setHorizontalHeader(i);
   }
   setCurrentCell(0, 0);
}

Cell* Spreadsheet::cell(int row, int column) const {
    return static_cast<Cell*>(item(row, column));
}

QString Spreadsheet::text(int row, int column) const {
    Cell* c = cell(row, column);
    return (c == nullptr) ? "" : c->text();
}


QString Spreadsheet::formula(int row, int column) const {
    Cell* c = cell(row, column);
    return (c == nullptr) ? "" : c->formula();
}

void Spreadsheet::setFormula(int row, int column, const QString& str) {
    Cell* c = cell(row, column);
    if (c == nullptr) {
        c = new Cell;
        setItem(row, column, c);
    }
    c->setFormula(str);
}

QString Spreadsheet::columnToString(int column) const {
    QString result{};
    do {
        result += QChar('A' + (column % 26));
        column /= 26;
    } while (column > 0);
    std::reverse(result.begin(), result.end());
    if (result.size() > 1)
        result[0] = QChar(result[0].unicode()-1);
    return result;
}

QString Spreadsheet::currentLocation() const {
    return columnToString(currentColumn()) + QString::number(currentRow()+1);
}

QString Spreadsheet::currentFormula() const {
    return formula(currentRow(), currentColumn());
}

void Spreadsheet::slotSpreadsheetChanged() {
    if (autoRecalc)
        recalculate();
    emit modified();
}

bool Spreadsheet::writeFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"), tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_12);
    out << quint32(CheckValue);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    for (int row = 0; row < rCount; ++row)
        for (int column = 0; column < colCount; ++column) {
            QString str = formula(row, column);
            if (!str.isEmpty())
                out << quint16(row) << quint16(column) << str;
        }
    QApplication::restoreOverrideCursor();
    return true;
}


bool Spreadsheet::readFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly) && !fileName.endsWith(".sp")) {
        QMessageBox::warning(this, tr("Spreadsheet"), tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_12);
    quint32 checkVal;
    in >> checkVal;

    if (checkVal != CheckValue) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("The file is not a Spreadsheet file."));
        return false;
    }
    clear();
    quint16 row, column;
    QString str;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (!in.atEnd()) {
        in >> row >> column >> str;
        setFormula(row, column, str);
    }
    QApplication::restoreOverrideCursor();
    return true;

}

void Spreadsheet::cut() {
    copy();
    del();
}

QTableWidgetSelectionRange Spreadsheet::selectedRange() const {
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if (ranges.isEmpty() || ranges.count() > 1) {
        return QTableWidgetSelectionRange();
    }
    return ranges.first();
}

void Spreadsheet::copy() {
    QTableWidgetSelectionRange range = selectedRange();
    QString str;
    for (int i = 0; i < range.rowCount(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < range.columnCount(); ++j) {
            if (j > 0)
                str += "\t";
            str += formula(range.topRow() + i, range.leftColumn() + j);
        }
    }
    QApplication::clipboard()->setText(str);
}

void Spreadsheet::paste() {
    QTableWidgetSelectionRange range = selectedRange();
    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;
    if (range.rowCount() * range.columnCount() != 1 && (range.rowCount() != numRows
                || range.columnCount() != numColumns)) {
        QMessageBox::information(this, tr("Spreadsheet"),
                                 tr("The information cannot be pasted because the copy "
                                    "and paste areas aren’t the same size."));
        return;
    }
    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < rowCount() && column < columnCount())
                setFormula(row, column, columns[j]);
        }
    }
    slotSpreadsheetChanged();
}


void Spreadsheet::del() {
    QList<QTableWidgetItem *> items = selectedItems();
    if (!items.isEmpty()) {
        for(auto & item: items)
            delete item;
        slotSpreadsheetChanged();
    }
}

void Spreadsheet::selectCurrentRow() {
    selectRow(currentRow());
}

void Spreadsheet::selectCurrentColumn() {
    selectColumn(currentColumn());
}


void Spreadsheet::recalculate() {
    for (int i = 0; i < rCount; ++i) {
        for (int j = 0; j < colCount; ++j) {
            if (cell(i, j))
                cell(i, j)->setModified();
        }
    }
    viewport()->update();
}

void Spreadsheet::setAutoRecalculate(bool b) {
    autoRecalc = b;
    if (b)
        recalculate();
}

void Spreadsheet::sort(const SpreadsheetCompare &compare) {

    QList<QStringList> rows;
    QTableWidgetSelectionRange range = selectedRange();

    for (int i = 0; i < range.rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < range.columnCount(); ++j)
                row.append(formula(range.topRow() + i, range.leftColumn() + j));
        rows.append(row);
    }

    std::sort(rows.begin(), rows.end(), compare);

    for (int i = 0; i < range.rowCount(); ++i) {
        for (int j = 0; j < range.columnCount(); ++j)
            setFormula(range.topRow() + i, range.leftColumn() + j,
                       rows[i][j]);
    }

    clearSelection();
    slotSpreadsheetChanged();

}

void Spreadsheet::slotAddRow(int before) {

    qDebug() << "Spreadsheet::slotInsertRow(): before is: " << before;
    insertRow(before);
}

void Spreadsheet::slotAddColumn(int before) {
    qDebug() << "Spreadsheet::slotInsertColumn(): before is: " << before;
    insertColumn(before);
    for (int i = before; i < columnCount(); ++i)
        setHorizontalHeader(i);
}

void Spreadsheet::slotDeleteRow(int before) {
    removeRow(before);
}

void Spreadsheet::slotDeleteColumn(int before) {
    removeColumn(before);
    for (int i = before; i < columnCount(); ++i)
        setHorizontalHeader(i);
}

void Spreadsheet::setHorizontalHeader(int col) {
    QTableWidgetItem* item = new QTableWidgetItem;
    item->setText(columnToString(col));
    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground(QBrush(QColor(255, 160, 0)));
    setHorizontalHeaderItem(col, item);
}

OperationTable Spreadsheet::createOperationTable() {
    OperationTable operations{};
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
    return operations;
}

void Spreadsheet::findNext(const QString &str, Qt::CaseSensitivity cs) {

    int row = currentRow();
    int column = currentColumn() + 1;

    while (row < rCount) {
        while (column < colCount) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
            }
            ++column;
        }
        column = 0;
        ++row;
    }
    QApplication::beep();
}

void Spreadsheet::findPrevious(const QString &str, Qt::CaseSensitivity cs) {

    int row = currentRow();
    int column = currentColumn() - 1;

    while (row >= 0) {
        while (column >= 0) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
            }
            --column;
        }
        column = colCount - 1;
        --row;
    }
    QApplication::beep();
}

















