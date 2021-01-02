#include "pgadetectdetailtable.h"
#include "ui_pgadetectdetailtable.h"

PgaDetectDetailTable::PgaDetectDetailTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PgaDetectDetailTable)
{
    ui->setupUi(this);
}

PgaDetectDetailTable::~PgaDetectDetailTable()
{
    delete ui;
}

void PgaDetectDetailTable::setup(QString title, QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaInfos)
{
    ui->titleLB->setText(title);
    ui->pgaTW->setRowCount(0);

    for(int i=0;i<pgaInfos.count();i++)
    {
        ui->pgaTW->setRowCount(ui->pgaTW->rowCount()+1);
        ui->pgaTW->setItem(i, 1, new QTableWidgetItem(QString(pgaInfos.at(i).sta)));
        ui->pgaTW->setItem(i, 2, new QTableWidgetItem(QString(pgaInfos.at(i).chan)));
        ui->pgaTW->setItem(i, 0, new QTableWidgetItem(QString(pgaInfos.at(i).net)));
        ui->pgaTW->setItem(i, 3, new QTableWidgetItem(QString::number(pgaInfos.at(i).lat, 'f', 4)));
        ui->pgaTW->setItem(i, 4, new QTableWidgetItem(QString::number(pgaInfos.at(i).lon, 'f', 4)));
        QDateTime timeUTC, timeKST;
        timeUTC.setTimeSpec(Qt::UTC);
        timeUTC.setTime_t(pgaInfos.at(i).time);
        timeKST = convertKST(timeUTC);
        ui->pgaTW->setItem(i, 5, new QTableWidgetItem(timeKST.toString("hh:mm:ss")));
        ui->pgaTW->setItem(i, 6, new QTableWidgetItem(QString::number(pgaInfos.at(i).maxH, 'f', 4)));
    }

    for(int i=0;i<ui->pgaTW->rowCount();i++)
    {
        for(int j=0;j<ui->pgaTW->columnCount();j++)
        {
            if(ui->pgaTW->item(i, j)->text() != "") ui->pgaTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }

    ui->pgaTW->setFixedHeight(30*pgaInfos.count() + 23);
}
