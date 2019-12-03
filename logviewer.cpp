#include "logviewer.h"
#include "ui_logviewer.h"

LogViewer::LogViewer(QString logDir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogViewer)
{
    ui->setupUi(this);

    QDate date;
    date = QDate::currentDate();

    ui->dateEdit->setDate(date);
    ui->dateEdit->setCurrentSectionIndex(2);

    dir = logDir;

    connect(ui->readPB, SIGNAL(clicked(bool)), this, SLOT(showLog()));
}

LogViewer::~LogViewer()
{
    delete ui;
}

void LogViewer::showLog()
{
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setReadOnly(true);

    QDate date = ui->dateEdit->date();
    QString fileName = dir + "/KGOM." + date.toString("yyyyMMdd") + ".log";

    QFile file(fileName);
    if(!file.exists())
    {
        ui->plainTextEdit->appendPlainText(tr("No Log file"));
        return;
    }
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line;

        while(!stream.atEnd())
        {
            line = stream.readLine();

            ui->plainTextEdit->appendPlainText(line);
        }
        file.close();
    }
}
