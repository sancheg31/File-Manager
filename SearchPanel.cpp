#include <QtWidgets>
#include <QtConcurrent/QtConcurrent>

#include "SearchPanel.h"
#include "Pane.h"

enum { absoluteFileNameRole = Qt::UserRole + 1 };

static inline QString fileNameOfItem(const QTableWidgetItem *item) {
    return item->data(absoluteFileNameRole).toString();
}

SearchPanel::SearchPanel(QWidget *parent) : QWidget(parent),
    regexCheckBox(new QCheckBox("Regular expressions")), caseCheckBox(new QCheckBox("Case Sensivity")),
    wholeWordCheckBox(new QCheckBox("Whole Words")), filesFoundLabel(new QLabel()), fileComboBox(createComboBox(tr("*"))),
    textComboBox(createComboBox()), directoryComboBox(createComboBox(QDir::toNativeSeparators("D:/")))
{

    setWindowTitle(tr("Search Panel"));
    QPushButton* browseButton = new QPushButton(tr("&Browse..."), this);
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
    findButton = new QPushButton(tr("&Find"), this);
    stopButton = new QPushButton(tr("Stop"), this);
    stopButton->setEnabled(false);
    connect(findButton, SIGNAL(clicked()), this, SLOT(startFind()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(slotStopButtonClicked()));

    connect(fileComboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(animateFindClick()));
    connect(textComboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(animateFindClick()));
    connect(directoryComboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(animateFindClick()));

    createFilesTable();
    settings = restoreState();

    QGridLayout *mainLayout1 = new QGridLayout;
    mainLayout1->addWidget(new QLabel(tr("Named:")), 0, 0, 1, 1);
    mainLayout1->addWidget(fileComboBox, 0, 1, 1, 5);

    mainLayout1->addWidget(new QLabel(tr("Containing text:")), 1, 0, 1, 1);
    mainLayout1->addWidget(textComboBox, 1, 1, 1, 5);

    mainLayout1->addWidget(new QLabel(tr("In directory:")), 2, 0, 1, 1);
    mainLayout1->addWidget(directoryComboBox, 2, 1, 1, 4);
    mainLayout1->addWidget(browseButton, 2, 5, 1, 1);

    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addWidget(regexCheckBox);
    hLayout->addWidget(caseCheckBox);
    hLayout->addWidget(wholeWordCheckBox);

    QGridLayout *mainLayout2 = new QGridLayout;
    mainLayout2->addWidget(filesTable, 4, 0, 2, 6);
    mainLayout2->addWidget(filesFoundLabel, 6, 0, 1, 3);
    mainLayout2->addWidget(stopButton, 6, 4, 1, 1);
    mainLayout2->addWidget(findButton, 6, 5, 1, 1);

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(mainLayout1);
    mainLayout->addLayout(hLayout);
    mainLayout->addLayout(mainLayout2);

    connect(new QShortcut(QKeySequence::Quit, this), &QShortcut::activated,
        qApp, &QApplication::quit);

}

void SearchPanel::openFile(const QString &fileName) {
    QFileInfo fileInfo(fileName);
    if (fileInfo.isDir()) {
        emit directoryLoaded(fileInfo);
    } else if (fileInfo.isFile() && fileInfo.fileName().endsWith(".txt")) {
        emit fileLoaded(fileInfo);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void SearchPanel::saveState(QSettings* settings) const {
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("RegexCheckBox", regexCheckBox->isChecked());
    settings->setValue("WholeWordCheckBox", wholeWordCheckBox->isChecked());
    settings->setValue("CaseCheckBox", caseCheckBox->isChecked());
    settings->setValue("DirectoryComboBox", directoryComboBox->currentText());
    settings->setValue("FileComboBox", fileComboBox->currentText());
    settings->setValue("TextComboBox", textComboBox->currentText());

}

QSettings* SearchPanel::restoreState() {
    QSettings* settings = new QSettings();
    restoreGeometry(settings->value("Geometry").toByteArray());
    regexCheckBox->setChecked(settings->value("regexCheckBox").toBool());
    wholeWordCheckBox->setChecked(settings->value("WholeWordCheckBox").toBool());
    caseCheckBox->setChecked(settings->value("CaseCheckBox").toBool());
    directoryComboBox->setCurrentText(settings->value("DirectoryComboBox").toString());
    fileComboBox->setCurrentText(settings->value("FileComboBox").toString());
    textComboBox->setCurrentText(settings->value("TextComboBox").toString());
    return settings;
}

void SearchPanel::closeEvent(QCloseEvent* event) {
    saveState(settings);
    QWidget::closeEvent(event);
}

void SearchPanel::slotShow() {
    show();
}

void SearchPanel::slotStopButtonClicked() {
    stopIsClicked = true;
}

void SearchPanel::browse() {
    QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Find Files"),
                                                  directoryComboBox->currentText()));
    if (directory.isEmpty())
        return;
    if (directoryComboBox->findText(directory) == -1)
        directoryComboBox->addItem(directory);
    directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
}

static void updateComboBox(QComboBox *comboBox) {
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

void SearchPanel::startFind() {
    findButton->setEnabled(false);
    stopButton->setEnabled(true);
    stopIsClicked = false;
    filesFoundLabel->setText("Processing...");
    QFuture<void> future = QtConcurrent::run(this, &SearchPanel::find);
    findButton->setEnabled(true);
    stopButton->setEnabled(false);
}

void SearchPanel::find() {

    filesTable->setRowCount(0);

    QString fileName = fileComboBox->currentText();
    if (fileName.isEmpty() || fileName == tr(""))  {
        return;
    }
    QString text = textComboBox->currentText();
    QString path = QDir::cleanPath(directoryComboBox->currentText());
    currentDir = QDir(path);

    updateComboBox(fileComboBox);
    updateComboBox(textComboBox);
    updateComboBox(directoryComboBox);

    QStringList filter;
    if (!fileName.isEmpty())
        filter << fileName;
    QDirIterator it(path, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext() && !stopIsClicked) {
        QString str = it.next();
        if (equalWithCheckBoxes(str, fileName))
            files << str;
    }
    if (!text.isEmpty())
        files = findFiles(files, text);
    files.sort();
    showFiles(files);

}

void SearchPanel::animateFindClick() {
    findButton->animateClick();
}

QStringList SearchPanel::findFiles(const QStringList &files, const QString &text) {

    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find Files"));

    QMimeDatabase mimeDatabase;
    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %n...", nullptr, files.size()).arg(i));
        QCoreApplication::processEvents();

        if (progressDialog.wasCanceled())
            break;

        const QString fileName = files.at(i);
        const QMimeType mimeType = mimeDatabase.mimeTypeForFile(fileName);
        if (mimeType.isValid() && !mimeType.inherits(QStringLiteral("text/plain"))) {
            qWarning() << "Not searching binary file " << QDir::toNativeSeparators(fileName);
            continue;
        }
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if (line.contains(text, Qt::CaseInsensitive)) {
                    foundFiles << files.at(i);
                    break;
                }
            }
        }
    }
    return foundFiles;
}

void SearchPanel::showFiles(const QStringList &paths) {
    int row = 0;
    for (const QString &filePath : paths) {

        const QString toolTip = QDir::toNativeSeparators(filePath);
        const QString relativePath = QDir::toNativeSeparators(currentDir.relativeFilePath(filePath));
        const qint64 size = QFileInfo(filePath).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(relativePath);
        fileNameItem->setData(absoluteFileNameRole, QVariant(filePath));
        fileNameItem->setToolTip(toolTip);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *sizeItem = new QTableWidgetItem(QLocale().formattedDataSize(size));
        sizeItem->setData(absoluteFileNameRole, QVariant(filePath));
        sizeItem->setToolTip(toolTip);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
        ++row;
    }
    filesFoundLabel->setText(QString("%n file(s) found").arg(paths.size()) +
                             (paths.size() > 0 ? " (Double click on a file to open it)" : ""));
    filesFoundLabel->setWordWrap(true);
}

QComboBox *SearchPanel::createComboBox(const QString &text) {
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return comboBox;
}

void SearchPanel::createFilesTable() {
    filesTable = new QTableWidget(0, 2);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);
    filesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(filesTable, &QTableWidget::customContextMenuRequested,
            this, &SearchPanel::contextMenu);
    connect(filesTable, &QTableWidget::cellActivated,
            this, &SearchPanel::openFileOfItem);
}

bool SearchPanel::equalWithCheckBoxes(const QString& str, const QString& fileName) {
    if (!regexCheckBox->isChecked()) {
        if (!wholeWordCheckBox->isChecked())
            if ((QFileInfo(str).fileName().contains(fileName, caseCheckBox->isChecked() ?
                                                    Qt::CaseSensitive : Qt::CaseInsensitive) || fileName == tr("*")))
            return true;
        else if (wholeWordCheckBox->isChecked())
            if (QFileInfo(str).fileName() == fileName ||
                 fileName == tr("*"))
            return true;
    } else if (regexCheckBox->isChecked()) {
        QRegExp reg(fileName);
        if (reg.isValid() && reg.exactMatch(str))
            return true;
    }
    return false;
}


void SearchPanel::openFileOfItem(int row, int /* column */) {
    const QTableWidgetItem *item = filesTable->item(row, 0);
    openFile(fileNameOfItem(item));
}

void SearchPanel::contextMenu(const QPoint &pos) {
    const QTableWidgetItem *item = filesTable->itemAt(pos);
    if (!item)
        return;
    QMenu menu;
    QAction *openAction = menu.addAction("Open");
    QAction *action = menu.exec(filesTable->mapToGlobal(pos));
    if (!action)
        return;
    const QString fileName = fileNameOfItem(item);
    if (action == openAction)
        openFile(fileName);
}
