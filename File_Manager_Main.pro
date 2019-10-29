<<<<<<< HEAD
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
        main.cpp \
    Action.cpp \
    FileSystemModelFilterProxyModel.cpp \
    MainWindow.cpp \
    Pane.cpp \
    PaneSwitcher.cpp \
    Properties.cpp \
    SearchPanel.cpp \
    TextEditor.cpp \
    DocWindow.cpp \
    StringSet.cpp \
    EditorSearchPanel.cpp \
    FindReplaceController.cpp \
    DefaultViewSettings.cpp \
    TreeView.cpp \
    ListView.cpp \
    TableView.cpp \
    Parameters.cpp

HEADERS += \
    Action.h \
    FileSystemModelFilterProxyModel.h \
    MainWindow.h \
    Pane.h \
    PaneSwitcher.h \
    Properties.h \
    SearchPanel.h \
    TextEditor.h \
    DocWindow.h \
    StringSet.h \
    EditorSearchPanel.h \
    FindReplaceController.h \
    DefaultViewSettings.h \
    TreeView.h \
    ListView.h \
    TableView.h \
    Parameters.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
=======
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
        main.cpp \
    Action.cpp \
    FileSystemModelFilterProxyModel.cpp \
    MainWindow.cpp \
    Pane.cpp \
    PaneSwitcher.cpp \
    Properties.cpp \
    SearchPanel.cpp \
    TextEditor.cpp \
    DocWindow.cpp \
    StringSet.cpp \
    EditorSearchPanel.cpp \
    FindReplaceController.cpp \
    DefaultViewSettings.cpp \
    TreeView.cpp \
    ListView.cpp \
    TableView.cpp \
    Parameters.cpp

HEADERS += \
    Action.h \
    FileSystemModelFilterProxyModel.h \
    MainWindow.h \
    Pane.h \
    PaneSwitcher.h \
    Properties.h \
    SearchPanel.h \
    TextEditor.h \
    DocWindow.h \
    StringSet.h \
    EditorSearchPanel.h \
    FindReplaceController.h \
    DefaultViewSettings.h \
    TreeView.h \
    ListView.h \
    TableView.h \
    Parameters.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
>>>>>>> File_Manager_Main 2.0
