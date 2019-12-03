#ifndef PGAINFO_H
#define PGAINFO_H

#include "common.h"

#include <QWidget>
#include <QPainter>
#include <QLabel>

namespace Ui {
class PgaInfo;
}

class PgaInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PgaInfo(QWidget *parent = 0);
    ~PgaInfo();

    void setup(QString, int, QVector<_KGKIIS_GMPEAK_EVENT_STA_t>);

private:
    Ui::PgaInfo *ui;

    QTextCodec *codec;
};

#endif // PGAINFO_H
