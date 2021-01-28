#include "onsiteinfo.h"
#include "ui_onsiteinfo.h"

OnsiteInfo::OnsiteInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnsiteInfo)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");
}

OnsiteInfo::~OnsiteInfo()
{
    delete ui;
}

void OnsiteInfo::setup(_KGOnSite_Info_t info)
{
    QDateTime t; t.setTime_t(info.ttime);
    t.setTimeSpec(Qt::UTC);
    t = convertKST(t);
    ui->timeLB->setText(t.toString("yyyy-MM-dd hh:mm:ss"));
    ui->scnlLB->setText(QString(info.sta));

    setIntenWG(info.intensity);
    setAlertWG(info.alert);
    setMagWG(info.magnitude);
    setDistWG(info.distance);
    //setType(QString(info.net));

    if(QString(info.fromWhere).startsWith("K"))
        ui->scnlLB->setText(codec->toUnicode("현장경보 관측소 - ") + ui->scnlLB->text() + "(KISS)");
    else
        ui->scnlLB->setText(codec->toUnicode("현장경보 관측소 - ") + ui->scnlLB->text() + "(LOCAL)");
}

void OnsiteInfo::setType(QString net)
{
    if(net.startsWith("KG") || net.startsWith("KS"))
        ui->scnlLB->setText(codec->toUnicode("현장경보 관측소 - ") + ui->scnlLB->text() + "(KISS)");
    else
        ui->scnlLB->setText(codec->toUnicode("현장경보 관측소 - ") + ui->scnlLB->text() + "(LOCAL)");
}

void OnsiteInfo::setIntenWG(double inten)
{
    ui->intenPB->setValue(getIntenText(inten).toInt());
    QString style;

    style = "            QProgressBar:horizontal { border: 1px solid black;"
            "            border-radius: 3px;"
            "            background: white;"
            "            padding: 1px;"
            "            text-align: center;"
            "            }"
            "            QProgressBar::chunk:horizontal {"
            "            background: " + getIntenColor(inten).name() + ";"

            "            }";

    ui->intenPB->setStyleSheet(style);
}

void OnsiteInfo::setAlertWG(int alert)
{
    QPixmap myPix( QSize(ui->alertLB->width(), ui->alertLB->height()) );
    myPix.fill( Qt::white );

    QPainter painter(&myPix);
    painter.setBrush( QColor(alertColor(alert)) );
    painter.drawEllipse(QPoint(ui->alertLB->width() / 2, ui->alertLB->height() / 2),
                        ui->alertLB->width() / 2 - 5, ui->alertLB->height() / 2 - 5);

    ui->alertLB->setPixmap(myPix);

    //ui->alertLB->setText("ALERT\n" + alertText(alert));
    ui->alertLB->setText(codec->toUnicode("경보\n") + alertText(alert));
    QString style = "color:white; background-color:" + alertColor(alert);
    ui->alertLB->setStyleSheet(style);
}

void OnsiteInfo::setMagWG(double mag)
{
    ui->magLB->setText(getMagText(mag));
    QColor color = getMagColor(mag);
    QString style;
    if(mag < 4.5)
        style = "color: black; background-color:" + color.name();
    else
        style = "color: white; background-color:" + color.name();
    ui->magLB->setStyleSheet(style);
}

void OnsiteInfo::setDistWG(double dist)
{
    ui->distLB->setText(distText(dist));
    QString style = "background-color:" + distColor(dist);
    ui->distLB->setStyleSheet(style);
}
