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
    ArithmeticParser/BinaryOperation.cpp \
    ArithmeticParser/Interfaces/IOperation.cpp \
    ArithmeticParser/Interfaces/IParserTemplateMethod.cpp \
    ArithmeticParser/OperationContainer.cpp \
    ArithmeticParser/OperationTable.cpp \
    ArithmeticParser/Parser.cpp \
    ArithmeticParser/UnaryOperation.cpp \
    Cell.cpp \
    Dialogs/FindDialog.cpp \
    Dialogs/GoToCellDialog.cpp \
    Dialogs/Parameters.cpp \
    Dialogs/Properties.cpp \
    Dialogs/SortDialog.cpp \
    DocumentContainer.cpp \
    EditorSearchPanel.cpp \
    FileManager.cpp \
    FindReplaceController.cpp \
    Interfaces/IDocument.cpp \
    Interfaces/IPrototype.cpp \
    Interfaces/ISaveable.cpp \
    Pane.cpp \
    PaneSwitcher.cpp \
    ProxyModel.cpp \
    SearchPanel.cpp \
    SpreadSheet.cpp \
    SpreadSheetCompare.cpp \
    Styles/SpreadsheetStyle.cpp \
    TextEditor.cpp \
    Views/DefaultViewSettings.cpp \
    Views/ListView.cpp \
    Views/ProxyTreeView.cpp \
    Views/TableView.cpp \
    Views/TreeView.cpp \
    Windows/FileManagerWindow.cpp \
    Windows/SpreadsheetWindow.cpp \
    Windows/TextEditorWindow.cpp \
    docwindow.cpp \
    main.cpp


HEADERS += \
    Action.h \
    ArithmeticParser/BinaryOperation.h \
    ArithmeticParser/Interfaces/IOperation.h \
    ArithmeticParser/Interfaces/IParserTemplateMethod.h \
    ArithmeticParser/OperationContainer.h \
    ArithmeticParser/OperationTable.h \
    ArithmeticParser/Parser.h \
    ArithmeticParser/UnaryOperation.h \
    Cell.h \
    Dialogs/FindDialog.h \
    Dialogs/GoToCellDialog.h \
    Dialogs/Parameters.h \
    Dialogs/Properties.h \
    Dialogs/SortDialog.h \
    DocumentContainer.h \
    EditorSearchPanel.h \
    FileManager.h \
    Interfaces/IDocument.h \
    Interfaces/IPrototype.h \
    Interfaces/ISaveable.h \
    Interfaces/ISettings.h \
    Pane.h \
    PaneSwitcher.h \
    ProxyModel.h \
    SearchPanel.h \
    SpreadSheet.h \
    SpreadSheetCompare.h \
    Styles/SpreadsheetStyle.h \
    TextEditor.h \
    Views/DefaultViewSettings.h \
    Views/ListView.h \
    Views/ProxyTreeView.h \
    Views/TableView.h \
    Views/TreeView.h \
    Windows/FileManagerWindow.h \
    Windows/SpreadsheetWindow.h \
    Windows/TextEditorWindow.h \
    docwindow.h \
    findReplaceController.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    resources.qrc \
    resources.qrc

FORMS += \
    Dialogs/gotocelldialog.ui \
    Dialogs/sortdialog.ui \
    Windows/FileManagerWindow.ui

DISTFILES += \
    Images/About.png \
    Images/App.png \
    Images/Cascade.png \
    Images/Copy.png \
    Images/Cut.png \
    Images/DateAndTime.png \
    Images/Delete.png \
    Images/Exit.png \
    Images/Find.png \
    Images/FindNext.png \
    Images/FindPrevious.png \
    Images/Font.png \
    Images/GoTo.png \
    Images/Grid.png \
    Images/Hidden.png \
    Images/NewFile.png \
    Images/NewFolder.png \
    Images/Notepad.png \
    Images/Open.png \
    Images/Parameters.png \
    Images/Paste.png \
    Images/Recalculate.png \
    Images/Replace.png \
    Images/Save.png \
    Images/SaveAll.png \
    Images/SaveAs.png \
    Images/Select.png \
    Images/SelectAll.png \
    Images/SelectColumn.png \
    Images/SelectRow.png \
    Images/Sort.png \
    Images/Spreadsheet.png \
    Images/Tile.png \
    Images/Undo.png
