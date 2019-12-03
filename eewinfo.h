#ifndef EEWINFO_H
#define EEWINFO_H

#include "common.h"
#include "regends.h"

#include <QWidget>

namespace Ui {
class EewInfo;
}

class EewInfo : public QWidget
{
    Q_OBJECT

public:
    explicit EewInfo(QWidget *parent = 0);
    ~EewInfo();

    void setup(_EEWInfo, QString, double, double);

private:
    Ui::EewInfo *ui;

    QFont *topF;
    QFont *middleF;
    QFont *bottomF;

    QTextCodec *codec;
};

#endif // EEWINFO_H
