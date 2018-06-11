#include "sohmonitor.h"
#include "ui_sohmonitor.h"

SohMonitor::SohMonitor() :
    QDialog(),
    ui(new Ui::SohMonitor)
{
    ui->setupUi(this);
}

SohMonitor::~SohMonitor()
{
    delete ui;
}

void SohMonitor::setup(_STATION sta, int type) // type == 0 is local station
{
    setWindowTitle("< " + sta.sta + " > " + tr("State of health"));
    ui->title->setText(sta.sta);
    ui->chanLB->setText(sta.chan);
    ui->netLB->setText(sta.net);
    ui->locLB->setText(sta.loc);
    ui->latLB->setText(QString::number(sta.lat, 'f', 6));
    ui->lonLB->setText(QString::number(sta.lon, 'f', 6));
    ui->elevLB->setText(QString::number(sta.elev, 'f', 1));
    if(type == 0)
        ui->waveGB->show();
    else
        ui->waveGB->hide();
}
