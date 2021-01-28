#include "onsitedetailtable.h"
#include "ui_onsitedetailtable.h"

#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

OnsiteDetailTable::OnsiteDetailTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnsiteDetailTable)
{
    ui->setupUi(this);

    vsmallF = new QFont("Ubuntu", 6, QFont::Normal);
    midiumF = new QFont("Ubuntu", 10, QFont::Bold);
    smallF = new QFont("Ubuntu", 8, QFont::Normal);

    intenChartView = new QChartView();
    intenChartView->setRubberBand(QChartView::VerticalRubberBand);
    intenChartView->setRenderHint(QPainter::Antialiasing);
    intenChartView->setFixedSize(width() / 4, height());

    alertChartView = new QChartView();
    alertChartView->setRubberBand(QChartView::VerticalRubberBand);
    alertChartView->setRenderHint(QPainter::Antialiasing);
    alertChartView->setFixedSize(width() / 4, height());

    distChartView = new QChartView();
    distChartView->setRubberBand(QChartView::VerticalRubberBand);
    distChartView->setRenderHint(QPainter::Antialiasing);
    distChartView->setFixedSize(width() / 4, height());

    magChartView = new QChartView();
    magChartView->setRubberBand(QChartView::VerticalRubberBand);
    magChartView->setRenderHint(QPainter::Antialiasing);
    magChartView->setFixedSize(width() / 4, height());
}

OnsiteDetailTable::~OnsiteDetailTable()
{
    delete ui;
}

void OnsiteDetailTable::resizeEvent(QResizeEvent* event)
{
    intenChartView->setFixedSize(width() / 4, height());
    alertChartView->setFixedSize(width() / 4, height());
    magChartView->setFixedSize(width() / 4, height());
    distChartView->setFixedSize(width() / 4, height());
}

void OnsiteDetailTable::setup(QString title, QVector<_KGOnSite_Info_t> infos)
{
    ui->titleLB->setText(title);

    ui->onsiteTW->setRowCount(0);
    for(int i=0;i<infos.count();i++)
    {
        ui->onsiteTW->setRowCount(ui->onsiteTW->rowCount()+1);
        ui->onsiteTW->setItem(i, 0, new QTableWidgetItem(QString::number(infos.at(i).version)));
        ui->onsiteTW->setItem(i, 1, new QTableWidgetItem(QString(infos.at(i).msg_type)));
        ui->onsiteTW->setItem(i, 2, new QTableWidgetItem(QString::number(infos.at(i).ttime, 'f', 0)));

        ui->onsiteTW->setItem(i, 3, new QTableWidgetItem(QString(infos.at(i).sta)));
        ui->onsiteTW->setItem(i, 4, new QTableWidgetItem(QString(infos.at(i).chan)));
        ui->onsiteTW->setItem(i, 5, new QTableWidgetItem(QString(infos.at(i).net)));
        ui->onsiteTW->setItem(i, 6, new QTableWidgetItem(QString(infos.at(i).loc)));

        ui->onsiteTW->setItem(i, 7, new QTableWidgetItem(QString(infos.at(i).duration)));
        ui->onsiteTW->setItem(i, 8, new QTableWidgetItem(QString(infos.at(i).type)));
        ui->onsiteTW->setItem(i, 9, new QTableWidgetItem(QString::number(infos.at(i).disp_count, 'f', 2)));
        ui->onsiteTW->setItem(i, 10, new QTableWidgetItem(QString::number(infos.at(i).displacement, 'f', 2)));
        ui->onsiteTW->setItem(i, 11, new QTableWidgetItem(QString::number(infos.at(i).disp_time, 'f', 2)));
        ui->onsiteTW->setItem(i, 12, new QTableWidgetItem(QString::number(infos.at(i).pvel, 'f', 2)));
        ui->onsiteTW->setItem(i, 13, new QTableWidgetItem(QString::number(infos.at(i).pacc, 'f', 2)));
        ui->onsiteTW->setItem(i, 14, new QTableWidgetItem(QString::number(infos.at(i).tauc, 'f', 2)));
        ui->onsiteTW->setItem(i, 15, new QTableWidgetItem(QString::number(infos.at(i).pgv, 'f', 2)));
        ui->onsiteTW->setItem(i, 16, new QTableWidgetItem(QString::number(infos.at(i).pgv_uncertainty_low, 'f', 2)));
        ui->onsiteTW->setItem(i, 17, new QTableWidgetItem(QString::number(infos.at(i).pgv_uncertainty_high, 'f', 2)));
        ui->onsiteTW->setItem(i, 18, new QTableWidgetItem(QString::number(infos.at(i).alert)));
        ui->onsiteTW->setItem(i, 19, new QTableWidgetItem(QString::number(infos.at(i).snr, 'f', 2)));
        ui->onsiteTW->setItem(i, 20, new QTableWidgetItem(QString::number(infos.at(i).log_disp_ratio, 'f', 2)));
        ui->onsiteTW->setItem(i, 21, new QTableWidgetItem(QString::number(infos.at(i).log_PdPv, 'f', 2)));
        ui->onsiteTW->setItem(i, 22, new QTableWidgetItem(QString::number(infos.at(i).dB_Pd, 'f', 2)));
        ui->onsiteTW->setItem(i, 23, new QTableWidgetItem(QString::number(infos.at(i).intensity, 'f', 2)));
        ui->onsiteTW->setItem(i, 24, new QTableWidgetItem(QString::number(infos.at(i).intensity_uncertainty_low, 'f', 2)));
        ui->onsiteTW->setItem(i, 25, new QTableWidgetItem(QString::number(infos.at(i).intensity_uncertainty_high, 'f', 2)));
        ui->onsiteTW->setItem(i, 26, new QTableWidgetItem(QString::number(infos.at(i).magnitude, 'f', 1)));
        ui->onsiteTW->setItem(i, 27, new QTableWidgetItem(QString::number(infos.at(i).magnitude_uncertainty_low, 'f', 2)));
        ui->onsiteTW->setItem(i, 28, new QTableWidgetItem(QString::number(infos.at(i).magnitude_uncertainty_high, 'f', 2)));
        ui->onsiteTW->setItem(i, 29, new QTableWidgetItem(QString::number(infos.at(i).distance, 'f', 2)));
        ui->onsiteTW->setItem(i, 30, new QTableWidgetItem(QString::number(infos.at(i).distance_uncertainty_low, 'f', 2)));
        ui->onsiteTW->setItem(i, 31, new QTableWidgetItem(QString::number(infos.at(i).distance_uncertainty_high, 'f', 2)));
    }

    for(int i=0;i<ui->onsiteTW->rowCount();i++)
    {
        for(int j=0;j<ui->onsiteTW->columnCount();j++)
        {
            if(ui->onsiteTW->item(i, j)->text() != "") ui->onsiteTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }

    ui->onsiteTW->setFixedHeight(30*infos.count()+40);

    QVector<int> duration;
    QVector<double> low_inten, inten, high_inten;
    QVector<int> alert;
    QVector<double> low_mag, mag, high_mag;
    QVector<double> low_dist, dist, high_dist;

    for(int i=0;i<infos.count();i++)
    {
        duration.push_back(QString(infos.at(i).duration).toInt());
        alert.push_back(infos.at(i).alert);
        low_inten.push_back(infos.at(i).intensity_uncertainty_low);
        inten.push_back(infos.at(i).intensity);
        high_inten.push_back(infos.at(i).intensity_uncertainty_high);
        low_mag.push_back(infos.at(i).magnitude_uncertainty_low);
        mag.push_back(infos.at(i).magnitude);
        high_mag.push_back(infos.at(i).magnitude_uncertainty_high);
        low_dist.push_back(infos.at(i).distance_uncertainty_low);
        dist.push_back(infos.at(i).distance);
        high_dist.push_back(infos.at(i).distance_uncertainty_high);
    }

    drawIntenBarGraph(duration, low_inten, inten, high_inten);
    drawAlertBarGraph(duration, alert);
    drawMagBarGraph(duration, low_mag, mag, high_mag);
    drawDistBarGraph(duration, low_dist, dist, high_dist);
}

void OnsiteDetailTable::drawIntenBarGraph(QVector<int> dur, QVector<double> low_inten, QVector<double> inten, QVector<double> high_inten)
{
    intenChart = new QChart();
    intenChart->setTitle(tr("INTENSITY"));
    intenChart->setTitleFont(*midiumF);
    intenChart->setAnimationOptions(QChart::SeriesAnimations);
    intenChart->layout()->setContentsMargins(0, 0, 0, 0);
    intenChart->createDefaultAxes();
    intenChart->legend()->hide();

    intenChartView->setChart(intenChart);

    ui->chartLO->addWidget(intenChartView);

    QStringList xCategories;
    QBoxPlotSeries *intenSeries = new QBoxPlotSeries();
    intenSeries->setPen(QPen(Qt::black));
    intenSeries->setBrush(QBrush(Qt::red));

    double minY = 999, maxY = -999;

    for(int i=0;i<dur.count();i++)
    {
        if(high_inten.at(i) > maxY)
            maxY = high_inten.at(i);
        if(low_inten.at(i) < minY)
            minY = low_inten.at(i);

        xCategories << QString::number(i+1) + "sec";
        QBoxSet *box = new QBoxSet();
        box->setValue(QBoxSet::LowerExtreme, low_inten.at(i));
        box->setValue(QBoxSet::UpperExtreme, high_inten.at(i));
        box->setValue(QBoxSet::Median, inten.at(i));
        box->setValue(QBoxSet::LowerQuartile, inten.at(i) - 0.05);
        box->setValue(QBoxSet::UpperQuartile, inten.at(i) + 0.05);
        intenSeries->append(box);
    }

    intenChart->addSeries(intenSeries);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();

    axisX->setLabelsFont(*smallF);
    axisY->setLabelsFont(*smallF);

    axisY->setRange(minY-0.5, maxY+0.5);
    axisX->append(xCategories);

    intenChart->setAxisX(axisX, intenSeries);
    intenChart->setAxisY(axisY, intenSeries);
}

void OnsiteDetailTable::drawAlertBarGraph(QVector<int> dur, QVector<int> alert)
{
    alertChart = new QChart();
    alertChart->setTitle(tr("ALERT"));
    alertChart->setTitleFont(*midiumF);
    alertChart->setAnimationOptions(QChart::SeriesAnimations);
    alertChart->layout()->setContentsMargins(0, 0, 0, 0);
    alertChart->createDefaultAxes();
    alertChart->legend()->hide();

    alertChartView->setChart(alertChart);

    ui->chartLO->addWidget(alertChartView);

    QColor *alertColors = new QColor[4];
    alertColors[0] = QColor("#CCCCCC");
    alertColors[1] = QColor("#46BFBD");
    alertColors[2] = QColor("#FDB45C");
    alertColors[3] = QColor("#F7464A");

    QStringList xCategories;

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();

    axisX->setLabelsFont(*smallF);
    axisY->setLabelsFont(*smallF);
    axisY->setRange(0, 4);
    axisY->setTickCount(5);

    for(int i=0;i<dur.count();i++)
    {
        xCategories << QString::number(i+1) + "sec";

        QScatterSeries *series = new QScatterSeries();
        series->setName("scatter");
        series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        series->setMarkerSize(20.0);
        series->append(i, alert.at(i));
        series->setBrush(alertColors[alert.at(i)]);
        series->setPen(QPen(Qt::black));
        alertChart->addSeries(series);
        axisX->append(xCategories);
        alertChart->setAxisX(axisX, series);
        alertChart->setAxisY(axisY, series);
    }
}

void OnsiteDetailTable::drawMagBarGraph(QVector<int> dur, QVector<double> low_mag, QVector<double> mag, QVector<double> high_mag)
{
    magChart = new QChart();
    magChart->setTitle(tr("MAGNITUDE"));
    magChart->setTitleFont(*midiumF);
    magChart->setAnimationOptions(QChart::SeriesAnimations);
    magChart->layout()->setContentsMargins(0, 0, 0, 0);
    magChart->createDefaultAxes();
    magChart->legend()->setFont(*smallF);
    magChart->legend()->hide();

    magChartView->setChart(magChart);

    ui->chartLO->addWidget(magChartView);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();

    QStringList xCategories;
    QBoxPlotSeries *magSeries = new QBoxPlotSeries();
    magSeries->setPen(QPen(Qt::black));
    magSeries->setBrush(QBrush(Qt::red));

    double minY = 999, maxY = -999;

    for(int i=0;i<dur.count();i++)
    {
        if(high_mag.at(i) > maxY)
            maxY = high_mag.at(i);
        if(low_mag.at(i) < minY)
            minY = low_mag.at(i);

        xCategories << QString::number(i+1) + "sec";
        QBoxSet *box = new QBoxSet();
        box->setValue(QBoxSet::LowerExtreme, low_mag.at(i));
        box->setValue(QBoxSet::UpperExtreme, high_mag.at(i));
        box->setValue(QBoxSet::Median, mag.at(i));
        box->setValue(QBoxSet::LowerQuartile, mag.at(i) - 0.05);
        box->setValue(QBoxSet::UpperQuartile, mag.at(i) + 0.05);
        magSeries->append(box);
    }
    magChart->addSeries(magSeries);

    axisX->setLabelsFont(*smallF);
    axisY->setLabelsFont(*smallF);
    axisY->setRange(minY-0.5, maxY+0.5);

    axisX->append(xCategories);

    magChart->setAxisX(axisX, magSeries);
    magChart->setAxisY(axisY, magSeries);
}

void OnsiteDetailTable::drawDistBarGraph(QVector<int> dur, QVector<double> low_dist, QVector<double> dist, QVector<double> high_dist)
{
    distChart = new QChart();
    distChart->setTitle(tr("DISTANCE"));
    distChart->setTitleFont(*midiumF);
    distChart->setAnimationOptions(QChart::SeriesAnimations);
    distChart->layout()->setContentsMargins(0, 0, 0, 0);
    distChart->createDefaultAxes();
    distChart->legend()->setFont(*smallF);
    distChart->legend()->hide();

    distChartView->setChart(distChart);

    ui->chartLO->addWidget(distChartView);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();

    QStringList xCategories;
    QBoxPlotSeries *distSeries = new QBoxPlotSeries();
    distSeries->setPen(QPen(Qt::black));
    distSeries->setBrush(QBrush(Qt::red));

    double minY = 999, maxY = -999;

    for(int i=0;i<dur.count();i++)
    {
        if(high_dist.at(i) > maxY)
            maxY = high_dist.at(i);
        if(low_dist.at(i) < minY)
            minY = low_dist.at(i);

        xCategories << QString::number(i+1) + "sec";
        QBoxSet *box = new QBoxSet();
        box->setValue(QBoxSet::LowerExtreme, low_dist.at(i));
        box->setValue(QBoxSet::UpperExtreme, high_dist.at(i));
        box->setValue(QBoxSet::Median, dist.at(i));
        box->setValue(QBoxSet::LowerQuartile, dist.at(i) - 10);
        box->setValue(QBoxSet::UpperQuartile, dist.at(i) + 10);
        distSeries->append(box);
    }

    distChart->addSeries(distSeries);

    axisX->setLabelsFont(*smallF);
    axisY->setLabelsFont(*smallF);
    axisY->setRange(minY-50, maxY+50);

    axisX->append(xCategories);

    distChart->setAxisX(axisX, distSeries);
    distChart->setAxisY(axisY, distSeries);
}
