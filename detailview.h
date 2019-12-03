#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include "common.h"
#include "regends.h"
#include "eewdetailtable.h"
#include "onsitedetailtable.h"
#include "pgadetailtable.h"

#include <QDialog>

namespace Ui {
class DetailView;
}

class DetailView : public QDialog
{
    Q_OBJECT

public:
    explicit DetailView(QWidget *parent = 0);
    ~DetailView();

    void setup(QVector<_KGOnSite_Info_t>, QVector<_EEWInfo>, QString, int, QVector<_KGKIIS_GMPEAK_EVENT_STA_t>, QString, QString);

private:
    Ui::DetailView *ui;

    QTextCodec *codec;
};

#endif // DETAILVIEW_H
