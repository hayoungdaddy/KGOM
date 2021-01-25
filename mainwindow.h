#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"
#include "writelog.h"
#include "eventlist.h"
#include "configurationwizard.h"
#include "logviewer.h"
#include "recvmessage.h"
#include "sohmonitor.h"
#include "onsiteinfo.h"
#include "eewinfo.h"
#include "pgainfo.h"
#include "pgaalertinfo.h"
#include "searchform.h"
#include "regends.h"
#include "detailview.h"
#include "aboutthis.h"
#include "configuration.h"
#include "tcpalarm.h"
#include "alarmdevicemonitor.h"

#include <QMainWindow>
#include <QQuickView>
#include <QtQuick>
#include <QProcessEnvironment>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QWidgetAction>
#include <QLCDNumber>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QMutex>

#include <QQmlApplicationEngine>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString configFile = 0, QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void setVisible(bool visible) override;

private:
    Ui::MainWindow *ui;

    _CONFIGURE configure;

    ConfigurationWizard *wizard;
    Configuration *configuration;
    DetailView *detailview;
    AboutThis *aboutthis;
    WriteLog *log;
    LogViewer *logviewer;

    RecvEEWMessage *krecveew;
    RecvOnsiteMessage *krecvOnsite;
    RecvSOHMessage *krecvSoh;
    RecvQSCD20Message *krecvQSCD;
    RecvOnsiteMessage *lrecvOnsite;
    RecvQSCD20Message *lrecvQSCD;
    RecvSOHMessage *lrecvSoh;

    // Search form widget
    SearchForm *searchform;
    int sfMagIndex;
    double sfMinMag, sfMaxMag;
    int sfDateIndex, sfNEvents, sfNDays;
    QDate sfStartDate, sfEndDate;

    QString lastObjectName;

    // About Read Configure
    void readConfigure(QString);

    // About decoration gui
    void decorationGUI();

    QTextCodec *codec;
    bool canAccessInternet;

    // About Tray mode
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    void createTrayActions();
    void createTrayIcon();

    // About events Map & communicating beetween qml and cpp
    QObject *eRootObj;
    QVariant eReturnedValue;
    QWidget *eMapContainer;

    // About alert Map & communicating beetween qml and cpp
    QObject *aRootObj;
    QVariant aReturnedValue;
    QWidget *aMapContainer;

    // About realTime Map & communicating beetween qml and cpp
    QObject *rRootObj;
    QVariant rReturnedValue;
    QWidget *rMapContainer;

    // About a Toolbar
    QLCDNumber *sysLN;
    QPushButton *stopBlinkPB;
    void createActionsOnToolbar();
    void createStaCircleOnRealTimePGAMap();
    QWidgetAction *sohLocalWA[MAX_LOCALSTA_NUM];
    QWidgetAction *sohKissWA[MAX_KISSSTA_NUM];
    SohMonitor *sohLocalMon[MAX_LOCALSTA_NUM];
    SohMonitor *sohKissMon[MAX_KISSSTA_NUM];
    QPushButton *sohLocalPB[MAX_LOCALSTA_NUM];
    QPushButton *sohKissPB[MAX_KISSSTA_NUM];

    QWidgetAction *alarmMonWA;
    AlarmDeviceMonitor *alarmMon;
    QPushButton *alarmMonPB;

    // About Database & table
    QSqlDatabase db;
    QSqlQueryModel *eventModel;
    QSqlQueryModel *onsiteModel;
    QSqlQueryModel *pgaModel;
    QSqlQueryModel *eewModel;
    QSqlQueryModel *pgaDetectModel;
    void setEventsTab(double, double, int, int, int, QDate, QDate);

    // About event
    int eventState; // 0: No Event,  1: Green Alarm,  2: Yellow Alarm,  3: Red Alarm
    double maxMag;
    QDateTime eventStartTimeUTC;
    QDateTime eventStartTimeKST;
    QTimer *blinkTimer;
    //ControlAlarm *controlAlarm;
    int getLastEvid();
    int getEventStateUsingMag(double, double, int);
    int getEventStateUsingInten(int, int);
    void alerting(int);
    _PGA_DETECTION pgaDetection;

    // About animations
    QDateTime endTimeP, endTimeS;
    double remainSecPAbsolute, remainSecSAbsolute;
    double remainSecPRelative, remainSecSRelative;
    int blinkCount;
    double aniLat, aniLon;
    QTimer *aniTimer;
    QString Intensity, Vibration;
    QColor blinkColor;

    // draw circle, marker on map
    void drawEEWOnMap(_EEWInfo);
    void drawOnsiteOnMap(int, _KGOnSite_Info_t);
    void drawPGAOnMap(_KGKIIS_GMPEAK_EVENT_STA_t);
    void drawIntensityOnMap(QString, QString);

    // for RealTime PGA
    QMutex mutex;
    QMultiMap<int, _QSCD_FOR_MULTIMAP> pgaHouse;
    void resetStaCircleOnMap();
    void setupPGATable();
    void adjustRealTimePGAMap();

private slots:
    void setup();
    void getEventInfo(int);
    void setAlertTab(QVector<_KGOnSite_Info_t>, QVector<_EEWInfo>, int, QVector<_KGKIIS_GMPEAK_EVENT_STA_t>, QVector<_KGKIIS_GMPEAK_EVENT_STA_t>, QString);
    void showViewDetail();

    // action slots
    void configureWizardActionTriggered();
    void configurationActionTriggered();
    void logViewerActionTriggered();
    void aboutthisActionTriggered();

    /* recv message from activeMQ */
    void rvEEWInfo(_EEWInfo);
    void rvOnsiteInfo(_KGOnSite_Info_t);
    void rvPGAMultiMap(QMultiMap<int, _QSCD_FOR_MULTIMAP>);
    void rvPGAInfos(_PGA_DETECTION);
    void rvSOHInfo(_KGOnSite_SOH_t, int, int);
    void recvSOHfromWG(int, QString, QString);

    // System time & Diff. time
    void doRepeatWork();
    void setDiffTime();

    void sohPBClicked();
    void alarmMonClicked();
    void recvAlarmDeviceSOHfromWG(int);

    void blinkingWindow();
    void stopBlinkPBClicked();

    void showSearchWindow();

    // for animation
    void showAnimation();

    void eventClicked(QString, QString, QString);
    void eventDoubleClicked(QString);

    // signal & slot with qml
    void _qmlSignalfromEMap(QString, QString, QString);

    void recvSearchOptions(int, double, double, int, int, int, QDate, QDate);

    void eventReplayPBClicked();
};

#endif // MAINWINDOW_H
