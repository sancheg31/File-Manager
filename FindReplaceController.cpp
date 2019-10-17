#include <QTextCursor>

#include "FindReplaceController.h"
#include "docwindow.h"

FindReplaceController::FindReplaceController(bool b, QObject * obj): QObject(obj), edit(nullptr), f(""), r(""), isCaseSensitive(b) {
}

FindReplaceController::FindReplaceController(QTextEdit* e, const QString& find, const QString& replace,
                                             bool b, QObject* obj):
    QObject(obj), edit(e), f(find), r(replace), isCaseSensitive(b)
{

}

QString FindReplaceController::getFindLine() const {
    return f;
}

QString FindReplaceController::getReplaceLine() const {
    return r;
}

QTextEdit* FindReplaceController::getEdit() const {
    return edit;
}


void FindReplaceController::changeEdit(QTextEdit *e) {
    edit = e;
    findAllOccurences();
}

void FindReplaceController::changeFindLine(const QString& l) {
    f = l;
    findAllOccurences();
}

void FindReplaceController::changeReplaceLine(const QString& l) {
    r = l;
    findAllOccurences();
}

void FindReplaceController::changeSensivity(bool b) {
    isCaseSensitive = b;
    findAllOccurences();
}


void FindReplaceController::set(QTextEdit * e, const QString & ff, const QString & rr, bool b) {
    f = ff;
    r = rr;
    edit = e;
    isCaseSensitive = b;
    findAllOccurences();
}
#include <QDebug>

void FindReplaceController::findAllOccurences() {
    if (edit == nullptr || f == "")
        return;
    positions.clear();
    position = 0;
    QString text = edit->toPlainText();
    int counter = -1;
    do {
        counter = text.indexOf(f, counter+1, isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
        if (counter != -1)
            positions.push_back(QPair{counter, false});
    } while (counter != -1);
    if (positions.count() > 0)
        find(position);
    emit controllerChanged();
}

void FindReplaceController::find(int i) {
    QTextCursor cursor = edit->textCursor();
    cursor.setPosition(positions.at(i).first, QTextCursor::MoveAnchor);
    edit->setTextCursor(cursor);
    edit->find(f);
}

void FindReplaceController::replace(int i) {
    QString text = edit->toPlainText();
    text = text.replace(positions.at(i).first, f.count(), r);
    edit->setPlainText(text);
    positions[i].second = true;
}

bool FindReplaceController::replace() {
    if (!positions.at(position).second) {
        replace(position);
        return true;
    }
    return false;
}

bool FindReplaceController::findNext() {
    if (f.isEmpty() || edit == nullptr)
        return false;
    int tempPosition = position+1;
    while (tempPosition < positions.count() && positions.at(tempPosition).second)
        ++tempPosition;
    if (tempPosition >= positions.count())
        return false;
    find(tempPosition);
    position = tempPosition;
    return true;
}

bool FindReplaceController::findPrevious() {
    if (f.isEmpty() || edit == nullptr)
        return false;
    int tempPosition = position - 1;
    while (tempPosition > -1 && positions.at(tempPosition).second)
        --tempPosition;
    if (tempPosition <= -1)
        return false;
    find(tempPosition);
    position = tempPosition;
    return true;
}

bool FindReplaceController::replaceNext() {
    if (f.isEmpty() || edit == nullptr)
        return false;
    int tempPosition = position + 1;
    while (tempPosition < positions.count() && positions.at(tempPosition).second)
        ++tempPosition;
    if (tempPosition >= positions.count())
        return false;
    find(tempPosition);
    position = tempPosition;
    return true;
}

bool FindReplaceController::replacePrevious() {
    if (f.isEmpty() || edit == nullptr)
        return false;
    int tempPosition = position - 1;
    while (tempPosition > -1 && positions.at(tempPosition).second)
        --tempPosition;
    if (tempPosition <= -1)
        return false;
    find(tempPosition);
    position = tempPosition;
    return true;
}


