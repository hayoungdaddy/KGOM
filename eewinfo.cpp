#include "eewinfo.h"
#include "ui_eewinfo.h"

EewInfo::EewInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EewInfo)
{
    ui->setupUi(this);

    topF = new QFont("Ubuntu", 12, QFont::Bold);
    middleF = new QFont("Ubuntu", 11, QFont::Bold);
    bottomF = new QFont("Ubuntu", 11, QFont::Normal);
}

EewInfo::~EewInfo()
{
    delete ui;
}

void EewInfo::setup(_EEWInfo eewInfo)
{
    QDateTime t; t.setTime_t(eewInfo.origin_time);
    t.setTimeSpec(Qt::UTC);
    t = convertKST(t);
    ui->timeLB->setText(t.toString("yyyy-MM-dd hh:mm:ss"));

    ui->magLB->setText("MAGNITUDE\n" + QString::number(eewInfo.magnitude, 'f', 2));
    ui->latLB->setText("LATITUDE\n" + QString::number(eewInfo.latitude, 'f', 4));
    ui->longLB->setText("LONGITUDE\n" + QString::number(eewInfo.longitude, 'f', 4));
}
