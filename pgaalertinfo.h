#ifndef PGAALERTINFO_H
#define PGAALERTINFO_H

#include "common.h"

#include <QWidget>
#include <QPainter>
#include <QLabel>

namespace Ui {
class PgaAlertInfo;
}

class PgaAlertInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PgaAlertInfo(QWidget *parent = 0);
    ~PgaAlertInfo();

    void setup(_KGKIIS_GMPEAK_EVENT_STA_t);

private:
    Ui::PgaAlertInfo *ui;

    QTextCodec *codec;
};

#endif // PGAALERTINFO_H
