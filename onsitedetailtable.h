#ifndef ONSITEDETAILTABLE_H
#define ONSITEDETAILTABLE_H

#include "common.h"
#include <QtCharts>

#include <QWidget>

namespace Ui {
class OnsiteDetailTable;
}

class OnsiteDetailTable : public QWidget
{
    Q_OBJECT

public:
    explicit OnsiteDetailTable(QWidget *parent = 0);
    ~OnsiteDetailTable();

    void setup(QString, QVector<_KGOnSite_Info_t>);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::OnsiteDetailTable *ui;

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

    void drawIntenBarGraph(QVector<int>, QVector<double>, QVector<double>, QVector<double>);
    void drawAlertBarGraph(QVector<int>, QVector<int>);
    void drawMagBarGraph(QVector<int>, QVector<double>, QVector<double>, QVector<double>);
    void drawDistBarGraph(QVector<int>, QVector<double>, QVector<double>, QVector<double>);
};

#endif // ONSITEDETAILTABLE_H
