#include "EditorSearchPanel.h"

EditorSearchPanel::EditorSearchPanel(QWidget *wgt) : QWidget(wgt) {
    searchForLine = new QLineEdit;
    replaceLine = new QLineEdit;

    previous = new QPushButton(tr("&Search previous"));
    proceed = new QPushButton(tr("&Continue searching"));
    replace = new QPushButton(tr("&Replace"));
    replaceContinue = new QPushButton(tr("&Replace and continue searching"));
    replaceAll = new QPushButton("Replace all");

    isCaseSensitive = new QCheckBox(tr("Case sensitive"));
    isWholeWord = new QCheckBox(tr("Whole words"));
    isRegExp = new QCheckBox(tr("Use regular expresion"));

    proceed->setEnabled(false);
    previous->setEnabled(false);
    replace->setEnabled(false);
    replaceContinue->setEnabled(false);
    replaceAll->setEnabled(false);

    replaceLine->setMinimumWidth(550);
    searchForLine->setMinimumWidth(550);
    QHBoxLayout * checkGroupLayout = new QHBoxLayout;
    checkGroupLayout->addWidget(isCaseSensitive);
    checkGroupLayout->addWidget(isWholeWord);
    checkGroupLayout->addWidget(isRegExp);

    QGridLayout * searchLayout = new QGridLayout;
    searchLayout->addWidget(new QLabel(tr("Search")), 0, 0, 1, 1);
    searchLayout->addWidget(searchForLine, 0, 1, 1, 4);
    searchLayout->addWidget(previous, 0, 5, 1, 1);
    searchLayout->addWidget(proceed, 0, 6, 1, 1);

    searchLayout->addWidget(new QLabel(tr("Replace")), 1, 0, 1, 1);
    searchLayout->addWidget(replaceLine, 1, 1, 1, 4);
    searchLayout->addWidget(replace, 1, 5);
    searchLayout->addWidget(replaceContinue, 1, 6);
    searchLayout->addWidget(replaceAll, 1, 7);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(searchLayout);
    mainLayout->addLayout(checkGroupLayout);

    connect(searchForLine, SIGNAL(textChanged(const QString&)), this, SLOT(setButtonsEnabled(const QString&)));
    setLayout(mainLayout);
}

void EditorSearchPanel::setButtonsEnabled(const QString& text){
    previous->setEnabled(!text.isEmpty());
    proceed->setEnabled(!text.isEmpty());
    replace->setEnabled(!text.isEmpty());
    replaceContinue->setEnabled(!text.isEmpty());
    replaceAll->setEnabled(!text.isEmpty());
}
