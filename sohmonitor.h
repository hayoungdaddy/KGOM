#ifndef SOHMONITOR_H
#define SOHMONITOR_H

#include "common.h"

#include <QDialog>

namespace Ui {
class SohMonitor;
}

class SohMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit SohMonitor();
    ~SohMonitor();

    void setup(_STATION sta, int type);

private:
    Ui::SohMonitor *ui;
};

#endif // SOHMONITOR_H
