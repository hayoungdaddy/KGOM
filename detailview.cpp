#include "detailview.h"
#include "ui_detailview.h"

#include <QPainter>

DetailView::DetailView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailView)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");
}

DetailView::~DetailView()
{
    delete ui;
}

void DetailView::setup(QVector<_KGOnSite_Info_t> onsiteInfos, QVector<_EEWInfo> eewInfos,
                       QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaDetectInfos, int pgaTime, QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaInfos, QString evid, QString homeDir)
{
    ui->titleLB->setText("EVENT ID : " + evid);

    _EEWInfo summaryInfo;

    // set summary
    if(!eewInfos.isEmpty())
        summaryInfo = eewInfos.first();
    else
    {
        int maxDuration = 0;
        for(int i=0;i<onsiteInfos.count();i++)
        {
            if(QString(onsiteInfos.at(i).duration).toInt() > maxDuration)
                maxDuration = QString(onsiteInfos.at(i).duration).toInt();
        }

        for(int i=0;i<onsiteInfos.count();i++)
        {
            if(QString(onsiteInfos.at(i).duration).toInt() == maxDuration)
            {
                summaryInfo.origin_time = onsiteInfos.at(i).ttime;
                summaryInfo.latitude = onsiteInfos.at(i).lat;
                summaryInfo.longitude = onsiteInfos.at(i).lon;
                summaryInfo.magnitude = onsiteInfos.at(i).magnitude;
                break;
            }
        }
    }

    QDateTime dtUTC, dtKST;
    dtUTC.setTimeSpec(Qt::UTC);
    dtUTC.setTime_t(summaryInfo.origin_time);
    dtKST = convertKST(dtUTC);
    ui->timeLB->setText(dtKST.toString("yyyy-MM-dd hh:mm:ss"));
    QProcess process;
    QString cmd = homeDir + "/bin/" + find_loc_program + " " + QString::number(summaryInfo.latitude, 'f', 4) + " " + QString::number(summaryInfo.longitude, 'f', 4);
    process.start(cmd);
    process.waitForFinished(-1); // will wait forever until finished
    QString stdout = process.readAllStandardOutput();
    ui->locLB->setText(stdout.section("\n", 0, 0));
    ui->magLB->setText(QString::number(summaryInfo.magnitude, 'f', 1));
    ui->magNameLB->setText("규모:");
    ui->mag1LB->show();
    ui->mag2LB->show();
    ui->mag3LB->show();
    ui->mag4LB->show();

    QPixmap pm(70, 70);
    pm.fill();
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setFont(QFont("Open Sans", 10, QFont::Bold));
    if(summaryInfo.magnitude < magValue[1])
    {
        p.setPen(QPen(getMagColor(2)));
        p.setBrush(QBrush(getMagColor(2)));
    }
    else
    {
        p.setPen(QPen(Qt::black));
        p.setBrush(QBrush(Qt::white));
    }
    p.drawEllipse(5, 5, 60, 60);
    p.setPen(QPen(Qt::black));
    p.drawText(pm.rect(), Qt::AlignCenter, "M<3");
    ui->mag1LB->setPixmap(pm);
    if(magValue[1] <= summaryInfo.magnitude && summaryInfo.magnitude < magValue[2])
    {
        p.setPen(QPen(getMagColor(4)));
        p.setBrush(QBrush(getMagColor(4)));
    }
    else
    {
        p.setPen(QPen(Qt::black));
        p.setBrush(QBrush(Qt::white));
    }
    p.drawEllipse(5, 5, 60, 60);
    p.setPen(QPen(Qt::black));
    p.drawText(pm.rect(), Qt::AlignCenter, "3≤M<4.5");
    ui->mag2LB->setPixmap(pm);
    if(magValue[2] <= summaryInfo.magnitude && summaryInfo.magnitude < magValue[3])
    {
        p.setPen(QPen(getMagColor(5)));
        p.setBrush(QBrush(getMagColor(5)));
    }
    else
    {
        p.setPen(QPen(Qt::black));
        p.setBrush(QBrush(Qt::white));
    }
    p.drawEllipse(5, 5, 60, 60);
    if(magValue[2] <= summaryInfo.magnitude && summaryInfo.magnitude < magValue[3])
    {
            p.setPen(QPen(Qt::white));
    }
    else
    {
            p.setPen(QPen(Qt::black));
    }

    p.drawText(pm.rect(), Qt::AlignCenter, "4.5≤M<6");
    ui->mag3LB->setPixmap(pm);
    if(summaryInfo.magnitude >= magValue[3])
    {
        p.setPen(QPen(getMagColor(6)));
        p.setBrush(QBrush(getMagColor(6)));
    }
    else
    {
        p.setPen(QPen(Qt::black));
        p.setBrush(QBrush(Qt::white));
    }
    p.drawEllipse(5, 5, 60, 60);
    if(summaryInfo.magnitude >= magValue[3])
    {
            p.setPen(QPen(Qt::white));
    }
    else
    {
            p.setPen(QPen(Qt::black));
    }
    p.drawText(pm.rect(), Qt::AlignCenter, "M≥6");
    ui->mag4LB->setPixmap(pm);

    if(eewInfos.isEmpty() && onsiteInfos.isEmpty())
    {
        QDateTime dtUTC, dtKST;
        dtUTC.setTimeSpec(Qt::UTC);
        dtUTC.setTime_t(pgaInfos.at(0).time);
        dtKST = convertKST(dtUTC);
        ui->timeLB->setText(dtKST.toString("yyyy-MM-dd hh:mm:ss"));

        ui->locLB->setText(codec->toUnicode("최대지반가속도에 의한 이벤트 감지"));
        float maxPGA = 0;
        for(int i=0;i<pgaDetectInfos.size();i++)
        {
            if(pgaDetectInfos.at(i).maxH >= maxPGA)
                maxPGA = pgaDetectInfos.at(i).maxH;
        }
        QString pgaS = QString::number(maxPGA, 'f', 1);
        ui->magLB->setText(pgaS);
        ui->magNameLB->setText(codec->toUnicode("최대지반가속도:"));
        ui->mag1LB->hide();
        ui->mag2LB->hide();
        ui->mag3LB->hide();
        ui->mag4LB->hide();
    }

    QLayoutItem *child;

    while ((child = ui->detailLO->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    int count = 0;

    if(!eewInfos.isEmpty())
    {
        count ++;
        EewDetailTable *eewdetailtable = new EewDetailTable;
        //eewdetailtable->setup(QString::number(count) + ". Network Earthquake Early Warning.", eewInfos);
        eewdetailtable->setup(QString::number(count) + codec->toUnicode(". 지진관측망 조기경보"), eewInfos);
        ui->detailLO->addWidget(eewdetailtable);

        QSpacerItem *spacer = new QSpacerItem(10,20, QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->detailLO->addSpacerItem(spacer);
    }

    if(!onsiteInfos.isEmpty())
    {
        while(!onsiteInfos.isEmpty())
        {
            count ++;
            QVector<_KGOnSite_Info_t> tempInfos;
            tempInfos.push_back(onsiteInfos.at(0));
            QVector<int> deleteIndex;
            for(int i=0;i<onsiteInfos.count();i++)
            {
                if(QString(tempInfos.at(0).sta).startsWith(QString(onsiteInfos.at(i).sta)) && tempInfos.at(0).ttime == onsiteInfos.at(i).ttime &&
                        QString(tempInfos.at(0).net).startsWith(QString(onsiteInfos.at(i).net)) && !QString(tempInfos.at(0).duration).startsWith(QString(onsiteInfos.at(i).duration)))
                {
                    tempInfos.push_back(onsiteInfos.at(i));
                    deleteIndex.push_back(i);
                }
            }
            OnsiteDetailTable *onsitedetailtable = new OnsiteDetailTable;
            onsitedetailtable->setup(QString::number(count) + ". 현장경보", tempInfos);
            ui->detailLO->addWidget(onsitedetailtable);

            onsiteInfos.remove(0);
            for(int i=0;i<deleteIndex.count();i++)
                onsiteInfos.remove(deleteIndex.at(i)-1-i);
        }

        QSpacerItem *spacer = new QSpacerItem(10,20, QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->detailLO->addSpacerItem(spacer);
    }

    if(!pgaDetectInfos.isEmpty())
    {
        count ++;
        PgaDetectDetailTable *pgadetectdetailtable = new PgaDetectDetailTable;
        pgadetectdetailtable->setup(QString::number(count) + codec->toUnicode(". 최대지반가속도(최초값) 정보"), pgaDetectInfos);
        ui->detailLO->addWidget(pgadetectdetailtable);
    }

    if(!pgaInfos.isEmpty())
    {
        count ++;
        PgaDetailTable *pgadetailtable = new PgaDetailTable;
        double evlat = 0, evlon = 0;
        if(eewInfos.count() != 0)
        {
            evlat = eewInfos.first().latitude;
            evlon = eewInfos.first().longitude;
        }

        pgadetailtable->setup(QString::number(count) + codec->toUnicode(". 최대지반가속도(최대값) 정보"), pgaTime, pgaInfos, evid, homeDir, evlat, evlon);
        ui->detailLO->addWidget(pgadetailtable);
    }

    ui->detailLO->addStretch(1);
}
