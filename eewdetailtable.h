#ifndef EEWDETAILTABLE_H
#define EEWDETAILTABLE_H

#include "common.h"

#include <QWidget>

namespace Ui {
class EewDetailTable;
}

class EewDetailTable : public QWidget
{
    Q_OBJECT

public:
    explicit EewDetailTable(QWidget *parent = 0);
    ~EewDetailTable();

    void setup(QString, QVector<_EEWInfo>);

private:
    Ui::EewDetailTable *ui;
};

#endif // EEWDETAILTABLE_H
