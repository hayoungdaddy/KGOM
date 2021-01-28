#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QTextStream>
#include <QDebug>

namespace Ui {
class LogViewer;
}

class LogViewer : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewer(QString logDir = 0, QWidget *parent = 0);
    ~LogViewer();

    //void setup()

private:
    Ui::LogViewer *ui;
    QString dir;

private slots:
    void showLog();
};

#endif // LOGVIEWER_H
