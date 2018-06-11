#include "tabinfo.h"
#include "ui_tabinfo.h"

#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

TabInfo::TabInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabInfo)
{
    ui->setupUi(this);

    intendialog = new IntensityDialog(this);     intendialog->hide();
    alertdialog = new AlertDialog(this);     alertdialog->hide();
    magdialog = new MagDialog(this);    magdialog->hide();
    distdialog = new DistanceDialog(this);  distdialog->hide();

    topF = new QFont("Ubuntu", 12, QFont::Bold);
    middleF = new QFont("Ubuntu", 11, QFont::Bold);
    bottomF = new QFont("Ubuntu", 11, QFont::Normal);

/*
    intenList << "I" << "II" << "III" << "IV" << "V" << "VI" << "VII" << "VIII" << "IX" << "X+";
    intenColors = new QColor[24];
    intenColors[0] = QColor("#ffffff");
    intenColors[1] = QColor("#bfccff");
    intenColors[2] = QColor("#a0e6ff");
    intenColors[3] = QColor("#80ffff");
    intenColors[4] = QColor("#7aff93");
    intenColors[5] = QColor("#ffff00");
    intenColors[6] = QColor("#ffc800");
    intenColors[7] = QColor("#ff9100");
    intenColors[8] = QColor("#ff0000");
    intenColors[9] = QColor("#c80000");
    intenSubList << "No Damage\n(no shaking)"
                 << "No Damage\n(weak shaking)"
                 << "No Damage\n(weak shaking)"
                 << "No Damage\n(light shaking)"
                 << "Very Light Damage\n(moderate shaking)"
                 << "Light Damage\n(strong shaking)"
                 << "Moderate Damage\n(very strong shaking)"
                 << "Moderate-Heavy Damage\n(severe shaking)"
                 << "Heavy Damage\n(violent shaking)"
                 << "Very Heavy Damage\n(extreme shaking)";

    alertList << "No\nDamage" << "Far\nDamage" << "Local\nDamage" << "Local & Far\nDamage";
    alertColors = new QColor[4];
    alertColors[0] = QColor("#CCCCCC");
    alertColors[1] = QColor("#46BFBD");
    alertColors[2] = QColor("#FDB45C");
    alertColors[3] = QColor("#F7464A");

    magList << "Small" << "Medium" << "Moderate" << "Large";
    magColors = new QColor[4];
    magColors[0] = QColor("#CCFFCC");
    magColors[1] = QColor("#FFFF33");
    magColors[2] = QColor("#FF9900");
    magColors[3] = QColor("#FF3333");
    magSubList << "M < 3" << "3 < M < 5" << "5 < M < 6.5" << "M > 6.5";

    distList << "Far" << "Medium" << "Near";
    distColors = new QColor[3];
    distColors[0] = QColor("#CCFFCC");
    distColors[2] = QColor("#FF3333");
    distColors[1] = QColor("#FFFF33");
    distSubList << "KM > 150" << "50 < KM < 150" << "KM < 50";
*/
}

TabInfo::~TabInfo()
{
    delete ui;
}

void TabInfo::setup(QString homeDir, _KGOnSite_Pick_t pick, _KGOnSite_Info_t info)
{
    QLayoutItem *child;
    while ((child = ui->onsiteLO->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    QString scnl;
    scnl = QString::fromStdString(pick.sta) + "/" + QString::fromStdString(pick.chan) + "/" +
            QString::fromStdString(pick.net) + "/" + QString::fromStdString(pick.loc);
    ui->scnlLB->setText(scnl);

    setIntenWG(info.intensity);
    setAlertWG(info.alert);
    setMagWG(info.magnitude);
    setDistWG(info.distance);

    QString dir, dfile;
    QDateTime tt;
    tt.setTime_t( (uint) pick.ttime );
    tt.setTimeSpec(Qt::UTC);
    tt = convertKST(tt);
    dir = tt.toString("yyyy/MMdd");
    dfile = QString::number(pick.ttime,'f',0) + QString::fromStdString(pick.sta) + ".dat";
    setWaveform(homeDir + "/data/" + dir, dfile, pick.ttime);
}

void TabInfo::intenPBClicked()
{
    intendialog->show();
}

void TabInfo::alertPBClicked()
{
    alertdialog->show();
}

void TabInfo::magPBClicked()
{
    magdialog->show();
}

void TabInfo::distPBClicked()
{
    distdialog->show();
}

void TabInfo::resizeEvent(QResizeEvent* event)
{
    intenChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());
    intenPB->setGeometry(intenChartView->width() / 4, 10, intenChartView->width() / 2, 20);
    intenMLB->setGeometry(0, 0, intenChartView->width(), intenChartView->height());
    intenBLB->setGeometry(0, intenChartView->height() - 40, intenChartView->width(), 40);

    alertChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());
    alertPB->setGeometry(alertChartView->width() / 4, 10, alertChartView->width() / 2, 20);
    alertMLB->setGeometry(0, 0, alertChartView->width(), alertChartView->height());
    alertBLB->setGeometry(0, alertChartView->height() - 40, alertChartView->width(), 40);

    magChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());
    magPB->setGeometry(magChartView->width() / 4, 10, magChartView->width() / 2, 25);
    magMLB->setGeometry(0, 0, magChartView->width(), magChartView->height());
    magBLB->setGeometry(0, magChartView->height() - 40, magChartView->width(), 40);

    distChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());
    distPB->setGeometry(distChartView->width() / 4, 10, distChartView->width() / 2, 25);
    distMLB->setGeometry(0, 0, distChartView->width(), distChartView->height());
    distBLB->setGeometry(0, distChartView->height() - 40, distChartView->width(), 40);

    ui->waveform->replot();

    update();
    QWidget::resizeEvent(event);
}

void TabInfo::setIntenWG(double inten)
{
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.45);

    series->append(intenText(inten), intenText(inten).toInt());
    series->append("Others", NUM_INTENLIST-intenText(inten).toInt());

    QPieSlice *slice1 = series->slices().at(0);
    slice1->setLabelVisible(false);
    slice1->setPen(QPen(Qt::black, 0.1));
    slice1->setBrush(QColor(intenColor(inten)));

    QPieSlice *slice2 = series->slices().at(1);
    slice2->setPen(QPen(Qt::black, 0.1));
    slice2->setBrush(Qt::white);

    intenChart = new QChart();
    intenChart->addSeries(series);
    intenChart->legend()->hide();
    intenChart->layout()->setContentsMargins(0, 0, 0, 0);
    intenChart->setAnimationDuration(2000);
    intenChart->setAnimationOptions(QChart::AllAnimations);

    intenChartView = new QChartView(intenChart);
    intenChartView->setRenderHint(QPainter::Antialiasing);
    intenChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());

    ui->onsiteLO->addWidget(intenChartView);

    intenMLB = new QLabel(intenChartView);
    intenMLB->setStyleSheet("background-color: rgba(255, 255, 255, 10)");
    intenMLB->setGeometry(0, 0, intenChartView->width(), intenChartView->height());
    intenMLB->setFont(*middleF);
    intenMLB->setText(intenText(inten));
    intenMLB->setAlignment(Qt::AlignCenter);

    intenBLB = new QLabel(intenChartView);
    intenBLB->setStyleSheet("background-color: rgba(255, 255, 255, 10)");
    intenBLB->setGeometry(0, intenChartView->height() - 40, intenChartView->width(), 40);
    intenBLB->setFont(*bottomF);
    intenBLB->setText(intenSubText(inten));
    intenBLB->setAlignment(Qt::AlignCenter);

    intenPB = new QPushButton(intenChartView);
    connect(intenPB, SIGNAL(clicked(bool)), this, SLOT(intenPBClicked()));
    intenPB->setGeometry(intenChartView->width() / 4, 10, intenChartView->width() / 2, 25);
    intenPB->setFont(*topF);
    intenPB->setText(tr("INTENSITY"));
}

void TabInfo::setAlertWG(int alert)
{
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append(alertText(alert), alert);
    series->append("Others", NUM_ALERTLIST-alert);

    QPieSlice *slice1 = series->slices().at(0);
    slice1->setLabelVisible(false);
    slice1->setPen(QPen(Qt::black, 0.1));
    slice1->setBrush(QColor(alertColor(alert)));

    QPieSlice *slice2 = series->slices().at(1);
    slice2->setPen(QPen(Qt::black, 0.1));
    slice2->setBrush(Qt::white);

    alertChart = new QChart();
    alertChart->addSeries(series);
    alertChart->legend()->hide();
    alertChart->layout()->setContentsMargins(0, 0, 0, 0);
    alertChart->setAnimationDuration(2000);
    alertChart->setAnimationOptions(QChart::AllAnimations);

    alertChartView = new QChartView(alertChart);
    alertChartView->setRenderHint(QPainter::Antialiasing);
    alertChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());

    ui->onsiteLO->addWidget(alertChartView);

    alertMLB = new QLabel(alertChartView);
    alertMLB->setStyleSheet("background-color: rgba(255, 255, 255, 10)");
    alertMLB->setGeometry(0, 0, alertChartView->width(), alertChartView->height());
    alertMLB->setFont(*middleF);
    alertMLB->setText(alertText(alert));
    alertMLB->setAlignment(Qt::AlignCenter);

    alertBLB = new QLabel(alertChartView);
    alertBLB->setStyleSheet("background-color: rgba(255, 255, 255, 10)");
    alertBLB->setGeometry(0, alertChartView->height() - 40, alertChartView->width(), 40);
    alertBLB->setFont(*bottomF);
    alertBLB->setText(alertSubText(alert));
    alertBLB->setAlignment(Qt::AlignCenter);

    alertPB = new QPushButton(alertChartView);
    connect(alertPB, SIGNAL(clicked(bool)), this, SLOT(alertPBClicked()));
    alertPB->setGeometry(alertChartView->width() / 4, 10, alertChartView->width() / 2, 25);
    alertPB->setFont(*topF);
    alertPB->setText(tr("ALERT"));
}

void TabInfo::setMagWG(double mag)
{
    int magI;
    if(mag < 4) magI = 0;
    else if(mag >= 4 && mag < 5) magI = 1;
    else if(mag >= 5) magI = 2;

    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append(magText(mag), magI+1);
    series->append("Others", NUM_MAGLIST-(magI+1));

    QPieSlice *slice1 = series->slices().at(0);
    slice1->setLabelVisible(false);
    slice1->setPen(QPen(Qt::black, 0.1));
    slice1->setBrush(QColor(magColor(mag)));

    QPieSlice *slice2 = series->slices().at(1);
    slice2->setPen(QPen(Qt::black, 0.1));
    slice2->setBrush(Qt::white);

    magChart = new QChart();
    magChart->addSeries(series);
    magChart->legend()->hide();
    magChart->layout()->setContentsMargins(0, 0, 0, 0);
    magChart->setAnimationDuration(2000);
    magChart->setAnimationOptions(QChart::AllAnimations);

    magChartView = new QChartView(magChart);
    magChartView->setRenderHint(QPainter::Antialiasing);
    magChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());

    ui->onsiteLO->addWidget(magChartView);

    magMLB = new QLabel(magChartView);
    magMLB->setStyleSheet("background-color: rgba(255, 255, 255, 10)");
    magMLB->setGeometry(0, 0, magChartView->width(), magChartView->height());
    magMLB->setFont(*middleF);
    magMLB->setText(magText(mag));
    magMLB->setAlignment(Qt::AlignCenter);

    magBLB = new QLabel(magChartView);
    magBLB->setStyleSheet("background-color: rgba(255, 255, 255, 10)");
    magBLB->setGeometry(0, magChartView->height() - 40, magChartView->width(), 40);
    magBLB->setFont(*bottomF);
    magBLB->setText(magText(mag));
    magBLB->setAlignment(Qt::AlignCenter);

    magPB = new QPushButton(magChartView);
    connect(magPB, SIGNAL(clicked(bool)), this, SLOT(magPBClicked()));
    magPB->setGeometry(25, 15, 150, 25);
    magPB->setGeometry(magChartView->width() / 4, 10, magChartView->width() / 2, 25);
    magPB->setFont(*topF);
    magPB->setText(tr("MAGNITUDE"));
}

void TabInfo::setDistWG(double dist)
{
    int distI;
    if(dist > 50) distI = 0;
    else if(dist <= 50) distI = 1;

    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append(distText(dist), distI+1);
    series->append("Others", NUM_DISTLIST-(distI+1));

    QPieSlice *slice1 = series->slices().at(0);
    slice1->setLabelVisible(false);
    slice1->setPen(QPen(Qt::black, 0.1));
    slice1->setBrush(QColor(distColor(dist)));

    QPieSlice *slice2 = series->slices().at(1);
    slice2->setPen(QPen(Qt::black, 0.1));
    slice2->setBrush(Qt::white);

    distChart = new QChart();
    distChart->addSeries(series);
    distChart->legend()->hide();
    distChart->layout()->setContentsMargins(0, 0, 0, 0);

    distChart->setAnimationDuration(2000);
    distChart->setAnimationOptions(QChart::AllAnimations);

    distChartView = new QChartView(distChart);
    distChartView->setRenderHint(QPainter::Antialiasing);
    distChartView->setFixedSize(width() / 4, this->height() - ui->waveform->height());

    ui->onsiteLO->addWidget(distChartView);

    distMLB = new QLabel(distChartView);
    distMLB->setGeometry(0, 0, distChartView->width(), distChartView->height());
    distMLB->setFont(*middleF);
    distMLB->setStyleSheet("background-color: rgba(255, 255, 255, 10);");
    distMLB->setText(distText(dist));
    distMLB->setAlignment(Qt::AlignCenter);

    distBLB = new QLabel(distChartView);
    distBLB->setGeometry(0, distChartView->height() - 40, distChartView->width(), 40);
    distBLB->setFont(*bottomF);
    distBLB->setText(distText(dist));
    distBLB->setAlignment(Qt::AlignCenter);

    distPB = new QPushButton(distChartView);
    connect(distPB, SIGNAL(clicked(bool)), this, SLOT(distPBClicked()));
    distPB->setGeometry(distChartView->width() / 4, 10, distChartView->width() / 2, 25);
    distPB->setFont(*topF);
    distPB->setText(tr("DISTANCE"));
}

void TabInfo::setWaveform(QString dir, QString fName, double ttime)
{
    //qDebug() << dir << fName;
    //ui->waveform->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QCPGraph *graph = ui->waveform->addGraph();
    QVector<double> time, data;
    int nSamp;
    double delta, sps, firstTime, min = 999999, max = -999999;

    QFile file(dir + "/" + fName);
    if(!file.exists())
    {
        qDebug() << "Waveform file doesn't exists.";
        graph->setData(time, data);
        /*
        ui->waveform->removeGraph(graph);
        ui->waveform->replot();

        */
        ui->waveform->hide();
        return;
    }

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
    ui->ttimeLB->setText(t.toString("yyyy-MM-dd hh:mm:ss.")+mSec);

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
