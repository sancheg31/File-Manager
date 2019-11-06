#include <QtGui>
#include <QTableWidget>

#include "Cell.h"
#include "Spreadsheet.h"
#include "TableParser.h"

Cell::Cell(): modified(true) {
}

QTableWidgetItem* Cell::clone() const {
    return new Cell(*this);
}

void Cell::setData(int role, const QVariant &value) {
    QTableWidgetItem::setData(role, value);
    if (role == Qt::EditRole)
        setModified();
}

QVariant Cell::data(int role) const {

    if (role == Qt::DisplayRole) {
        if (value().isValid()) {
            return value().toString();
        } else {
            return "####";
        }
    } else if (role == Qt::TextAlignmentRole) {
        if (value().type() == QVariant::String) {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        } else {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    } else {
        return QTableWidgetItem::data(role);
    }

}

void Cell::setFormula(const QString &formula) {
    setData(Qt::EditRole, formula);
}

QString Cell::formula() const {
    return data(Qt::EditRole).toString();
}

void Cell::setModified() {
    modified = true;
}

QVariant Cell::value() const {

    if (modified) {
        modified = false;

        QString formulaStr = formula();
        if (formulaStr.startsWith('\'')) {
            cachedValue = formulaStr.mid(1);
        } else if (formulaStr.startsWith('=')) {
            auto sp = qobject_cast<Spreadsheet*>(tableWidget());
            cachedValue = sp->parser()->parse(formulaStr);

        } else {
            bool ok;
            double d = formulaStr.toDouble(&ok);
            if (ok) {
                cachedValue = d;
            } else {
                cachedValue = (formulaStr.isEmpty()) ? 0 : formulaStr;
            }
        }
    }
    return cachedValue;

}
