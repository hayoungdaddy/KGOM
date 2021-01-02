#ifndef PGADETECTDETAILTABLE_H
#define PGADETECTDETAILTABLE_H

#include "common.h"

#include <QWidget>
#include <QDir>

namespace Ui {
class PgaDetectDetailTable;
}

class PgaDetectDetailTable : public QWidget
{
    Q_OBJECT

public:
    explicit PgaDetectDetailTable(QWidget *parent = 0);
    ~PgaDetectDetailTable();

    void setup(QString, QVector<_KGKIIS_GMPEAK_EVENT_STA_t>);

private:
    Ui::PgaDetectDetailTable *ui;
};

#endif // EEWDETAILTABLE_H
