#include "onsitechart.h"
#include "ui_onsitechart.h"

#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

OnsiteChart::OnsiteChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnsiteChart)
{
    ui->setupUi(this);

    vsmallF = new QFont("Ubuntu", 6, QFont::Normal);
    midiumF = new QFont("Ubuntu", 10, QFont::Bold);
    smallF = new QFont("Ubuntu", 8, QFont::Normal);
}

OnsiteChart::~OnsiteChart()
{
    delete ui;
}

void OnsiteChart::resizeEvent(QResizeEvent* event)
{
    intenChartView->setFixedSize(width() / 4, height());
    alertChartView->setFixedSize(width() / 4, height());
    magChartView->setFixedSize(width() / 4, height());
    distChartView->setFixedSize(width() / 4, height());

    update();
    QWidget::resizeEvent(event);
}

void OnsiteChart::setup(_KGOnSite_Pick_t pick)
{
    QLayoutItem *child;
    while ((child = ui->chartLO->takeAt(0)) != 0)
        delete child;

    this->model = new QSqlQueryModel();

    QString query;
    query = "select * from onsiteInfo where sta = '" + QString(pick.sta) + "' and chan = '" + QString(pick.chan) + "' and net = '" +
            QString(pick.net) + "' and loc = '" + QString(pick.loc) + "' and ttime = " + QString::number(pick.ttime, 'f', 6);

    model->setQuery(query);

    QVector<int> duration;
    QVector<double> low_inten, inten, high_inten;
    QVector<int> alert;
    QVector<double> low_mag, mag, high_mag;
    QVector<double> low_dist, dist, high_dist;

    for(int i=0;i<model->rowCount();++i)
    {
        QString scnl;
        scnl = model->record(i).value("sta").toString() + "/" + model->record(i).value("chan").toString() + "/" +
                model->record(i).value("net").toString() + "/" + model->record(i).value("loc").toString();

        duration.push_back(model->record(i).value("duration").toInt());
        alert.push_back(model->record(i).value("alert").toInt());
        low_inten.push_back(model->record(i).value("intensity_uncertainty_low").toDouble());
        inten.push_back(model->record(i).value("intensity").toDouble());
        high_inten.push_back(model->record(i).value("intensity_uncertainty_high").toDouble());
        low_mag.push_back(model->record(i).value("magnitude_uncertainty_low").toDouble());
        mag.push_back(model->record(i).value("magnitude").toDouble());
        high_mag.push_back(model->record(i).value("magnitude_uncertainty_high").toDouble());
        low_dist.push_back(model->record(i).value("distance_uncertainty_low").toDouble());
        dist.push_back(model->record(i).value("distance").toDouble());
        high_dist.push_back(model->record(i).value("distance_uncertainty_high").toDouble());
    }

    drawIntenBarGraph(duration, low_inten, inten, high_inten);
    drawAlertBarGraph(duration, alert);
    drawMagBarGraph(duration, low_mag, mag, high_mag);
    drawDistBarGraph(duration, low_dist, dist, high_dist);
}

void OnsiteChart::drawIntenBarGraph(QVector<int> dur, QVector<double> low_inten, QVector<double> inten, QVector<double> high_inten)
{
    intenChart = new QChart(); intenChart->setTitle(tr("INTENSITY"));
    intenChart->setTitleFont(*midiumF);
    intenChart->setAnimationOptions(QChart::SeriesAnimations);
    intenChart->layout()->setContentsMargins(0, 0, 0, 0);
    intenChart->createDefaultAxes();
    intenChart->legend()->hide();

    intenChartView = new QChartView(intenChart);
    intenChartView->setRubberBand(QChartView::VerticalRubberBand);
    intenChartView->setRenderHint(QPainter::Antialiasing);
    intenChartView->setFixedSize(width() / 4, height());

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
    /*
    axisY->setRange(0, 10);
    axisY->setTickCount(11);
    */
    axisY->setRange(minY-0.5, maxY+0.5);
    axisX->append(xCategories);

    intenChart->setAxisX(axisX, intenSeries);
    intenChart->setAxisY(axisY, intenSeries);
}

void OnsiteChart::drawAlertBarGraph(QVector<int> dur, QVector<int> alert)
{
    alertChart = new QChart(); alertChart->setTitle(tr("ALERT"));
    alertChart->setTitleFont(*midiumF);
    alertChart->setAnimationOptions(QChart::SeriesAnimations);
    alertChart->layout()->setContentsMargins(0, 0, 0, 0);
    alertChart->createDefaultAxes();
    alertChart->legend()->hide();

    alertChartView = new QChartView(alertChart);
    alertChartView->setRubberBand(QChartView::VerticalRubberBand);
    alertChartView->setRenderHint(QPainter::Antialiasing);

    alertChartView->setFixedSize(width() / 4, height());

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

void OnsiteChart::drawMagBarGraph(QVector<int> dur, QVector<double> low_mag, QVector<double> mag, QVector<double> high_mag)
{
    magChart = new QChart(); magChart->setTitle(tr("MAGNITUDE"));
    magChart->setTitleFont(*midiumF);
    magChart->setAnimationOptions(QChart::SeriesAnimations);
    magChart->layout()->setContentsMargins(0, 0, 0, 0);
    magChart->createDefaultAxes();
    magChart->legend()->setFont(*smallF);
    magChart->legend()->hide();

    magChartView = new QChartView(magChart);
    magChartView->setRubberBand(QChartView::VerticalRubberBand);
    magChartView->setRenderHint(QPainter::Antialiasing);
    magChartView->setFixedSize(width() / 4, height());

    ui->chartLO->addWidget(magChartView);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();
    //QCategoryAxis *magYAxis = new QCategoryAxis();

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

    /*
    magYAxis->setLabelsAngle(90);
    magYAxis->append("Small", 3); magYAxis->append("Medium", 5);
    magYAxis->append("Moderate", 6.5); magYAxis->append("Large", 10);
    magYAxis->setMax(10); magYAxis->setMin(0);
    */

    axisX->setLabelsFont(*smallF);
    axisY->setLabelsFont(*smallF);
    //magYAxis->setLabelsFont(*vsmallF);
    //axisY->setRange(0, 10);
    //axisY->setTickCount(11);
    axisY->setRange(minY-0.5, maxY+0.5);

    /*
    magYAxis->setGridLinePen(QPen(Qt::blue));
    magYAxis->setLabelsColor(QColor(Qt::blue));
    */

    axisX->append(xCategories);

    magChart->setAxisX(axisX, magSeries);
    magChart->setAxisY(axisY, magSeries);
    //magChart->addAxis(magYAxis, Qt::AlignRight);
}

void OnsiteChart::drawDistBarGraph(QVector<int> dur, QVector<double> low_dist, QVector<double> dist, QVector<double> high_dist)
{
    distChart = new QChart(); distChart->setTitle(tr("DISTANCE"));
    distChart->setTitleFont(*midiumF);
    distChart->setAnimationOptions(QChart::SeriesAnimations);
    distChart->layout()->setContentsMargins(0, 0, 0, 0);
    distChart->createDefaultAxes();
    distChart->legend()->setFont(*smallF);
    distChart->legend()->hide();

    distChartView = new QChartView(distChart);
    distChartView->setRubberBand(QChartView::VerticalRubberBand);
    distChartView->setRenderHint(QPainter::Antialiasing);
    distChartView->setFixedSize(width() / 4, height());

    ui->chartLO->addWidget(distChartView);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();
    //QCategoryAxis *distYAxis = new QCategoryAxis();

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

    /*
    distYAxis->setLabelsAngle(90);
    distYAxis->append("Near", 50); distYAxis->append("Medium", 150); distYAxis->append("Far", 200);
    distYAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionCenter);
    distYAxis->setMax(200); distYAxis->setMin(0);
    */

    axisX->setLabelsFont(*smallF);
    axisY->setLabelsFont(*smallF);
    //distYAxis->setLabelsFont(*vsmallF);

    //axisY->setRange(0, 200);
    //axisY->setTickCount(5);
    axisY->setRange(minY-50, maxY+50);

    axisX->append(xCategories);
    /*
    distYAxis->setGridLinePen(QPen(Qt::blue));
    distYAxis->setLabelsColor(QColor(Qt::blue));
    */

    distChart->setAxisX(axisX, distSeries);
    distChart->setAxisY(axisY, distSeries);
    //distChart->addAxis(distYAxis, Qt::AlignRight);
}
