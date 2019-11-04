
#include <QtWidgets>

#include "Action.h"
#include "TextEditor.h"

TextEditor::TextEditor(QObject * obj): QObject(obj), actDoc(nullptr) { }

const DocumentContainer& TextEditor::getDocuments() const {
    return documents;
}

bool TextEditor::isNewFile() const {
    return actDoc->fileName().contains("new ") && !actDoc->fileName().endsWith(".txt");
}

bool TextEditor::loadFile(const QFileInfo& fileInfo) {
    if (!fileInfo.isFile() || documents.contains(fileInfo.absoluteFilePath()))
        return false;
    loadFiles(QStringList() << fileInfo.absoluteFilePath());
    return true;
}

bool  TextEditor::loadFile(const QString& fileName, const QString& text) {
    if (QFileInfo(fileName).isFile())
        return loadFile(QFileInfo(fileName));
    else if (fileName.contains("new ") && !fileName.endsWith(".txt")) {
        IDocument* doc = createNewDocument();
        doc->setText(text);
        return true;
    }
    return false;
}

bool TextEditor::setActiveDocument(IDocument* doc) {
    if (!doc || documents.contains(doc->fileName())) {
        actDoc = doc;
        return true;
    }
    return false;
}

IDocument* TextEditor::activeDocument() const {
    return actDoc;
}

IDocument* TextEditor::document(const QString& title) const {
    if (documents.contains(title))
        return documents.value(title);
    else
        return nullptr;
}

bool TextEditor::newDoc() {
    createNewDocument();
    return true;
}

bool TextEditor::load(const QString& str) {
    qDebug() << "TextEditor::load(): document contains str: " << str << " " << documents.contains(str);
    if (str.isEmpty() || documents.contains(str)) {
        QMessageBox::warning(nullptr, "Warning", str.isEmpty() ? "No File Name" : "File has already opened",
                             QMessageBox::Ok);
        return false;
    }
    IDocument* doc = createNewDocument();
    doc->load(str);
    return true;
}

bool TextEditor::save() {
    if (isNewFile())
        return false;
    actDoc->save();
    return true;
}

bool TextEditor::saveAs(const QString& str) {
    if (str.isEmpty() || documents.contains(str)) {
        QMessageBox::warning(nullptr, tr("Warning"), (str.isEmpty() ? "No File Name" :
                                      "The File is Already Opened in File Editor"), QMessageBox::Ok);
        return false;
    }
    actDoc->saveAs(str);
    return true;
}

void TextEditor::cut() {
    actDoc->cut();
}

void TextEditor::copy() {
    actDoc->copy();
}

void TextEditor::paste() {
    actDoc->paste();
}

void TextEditor::undo() {
    actDoc->undo();
}

void TextEditor::del() {
    actDoc->textCursor().removeSelectedText();
}

void TextEditor::selectAll() {
    actDoc->selectAll();
}

void TextEditor::setDateAndTime(QDateTime dateTime) {
    actDoc->insertPlainText(dateTime.toString(Qt::SystemLocaleShortDate));
}

void TextEditor::setFont(QFont font) {
    actDoc->setFont(font);
}

void TextEditor::slotCloseFile(const QString&) {
   if (activeDocument()->state() == IDocument::State::Modified) {
       emit fileAboutToBeClosed(activeDocument());
   }
   if (activeDocument()->state() != IDocument::State::Modified) {
       documents.remove(activeDocument()->fileName());
   }
}

void TextEditor::slotStateChanged(IDocument::State state) {
    if (state == IDocument::State::Modified)
        activeDocument()->setWindowIcon(QPixmap(":/Images/ModifiedNewFile.png"));
    else if (state == IDocument::State::Saved)
        activeDocument()->setWindowIcon(QPixmap(":/Images/NewFile.png"));
    qDebug() << "TextEditor::slotStateChanged(): Icon should be set";
}

//private methods

void TextEditor::loadFiles(const QStringList& list) {
    IDocument* doc = nullptr;
    for (const auto & x: list) {
        if (documents.contains(x))
            continue;
        doc = createNewDocument();
        doc->load(x);
    }
}

void TextEditor::loadFiles(const QList<QFileInfo>& list) {
    IDocument* doc = nullptr;
    for (const auto & x: list) {
        if (documents.contains(x.absoluteFilePath()))
            continue;
        doc = createNewDocument();
        doc->load(x.fileName());
    }
}

IDocument* TextEditor::createNewDocument() {
    int counter = 1;
    QString title = QString("new 1");
    while (documents.contains(title)) {
        title = QString("new %1").arg(++counter);
    }

    IDocument* doc = new DocWindow;
    //doc->setAttribute(Qt::WA_DeleteOnClose);
    doc->setWindowTitle(title);
    doc->setWindowIcon(QIcon(":/Images/NewFile.png"));

    actDoc = doc;
    documents.insert(doc->fileName(), doc);
    connect(doc, SIGNAL(fileNameChanged(const QString&, const QString&)),
            &documents, SLOT(slotReplace(const QString&, const QString&)));
    //connect(doc, SIGNAL(fileClosed(const QString&)), &documents, SLOT(slotRemove(const QString&)));
    connect(doc, SIGNAL(fileClosed(const QString&)), this, SLOT(slotCloseFile(const QString&)));
    connect(doc, SIGNAL(stateChanged(IDocument::State)), this, SLOT(slotStateChanged(IDocument::State)));
    return doc;
}

QSettings* TextEditor::restoreState() {
    QSettings* settings = new QSettings("Kushnirenko, K-26", "Text Editor");
    int listCount(settings->value("FileNumber").toInt());

    for (int i = 0; i < listCount; ++i) {
        QString fileName(settings->value(QString("File%1").arg(i)).toString());
        auto state = static_cast<IDocument::State>(settings->value(QString("FileState%1").arg(i)).toInt());
        if (fileName.endsWith(".txt") && QFileInfo(fileName).isFile()) {
            loadFile(QFileInfo(fileName));
        }
        else if (!fileName.endsWith(".txt") && fileName.contains("new ")) {
            QString plainText(settings->value(QString("FileInfo%1").arg(i)).toString());
            loadFile(fileName, plainText);
        }
        activeDocument()->setState(state);
    }

    QString actDocName = settings->value("ActiveDocument").toString();
    actDoc = (actDocName == "") ? nullptr : documents.value(actDocName);
    return settings;
}

void TextEditor::saveState(QSettings* settings) const {
    settings->setValue("FileNumber", documents.count());
    settings->setValue("ActiveDocument", (actDoc) ? actDoc->fileName() : QString(""));

    int ind = 0;
    for(auto it = documents.begin(); it != documents.end(); ++it) {
        settings->setValue(QString("File%1").arg(ind), it.key());
        settings->setValue(QString("FileState%1").arg(ind), (int)it.value()->state());
        if (it.key().contains("new ") && !it.key().endsWith(".txt")) {
            settings->setValue(QString("FileInfo%1").arg(ind), it.value()->document()->toRawText());
            //settings->setValue(QString("Font%1").arg(ind), activeDocument()->font().toString());
        }
        ++ind;
    }
}
