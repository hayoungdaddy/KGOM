#include "eewinfo.h"
#include "ui_eewinfo.h"

#include <QPainter>

EewInfo::EewInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EewInfo)
{
    ui->setupUi(this);

    topF = new QFont("Ubuntu", 12, QFont::Bold);
    middleF = new QFont("Ubuntu", 11, QFont::Bold);
    bottomF = new QFont("Ubuntu", 11, QFont::Normal);

    codec = QTextCodec::codecForName("utf-8");
}

EewInfo::~EewInfo()
{
    delete ui;
}

void EewInfo::setup(_EEWInfo eewInfo, QString hd, double myLat, double myLon)
{
    QDateTime t; t.setTime_t(eewInfo.origin_time);
    t.setTimeSpec(Qt::UTC);
    t = convertKST(t);
    ui->timeLB->setText(t.toString("yyyy-MM-dd hh:mm:ss"));

    /*
    ui->latLB->setText("Lat.\n" + QString::number(eewInfo.latitude, 'f', 4));
    ui->longLB->setText("Lon.\n" + QString::number(eewInfo.longitude, 'f', 4));
    ui->hdLB->setText("Home Dist.\n" + hd + " Km");
    */
    ui->latLB->setText(QString::number(eewInfo.latitude, 'f', 4));
    ui->lonLB->setText(QString::number(eewInfo.longitude, 'f', 4));
    ui->hdLB->setText(hd + " Km");

    double dist = getDistance(eewInfo.latitude, eewInfo.longitude, myLat, myLon);

    ui->intenLB->setText(QString::number(getIntenFromMag(eewInfo.magnitude, dist)));
    QColor intenColor = getIntenColor(getIntenFromMag(eewInfo.magnitude, dist));
    ui->intenLB->setStyleSheet("background-color:" + intenColor.name() + ";");

    QColor magColor = getMagColor(eewInfo.magnitude);

    QPixmap pm(60,60);
    pm.fill();
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(magColor));
    p.setBrush(QBrush(magColor));
    p.drawEllipse(5, 5, 50, 50);

    if(eewInfo.magnitude >= 4.5) p.setPen(QPen(Qt::white));
    else p.setPen(QPen(Qt::black));

    p.setFont(QFont("Open Sans", 11, QFont::Bold));
    p.drawText(pm.rect(), Qt::AlignCenter, "M " + QString::number(eewInfo.magnitude, 'f', 1));
    ui->magLB->setPixmap(pm);
}
