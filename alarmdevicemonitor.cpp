#include "alarmdevicemonitor.h"
#include "ui_alarmdevicemonitor.h"

AlarmDeviceMonitor::AlarmDeviceMonitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlarmDeviceMonitor)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");

    //useAlarmSound = true;
    //ui->useAlarmSoundCB->setChecked(true);

    tcpsocket = new QTcpSocket();
    tcpsocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    //connect(tcpsocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    doRepeatTimer = new QTimer;
    doRepeatTimer->start(60000); // every 1 minutes
    connect(doRepeatTimer, SIGNAL(timeout()), this, SLOT(doRepeatWork()));

    connect(ui->greenBPB, SIGNAL(clicked()), this, SLOT(blinkTestGREEN()));
    connect(ui->orangeBPB, SIGNAL(clicked()), this, SLOT(blinkTestYELLOW()));
    connect(ui->redBPB, SIGNAL(clicked()), this, SLOT(blinkTestRED()));
    connect(ui->greenSPB, SIGNAL(clicked()), this, SLOT(stopBlinkAll()));
    connect(ui->orangeSPB, SIGNAL(clicked()), this, SLOT(stopBlinkAll()));
    connect(ui->redSPB, SIGNAL(clicked()), this, SLOT(stopBlinkAll()));
    connect(ui->sound1PB, SIGNAL(clicked()), this, SLOT(sound1Test()));
    connect(ui->sound2PB, SIGNAL(clicked()), this, SLOT(sound2Test()));
    connect(ui->stopSoundPB, SIGNAL(clicked()), this, SLOT(stopBlinkAll()));
    connect(ui->stopAllPB, SIGNAL(clicked()), this, SLOT(stopBlinkAll()));

    connect(ui->useAlarmSoundCB, SIGNAL(clicked()), this, SLOT(useAlarmSoundCBClicked()));
}

AlarmDeviceMonitor::~AlarmDeviceMonitor()
{
    tcpsocket->close();
    delete ui;
}

bool AlarmDeviceMonitor::send_data(char *data, int len)
{
    if(tcpsocket->state() == QAbstractSocket::ConnectedState)
    {
        tcpsocket->write(data, len);
        //qDebug() << "sendOK";
        return tcpsocket->waitForBytesWritten();
    }
    else
    {
        //qDebug() <<"sendFail";
        return false;
    }
}

void AlarmDeviceMonitor::doRepeatWork()
{
    //qDebug() << tcpsocket->state();

    if(tcpsocket->state() == QAbstractSocket::ConnectedState)
    {
        ui->statusLB->setStyleSheet("background-color: rgb(29, 219, 22);");
        ui->statusLB->setText(codec->toUnicode("상태 : 경보기 연결됨"));
        emit sendAlarmDeviceSOHtoMainWindow(1);
    }
    else
    {
        ui->statusLB->setStyleSheet("background-color: rgb(255, 51, 51);");
        ui->statusLB->setText(codec->toUnicode("상태 : 경보기 연결 해제됨 (재연결 중)"));
        newConnection();
        emit sendAlarmDeviceSOHtoMainWindow(0);
    }
}

void AlarmDeviceMonitor::useAlarmSoundCBClicked()
{
    if(ui->useAlarmSoundCB->isChecked())
        useAlarmSound = true;
    else
        useAlarmSound = false;

    qDebug() << useAlarmSound;

    emit sendAlarmSoundtoMainWindow(useAlarmSound);
}

void AlarmDeviceMonitor::setup(QString ipS, int portI ,int use_alarm_sound)
{
    ipaddress = ipS;
    port = portI;

    if(use_alarm_sound == 1)
    {
        useAlarmSound = true;
        ui->useAlarmSoundCB->setChecked(true);
    }
    else
    {
        useAlarmSound = false;
        ui->useAlarmSoundCB->setChecked(false);
    }

    newConnection();
}

void AlarmDeviceMonitor::readyRead()
{
    while(tcpsocket->bytesAvailable() > 0)
    {
        //qDebug() << tcpsocket->readAll();
    }
}

void AlarmDeviceMonitor::disconnected()
{
    tcpsocket->close();
}

void AlarmDeviceMonitor::blinkRED()
{
    WCommand[RLAMP] = LAMP_ON ;

    if(useAlarmSound)
        WCommand[SOUND] = STYPE_B___ ;

    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::blinkYELLOW()
{
    WCommand[YLAMP] = LAMP_ON ;

    if(useAlarmSound)
        WCommand[SOUND] = STYPE_pppp ;
    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::blinkGREEN()
{
    WCommand[GLAMP] = LAMP_ON ;
    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::stopBlinkAll()
{
    memset( WCommand+1, 0x00, 9 ) ;
    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::blinkTestGREEN()
{
    WCommand[GLAMP] = LAMP_ON ;

    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::blinkTestYELLOW()
{
    WCommand[YLAMP] = LAMP_ON ;
    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::blinkTestRED()
{
    WCommand[RLAMP] = LAMP_ON ;
    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::sound1Test()
{
    WCommand[SOUND] = STYPE_B___ ;
    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::sound2Test()
{
    WCommand[SOUND] = STYPE_pppp ;
    send_data(WCommand, 10);
}

void AlarmDeviceMonitor::newConnection()
{
    tcpsocket->connectToHost(ipaddress, port);
    if(!tcpsocket->waitForConnected(3000))
    {
        ui->statusLB->setText(codec->toUnicode("상태 : 경보기 연결 해제됨 (재연결 중)"));
        emit sendAlarmDeviceSOHtoMainWindow(0);
        return;
    }

    if(tcpsocket->state() == QAbstractSocket::ConnectedState)
    {
        ui->statusLB->setStyleSheet("background-color: rgb(29, 219, 22);");
        ui->statusLB->setText(codec->toUnicode("상태 : 경보기 연결됨"));
        emit sendAlarmDeviceSOHtoMainWindow(1);
    }
}
