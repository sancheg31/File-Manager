#include "ListView.h"


ListView::ListView(QWidget* parent): QListView(parent) {
    setDefaultViewSettings(this);
    setWrapping(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}
