#ifndef SOHMONITOR_H
#define SOHMONITOR_H

#include "common.h"
#include "regends.h"

#include <QDialog>
#include <QTimer>

namespace Ui {
class SohMonitor;
}

class SohMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit SohMonitor();
    ~SohMonitor();

    void setup(_STATION);
    void update(_KGOnSite_SOH_t);

private:
    Ui::SohMonitor *ui;

    QTimer *doRepeatTimer;

private slots:
    void doRepeatWork();

signals:
    void sendSOHtoMainWindow(int, QString, QString);
};

#endif // SOHMONITOR_H
