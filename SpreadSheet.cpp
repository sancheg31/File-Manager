
#include <QFile>
#include <QMessageBox>
#include <QtWidgets>

#include "Cell.h"
#include "SpreadSheet.h"
#include "SpreadSheetCompare.h"
#include "Styles/SpreadsheetStyle.h"

Spreadsheet::Spreadsheet(QWidget* parent): QTableWidget(parent), autoRecalc(true) {
    setItemPrototype(new Cell);
    setSelectionMode(ContiguousSelection);
    //setStyle(new SpreadsheetStyle());
    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotSomethingChanged()));
    clear();
}

void Spreadsheet::clear() {

   setRowCount(0);
   setColumnCount(0);
   setRowCount(RowCount);
   setColumnCount(ColumnCount);

   for (int i = 0; i < ColumnCount; ++i) {
       QTableWidgetItem *item = new QTableWidgetItem;
       item->setText(QString(QChar('A' + i)));
       setHorizontalHeaderItem(i, item);
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

QString Spreadsheet::currentLocation() const {
    return QChar('A' + currentColumn()) + QString::number(currentRow()+1);
}

QString Spreadsheet::currentFormula() const {
    return formula(currentRow(), currentColumn());
}

void Spreadsheet::slotSomethingChanged() {
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
    out << quint32(MagicNumber);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    for (int row = 0; row < RowCount; ++row)
        for (int column = 0; column < ColumnCount; ++column) {
            QString str = formula(row, column);
            if (!str.isEmpty())
                out << quint16(row) << quint16(column) << str;
        }
    QApplication::restoreOverrideCursor();
    return true;
}


bool Spreadsheet::readFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"), tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_12);
    quint32 magic;
    in >> magic;

    if (magic != MagicNumber) {
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

void Spreadsheet::copy() {
    QTableWidgetSelectionRange range = selectedRange();
    QString str = formula(range.topRow(), range.leftColumn());
    for (int i = 1; i < range.rowCount(); ++i) {
        str += '\n';
        for (int j = 1; j < range.columnCount(); ++j)
            str += '\t' + formula(range.topRow() + i, range.leftColumn() + j);
    }
    QApplication::clipboard()->setText(str);
}

QTableWidgetSelectionRange Spreadsheet::selectedRange() const {
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if (ranges.isEmpty())
        return QTableWidgetSelectionRange();
    return ranges.first();
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
                tr("The information cannot be pasted because the copy and paste areas aren't the same size."));
        return;
    }

    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < RowCount && column < ColumnCount)
                setFormula(row, column, columns[j]);
        }
    }
    slotSomethingChanged();
}

void Spreadsheet::del() {
    QList<QTableWidgetItem *> items = selectedItems();
    if (!items.isEmpty()) {
        for(auto & item: items)
            delete item;
        slotSomethingChanged();
    }
}

void Spreadsheet::selectCurrentRow() {
    selectRow(currentRow());
}

void Spreadsheet::selectCurrentColumn() {
    selectColumn(currentColumn());
}


void Spreadsheet::recalculate() {
    for (int i = 0; i < RowCount; ++i) {
        for (int j = 0; j < ColumnCount; ++j) {
            if (cell(i, j))
                cell(i, j)->setDirty();
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
    slotSomethingChanged();

}

void Spreadsheet::findNext(const QString &str, Qt::CaseSensitivity cs) {

    int row = currentRow();
    int column = currentColumn() + 1;

    while (row < RowCount) {
        while (column < ColumnCount) {
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
        column = ColumnCount - 1;
        --row;
    }
    QApplication::beep();
}

















