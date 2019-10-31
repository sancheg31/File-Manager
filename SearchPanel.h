#pragma once

#include <QWidget>
#include <QDir>

#include "ISettings.h"

class QComboBox;
class QCheckBox;
class QLabel;
class QPushButton;
class QTableWidget;


class SearchPanel : public QWidget, private ISettings {
    Q_OBJECT
public:
    SearchPanel(QWidget *parent = nullptr);

    void openFile(const QString&);
    void find();

protected:
    void closeEvent(QCloseEvent* event);

private slots:

    void slotShow();
    void slotStopButtonClicked();
    void browse();

    void startFind();
    void animateFindClick();
    void openFileOfItem(int row, int column);
    void contextMenu(const QPoint &pos);

private:

    virtual void saveState(QSettings*) const override;
    virtual QSettings* restoreState() override;

    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &paths);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();

    bool equalWithCheckBoxes(const QString& str, const QString& fileName);

    QComboBox* fileComboBox;
    QComboBox* textComboBox;
    QComboBox* directoryComboBox;
    QCheckBox* regexCheckBox;
    QCheckBox* caseCheckBox;
    QCheckBox* wholeWordCheckBox;

    QLabel *filesFoundLabel;
    QPushButton* findButton;
    QPushButton* stopButton;
    QTableWidget *filesTable;

    QSettings* settings;

    QDir currentDir;

    bool stopIsClicked;

signals:
    void fileLoaded(const QFileInfo&);
    void directoryLoaded(const QFileInfo&);
};

