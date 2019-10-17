#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("File Manager");
    MainWindow mainWindow;
    mainWindow.setWindowIcon(QIcon(":/Images/App.ico"));
    mainWindow.show();

    return app.exec();
}
