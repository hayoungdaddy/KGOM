#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

#include <QLockFile>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("kgomtr_ko");
    a.installTranslator(&translator);

    QString tmpDir = QDir::tempPath();
    QLockFile lockFile(tmpDir + "/<unique identifier>.lock");

    if(!lockFile.tryLock(100)){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You already have this app running."
                        "\r\nOnly one instance is allowed.");
        msgBox.exec();
        exit(-1);
    }

    MainWindow w(argv[1]);
    w.show();

    return a.exec();
}
