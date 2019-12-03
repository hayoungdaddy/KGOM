#ifndef PGADETAILTABLE_H
#define PGADETAILTABLE_H

#include "common.h"
#include <QtCharts>

#include <QWidget>
#include <QDir>

namespace Ui {
class PgaDetailTable;
}

class PgaDetailTable : public QWidget
{
    Q_OBJECT

public:
    explicit PgaDetailTable(QWidget *parent = 0);
    ~PgaDetailTable();

    void setup(QString, QString, int, QVector<_KGKIIS_GMPEAK_EVENT_STA_t>, QString, QString, double, double);

private:
    Ui::PgaDetailTable *ui;

    void drawPGAChart(QString, QString, QString, double, double);
};

#endif // EEWDETAILTABLE_H
