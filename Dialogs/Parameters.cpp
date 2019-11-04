
#include <QtWidgets>
#include <QPair>

#include "Dialogs/Parameters.h"


Parameters::Parameters(const QList<QPair<QString, bool>>& list, QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("Parameters"));
    setWindowIcon(QIcon::fromTheme("preferences-other", QIcon(":/Images/Preferences.ico")));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QCheckBox * checkBox = nullptr;
    for (auto & it: list) {
        checkBox = new QCheckBox(it.first);
        checkBox->setChecked(it.second);
        layout->addWidget(checkBox);
        checkBoxList.push_back(checkBox);
    }

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(slotReject()));

    layout->addWidget(buttonBox);
    setLayout(layout);

}

void Parameters::slotAccept() {
    QList<bool> list;
    for (auto & checkBox: checkBoxList)
        list.push_back(checkBox->isChecked());
    emit parametersChanged(list);
    done(1);
}

void Parameters::slotReject() {
    done(0);
}
