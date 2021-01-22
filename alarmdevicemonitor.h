#ifndef ALARMDEVICEMONITOR_H
#define ALARMDEVICEMONITOR_H

#include <QDialog>
#include <QTimer>
#include <QTextCodec>
#include <QTcpSocket>
#include <QTcpServer>

#define RLAMP 2
#define YLAMP 3
#define GLAMP 4
#define SOUND 7

#define LAMP_ON    0x02
#define LAMP_BLINK 0x01
#define LAMP_OFF   0x00

#define STYPE_BB      0x01
#define STYPE_BpBp    0x02
#define STYPE_BppB    0x03
#define STYPE_pppp    0x04
#define STYPE_B___    0x05

namespace Ui {
class AlarmDeviceMonitor;
}

class AlarmDeviceMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmDeviceMonitor(QWidget *parent = nullptr);
    ~AlarmDeviceMonitor();

    void setup(QString, int);

    bool alarmDeviceStatus;

    char WCommand[10]={0x57,} ;
    char RCommand[10]={0x52,} ;
    char RecvAck[10]={0x00,} ;

    bool send_data(char *, int);
    int  receive(char *, int);

    QString ipaddress;
    int port;

public slots:
    void newConnection();
    void disconnected();
    void readyRead();

    void blinkRED();
    void blinkYELLOW();
    void blinkGREEN();
    void blinkTestGREEN();
    void blinkTestYELLOW();
    void blinkTestRED();
    void sound1Test();
    void sound2Test();
    void stopBlinkAll();

private:
    Ui::AlarmDeviceMonitor *ui;
    QTimer *doRepeatTimer;

    QTextCodec *codec;

    QTcpSocket *tcpsocket;
    QTcpServer *server;
    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;

private slots:
    void doRepeatWork();

signals:
    void dataReceived(QByteArray);
    void sendAlarmDeviceSOHtoMainWindow(int);
};

#endif // ALARMDEVICEMONITOR_H
