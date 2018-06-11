#ifndef TABINFO_H
#define TABINFO_H

#include "common.h"
#include "selectvalues.h"

#include "qcustomplot.h"
#include "intensitydialog.h"
#include "alertdialog.h"
#include "magdialog.h"
#include "distancedialog.h"

#include <QWidget>
#include <QtCharts>

namespace Ui {
class TabInfo;
}

class TabInfo : public QWidget
{
    Q_OBJECT

public:
    explicit TabInfo(QWidget *parent = 0);
    ~TabInfo();

    void setup(QString homeDir, _KGOnSite_Pick_t pick, _KGOnSite_Info_t info);
    IntensityDialog *intendialog;
    AlertDialog *alertdialog;
    MagDialog *magdialog;
    DistanceDialog *distdialog;

protected:
    void resizeEvent(QResizeEvent* event);

private slots:
    void intenPBClicked();
    void alertPBClicked();
    void magPBClicked();
    void distPBClicked();

private:
    Ui::TabInfo *ui;

    void setIntenWG(double inten);
    void setAlertWG(int alert);
    void setMagWG(double mag);
    void setDistWG(double dist);
    void setWaveform(QString dir, QString fName, double ttime);

    QPushButton *intenPB;
    QPushButton *alertPB;
    QPushButton *magPB;
    QPushButton *distPB;

    QLabel *intenMLB, *intenBLB;
    QLabel *alertMLB, *alertBLB;
    QLabel *magMLB, *magBLB;
    QLabel *distMLB, *distBLB;

    QFont *topF;
    QFont *middleF;
    QFont *bottomF;

    /*
    QStringList intenList;
    QColor *intenColors;
    QStringList intenSubList;
    QStringList alertList;
    QColor *alertColors;
    QStringList magList;
    QStringList magSubList;
    QColor *magColors;
    QStringList distList;
    QColor *distColors;
    QStringList distSubList;
    */

    QChartView *intenChartView;
    QChart *intenChart;
    QChartView *alertChartView;
    QChart *alertChart;
    QChartView *magChartView;
    QChart *magChart;
    QChartView *distChartView;
    QChart *distChart;
};

#endif // TABINFO_H
