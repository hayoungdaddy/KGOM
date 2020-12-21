#include "pgainfo.h"
#include "ui_pgainfo.h"

PgaInfo::PgaInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PgaInfo)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");
}

PgaInfo::~PgaInfo()
{
    delete ui;
}

void PgaInfo::setup(QString chan, int eTime, QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaInfos)
{
    for(int i=0;i<pgaInfos.count();i++)
    {
        QString pgaS;
        if(chan.startsWith("Z")) pgaS = QString::number(pgaInfos.at(i).maxZ, 'f', 4);
        else if(chan.startsWith("N")) pgaS = QString::number(pgaInfos.at(i).maxN, 'f', 4);
        else if(chan.startsWith("E")) pgaS = QString::number(pgaInfos.at(i).maxE, 'f', 4);
        else if(chan.startsWith("H")) pgaS = QString::number(pgaInfos.at(i).maxH, 'f', 4);
        else if(chan.startsWith("T")) pgaS = QString::number(pgaInfos.at(i).maxT, 'f', 4);

        QLabel *staNameLB = new QLabel;
        QLabel *pgaLB = new QLabel;
        QLabel *timeLB = new QLabel;
        //staNameLB->setText(QString(pgaInfos.at(i).sta)+"/"+QString(pgaInfos.at(i).chan)+"/"+QString(pgaInfos.at(i).net)+"/"+QString(pgaInfos.at(i).loc));
        staNameLB->setText(QString(pgaInfos.at(i).sta));
        staNameLB->setAlignment(Qt::AlignCenter);
        pgaLB->setText(pgaS + " gal");
        pgaLB->setAlignment(Qt::AlignCenter);
        QDateTime tUTC, tKST;
        tUTC.setTimeSpec(Qt::UTC);
        tUTC.setTime_t(pgaInfos.at(i).time);

        tKST = convertKST(tUTC);
        timeLB->setText(tKST.toString("hh:mm:ss"));
        timeLB->setAlignment(Qt::AlignCenter);
        ui->bodyLO->addWidget(staNameLB, i, 0);
        ui->bodyLO->addWidget(pgaLB, i, 1);
        ui->bodyLO->addWidget(timeLB, i, 2);
    }

    QDateTime tUTC, tKST;
    tUTC.setTimeSpec(Qt::UTC);
    tUTC.setTime_t(eTime);

    tKST = convertKST(tUTC);
    ui->timeLB->setText(tKST.toString("yyyy-MM-dd hh:mm:ss"));
    ui->scnlLB->setText(codec->toUnicode("최대지반가속도(") + chan + ") 정보");
}
