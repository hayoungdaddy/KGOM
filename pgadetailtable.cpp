#include "pgadetailtable.h"
#include "ui_pgadetailtable.h"

PgaDetailTable::PgaDetailTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PgaDetailTable)
{
    ui->setupUi(this);
}

PgaDetailTable::~PgaDetailTable()
{
    delete ui;
}

void PgaDetailTable::setup(QString title, int eTime, QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaInfos, QString evid, QString homeDir, double evLat, double evLon)
{
    ui->titleLB->setText(title);
    ui->pgaTW->setRowCount(0);

    for(int i=0;i<pgaInfos.count();i++)
    {
        ui->pgaTW->setRowCount(ui->pgaTW->rowCount()+1);
        ui->pgaTW->setItem(i, 1, new QTableWidgetItem(QString(pgaInfos.at(i).sta)));
        ui->pgaTW->setItem(i, 2, new QTableWidgetItem(QString(pgaInfos.at(i).chan)));
        ui->pgaTW->setItem(i, 0, new QTableWidgetItem(QString(pgaInfos.at(i).net)));
        ui->pgaTW->setItem(i, 3, new QTableWidgetItem(QString::number(pgaInfos.at(i).lat, 'f', 4)));
        ui->pgaTW->setItem(i, 4, new QTableWidgetItem(QString::number(pgaInfos.at(i).lon, 'f', 4)));
        QDateTime timeUTC, timeKST;
        timeUTC.setTimeSpec(Qt::UTC);
        timeUTC.setTime_t(pgaInfos.at(i).time);
        timeKST = convertKST(timeUTC);
        ui->pgaTW->setItem(i, 5, new QTableWidgetItem(timeKST.toString("hh:mm:ss")));
        ui->pgaTW->setItem(i, 6, new QTableWidgetItem(QString::number(pgaInfos.at(i).maxH, 'f', 4)));
    }

    for(int i=0;i<ui->pgaTW->rowCount();i++)
    {
        for(int j=0;j<ui->pgaTW->columnCount();j++)
        {
            if(ui->pgaTW->item(i, j)->text() != "") ui->pgaTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }

    ui->pgaTW->setFixedHeight(30*pgaInfos.count() + 23);

    QString path;
    QDateTime evTime;
    evTime.setTime_t(eTime);
    path = homeDir + "/data/PGA/" + evTime.toString("yyyy/MM/") + evid + "/";
    QDir pgaFilePathD(path);
    if(!pgaFilePathD.exists())
    {
        ui->chartFrame->hide();
    }
    else
    {
        ui->chartFrame->show();
        double maxPGA = 0;
        for(int i=0;i<pgaInfos.count();i++)
        {
            if(maxPGA < pgaInfos.at(i).maxH)
                maxPGA = pgaInfos.at(i).maxH;
        }

        for(int i=0;i<pgaInfos.count();i++)
        {
            drawPGAChart(path, pgaInfos.at(i).net, pgaInfos.at(i).sta, 0, maxPGA);
        }
        /*
        if(evLat == 0 && evLon == 0)
        {
            for(int i=0;i<pgaInfos.count();i++)
            {
                drawPGAChart(path, pgaInfos.at(i).net, pgaInfos.at(i).sta, 0, maxPGA);
            }
        }
        else
        {
            QVector<double> dist;
            QVector<double> sortedDist;
            QVector<int> sortedIndex;

            for(int i=0;i<pgaInfos.count();i++)
            {
                dist.push_back(getDistance(pgaInfos.at(i).lat, pgaInfos.at(i).lon, evLat, evLon));
            }

            for(int i=0;i<dist.count();i++)
            {
                if(sortedDist.isEmpty())
                {
                    sortedDist.push_back(dist.at(i));
                    sortedIndex.push_back(i);
                }
                else
                {
                    int commit = 0;
                    for(int j=0;j<sortedDist.count();j++)
                    {
                        if(sortedDist.at(j) > dist.at(i))
                        {
                            sortedDist.insert(j, dist.at(i));
                            sortedIndex.insert(j, i);
                            commit = 1;
                            break;
                        }
                    }
                    if(commit == 0)
                    {
                        sortedDist.push_back(dist.at(i));
                        sortedIndex.push_back(i);
                    }
                }
            }

            for(int i=0;i<pgaInfos.count();i++)
            {
                drawPGAChart(path, pgaInfos.at(sortedIndex.at(i)).net, pgaInfos.at(sortedIndex.at(i)).sta, sortedDist.at(i), maxPGA);
            }
        }
        */

        ui->chartFrame->setMinimumHeight(300*pgaInfos.count());
    }
}

void PgaDetailTable::drawPGAChart(QString path, QString net, QString sta, double dist, double maxPGA)
{
    QFont *smallF = new QFont("Ubuntu", 8, QFont::Normal);

    QChartView *pgaChartView = new QChartView();
    QChart *pgaChart = new QChart();
    QLineSeries *pgaSeries = new QLineSeries();

    if(dist == 0)
        pgaSeries->setName(net + "/" + sta);
    else
        pgaSeries->setName(net + "/" + sta + "(" + QString::number(dist, 'f', 2) + "Km)");

    QDateTime local(QDateTime::currentDateTime());
    QDateTime UTC(QDateTime::currentDateTimeUtc());
    QDateTime dt(UTC.date(), UTC.time(), Qt::LocalTime);

    QFile pgaFile(path + "/" + net + "_" + sta);
    if(!pgaFile.exists())
        pgaFile.setFileName(path + "/" + net + sta + "_" + sta);

    if(pgaFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&pgaFile);
        QString line, _line;

        QDateTime dataTimeUTC;
        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            dataTimeUTC.setTime_t(_line.section(" ", 0, 0).toInt());
            qint64 ttt = dataTimeUTC.toMSecsSinceEpoch();

            if(dt.secsTo(local) == 0) // if the system timezone is UTC
            {
                ttt = ttt + (3600000 * 9);
            }
            pgaSeries->append(ttt, _line.section(" ", 1, 1).toDouble());
        }
        pgaFile.close();
    }

    pgaChart->addSeries(pgaSeries);
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("hh:mm:ss");
    //axisX->setTitleText("Time");
    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%.4f");
    axisY->setTitleText("Horizontal PGA");
    //axisX->setLabelsFont(*smallF);
    //axisY->setLabelsFont(*smallF);
    axisY->setRange(0, maxPGA);
    pgaChart->setAxisX(axisX, pgaSeries);
    pgaChart->setAxisY(axisY, pgaSeries);
    pgaChartView->setRenderHint(QPainter::Antialiasing);
    pgaChartView->setChart(pgaChart);
    //pgaChartView->setMinimumHeight(400);

    ui->chartLO->addWidget(pgaChartView);
}
