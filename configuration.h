#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "common.h"
#include "writelog.h"

#include <QDialog>

namespace Ui {
class Configuration;
}

class Configuration : public QDialog
{
    Q_OBJECT

public:
    explicit Configuration(QWidget *parent = 0);
    ~Configuration();

    void setup(_CONFIGURE);

    _CONFIGURE con;

private:
    Ui::Configuration *ui;

    QString logDir;
    WriteLog *log;

    QTextCodec *codec;

public slots:
    void writeConfigureToFile();

signals:
    void resetMainWindow();
};

#endif // CONFIGURATION_H
