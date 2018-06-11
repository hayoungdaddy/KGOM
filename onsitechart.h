#ifndef ONSITECHART_H
#define ONSITECHART_H

#include "common.h"

#include <QWidget>
#include <QtCharts>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

namespace Ui {
class OnsiteChart;
}

class OnsiteChart : public QWidget
{
    Q_OBJECT

public:
    explicit OnsiteChart(QWidget *parent = 0);
    ~OnsiteChart();

    void setup(_KGOnSite_Pick_t pick);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::OnsiteChart *ui;

    QFont *midiumF;
    QFont *smallF;
    QFont *vsmallF;

    QChartView *intenChartView;
    QChartView *alertChartView;
    QChartView *magChartView;
    QChartView *distChartView;
    QChart *intenChart;
    QChart *alertChart;
    QChart *magChart;
    QChart *distChart;

    QSqlQueryModel *model;

    void drawIntenBarGraph(QVector<int> dur, QVector<double> low_inten, QVector<double> inten, QVector<double> high_inten);
    void drawAlertBarGraph(QVector<int> dur, QVector<int> alert);
    void drawMagBarGraph(QVector<int> dur, QVector<double> low_mag, QVector<double> mag, QVector<double> high_mag);
    void drawDistBarGraph(QVector<int> dur, QVector<double> low_dist, QVector<double> dist, QVector<double> high_dist);
};

#endif // ONSITECHART_H
