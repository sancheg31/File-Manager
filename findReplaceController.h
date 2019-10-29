#ifndef FINDREPLACECONTROLLER_H
#define FINDREPLACECONTROLLER_H

#include <QTextEdit>
#include <QList>
#include <QPair>
#include <QObject>

class FindReplaceController: public QObject {
    Q_OBJECT
public:
    FindReplaceController(bool = false, QObject* = nullptr);
    FindReplaceController(QTextEdit*, const QString&, const QString&, bool = false, QObject* = nullptr);
    void set(QTextEdit*, const QString&, const QString&, bool);

    QTextEdit* getEdit() const;
    QString getFindLine() const;
    QString getReplaceLine() const;

    bool findNext();
    bool findPrevious();
    bool replace();
    bool replaceNext();
    bool replacePrevious();

private:

    void replace(int);
    void find(int);
    void findAllOccurences();

    QList<QPair<int, bool>> positions;
    QTextEdit* edit;
    QString f;
    QString r;

    int position;
    bool isCaseSensitive;

public slots:
    void changeEdit(QTextEdit*);
    void changeFindLine(const QString&);
    void changeReplaceLine(const QString&);
    void changeSensivity(bool);
signals:
    void controllerChanged();
};

#endif // FINDREPLACECONTROLLER_H
