#include "reportviewer.h"
#include "ui_reportviewer.h"

#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

ReportViewer::ReportViewer(QString evid, QVector<_REPORT> reportV, _CONFIGURE configure) :
    QDialog(),
    ui(new Ui::ReportViewer)
{
    ui->setupUi(this);

    memcpy(&con, &configure, sizeof(_CONFIGURE));

    ui->regendFrame->hide();

    // set title
    ui->titleLB->setText(tr("KGom Report") + " (Event ID : " + evid + ")");
    setSummaryGB(reportV);
    setListGB(reportV);
    setOnsiteGB(reportV);

    // Load a osm map
    QQuickView *view = new QQuickView();
    mapContainer = QWidget::createWindowContainer(view, this);
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    mapContainer->setMinimumHeight(300);
    mapContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mapContainer->setFocusPolicy(Qt::TabFocus);
    view->setSource(QUrl(QStringLiteral("qrc:/Viewmap.qml")));
    ui->mapLO->addWidget(mapContainer);
    rootObj = view->rootObject();

    QMetaObject::invokeMethod(this->rootObj, "mapReset", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, configure.KGONSITE_HOME + "/params/maps"), Q_ARG(QVariant, "osm"));

    QMetaObject::invokeMethod(this->rootObj, "clearMap", Q_RETURN_ARG(QVariant, returnedValue));

    // create station marker on map (just create, do not show)
    QMetaObject::invokeMethod(this->rootObj, "createLocalStaMarker", Q_RETURN_ARG(QVariant, returnedValue));
    QMetaObject::invokeMethod(this->rootObj, "createKissStaMarker", Q_RETURN_ARG(QVariant, returnedValue));

    int kissSta = 0;
    int localSta = 0;
    for(int i=reportV.count()-1;i>=0;i--)
    {
        if(QString(reportV.at(i).pick.net).startsWith("KG") || QString(reportV.at(i).pick.net).startsWith("KS"))
        {
            drawOnsiteOnMap(1, reportV.at(i).pick, reportV.at(i).infos.last(), kissSta);
            kissSta++;
        }
        else
        {
            drawOnsiteOnMap(0, reportV.at(i).pick, reportV.at(i).infos.last(), localSta);
            localSta++;
        }

        QString dir, dfile;
        QDateTime tt;
        tt.setTime_t( (uint) reportV.at(i).pick.ttime );
        tt.setTimeSpec(Qt::UTC);
        tt = convertKST(tt);
        dir = tt.toString("yyyy/MMdd");
        dfile = QString::number(reportV.at(i).pick.ttime,'f',0) + QString::fromStdString(reportV.at(i).pick.sta) + ".dat";
        setWaveform(configure.KGONSITE_HOME + "/data/" + dir, dfile, reportV.at(i).pick.ttime);
    }
}

ReportViewer::~ReportViewer()
{
    delete ui;
}

void ReportViewer::setSummaryGB(QVector<_REPORT>)
{


}

void ReportViewer::setListGB(QVector<_REPORT> reportV)
{
    for(int i=0;i<reportV.count();i++)
    {
        listInfo[i] = new ListInfo;
        listInfo[i]->setup(reportV.at(i).pick, reportV.at(i).infos.last());
        ui->listLO->addWidget(listInfo[i]);
    }
}

void ReportViewer::setOnsiteGB(QVector<_REPORT> reportV)
{
    for(int i=0;i<reportV.count();i++)
    {
        QLabel *titleLB = new QLabel;
        QString scnl, time, polar, weight, amp, per;
        scnl = QString(reportV.at(i).pick.sta) + "/" + QString(reportV.at(i).pick.chan) + "/" +
                QString(reportV.at(i).pick.net) + "/" + QString(reportV.at(i).pick.loc);
        QDateTime tt; tt.setTime_t(reportV.at(i).pick.ttime);
        tt.setTimeSpec(Qt::UTC);
        time = tt.toString("yyyy/MM/dd hh:mm:ss (KST)");
        polar = QString(reportV.at(i).pick.polarity);
        weight = QString::number(reportV.at(i).pick.weight);
        amp = QString::number(reportV.at(i).pick.amplitude);
        per = QString::number(reportV.at(i).pick.period, 'f', 3);
        titleLB->setText("- " +  scnl + " " + time + " Polarity:" + polar + " Weight:" + weight +
                         " Amplitude:" + amp + " Period:" + per);
        //ui->onsiteLO->addWidget(titleLB);

        if(reportV.at(i).infos.count() != 0)
        {
            QTableWidget *table = new QTableWidget(reportV.at(i).infos.count(), 10);
            table->setFixedHeight(130);
            table->verticalHeader()->hide();
            QTableWidgetItem *durHItem = new QTableWidgetItem(tr("duration"));
            table->setHorizontalHeaderItem(0, durHItem);
            // to do

            for(int j=0;j<reportV.at(i).infos.count();j++)
            {
                table->setItem(j, 0, new QTableWidgetItem(QString(reportV.at(i).infos.at(j).duration)));
                // to do
            }
            //ui->onsiteLO->addWidget(table);
        }
    }

    //ui->onsiteLO->addStretch(1);

    // draw Inten line graph
    QChart *intenChart = new QChart();
    intenChart->legend()->hide();
    intenChart->createDefaultAxes();
    intenChart->setTitle("Determined Intensity for each station");
    intenChart->layout()->setContentsMargins(0, 0, 0, 0);

    QValueAxis *intenAxisX = new QValueAxis();
    QValueAxis *intenAxisY = new QValueAxis();
    intenAxisX->setRange(1, 3);
    intenAxisX->setTickCount(3);
    intenAxisY->setRange(0, 10);
    intenAxisY->setTickCount(5);

    for(int i=0;i<reportV.count();i++)
    {
        QLineSeries *intenSeries = new QLineSeries();
        for(int j=0;j<reportV.at(i).infos.count();j++)
        {
            intenSeries->append(QString(reportV.at(i).infos.at(j).duration).toInt(), reportV.at(i).infos.at(j).intensity);

        }
        intenChart->addSeries(intenSeries);
        intenChart->setAxisX(intenAxisX, intenSeries);
        intenChart->setAxisY(intenAxisY, intenSeries);
    }
    QChartView *intenChartView = new QChartView(intenChart);
    intenChartView->setFixedSize(200, 200);
    intenChartView->setRenderHint(QPainter::Antialiasing);

    //ui->onsiteLO->addWidget(intenChartView);
}

void ReportViewer::drawOnsiteOnMap(int net, _KGOnSite_Pick_t pick, _KGOnSite_Info_t info, int i)
{
    double radius = info.distance * 1000;
    int zoomlevel = 10;
    int which = i;
    QString text;
    text = "Alert:" + QString::number(info.alert) + ", Dist.:" + QString::number(info.distance,'f',1) + "Km";

    QMetaObject::invokeMethod(rootObj, "addCircle",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, radius));

    QMetaObject::invokeMethod(rootObj, "addStaMarker",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, net),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, pick.sta), Q_ARG(QVariant, which),
                              Q_ARG(QVariant, zoomlevel));

    QMetaObject::invokeMethod(rootObj, "addText",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, text));


    /*
    QMetaObject::invokeMethod(this->rootObj, "addPolyline",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, pick.lat + 1), Q_ARG(QVariant, pick.lon + 1));
                              */
}

void ReportViewer::setWaveform(QString dir, QString fName, double ttime)
{
    //qDebug() << dir << fName;
    //ui->waveform->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    //QCPGraph *graph = ui->waveform->addGraph();

    QVector<double> time, data;
    int nSamp;
    double delta, sps, firstTime, min = 999999, max = -999999;

    QFile file(dir + "/" + fName);
    if(!file.exists())
    {
        qDebug() << "Waveform file doesn't exists.";
        //graph->setData(time, data);
        /*
        ui->waveform->removeGraph(graph);
        ui->waveform->replot();

        */
        //ui->waveform->hide();
        return;
    }

    QCPGraph *graph = ui->waveform->addGraph();

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;

        line = stream.readLine(); _line = line.simplified();
        firstTime = _line.section(" ", 3, 3).section("=", 1, 1).toDouble();
        nSamp = _line.section(" ", 4, 4).section("=", 1, 1).toInt();
        sps = _line.section(" ", 5, 5).section("=", 1, 1).toDouble();
        delta = 1.0 / sps;
        line = stream.readLine(); line = stream.readLine();
        int count = 0;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            double t = firstTime + (delta * count);
            time.push_back(t);
            data.push_back(line.toDouble());
            if(min > line.toDouble()) min = line.toDouble();
            if(max < line.toDouble()) max = line.toDouble();
            count++;

        }
        file.close();
    }

    graph->setData(time, data);
    graph->setPen(QPen(Qt::blue));
    graph->rescaleKeyAxis();

    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("hh:mm:ss");
    ui->waveform->xAxis->setTicker(timeTicker);
    ui->waveform->axisRect()->setupFullAxesBox();
    ui->waveform->yAxis->setRange(min, max);
    ui->waveform->xAxis->setRange(time.first(), time.last());
    //ui->waveform->yAxis->setVisible(false);
    ui->waveform->axisRect()->setAutoMargins(QCP::msTop|QCP::msBottom);
    ui->waveform->axisRect()->setMargins(QMargins(0,2,0,0));

    /* temp */
    ttime = 1473677075.830;

    /* set Labels */
    QDateTime t; t.setTime_t(ttime);
    t.setTimeSpec(Qt::UTC);
    t = convertKST(t);
    QString mSec = QString::number(ttime, 'f', 3).section(".",1,1);
    //ui->ttimeLB->setText(t.toString("yyyy-MM-dd hh:mm:ss.")+mSec);

    /* draw a P Line */
    QCPItemLine *pLine = new QCPItemLine(ui->waveform);
    pLine->setPen(QPen(Qt::red));
    pLine->start->setCoords(ttime, ui->waveform->yAxis->range().upper);
    pLine->end->setCoords(ttime, ui->waveform->yAxis->range().lower);

    /* draw a rect from 3 seconds to ttime */
    QCPItemRect *pRect = new QCPItemRect(ui->waveform);
    pRect->setPen(QPen(Qt::white));
    QColor yellow = Qt::yellow;
    yellow.setAlphaF( 0.3 );
    pRect->setBrush(yellow);
    pRect->topLeft->setCoords(ttime + delta, ui->waveform->yAxis->range().upper);
    pRect->bottomRight->setCoords(ttime + 3, ui->waveform->yAxis->range().lower);

    /* point P */
    /*
    QCPItemTracer *phaseTracer = new QCPItemTracer(ui->waveform);
    phaseTracer->setGraph(graph);
    phaseTracer->setGraphKey(ttime);
    phaseTracer->setStyle(QCPItemTracer::tsCircle);
    phaseTracer->setPen(QPen(Qt::red));
    phaseTracer->setBrush(Qt::red);
    phaseTracer->setSize(7);
    phaseTracer->setVisible(false);

    QDateTime t; t.setTime_t(ttime);
    QCPItemText *phaseTracerText = new QCPItemText(ui->waveform);
    phaseTracerText->position->setType(QCPItemPosition::ptAbsolute);
    phaseTracerText->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    phaseTracerText->position->setCoords(firstTime+10, 1);
    phaseTracerText->setText(t.toString("yyyy-MM-dd hh:mm:ss"));
    phaseTracerText->setTextAlignment(Qt::AlignLeft);
    phaseTracerText->setFont(*topF);
    phaseTracerText->setPadding(QMargins(8, 0, 0, 0));
    */


    //QCPItemCurve *phaseTracerArrow;

    ui->waveform->replot();
}
