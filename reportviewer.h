#ifndef REPORTVIEWER_H
#define REPORTVIEWER_H

#include "common.h"
#include "listinfo.h"
#include "qcustomplot.h"

#include <QDialog>

#include <QQuickView>
#include <QtQuick>

#include <QtCharts>

namespace Ui {
class ReportViewer;
}

class ReportViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ReportViewer(QString evid, QVector<_REPORT> reportV, _CONFIGURE configure);
    ~ReportViewer();

private:
    Ui::ReportViewer *ui;

    QObject *rootObj;
    QVariant returnedValue;
    _CONFIGURE con;

    ListInfo *listInfo[MAX_ONSITE_NUM];
    QWidget *mapContainer;

    void setSummaryGB(QVector<_REPORT>);
    void setListGB(QVector<_REPORT>);
    void setOnsiteGB(QVector<_REPORT>);
    void drawOnsiteOnMap(int, _KGOnSite_Pick_t, _KGOnSite_Info_t, int);
    void setWaveform(QString dir, QString fName, double ttime);

};

#endif // REPORTVIEWER_H
