#include "eventlist.h"
#include "ui_eventlist.h"

#include <QPainter>

EventList::EventList(int mode, _EEWInfo eewInfo, QString hd, QString homeDir, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventList)
{
    ui->setupUi(this);

    ui->idLB->setText("ID : " + QString::number(eewInfo.evid) + "  ");
    ui->hdLB->setText(hd);

    ui->latLB->hide();
    ui->lonLB->hide();
    ui->latLB->setText(QString::number(eewInfo.latitude, 'f', 4));
    ui->lonLB->setText(QString::number(eewInfo.longitude, 'f', 4));

    codec = QTextCodec::codecForName("utf-8");

    QColor magColor = getMagColor(eewInfo.magnitude);

    QPixmap pm(80,80);
    pm.fill();
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    if(mode == 0)
    {
        p.setPen(QPen(magColor));
        p.setBrush(QBrush(magColor));
        p.drawEllipse(10, 10, 60, 60);

        if(eewInfo.magnitude >= 4.5) p.setPen(QPen(Qt::white));
        else p.setPen(QPen(Qt::black));

        p.setFont(QFont("Open Sans", 11, QFont::Bold));
        p.drawText(pm.rect(), Qt::AlignCenter, "M " + QString::number(eewInfo.magnitude, 'f', 1));
    }
    else if(mode == 1)
    {
        p.setPen(QPen(Qt::white));
        p.setBrush(QBrush(Qt::red));
        p.drawEllipse(10, 10, 60, 60);
        p.setFont(QFont("Open Sans", 11, QFont::Bold));
        p.drawText(pm.rect(), Qt::AlignCenter, QString::number(eewInfo.magnitude, 'f', 1) + " gal");
    }

    ui->magLB->setPixmap(pm);

    QDateTime etUTC;
    QDateTime etKST;
    etUTC.setTimeSpec(Qt::UTC);
    etUTC.setTime_t(eewInfo.origin_time);
    etKST = convertKST(etUTC);

    ui->timeLB->setText(etKST.toString("yyyy-MM-dd hh:mm:ss (KST)"));

    QProcess process;
    QString cmd = homeDir + "/bin/" + find_loc_program + " " + QString::number(eewInfo.latitude, 'f', 4) + " " + QString::number(eewInfo.longitude, 'f', 4);
    process.start(cmd);
    process.waitForFinished(-1); // will wait forever until finished

    QString stdout = process.readAllStandardOutput();

    ui->locLB->setText(stdout);

    if(mode == 1)
    {
        ui->locLB->setText(codec->toUnicode("최대지반가속도에 의한 이벤트 감지"));
        ui->hdLB->setText("");
    }
}

EventList::~EventList()
{
    delete ui;
}

void EventList::mousePressEvent(QMouseEvent* event)
{
    emit clicked(ui->idLB->text().section(":",1,1).simplified(), ui->latLB->text(), ui->lonLB->text());
}


void EventList::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked(ui->idLB->text());
}
