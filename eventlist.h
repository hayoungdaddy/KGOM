#ifndef EVENTLIST_H
#define EVENTLIST_H

#include "common.h"
#include "regends.h"

#include <QWidget>
#include <QDateTime>
#include <QProcess>

namespace Ui {
class EventList;
}

class EventList : public QWidget
{
    Q_OBJECT

public:
    explicit EventList(int mode, _EEWInfo eewInfo, QString hd, QString homeDir, QWidget *parent = 0);
    ~EventList();

private:
    Ui::EventList *ui;

    QTextCodec *codec;

signals:
    void clicked(QString, QString, QString);
    void doubleClicked(QString);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // EVENTLIST_H
