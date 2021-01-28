#ifndef ONSITEINFO_H
#define ONSITEINFO_H

#include "common.h"
#include "regends.h"

#include <QWidget>
#include <QPainter>

namespace Ui {
class OnsiteInfo;
}

class OnsiteInfo : public QWidget
{
    Q_OBJECT

public:
    explicit OnsiteInfo(QWidget *parent = 0);
    ~OnsiteInfo();

    void setup(_KGOnSite_Info_t);

private:
    Ui::OnsiteInfo *ui;

    void setIntenWG(double);
    void setAlertWG(int);
    void setMagWG(double);
    void setDistWG(double);
    void setType(QString);

    QTextCodec *codec;
};

#endif // ONSITEINFO_H
