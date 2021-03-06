#ifndef LISTINFO_H
#define LISTINFO_H

#include "common.h"
#include "selectvalues.h"

/*
#include "intensitydialog.h"
#include "alertdialog.h"
#include "magdialog.h"
#include "distancedialog.h"
*/

#include <QWidget>
#include <QPainter>

namespace Ui {
class ListInfo;
}

class ListInfo : public QWidget
{
    Q_OBJECT

public:
    explicit ListInfo(QWidget *parent = 0);
    ~ListInfo();

    void setup(_KGOnSite_Pick_t pick, _KGOnSite_Info_t info);

private:
    Ui::ListInfo *ui;

    void setIntenWG(double inten);
    void setAlertWG(int alert);
    void setMagWG(double mag);
    void setDistWG(double dist);
    void setType(QString net);

    QFont *topF;
    QFont *middleF;
    QFont *bottomF;
};

#endif // LISTINFO_H
