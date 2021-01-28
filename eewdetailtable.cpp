#include "eewdetailtable.h"
#include "ui_eewdetailtable.h"

EewDetailTable::EewDetailTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EewDetailTable)
{
    ui->setupUi(this);
}

EewDetailTable::~EewDetailTable()
{
    delete ui;
}

void EewDetailTable::setup(QString title, QVector<_EEWInfo> eewInfos)
{
    ui->titleLB->setText(title);
    ui->eewTW->setRowCount(0);

    int rowCount = 0;
    for(int i=eewInfos.count()-1;i>=0;i--)
    {
        ui->eewTW->setRowCount(ui->eewTW->rowCount()+1);
        //ui->eewTW->setItem(rowCount, 0, new QTableWidgetItem(QString(eewInfos.at(i).type)));
        ui->eewTW->setItem(rowCount, 0, new QTableWidgetItem(QString::number(eewInfos.at(i).eew_evid)));
        ui->eewTW->setItem(rowCount, 1, new QTableWidgetItem(QString::number(eewInfos.at(i).version)));
        if(eewInfos.at(i).message_category == 0) ui->eewTW->setItem(rowCount, 2, new QTableWidgetItem("LIVE"));
        else ui->eewTW->setItem(rowCount, 2, new QTableWidgetItem("TEST"));
        if(eewInfos.at(i).message_type == 0) ui->eewTW->setItem(rowCount, 3, new QTableWidgetItem("NEW"));
        else if(eewInfos.at(i).message_type == 1) ui->eewTW->setItem(rowCount, 3, new QTableWidgetItem("UPDATE"));
        else ui->eewTW->setItem(rowCount, 3, new QTableWidgetItem("DELETE"));
        ui->eewTW->setItem(rowCount, 4, new QTableWidgetItem(QString::number(eewInfos.at(i).magnitude, 'f', 2)));
        ui->eewTW->setItem(rowCount, 5, new QTableWidgetItem(QString::number(eewInfos.at(i).magnitude_uncertainty, 'f', 2)));
        ui->eewTW->setItem(rowCount, 6, new QTableWidgetItem(QString::number(eewInfos.at(i).latitude, 'f', 2)));
        ui->eewTW->setItem(rowCount, 7, new QTableWidgetItem(QString::number(eewInfos.at(i).latitude_uncertainty, 'f', 2)));
        ui->eewTW->setItem(rowCount, 8, new QTableWidgetItem(QString::number(eewInfos.at(i).longitude, 'f', 2)));
        ui->eewTW->setItem(rowCount, 9, new QTableWidgetItem(QString::number(eewInfos.at(i).longitude_uncertainty, 'f', 2)));
        ui->eewTW->setItem(rowCount, 10, new QTableWidgetItem(QString::number(eewInfos.at(i).depth, 'f', 2)));
        ui->eewTW->setItem(rowCount, 11, new QTableWidgetItem(QString::number(eewInfos.at(i).depth_uncertainty, 'f', 2)));
        ui->eewTW->setItem(rowCount, 12, new QTableWidgetItem(QString::number(eewInfos.at(i).origin_time, 'f', 0)));
        ui->eewTW->setItem(rowCount, 13, new QTableWidgetItem(QString::number(eewInfos.at(i).origin_time_uncertainty, 'f', 0)));
        ui->eewTW->setItem(rowCount, 14, new QTableWidgetItem(QString::number(eewInfos.at(i).number_stations, 'f', 0)));
        ui->eewTW->setItem(rowCount, 15, new QTableWidgetItem(QString::number(eewInfos.at(i).number_triggers, 'f', 0)));
        ui->eewTW->setItem(rowCount, 16, new QTableWidgetItem(QString::number(eewInfos.at(i).sent_flag)));
        ui->eewTW->setItem(rowCount, 17, new QTableWidgetItem(QString::number(eewInfos.at(i).sent_time, 'f', 2)));
        ui->eewTW->setItem(rowCount, 18, new QTableWidgetItem(QString::number(eewInfos.at(i).percentsta, 'f', 2)));
        ui->eewTW->setItem(rowCount, 19, new QTableWidgetItem(QString::number(eewInfos.at(i).misfit, 'f', 2)));
        ui->eewTW->setItem(rowCount, 20, new QTableWidgetItem(QString::number(eewInfos.at(i).status)));
        rowCount++;
    }

    for(int i=0;i<ui->eewTW->rowCount();i++)
    {
        for(int j=0;j<ui->eewTW->columnCount();j++)
        {
            if(ui->eewTW->item(i, j)->text() != "") ui->eewTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}
