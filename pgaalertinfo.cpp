#include "pgaalertinfo.h"
#include "ui_pgaalertinfo.h"

PgaAlertInfo::PgaAlertInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PgaAlertInfo)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");
}

PgaAlertInfo::~PgaAlertInfo()
{
    delete ui;
}

void PgaAlertInfo::setup(_KGKIIS_GMPEAK_EVENT_STA_t staPGA)
{
    QString pgaS = QString::number(staPGA.maxH, 'f', 4);

    ui->scnlLB->setText(QString(staPGA.net) + "/" + QString(staPGA.sta));
    ui->pgaLB->setText(pgaS + " gal");

    QDateTime tUTC, tKST;
    tUTC.setTimeSpec(Qt::UTC);
    tUTC.setTime_t(staPGA.time);

    tKST = convertKST(tUTC);
    ui->timeLB->setText(tKST.toString("hh:mm:ss"));

    int legendIndex = getLegendIndex(staPGA.maxH);

    QString styleSheet;
    styleSheet = "background-color: " + pgaColor[legendIndex].name() + ";color: black;";
    if(legendIndex >= 7)
        styleSheet = "background-color: " + pgaColor[legendIndex].name() + ";color: white;";

    ui->intenLB->setText(codec->toUnicode("진도") + "\n" + intenText[legendIndex]);
    ui->intenLB->setStyleSheet(styleSheet);
    ui->pgaLB->setStyleSheet(styleSheet);
}
