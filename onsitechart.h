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

    void setup(QVector<_KGOnSite_Info_t>);

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

    void drawIntenBarGraph(QVector<int>, QVector<double>, QVector<double>, QVector<double>);
    void drawAlertBarGraph(QVector<int>, QVector<int>);
    void drawMagBarGraph(QVector<int>, QVector<double>, QVector<double>, QVector<double>);
    void drawDistBarGraph(QVector<int>, QVector<double>, QVector<double>, QVector<double>);
};

#endif // ONSITECHART_H
