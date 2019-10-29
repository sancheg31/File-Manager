#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QCheckBox>

class EditorSearchPanel : public QWidget {
   Q_OBJECT

public:

    QLineEdit *searchForLine;
    QLineEdit *replaceLine;

    QPushButton* previous;
    QPushButton* proceed;
    QPushButton* replace;
    QPushButton* replaceContinue;
    QPushButton* replaceAll;

    QCheckBox *isCaseSensitive;
    QCheckBox *isWholeWord;
    QCheckBox *isRegExp;


public:
    EditorSearchPanel(QWidget *wgt = nullptr);
private slots:
    void setButtonsEnabled(const QString&);
};
