#-------------------------------------------------
#
# Project created by QtCreator 2019-10-13T13:06:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = File_Manager_Main
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
    Action.cpp \
    DocWindow.cpp \
    EditorSearchPanel.cpp \
    FileSystemModelFilterProxyModel.cpp \
    FindReplaceController.cpp \
    IDocument.cpp \
    MainWindow.cpp \
    Pane.cpp \
    PaneSwitcher.cpp \
    SearchPanel.cpp \
    StringSet.cpp \
    TextEditor.cpp \
    main.cpp \
    Cell.cpp \
    SpreadSheetCompare.cpp \
    SpreadsheetWindow.cpp \
    Views/ListView.cpp \
    Views/TableView.cpp \
    Views/TreeView.cpp \
    Views/DefaultViewSettings.cpp \
    Views/ProxyTreeView.cpp \
    SpreadSheet.cpp \
    Dialogs/FindDialog.cpp \
    Dialogs/GoToCellDialog.cpp \
    Dialogs/SortDialog.cpp \
    Dialogs/Parameters.cpp \
    Dialogs/Properties.cpp \
    TextEditorWindow.cpp \
    DocumentContainer.cpp

HEADERS += \
    Action.h \
    DocWindow.h \
    EditorSearchPanel.h \
    FileSystemModelFilterProxyModel.h \
    FindReplaceController.h \
    ISettings.h \
    MainWindow.h \
    Pane.h \
    PaneSwitcher.h \
    SearchPanel.h \
    StringSet.h \
    TextEditor.h \
    Views/DefaultViewSettings.h \
    Views/ListView.h \
    Views/TableView.h \
    Views/TreeView.h \
    Views/ProxyTreeView.h \
    SpreadSheet.h \
    Cell.h \
    SpreadSheetCompare.h \
    SpreadsheetWindow.h \
    Dialogs/Parameters.h \
    Dialogs/Properties.h \
    Dialogs/FindDialog.h \
    Dialogs/GoToCellDialog.h \
    Dialogs/SortDialog.h \
    TextEditorWindow.h \
    DocumentContainer.h \
    IDocument.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

FORMS += \
    Dialogs/gotocelldialog.ui \
    Dialogs/sortdialog.ui
