#include "sohmonitor.h"
#include "ui_sohmonitor.h"

SohMonitor::SohMonitor() :
    QDialog(),
    ui(new Ui::SohMonitor)
{
    ui->setupUi(this);

    doRepeatTimer = new QTimer;
    doRepeatTimer->start(20000);
    connect(doRepeatTimer, SIGNAL(timeout()), this, SLOT(doRepeatWork()));
}

SohMonitor::~SohMonitor()
{
    delete ui;
}

void SohMonitor::setup(_STATION sta)
{
    setWindowTitle("<" + sta.net + "/" + sta.sta + "> " + tr("State of health"));
    ui->titleLB->setText(sta.sta);
    ui->staLB->setText(sta.sta);
    ui->chanLB->setText(sta.chan);
    ui->netLB->setText(sta.net);
    ui->locLB->setText(sta.loc);
    ui->latLB->setText(QString::number(sta.lat, 'f', 6));
    ui->lonLB->setText(QString::number(sta.lon, 'f', 6));
    ui->elevLB->setText(QString::number(sta.elev, 'f', 1));
}

void SohMonitor::update(_KGOnSite_SOH_t soh)
{
    QDateTime ldtUTC, ldtKST, lstUTC, lstKST;
    ldtUTC.setTimeSpec(Qt::UTC);
    ldtUTC.setTime_t(soh.dtime);
    ldtKST = convertKST(ldtUTC);
    lstUTC = QDateTime::currentDateTimeUtc();
    lstKST = convertKST(lstUTC);

    ui->lastDataTimeLB->setText(ldtKST.toString("yyyy-MM-dd hh:mm:ss"));
    ui->lastSohTimeLB->setText(lstKST.toString("yyyy-MM-dd hh:mm:ss"));
}

void SohMonitor::doRepeatWork()
{
    QDateTime nowUTC = QDateTime::currentDateTimeUtc();
    QDateTime nowKST;
    nowKST = convertKST(nowUTC);

    QDateTime ldt, lst;
    ldt.setTimeSpec(Qt::UTC);
    lst.setTimeSpec(Qt::UTC);
    ldt = QDateTime::fromString(ui->lastDataTimeLB->text(), "yyyy-MM-dd hh:mm:ss");
    lst = QDateTime::fromString(ui->lastSohTimeLB->text(), "yyyy-MM-dd hh:mm:ss");

    QString ldt_style, lst_style, title_style;
    int ldt_status = 0, lst_status = 0, totalStatus = 0;
    int ldt_diff, lst_diff;

    ldt_diff = nowUTC.toTime_t() - ldt.toTime_t();
    lst_diff = nowUTC.toTime_t() - lst.toTime_t();

    if(ldt_diff <= sohValue[1])
    {
        ldt_style = "background-color: " + sohColor[0].name();
        ldt_status = 0;
    }
    else if(ldt_diff > sohValue[1] && ldt_diff <= sohValue[2])
    {
        ldt_style = "background-color: " + sohColor[1].name();
        ldt_status = 1;
    }
    else if(ldt_diff > sohValue[2])
    {
        ldt_style = "background-color: " + sohColor[2].name();
        ldt_status = 2;
    }

    if(lst_diff <= sohValue[1])
    {
        lst_style = "background-color: " + sohColor[0].name();
        lst_status = 0;
    }
    else if(lst_diff > sohValue[1] && lst_diff <= sohValue[2])
    {
        lst_style = "background-color: " + sohColor[1].name();
        lst_status = 1;
    }
    else if(lst_diff > sohValue[2])
    {
        lst_style = "background-color: " + sohColor[2].name();
        lst_status = 2;
    }

    ui->lastDataTimeLB->setStyleSheet(ldt_style);
    ui->lastSohTimeLB->setStyleSheet(lst_style);

    if(ldt_status == 0 && lst_status == 0)
        totalStatus = 0;
    else if(ldt_status == 2 || lst_status == 2)
        totalStatus = 2;
    else
        totalStatus = 1;

    title_style = "background-color: " + sohColor[totalStatus].name();

    ui->titleLB->setStyleSheet(title_style);

    emit sendSOHtoMainWindow(totalStatus, ui->staLB->text(), ui->netLB->text());
}
