#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"
#include "tabinfo.h"
#include "listinfo.h"
#include "eewinfo.h"
#include "qcustomplot.h"
#include "configurationwizard.h"
#include "writelog.h"
#include "reportviewer.h"
#include "sohmonitor.h"
#include "onsitechart.h"
#include "recvmessage.h"
#include "logviewer.h"

#include <QMainWindow>
#include <QQuickView>
#include <QtQuick>
#include <QProcessEnvironment>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QWidgetAction>
#include <QPushButton>
#include <QLCDNumber>
#include <QLabel>
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

    // About Tray mode
    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow *ui;

    QObject *rootObj;
    QVariant returnedValue;
    _CONFIGURE configure;
    QWidget *mapContainer;

    WriteLog *log;

    int maxHeight;

    /* About Tray mode */
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    void createTrayActions();
    void createTrayIcon();

    /* About SOH on Toolbar */
    QLCDNumber *sysLN;
    QPushButton *stopBlinkBP;
    void createActionsOnToolbar();
    QWidgetAction *sohLocalWA[MAX_LOCALSTA_NUM];
    QWidgetAction *sohKissWA[MAX_KISSSTA_NUM];
    SohMonitor *sohLocalMon[MAX_LOCALSTA_NUM];
    SohMonitor *sohKissMon[MAX_KISSSTA_NUM];
    QPushButton *sohLocalPB[MAX_LOCALSTA_NUM];
    QPushButton *sohKissPB[MAX_KISSSTA_NUM];

    /* Abount Timezone action */
    void gmtActionTriggered();
    void kstActionTriggered();

    /* about Database & table */
    QSqlDatabase db;
    QSqlQueryModel *model;
    QSqlQueryModel *model2;
    QSqlQueryModel *model3;

    QSqlQueryModel *evidModel;
    QSqlQueryModel *pickModel;
    QSqlQueryModel *onsiteModel;
    QSqlQueryModel *eewModel;
    QSqlQueryModel *eewModel2;
    void setEventTable(QDate sDate, QDate eDate, int type);
    void setOnsiteTable(_KGOnSite_Pick_t pick);
    void setEEWTable(_EEWInfo eewInfo);

    /* onsite charts */
    OnsiteChart *onsitechart;

    void readConfigure(QString configFile);

    /* Configure Wizard */
    ConfigurationWizard *wizard;

    /* draw circle, marker on map */
    void drawEEWOnMap(_EEWInfo);
    void drawOnsiteOnMap(int, _KGOnSite_Pick_t, _KGOnSite_Info_t, int);

    /* for event */
    int eventMode;
    //int lastEventID;
    QDateTime eventStartTime;
    double eventFirstStationLat;
    double eventFirstStationLon;
    QTimer *bTimer;
    void processEvent(int);
    int getLastEvid();

    /* for recv. thread */
    RecvEEWMessage *recveew;
    QThread *recvThread;
    RecvMessageUDP *recvMessageUDP;

    /* for animation */
    double timeP, timeS, count;
    double aniLat, aniLon;
    QTimer *aniTimer;

private slots:
    void setup();
    void setSummaryTab(QVector<_KGOnSite_Pick_t> picks, QVector<_KGOnSite_Info_t> infos, QVector<_EEWInfo> eewInfos, QString evid);

    /* action slots */
    void actionConfigureWizardTriggered();
    void listModeActionTriggered();
    void tabModeActionTriggered();
    void allModeActionTriggered();
    void logViewerActionTriggered();

    void showSysTime();
    void setDiffTime();

    /* table clicked */
    void eventTWClicked(int, int);
    void eventTWDoubleClicked(int, int);
    void clearPBClicked();
    void daysReadPBClicked();
    void dateReadPBClicked();

    void sohPBClicked();

    /* recv message from activeMQ */
    void rvPickInfo(unsigned char *);
    void rvOnsiteInfo(unsigned char *);
    void rvWaveform(unsigned char *);
    void rvSOH(unsigned char *);
    void rvEEWInfo(_EEWInfo);

    void blinkingWindow();
    void stopBlinkPBClicked();

    /* for animation */
    void showAnimation();
};

#endif // MAINWINDOW_H
