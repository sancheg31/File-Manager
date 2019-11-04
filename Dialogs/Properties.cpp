
#include <QtWidgets>

#include "Pane.h"
#include "Dialogs/Properties.h"

Properties::Properties(const QFileInfo& fileInfo, QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Properties"));
    setWindowIcon(QIcon(":/Images/Properties.ico"));
    QVBoxLayout* layout = new QVBoxLayout(this);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(slotReject()));
    layout->addLayout(formLayout(fileInfo));
    layout->addWidget(buttonBox);
    setLayout(layout);
}

Properties::Properties(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Properties"));
    setWindowIcon(QIcon(":/Images/Properties.ico"));
}

QString Properties::detFileSizeNotation(int64_t fileSize) const {
    QString result;
    if (fileSize < 1024)
        result = QString::number(fileSize) + " bytes";
    else if (fileSize < 1024 * 1024)
        result = QString::number(fileSize/1024) + " KB";
    else if (fileSize < static_cast<int64_t>(1024 * 1024 * 1024))
        result = QString::number(static_cast<int64_t>(fileSize/1024/1024.0*10)/10.0) + " MB";
    else
        result = QString::number(static_cast<int64_t>(fileSize/1024/1024.0/1024*10)/10.0) + " GB";
    return result;
}

QFormLayout* Properties::formLayout(const QFileInfo& fileInfo) const {

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setAlignment(Qt::AlignCenter);
    formLayout->setMargin(5);
    formLayout->addRow(tr("Name:"), new QLabel(fileInfo.fileName()));
    formLayout->addRow(tr("Path:"), new QLabel(fileInfo.path()));
    formLayout->addRow(tr("Size:"), new QLabel(fileInfo.isDir() ? "Unknown" : detFileSizeNotation(fileInfo.size())));
    formLayout->addRow(tr("Created:"), new QLabel(fileInfo.birthTime().toString()));
    formLayout->addRow(tr("Date Modified:"), new QLabel(fileInfo.lastModified().toString()));
    formLayout->addRow(tr("Date Read:"), new QLabel(fileInfo.lastRead().toString()));
    formLayout->addRow(tr("Owner:"), new QLabel(fileInfo.owner()));
    formLayout->addRow(tr("Group:"), new QLabel(fileInfo.group()));
    return formLayout;

}
void Properties::slotAccept() {
    done(1);
}

void Properties::slotReject() {
    done(0);
}
