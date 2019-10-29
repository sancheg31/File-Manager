#include <QHeaderView>

#include "Views/TableView.h"

TableView::TableView(QWidget* parent): QTableView(parent) {
    setDefaultViewSettings(this);
    setWordWrap(true);
    setShowGrid(false);
    this->verticalHeader()->hide();
    setCornerButtonEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}
