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

void PgaInfo::setup(int eTime, QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaInfos)
{
    QLabel *titleLB[4];
    for(int i=0;i<4;i++)
    {
        titleLB[i] = new QLabel;
        if(i == 0) titleLB[i]->setText("Station");
        else if(i == 1) titleLB[i]->setText("PGA(gal)");
        else if(i == 2) titleLB[i]->setText(codec->toUnicode("진도"));
        else if(i == 3) titleLB[i]->setText("Time(KST)");
        titleLB[i]->setAlignment(Qt::AlignCenter);
        titleLB[i]->setFont(QFont("Open Sans", 10, QFont::Bold));
        titleLB[i]->setStyleSheet("border: 1px solid black;");
        ui->bodyLO->addWidget(titleLB[i], 0, i);
    }
    for(int i=0;i<pgaInfos.count();i++)
    {
        QString pgaS;
        int legendIndex;
        QString styleSheet;
        pgaS = QString::number(pgaInfos.at(i).maxH, 'f', 4);

        QFuture<int> future = QtConcurrent::run(getLegendIndex, pgaInfos.at(i).maxH);
        future.waitForFinished();
        legendIndex = future.result();

        styleSheet = "background-color: " + pgaColor[legendIndex].name() + ";color: black;";
        if(legendIndex >= 7)
            styleSheet = "background-color: " + pgaColor[legendIndex].name() + ";color: white;";

        QLabel *staNameLB = new QLabel;
        QLabel *pgaLB = new QLabel;
        QLabel *intenLB = new QLabel;
        QLabel *timeLB = new QLabel;

        staNameLB->setText(QString(pgaInfos.at(i).net) + "/" + QString(pgaInfos.at(i).sta));
        staNameLB->setAlignment(Qt::AlignCenter);

        pgaLB->setText(pgaS + " gal");
        pgaLB->setAlignment(Qt::AlignCenter);

        intenLB->setText(intenText[legendIndex]);
        intenLB->setAlignment(Qt::AlignCenter);
        intenLB->setStyleSheet(styleSheet);

        QDateTime tUTC, tKST;
        tUTC.setTimeSpec(Qt::UTC);
        tUTC.setTime_t(pgaInfos.at(i).time);
        tKST = convertKST(tUTC);
        timeLB->setText(tKST.toString("hh:mm:ss"));
        timeLB->setAlignment(Qt::AlignCenter);

        ui->bodyLO->addWidget(staNameLB, i+1, 0);
        ui->bodyLO->addWidget(pgaLB, i+1, 1);
        ui->bodyLO->addWidget(intenLB, i+1, 2);
        ui->bodyLO->addWidget(timeLB, i+1, 3);
    }

    QDateTime tUTC, tKST;
    tUTC.setTimeSpec(Qt::UTC);
    tUTC.setTime_t(eTime);

    tKST = convertKST(tUTC);
    ui->timeLB->setText(tKST.toString("yyyy-MM-dd hh:mm:ss"));
}
