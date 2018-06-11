#include "listinfo.h"
#include "ui_listinfo.h"

ListInfo::ListInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListInfo)
{
    ui->setupUi(this);

    topF = new QFont("Ubuntu", 12, QFont::Bold);
    middleF = new QFont("Ubuntu", 11, QFont::Bold);
    bottomF = new QFont("Ubuntu", 11, QFont::Normal);
}

ListInfo::~ListInfo()
{
    delete ui;
}

void ListInfo::setup(_KGOnSite_Pick_t pick, _KGOnSite_Info_t info)
{
    QDateTime t; t.setTime_t(pick.ttime);
    t.setTimeSpec(Qt::UTC);
    t = convertKST(t);
    ui->timeLB->setText(t.toString("yyyy-MM-dd hh:mm:ss"));

    QString scnl;
    scnl = QString::fromStdString(pick.sta) + "/" + QString::fromStdString(pick.chan) + "/" +
            QString::fromStdString(pick.net) + "/" + QString::fromStdString(pick.loc);
    ui->scnlLB->setText(QString::fromStdString(pick.sta));

    setIntenWG(info.intensity);
    setAlertWG(info.alert);
    setMagWG(info.magnitude);
    setDistWG(info.distance);
    setType(QString::fromStdString(pick.net));
}

void ListInfo::setType(QString net)
{
    if(net.startsWith("KG") || net.startsWith("KS"))
    {
        ui->typeLB->setText(tr("K"));
        ui->typeLB->setStyleSheet("background-color: green; color: white");
    }
    else
    {
        ui->typeLB->setText(tr("L"));
        ui->typeLB->setStyleSheet("background-color: rgb(114, 159, 207); color: white");
    }
}

void ListInfo::setIntenWG(double inten)
{
    /*
    ui->intenLB->setText(intenText(inten));
    QString style = "background-color:" + intenColor(inten);
    ui->intenLB->setStyleSheet(style);
    */
    ui->intenPB->setValue(intenText(inten).toInt());
    QString style = "QProgressBar::chunk { background:" + intenColor(inten) + ";}";
    //qDebug() << style;
    //ui->intenPB->setStyleSheet(style);

    style = "            QProgressBar:horizontal { border: 1px solid black;"
            "            border-radius: 3px;"
            "            background: white;"
            "            padding: 1px;"
            "            text-align: center;"

            "            }"
            "            QProgressBar::chunk:horizontal {"
            "            background: " + intenColor(inten) + ";"

            "            }";

    //qDebug() << style;
    ui->intenPB->setStyleSheet(style);
}

void ListInfo::setAlertWG(int alert)
{
    QPixmap myPix( QSize(ui->alertLB->width(), ui->alertLB->height()) );
    myPix.fill( Qt::white );

    QPainter painter(&myPix);
    painter.setBrush( QColor(alertColor(alert)) );
    painter.drawEllipse(QPoint(ui->alertLB->width() / 2, ui->alertLB->height() / 2),
                        ui->alertLB->width() / 2 - 5, ui->alertLB->height() / 2 - 5);

    ui->alertLB->setPixmap(myPix);

    ui->alertLB->setText("ALERT\n" + alertText(alert));
    QString style = "color:white; background-color:" + alertColor(alert);
    ui->alertLB->setStyleSheet(style);
}

void ListInfo::setMagWG(double mag)
{
    ui->magLB->setText(magText(mag));
    QString style = "background-color:" + magColor(mag);
    ui->magLB->setStyleSheet(style);
    //ui->magPB->setValue(magText(mag).toInt() + 1);
}

void ListInfo::setDistWG(double dist)
{
    ui->distLB->setText(distText(dist));
    QString style = "background-color:" + distColor(dist);
    ui->distLB->setStyleSheet(style);
    //ui->distPB->setValue(distText(dist).toInt() + 1);
}
