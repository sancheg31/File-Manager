
#include <QApplication>
#include <QSplashScreen>

#include "Windows/FileManagerWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("File Manager");

    QSplashScreen* splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/Images/SplashScreen.jpg"));
    splash->show();
    splash->showMessage(QObject::tr("Setting up the main window..."), Qt::AlignRight | Qt::AlignTop , Qt::white);

    FileManagerWindow mainWindow;
    mainWindow.setWindowIcon(QIcon(":/Images/App.png"));
    mainWindow.show();

    splash->finish(&mainWindow);
    delete splash;
    return app.exec();
}
