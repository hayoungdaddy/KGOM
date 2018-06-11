#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString configFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMenuBar(ui->menuBar);

    qRegisterMetaType<_EEWInfo>("_EEWInfo");

    eventMode = 0;
    bTimer = new QTimer;
    bTimer->stop();
    connect(bTimer, SIGNAL(timeout()), this, SLOT(blinkingWindow()));

    aniTimer = new QTimer;
    aniTimer->stop();
    connect(aniTimer, SIGNAL(timeout()), this, SLOT(showAnimation()));

    QRect rec = QApplication::desktop()->screenGeometry();
    maxHeight = rec.height();

    // default mode is list mode
    ui->actionList->setEnabled(false);
    ui->tabWG->hide();

    // onsite charts on events tab
    onsitechart = new OnsiteChart(this);
    ui->chartLO->addWidget(onsitechart);

    // Tab view init.
    ui->tabWG->removeTab(0);

    // Setup Environment
    configure.KGONSITE_HOME = QProcessEnvironment::systemEnvironment().value("KGONSITE_HOME");
    configure.configFileName = configFile;

    // define consumer for eewinfo
    /*
    recveew = new RecvEEWMessage("failover:(tcp://210.98.8.82:61616)", "elarms",
                                        "h3ePs7bn", "elarms.data.client", true, false);
    recveew->start();
    connect(recveew, SIGNAL(_rvEEWInfo(_EEWInfo)), this, SLOT(rvEEWInfo(_EEWInfo)));
    */

    // define socket for onsite
    recvMessageUDP = new RecvMessageUDP();
    recvThread = new QThread(this);
    connect(recvMessageUDP, SIGNAL(_rvPickInfo(unsigned char *)), this, SLOT(rvPickInfo(unsigned char *)));
    connect(recvMessageUDP, SIGNAL(_rvOnsiteInfo(unsigned char *)), this, SLOT(rvOnsiteInfo(unsigned char *)));

    // for wizard
    wizard = new ConfigurationWizard(this);
    wizard->hide();
    connect(wizard, SIGNAL(resetMainWindow()), this, SLOT(setup()));

    // for Logging
    log = new WriteLog();
    log->write(configure.KGONSITE_HOME + "/logs/", "======================================================");
    log->write(configure.KGONSITE_HOME + "/logs/", "KGOnSite Started.");

    // About Tray mode
    createTrayActions();
    createTrayIcon();
    QIcon icon(":/Icon/icon1.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->show();

    // for Action
    ui->actionWizard->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    ui->actionList->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    ui->actionTab->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    ui->actionAll->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    ui->actionViewLogs->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    connect(ui->actionList, &QAction::triggered, this, &MainWindow::listModeActionTriggered);
    connect(ui->actionTab, &QAction::triggered, this, &MainWindow::tabModeActionTriggered);
    connect(ui->actionAll, &QAction::triggered, this, &MainWindow::allModeActionTriggered);
    connect(ui->actionWizard, &QAction::triggered, this, &MainWindow::actionConfigureWizardTriggered);
    connect(ui->actionViewLogs, &QAction::triggered, this, &MainWindow::logViewerActionTriggered);

    // SOH widget declaration
    for(int i=0;i<MAX_LOCALSTA_NUM;i++)
    {
        sohLocalWA[i] = new QWidgetAction(this);
        sohLocalPB[i] = new QPushButton;
        sohLocalMon[i] = new SohMonitor;
    }
    for(int i=0;i<MAX_KISSSTA_NUM;i++)
    {
        sohKissWA[i] = new QWidgetAction(this);
        sohKissPB[i] = new QPushButton;
        sohKissMon[i] = new SohMonitor;
    }

    // QTimer & QLCDNumber for system time
    QTimer *systemTimer = new QTimer;
    connect(systemTimer, SIGNAL(timeout()), this, SLOT(showSysTime()));
    systemTimer->start(1000);
    sysLN = new QLCDNumber();
    sysLN->setDigitCount(8);
    sysLN->setFixedWidth(130);
    sysLN->setPalette(Qt::black);
    sysLN->setStyleSheet("color:black;");

    QTimer *diffTimer = new QTimer(this);
    diffTimer->start(1000);
    connect(diffTimer, SIGNAL(timeout()), this, SLOT(setDiffTime()));

    // Setting ToolBar
    //addToolBar(Qt::BottomToolBarArea, ui->toolBar);
    QLabel *sysTimeLB = new QLabel;
    sysTimeLB->setText(tr("System Time (KST) : "));

    stopBlinkBP = new QPushButton("Stop Blinking");

    QWidget *empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    ui->mainToolBar->addWidget(sysTimeLB);
    ui->mainToolBar->addWidget(sysLN);
    ui->mainToolBar->addWidget(stopBlinkBP);

    connect(stopBlinkBP, SIGNAL(clicked(bool)), this, SLOT(stopBlinkPBClicked()));
    ui->mainToolBar->addWidget(empty);
    ui->mainToolBar->actions().at(2)->setVisible(false);

    // Load a osm map
    QQuickView *view = new QQuickView();
    mapContainer = QWidget::createWindowContainer(view, this);
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    mapContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mapContainer->setFocusPolicy(Qt::TabFocus);
    view->setSource(QUrl(QStringLiteral("qrc:/Viewmap.qml")));

    ui->mapLO->addWidget(mapContainer);

    rootObj = view->rootObject();

    QMetaObject::invokeMethod(this->rootObj, "mapReset", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, configure.KGONSITE_HOME + "/params/maps"), Q_ARG(QVariant, "osm"));

    QMetaObject::invokeMethod(this->rootObj, "clearMap", Q_RETURN_ARG(QVariant, returnedValue));

    // create station marker on map (just create, do not show)
    QMetaObject::invokeMethod(this->rootObj, "createLocalStaMarker", Q_RETURN_ARG(QVariant, returnedValue));
    QMetaObject::invokeMethod(this->rootObj, "createKissStaMarker", Q_RETURN_ARG(QVariant, returnedValue));
    QMetaObject::invokeMethod(this->rootObj, "createEEWStarMarker", Q_RETURN_ARG(QVariant, returnedValue));
    QMetaObject::invokeMethod(this->rootObj, "createMyPositionMarker", Q_RETURN_ARG(QVariant, returnedValue));

    // Open DB
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(configure.KGONSITE_HOME + "/data/KGom.db");
    if(!db.open())
    {
        QMessageBox msgBox;
        msgBox.setText(db.lastError().text());
        log->write(configure.KGONSITE_HOME + "/logs/", "Failed openning a database. (" +
                   configure.KGONSITE_HOME + "/data/KGom.db)");
        msgBox.exec();
        exit(1);
    }
    log->write(configure.KGONSITE_HOME + "/logs/", "Succeed opening a database. <" +
               configure.KGONSITE_HOME + "/data/KGom/db>");
    this->model = new QSqlQueryModel();
    this->model2 = new QSqlQueryModel();
    this->model3 = new QSqlQueryModel();

    this->evidModel = new QSqlQueryModel();
    this->pickModel = new QSqlQueryModel();
    this->onsiteModel = new QSqlQueryModel();
    this->eewModel = new QSqlQueryModel();
    this->eewModel2 = new QSqlQueryModel();

    // table connections
    connect(ui->eventsTW, SIGNAL(cellClicked(int,int)), this, SLOT(eventTWClicked(int, int)));
    connect(ui->eventsTW, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(eventTWDoubleClicked(int,int)));
    connect(ui->clearPB, SIGNAL(clicked(bool)), this, SLOT(clearPBClicked()));
    connect(ui->daysReadPB, SIGNAL(clicked(bool)), this, SLOT(daysReadPBClicked()));
    connect(ui->dateReadPB, SIGNAL(clicked(bool)), this, SLOT(dateReadPBClicked()));
    QDate sDate, eDate;
    eDate = QDate::currentDate();
    sDate = eDate.addDays(- DEFAULT_READ_EVENTS_FOR_DAY);
    ui->endDE->setDate(eDate);
    ui->startDE->setDate(sDate);

    //log->write(configure.KGONSITE_HOME + "/logs/", "======================================================");

    // start programs
    setup();
}

MainWindow::~MainWindow()
{
    log->write(configure.KGONSITE_HOME + "/logs/", "KGOnSite Terminated.");
    delete ui;
}

void MainWindow::setup()
{
    eventMode = 0;
    readConfigure(configure.configFileName);
    createActionsOnToolbar();

    // set Tables
    QDateTime sDateTime, eDateTime = QDateTime::currentDateTimeUtc();
    sDateTime = convertKST(sDateTime);
    eDateTime = convertKST(eDateTime);
    sDateTime = eDateTime.addDays(- DEFAULT_READ_EVENTS_FOR_DAY);
    QDate sDate, eDate;
    sDate.setDate(sDateTime.toString("yyyy").toInt(), sDateTime.toString("MM").toInt(), sDateTime.toString("dd").toInt());
    eDate.setDate(eDateTime.toString("yyyy").toInt(), eDateTime.toString("MM").toInt(), eDateTime.toString("dd").toInt());
    setEventTable(sDate, eDate, 1);

    // define consumer for onsiteinfo
    recvMessageUDP->setup("127.0.0.1", 9987);
    recvMessageUDP->moveToThread(recvThread);
    recvThread->start();

    // define consumer for eewinfo
    //recveew = new RecvEEWMessage("failover:(tcp://210.98.8.82:61616)", "elarms", "h3ePs7bn", "elarms.data.client", true, false);
    QString failover = "failover:(tcp://" + configure.eew_ip + ":" + configure.eew_port + ")";
    recveew = new RecvEEWMessage(failover, configure.eew_user, configure.eew_passwd, configure.eew_topic, true, false);
    recveew->start();
    connect(recveew, SIGNAL(_rvEEWInfo(_EEWInfo)), this, SLOT(rvEEWInfo(_EEWInfo)));
}

void MainWindow::setEventTable(QDate sDate, QDate eDate, int type)
{
    QString query;
    query = "SELECT * FROM event WHERE lddate >= " + sDate.toString("yyyyMMdd") +
            " AND lddate <= " + eDate.toString("yyyyMMdd") + " ORDER BY evid DESC";
    this->evidModel->setQuery(query);

    QVector<int> evidV, nSpanV;

    ui->eventsTW->setRowCount(0);

    for(int i=0;i<this->evidModel->rowCount();++i)
    {
        query = "SELECT * FROM pickInfo WHERE evid = " + this->evidModel->record(i).value("evid").toString();
        this->pickModel->setQuery(query);

        if(this->pickModel->rowCount() > 0)
        {
            for(int j=0;j<this->pickModel->rowCount();++j)
            {
                QString scnl;
                scnl = this->pickModel->record(j).value("sta").toString() + "/" + this->pickModel->record(j).value("chan").toString() + "/" +
                        this->pickModel->record(j).value("net").toString() + "/" + this->pickModel->record(j).value("loc").toString();
                QDateTime ttime;
                ttime.setTimeSpec(Qt::UTC);
                ttime.setTime_t(this->pickModel->record(j).value("ttime").toInt());

                // check onsiteInfo table
                query = "select * from onsiteInfo where sta = '" +
                        scnl.section("/", 0, 0) + "' and chan = '" +
                        scnl.section("/", 1, 1) + "' and net = '" +
                        scnl.section("/", 2, 2) + "' and loc = '" +
                        scnl.section("/", 3, 3) + "' and ttime = " + this->pickModel->record(j).value("ttime").toString();

                this->onsiteModel->setQuery(query);

                if(this->onsiteModel->rowCount() == 0)
                    continue;

                ttime = convertKST(ttime);

                ui->eventsTW->setRowCount(ui->eventsTW->rowCount()+1);
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 0, new QTableWidgetItem(this->pickModel->record(j).value("evid").toString()));
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 1, new QTableWidgetItem(ttime.toString("yyyy-MM-dd hh:mm:ss")));
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 2, new QTableWidgetItem(scnl));
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 3, new QTableWidgetItem(this->pickModel->record(j).value("polarity").toString()));
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 4, new QTableWidgetItem(this->pickModel->record(j).value("weight").toString()));
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 5, new QTableWidgetItem(this->pickModel->record(j).value("amplitude").toString()));
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 6, new QTableWidgetItem(this->pickModel->record(j).value("period").toString()));
                ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 7, new QTableWidgetItem("View"));

                if(evidV.isEmpty())
                {
                    evidV.push_back(this->pickModel->record(j).value("evid").toInt());
                    nSpanV.push_back(1);
                }
                else
                {
                    if(evidV.last() == this->pickModel->record(j).value("evid").toInt())
                    {
                        nSpanV.last()++;
                    }
                    else
                    {
                        evidV.push_back(this->pickModel->record(j).value("evid").toInt());
                        nSpanV.push_back(1);
                    }
                }
            }
        }

        query = "SELECT * FROM eewInfo WHERE evid = " + this->evidModel->record(i).value("evid").toString();
        this->eewModel->setQuery(query);

        if(this->eewModel->rowCount() > 0)
        {
            /*
            QVector<int> eew_evids;

            for(int j=0;j<this->eewModel->rowCount();++j)
            {
                if(eew_evids.empty())
                    eew_evids.push_back(this->eewModel->record(j).value("eew_evid").toInt());
                else
                {
                    if(!eew_evids.indexOf(this->eewModel->record(j).value("eew_evid").toInt()))
                        eew_evids.push_back(this->eewModel->record(j).value("eew_evid").toInt());
                }
            }

            for(int j=0;j<eew_evids.count();j++)
            {
                query = "SELECT * FROM eewInfo WHERE eew_evid = " + QString::number(eew_evids.at(j)) +
                        " and evid = " + this->evidModel->record(i).value("evid").toString();
                this->eewModel2->setQuery(query);

                for(int k=0;k<this->eewModel2->rowCount();++k)
                {
                    QString scnl = tr("Earthquake Infomation");
                    QDateTime ttime;
                    ttime.setTimeSpec(Qt::UTC);
                    ttime.setTime_t(this->eewModel2->record(k).value("eew_evid").toInt());
                    */

            QVector<double> eew_origin_time;

            for(int j=0;j<this->eewModel->rowCount();++j)
            {
                if(eew_origin_time.empty())
                    eew_origin_time.push_back(this->eewModel->record(j).value("origin_time").toDouble());
                else
                {
                    if(!eew_origin_time.indexOf(this->eewModel->record(j).value("origin_time").toDouble()))
                        eew_origin_time.push_back(this->eewModel->record(j).value("origin_time").toDouble());
                }
            }

            for(int j=0;j<eew_origin_time.count();j++)
            {
                query = "SELECT * FROM eewInfo WHERE origin_time = " + QString::number(eew_origin_time.at(j), 'f', 3) +
                        " and evid = " + this->evidModel->record(i).value("evid").toString();
                this->eewModel2->setQuery(query);

                for(int k=0;k<this->eewModel2->rowCount();++k)
                {
                    QString scnl = tr("Earthquake Infomation");
                    QDateTime ttime;
                    ttime.setTimeSpec(Qt::UTC);
                    ttime.setTime_t(this->eewModel2->record(k).value("origin_time").toInt());

                    ttime = convertKST(ttime);

                    ui->eventsTW->setRowCount(ui->eventsTW->rowCount()+1);
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 0, new QTableWidgetItem(this->eewModel2->record(k).value("evid").toString()));
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 1, new QTableWidgetItem(ttime.toString("yyyy-MM-dd hh:mm:ss")));
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 2, new QTableWidgetItem(scnl));
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 3, new QTableWidgetItem("-"));
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 4, new QTableWidgetItem("-"));
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 5, new QTableWidgetItem("-"));
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 6, new QTableWidgetItem("-"));
                    ui->eventsTW->setItem(ui->eventsTW->rowCount()-1, 7, new QTableWidgetItem("View"));

                    if(evidV.isEmpty())
                    {
                        evidV.push_back(this->eewModel2->record(j).value("evid").toInt());
                        nSpanV.push_back(1);
                    }
                    else
                    {
                        if(evidV.last() == this->eewModel2->record(j).value("evid").toInt())
                        {
                            nSpanV.last()++;
                        }
                        else
                        {
                            evidV.push_back(this->eewModel2->record(j).value("evid").toInt());
                            nSpanV.push_back(1);
                        }
                    }
                }
            }
        }
    }

    ui->eventsTW->setCurrentCell(0, 0);

    for(int i=0;i<ui->eventsTW->rowCount();i++)
    {
        for(int j=0;j<ui->eventsTW->columnCount();j++)
        {
            ui->eventsTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
            //ui->eventsTW->item(i, j)->setFlags(Qt::ItemIsEditable);
        }
    }

    for(int i=0;i<ui->eventsTW->columnCount();i++)
        ui->eventsTW->setColumnWidth(i, (this->width()-50) / (ui->eventsTW->columnCount()+2));
    ui->eventsTW->setColumnWidth(1, ((this->width()-18) / (ui->eventsTW->columnCount()+2)) * 2);
    ui->eventsTW->setColumnWidth(2, ((this->width()-18) / (ui->eventsTW->columnCount()+2)) * 2);

    // set span for evid, report
    int nRow = 0;
    for(int i=0;i<nSpanV.count();i++)
    {
        if(nSpanV.at(i) != 1)
        {
            ui->eventsTW->setSpan(nRow, 0, nSpanV.at(i), 1);
            ui->eventsTW->setSpan(nRow, 7, nSpanV.at(i), 1);
        }
        nRow = nRow + nSpanV.at(i);
    }

    eventTWClicked(0, 0);

    if(type == 1)
        eventTWDoubleClicked(0, 1);


    qDebug() << "Completed setting event table";
}

void MainWindow::eventTWClicked(int row, int col)
{ 
    QString scnl;
    scnl = ui->eventsTW->item(row, 2)->text(); 
    QDateTime tt = QDateTime::fromString(ui->eventsTW->item(row, 1)->text(), "yyyy-MM-dd hh:mm:ss");
    tt.setTimeSpec(Qt::UTC);
    tt = convertUTC(tt);   
    ui->onsiteLB->setText(ui->eventsTW->item(row, 1)->text() + " " + scnl);
    ui->eventsTW->setCurrentCell(row, 1);

    if(scnl.startsWith("Earthquake"))
    {
        _EEWInfo eewInfo;
        eewInfo.kgom_event_id = ui->eventsTW->item(row, 0)->text();
        ui->onsiteTW->hide();
        ui->eewTW->show();
        setEEWTable(eewInfo);
        ui->chartFrame->hide();
    }
    else
    {
        _KGOnSite_Pick_t pick;
        strcpy(pick.sta, scnl.section("/", 0, 0).toLatin1().constData());
        strcpy(pick.chan, scnl.section("/", 1, 1).toLatin1().constData());
        strcpy(pick.net, scnl.section("/", 2, 2).toLatin1().constData());
        strcpy(pick.loc, scnl.section("/", 3, 3).toLatin1().constData());
        pick.ttime = tt.toTime_t();

        ui->onsiteTW->show();
        ui->eewTW->hide();
        setOnsiteTable(pick);
        ui->chartFrame->show();
    }

    QString evid;
    if(col == 7) // Report viewer
    {
        evid = ui->eventsTW->item(row, 0)->text();
        QString query = "SELECT * FROM pickInfo WHERE evid =" + evid;
        this->model->setQuery(query);

        QVector<_REPORT> reportV;

        for(int i=0;i<this->model->rowCount();i++)
        {
            query = "select * from onsiteInfo where sta = '" +
                    this->model->record(i).value("sta").toString() + "' and chan = '" +
                    this->model->record(i).value("chan").toString() + "' and net = '" +
                    this->model->record(i).value("net").toString() + "' and loc = '" +
                    this->model->record(i).value("loc").toString() + "' and ttime = " +
                    this->model->record(i).value("ttime").toString();
            this->model2->setQuery(query);

            if(this->model2->rowCount() <= 0)
                continue;

            _REPORT report;

            strcpy(report.pick.sta, this->model->record(i).value("sta").toString().toLatin1().constData());
            strcpy(report.pick.chan, this->model->record(i).value("chan").toString().toLatin1().constData());
            strcpy(report.pick.net, this->model->record(i).value("net").toString().toLatin1().constData());
            strcpy(report.pick.loc, this->model->record(i).value("loc").toString().toLatin1().constData());
            report.pick.lat = this->model->record(i).value("lat").toDouble();
            report.pick.lon = this->model->record(i).value("lon").toDouble();
            report.pick.elev = this->model->record(i).value("elev").toDouble();
            report.pick.ttime = this->model->record(i).value("ttime").toDouble();
            report.pick.polarity = this->model->record(i).value("polarity").toString().at(0).toLatin1();
            report.pick.weight = this->model->record(i).value("weight").toInt();
            report.pick.amplitude = this->model->record(i).value("amplitude").toInt();
            report.pick.period = this->model->record(i).value("period").toDouble();
            //QString temp = configure.KGONSITE_HOME + "/data/" + this->model->record(0).value("dir").toString();
            //strcpy(report.pick.dir, temp.toLatin1().constData());
            //strcpy(report.pick.dfile, this->model->record(0).value("dfile").toString().toLatin1().constData());

            /*
            query = "select * from onsiteInfo where sta = '" + QString(report.pick.sta) + "' and chan = '" + QString(report.pick.chan) + "' and net = '" +
                    QString(report.pick.net) + "' and loc = '" + QString(report.pick.loc) + "' and ttime = " + QString::number(report.pick.ttime, 'f', 6);
            this->model2->setQuery(query);
            */

            for(int j=0;j<this->model2->rowCount();j++)
            {
                _KGOnSite_Info_t info;
                info.duration = this->model2->record(j).value("duration").toString().at(0).toLatin1();
                info.alert = this->model2->record(j).value("alert").toInt();
                info.intensity = this->model2->record(j).value("intensity").toDouble();
                info.magnitude = this->model2->record(j).value("magnitude").toDouble();
                info.distance = this->model2->record(j).value("distance").toDouble();
                report.infos.push_back(info);
            }

            reportV.push_back(report);
        }

        ReportViewer *report = new ReportViewer(evid, reportV, configure);
        report->show();
    }
}

void MainWindow::eventTWDoubleClicked(int row, int col)
{
    QString evid = ui->eventsTW->item(row, 0)->text();
    QString query;

    QVector<_KGOnSite_Pick_t> picks;
    QVector<_KGOnSite_Info_t> infos;
    QVector<_EEWInfo> eewInfos;

    query = "select * from pickInfo where evid =" + evid;
    this->pickModel->setQuery(query);

    for(int i=0;i<this->pickModel->rowCount();i++)
    {  
        _KGOnSite_Pick_t pick;
        strcpy(pick.sta, this->pickModel->record(i).value("sta").toString().toLatin1().constData());
        strcpy(pick.chan, this->pickModel->record(i).value("chan").toString().toLatin1().constData());
        strcpy(pick.net, this->pickModel->record(i).value("net").toString().toLatin1().constData());
        strcpy(pick.loc, this->pickModel->record(i).value("loc").toString().toLatin1().constData());
        pick.lat = this->pickModel->record(i).value("lat").toDouble();
        pick.lon = this->pickModel->record(i).value("lon").toDouble();
        pick.elev = this->pickModel->record(i).value("elev").toDouble();
        pick.ttime = this->pickModel->record(i).value("ttime").toDouble();

        query = "select * from onsiteInfo where sta = '" + QString(pick.sta) + "' and chan = '" + QString(pick.chan) + "' and net = '" +
                QString(pick.net) + "' and loc = '" + QString(pick.loc) + "' and ttime = " + QString::number(pick.ttime, 'f', 6);
        this->onsiteModel->setQuery(query);

        if(this->onsiteModel->rowCount() > 0) // input last onsiteinfo only to infos.
        {
            picks.push_back(pick);

            _KGOnSite_Info_t info;
            info.duration = this->onsiteModel->record(this->onsiteModel->rowCount()-1).value("duration").toInt();
            info.alert = this->onsiteModel->record(this->onsiteModel->rowCount()-1).value("alert").toInt();
            info.intensity = this->onsiteModel->record(this->onsiteModel->rowCount()-1).value("intensity").toDouble();
            info.magnitude = this->onsiteModel->record(this->onsiteModel->rowCount()-1).value("magnitude").toDouble();
            info.distance = this->onsiteModel->record(this->onsiteModel->rowCount()-1).value("distance").toDouble();
            infos.push_back(info);
        }
    }

    query = "select * from eewInfo where evid = " + evid;
    this->eewModel->setQuery(query);

    QVector<int> eew_evids;

    for(int i=0;i<this->eewModel->rowCount();++i)
    {
        if(eew_evids.empty())
            eew_evids.push_back(this->eewModel->record(i).value("eew_evid").toInt());
        else
        {
            if(!eew_evids.indexOf(this->eewModel->record(i).value("eew_evid").toInt()))
                eew_evids.push_back(this->eewModel->record(i).value("eew_evid").toInt());
        }
    }

    for(int i=0;i<eew_evids.count();i++)
    {
        query = "SELECT * FROM eewInfo WHERE eew_evid = " + QString::number(eew_evids.at(i)) +
                " and evid = " + evid;
        this->eewModel2->setQuery(query);

        for(int k=0;k<this->eewModel2->rowCount();++k)
        {
            _EEWInfo eewInfo;
            eewInfo.magnitude = this->eewModel2->record(this->eewModel2->rowCount()-1).value("magnitude").toDouble();
            eewInfo.latitude = this->eewModel2->record(this->eewModel2->rowCount()-1).value("latitude").toDouble();
            eewInfo.longitude = this->eewModel2->record(this->eewModel2->rowCount()-1).value("longitude").toDouble();
            eewInfo.origin_time = this->eewModel2->record(this->eewModel2->rowCount()-1).value("origin_time").toDouble();
            eewInfo.eew_event_id = this->eewModel2->record(this->eewModel2->rowCount()-1).value("eew_evid").toInt();
            eewInfos.push_back(eewInfo);
        }
    }

    setSummaryTab(picks, infos, eewInfos, evid);
}

void MainWindow::setSummaryTab(QVector<_KGOnSite_Pick_t> picks, QVector<_KGOnSite_Info_t> infos,
                               QVector<_EEWInfo> eewInfos, QString evid)
{
    ui->tabWG->clear();
    QLayoutItem *child;

    while ((child = ui->listVLO->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    QMetaObject::invokeMethod(this->rootObj, "clearMap", Q_RETURN_ARG(QVariant, returnedValue));

    ui->evidLB->setText("Event ID:" + evid);

    /*
    int totalList = 0;
    totalList = picks.count() + eewInfos.count();
    */

    for(int i=0;i<picks.count();i++)
    {
        TabInfo *tabInfo = new TabInfo;
        tabInfo->setup(configure.KGONSITE_HOME, picks.at(i), infos.at(i));
        ListInfo *listInfo = new ListInfo;
        listInfo->setup(picks.at(i), infos.at(i));

        if(i == 0)
        {
            QDateTime ttime;
            ttime.setTime_t(picks.at(i).ttime);
            ttime.setTimeSpec(Qt::UTC);
            ttime = convertKST(ttime);
            ui->timeLB->setText(ttime.toString("yyyy-MM-dd hh:mm:ss ") +"KST");
        }

        ui->tabWG->addTab(tabInfo, QString::fromStdString(picks.at(i).sta));
        ui->listVLO->addWidget(listInfo);
    }

    for(int i=0;i<eewInfos.count();i++)
    {
        QDateTime ttime;
        ttime.setTime_t(eewInfos.at(i).origin_time);
        ttime.setTimeSpec(Qt::UTC);
        ttime = convertKST(ttime);
        ui->timeLB->setText(ttime.toString("yyyy-MM-dd hh:mm:ss ") +"KST");

        EewInfo *eew = new EewInfo;
        eew->setup(eewInfos.at(i));
        ui->listVLO->addWidget(eew);
    }

    ui->listVLO->addStretch(1);

    int kissSta = 0;
    int localSta = 0;
    for(int i=picks.count()-1;i>=0;i--)
    {
        if(QString(picks.at(i).net).startsWith("KG") || QString(picks.at(i).net).startsWith("KS"))
        {
            drawOnsiteOnMap(1, picks.at(i), infos.at(i), kissSta);
            kissSta++;
        }
        else
        {
            drawOnsiteOnMap(0, picks.at(i), infos.at(i), localSta);
            localSta++;
        }
    }

    for(int i=eewInfos.count()-1;i>=0;i--)
    {
        drawEEWOnMap(eewInfos.at(i));
    }

    if(ui->tabWG->currentIndex() != 0)
        ui->tabWG->setCurrentIndex(0);
    if(ui->mainTW->currentIndex() != 0)
        ui->mainTW->setCurrentIndex(0);
}

void MainWindow::setOnsiteTable(_KGOnSite_Pick_t pick)
{
    QString query;
    query = "select * from onsiteInfo where sta = '" + QString(pick.sta) + "' and chan = '" + QString(pick.chan) + "' and net = '" +
            QString(pick.net) + "' and loc = '" + QString(pick.loc) + "' and ttime = " + QString::number(pick.ttime, 'f', 6);

    this->model->setQuery(query);

    ui->onsiteTW->setRowCount(0);

    for(int i=0;i<this->model->rowCount();++i)
    {
        QString scnl;
        scnl = this->model->record(i).value("sta").toString() + "/" + this->model->record(i).value("chan").toString() + "/" +
                this->model->record(i).value("net").toString() + "/" + this->model->record(i).value("loc").toString();

        ui->onsiteTW->setRowCount(ui->onsiteTW->rowCount()+1);
        ui->onsiteTW->setItem(i, 0, new QTableWidgetItem(this->model->record(i).value("duration").toString()));
        ui->onsiteTW->setItem(i, 1, new QTableWidgetItem(this->model->record(i).value("pvel").toString()));
        ui->onsiteTW->setItem(i, 2, new QTableWidgetItem(this->model->record(i).value("pacc").toString()));
        ui->onsiteTW->setItem(i, 3, new QTableWidgetItem(this->model->record(i).value("tauc").toString()));
        ui->onsiteTW->setItem(i, 4, new QTableWidgetItem(this->model->record(i).value("pgv").toString()));
        ui->onsiteTW->setItem(i, 5, new QTableWidgetItem(this->model->record(i).value("pgv_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 6, new QTableWidgetItem(this->model->record(i).value("pgv_uncertainty_high").toString()));
        ui->onsiteTW->setItem(i, 7, new QTableWidgetItem(this->model->record(i).value("alert").toString()));
        ui->onsiteTW->setItem(i, 8, new QTableWidgetItem(this->model->record(i).value("intensity").toString()));
        ui->onsiteTW->setItem(i, 9, new QTableWidgetItem(this->model->record(i).value("intensity_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 10, new QTableWidgetItem(this->model->record(i).value("intensity_uncertainty_high").toString()));
        ui->onsiteTW->setItem(i, 11, new QTableWidgetItem(this->model->record(i).value("magnitude").toString()));
        ui->onsiteTW->setItem(i, 12, new QTableWidgetItem(this->model->record(i).value("magnitude_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 13, new QTableWidgetItem(this->model->record(i).value("magnitude_uncertainty_high").toString()));
        ui->onsiteTW->setItem(i, 14, new QTableWidgetItem(this->model->record(i).value("distance").toString()));
        ui->onsiteTW->setItem(i, 15, new QTableWidgetItem(this->model->record(i).value("distance_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 16, new QTableWidgetItem(this->model->record(i).value("distance_uncertainty_high").toString()));
    }

    for(int i=0;i<ui->onsiteTW->rowCount();i++)
    {
        for(int j=0;j<ui->onsiteTW->columnCount();j++)
        {
            ui->onsiteTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }

    onsitechart->setup(pick);
}

void MainWindow::setEEWTable(_EEWInfo eewInfo)
{
    QString query;
    query = "select * from eewInfo where evid = " + eewInfo.kgom_event_id;

    this->model->setQuery(query);

    ui->eewTW->setRowCount(0);

    ui->eewTW->setRowCount(ui->eewTW->rowCount()+1);
    ui->eewTW->setItem(0, 0, new QTableWidgetItem(this->model->record(0).value("type").toString()));
    ui->eewTW->setItem(0, 1, new QTableWidgetItem(this->model->record(0).value("version").toString()));
    ui->eewTW->setItem(0, 2, new QTableWidgetItem(this->model->record(0).value("category").toString()));
    ui->eewTW->setItem(0, 3, new QTableWidgetItem(this->model->record(0).value("magnitude").toString()));
    ui->eewTW->setItem(0, 4, new QTableWidgetItem(this->model->record(0).value("magnitude_uncertainty").toString()));
    ui->eewTW->setItem(0, 5, new QTableWidgetItem(this->model->record(0).value("latitude").toString()));
    ui->eewTW->setItem(0, 6, new QTableWidgetItem(this->model->record(0).value("latitude_uncertainty").toString()));
    ui->eewTW->setItem(0, 7, new QTableWidgetItem(this->model->record(0).value("longitude").toString()));
    ui->eewTW->setItem(0, 8, new QTableWidgetItem(this->model->record(0).value("longitude_uncertainty").toString()));
    ui->eewTW->setItem(0, 9, new QTableWidgetItem(this->model->record(0).value("depth").toString()));
    ui->eewTW->setItem(0, 10, new QTableWidgetItem(this->model->record(0).value("depth_uncertainty").toString()));
    ui->eewTW->setItem(0, 11, new QTableWidgetItem(this->model->record(0).value("origin_time").toString()));
    ui->eewTW->setItem(0, 12, new QTableWidgetItem(this->model->record(0).value("origin_time_uncertainty").toString()));
    ui->eewTW->setItem(0, 13, new QTableWidgetItem(this->model->record(0).value("number_stations").toString()));
    ui->eewTW->setItem(0, 14, new QTableWidgetItem(this->model->record(0).value("number_triggers").toString()));
    ui->eewTW->setItem(0, 15, new QTableWidgetItem(this->model->record(0).value("sent_flag").toString()));
    ui->eewTW->setItem(0, 16, new QTableWidgetItem(this->model->record(0).value("sent_time").toString()));
    ui->eewTW->setItem(0, 17, new QTableWidgetItem(this->model->record(0).value("percentsta").toString()));
    ui->eewTW->setItem(0, 18, new QTableWidgetItem(this->model->record(0).value("misfit").toString()));
    ui->eewTW->setItem(0, 19, new QTableWidgetItem(this->model->record(0).value("misfit").toString()));

    /*
    QString query;
    query = "select * from onsiteInfo where sta = '" + QString(pick.sta) + "' and chan = '" + QString(pick.chan) + "' and net = '" +
            QString(pick.net) + "' and loc = '" + QString(pick.loc) + "' and ttime = " + QString::number(pick.ttime, 'f', 6);

    this->model->setQuery(query);

    ui->onsiteTW->setRowCount(0);

    for(int i=0;i<this->model->rowCount();++i)
    {
        QString scnl;
        scnl = this->model->record(i).value("sta").toString() + "/" + this->model->record(i).value("chan").toString() + "/" +
                this->model->record(i).value("net").toString() + "/" + this->model->record(i).value("loc").toString();

        ui->onsiteTW->setRowCount(ui->onsiteTW->rowCount()+1);
        ui->onsiteTW->setItem(i, 0, new QTableWidgetItem(this->model->record(i).value("duration").toString()));
        ui->onsiteTW->setItem(i, 1, new QTableWidgetItem(this->model->record(i).value("pvel").toString()));
        ui->onsiteTW->setItem(i, 2, new QTableWidgetItem(this->model->record(i).value("pacc").toString()));
        ui->onsiteTW->setItem(i, 3, new QTableWidgetItem(this->model->record(i).value("tauc").toString()));
        ui->onsiteTW->setItem(i, 4, new QTableWidgetItem(this->model->record(i).value("pgv").toString()));
        ui->onsiteTW->setItem(i, 5, new QTableWidgetItem(this->model->record(i).value("pgv_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 6, new QTableWidgetItem(this->model->record(i).value("pgv_uncertainty_high").toString()));
        ui->onsiteTW->setItem(i, 7, new QTableWidgetItem(this->model->record(i).value("alert").toString()));
        ui->onsiteTW->setItem(i, 8, new QTableWidgetItem(this->model->record(i).value("intensity").toString()));
        ui->onsiteTW->setItem(i, 9, new QTableWidgetItem(this->model->record(i).value("intensity_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 10, new QTableWidgetItem(this->model->record(i).value("intensity_uncertainty_high").toString()));
        ui->onsiteTW->setItem(i, 11, new QTableWidgetItem(this->model->record(i).value("magnitude").toString()));
        ui->onsiteTW->setItem(i, 12, new QTableWidgetItem(this->model->record(i).value("magnitude_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 13, new QTableWidgetItem(this->model->record(i).value("magnitude_uncertainty_high").toString()));
        ui->onsiteTW->setItem(i, 14, new QTableWidgetItem(this->model->record(i).value("distance").toString()));
        ui->onsiteTW->setItem(i, 15, new QTableWidgetItem(this->model->record(i).value("distance_uncertainty_low").toString()));
        ui->onsiteTW->setItem(i, 16, new QTableWidgetItem(this->model->record(i).value("distance_uncertainty_high").toString()));
    }

    for(int i=0;i<ui->onsiteTW->rowCount();i++)
    {
        for(int j=0;j<ui->onsiteTW->columnCount();j++)
        {
            ui->onsiteTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
    */

}

void MainWindow::drawEEWOnMap(_EEWInfo info)
{
    QString text;
    text = "MAG:" + QString::number(info.magnitude, 'f', 2) + ", Lat.:" +
            QString::number(info.latitude, 'f' , 4) + ", Long.:" +
            QString::number(info.longitude, 'f', 4);

    QString tempMag = QString::number(info.magnitude, 'f', 2);

    QMetaObject::invokeMethod(this->rootObj, "addEEWStarMarker",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, info.latitude), Q_ARG(QVariant, info.longitude),
                              Q_ARG(QVariant, tempMag));

    /*
    QMetaObject::invokeMethod(this->rootObj, "addText",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, info.latitude), Q_ARG(QVariant, info.longitude),
                              Q_ARG(QVariant, text));
                              */
}

void MainWindow::drawOnsiteOnMap(int net, _KGOnSite_Pick_t pick, _KGOnSite_Info_t info, int i)
{
    double radius = info.distance * 1000;
    int zoomlevel = 10;
    int which = i;
    QString text;
    text = "Alert:" + QString::number(info.alert) + ", Dist.:" + QString::number(info.distance,'f',1) + "Km";

    QMetaObject::invokeMethod(this->rootObj, "addCircle",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, radius));

    QMetaObject::invokeMethod(this->rootObj, "addStaMarker",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, net),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, pick.sta), Q_ARG(QVariant, which),
                              Q_ARG(QVariant, zoomlevel));

    QMetaObject::invokeMethod(this->rootObj, "addText",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, text));


    /*
    QMetaObject::invokeMethod(this->rootObj, "addPolyline",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, pick.lat + 1), Q_ARG(QVariant, pick.lon + 1));
                              */
}

void MainWindow::clearPBClicked()
{
    ui->eventsTW->setRowCount(0);
}

void MainWindow::daysReadPBClicked()
{
    int days = ui->daysSB->value();
    QDate eDate = QDate::currentDate();
    QDate sDate;
    sDate = eDate.addDays(-days);

    setEventTable(sDate, eDate, 0);
}

void MainWindow::dateReadPBClicked()
{
    QDate sDate, eDate;
    sDate = ui->startDE->date();
    eDate = ui->endDE->date();

    setEventTable(sDate, eDate, 0);
}

void MainWindow::createTrayActions()
{
    minimizeAction = new QAction(tr("M&inimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Maxi&mize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    ui->menuOptions->addAction(quitAction);
}

void MainWindow::createActionsOnToolbar()
{
    for(int i=0;i<MAX_LOCALSTA_NUM;i++)
    {
        ui->mainToolBar->removeAction(sohLocalWA[i]);
    }
    for(int i=0;i<MAX_KISSSTA_NUM;i++)
    {
        ui->mainToolBar->removeAction(sohKissWA[i]);
    }

    for(int i=0;i<configure.localStaCount;i++)
    {
        sohLocalWA[i] = new QWidgetAction(this);
        sohLocalPB[i] = new QPushButton;
        sohLocalPB[i]->setText(configure.localStaVT.at(i).sta);
        sohLocalPB[i]->setFixedWidth(60); sohLocalPB[i]->setFixedHeight(25);
        sohLocalPB[i]->setStyleSheet("background-color: green; color: white");
        sohLocalWA[i]->setDefaultWidget(sohLocalPB[i]);
        sohLocalWA[i]->setStatusTip("Show Infomation for " + configure.localStaVT.at(i).sta + ".");
        ui->mainToolBar->addAction(sohLocalWA[i]);

        sohLocalPB[i]->setObjectName("LOCAL" + QString::number(i));
        sohLocalMon[i] = new SohMonitor;
        sohLocalMon[i]->setup(configure.localStaVT.at(i), 0);
        sohLocalMon[i]->hide();
        connect(sohLocalPB[i], SIGNAL(clicked(bool)), this, SLOT(sohPBClicked()));
    }

    for(int i=0;i<configure.kissStaCount;i++)
    {
        sohKissWA[i] = new QWidgetAction(this);
        sohKissPB[i] = new QPushButton;
        sohKissPB[i]->setText(configure.kissStaVT.at(i).sta);
        sohKissPB[i]->setFixedWidth(60); sohKissPB[i]->setFixedHeight(25);
        sohKissPB[i]->setStyleSheet("background-color: green; color: white");
        sohKissWA[i]->setDefaultWidget(sohKissPB[i]);
        sohKissWA[i]->setStatusTip("Show Infomation for " + configure.kissStaVT.at(i).sta + ".");
        ui->mainToolBar->addAction(sohKissWA[i]);

        sohKissPB[i]->setObjectName("KISS" + QString::number(i));
        sohKissMon[i] = new SohMonitor;
        sohKissMon[i]->setup(configure.kissStaVT.at(i), 1);
        sohKissMon[i]->hide();
        connect(sohKissPB[i], SIGNAL(clicked(bool)), this, SLOT(sohPBClicked()));
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    for(int i=0;i<ui->eventsTW->columnCount();i++)
        ui->eventsTW->setColumnWidth(i, (this->width()-50) / (ui->eventsTW->columnCount()+2));
    ui->eventsTW->setColumnWidth(1, ((this->width()-18) / (ui->eventsTW->columnCount()+2)) * 2);
    ui->eventsTW->setColumnWidth(2, ((this->width()-18) / (ui->eventsTW->columnCount()+2)) * 2);

    update();
    QMainWindow::resizeEvent(event);
}

void MainWindow::listModeActionTriggered()
{
    ui->actionList->setEnabled(false);
    ui->actionTab->setEnabled(true);
    ui->actionAll->setEnabled(true);
    ui->scrollArea->show();
    ui->regendFrame->show();
    ui->regendLine->show();
    ui->tabWG->hide();
}

void MainWindow::tabModeActionTriggered()
{
    ui->actionList->setEnabled(true);
    ui->actionTab->setEnabled(false);
    ui->actionAll->setEnabled(true);
    ui->scrollArea->hide();
    ui->regendFrame->hide();
    ui->regendLine->hide();
    ui->tabWG->show();
}

void MainWindow::allModeActionTriggered()
{
    ui->actionList->setEnabled(true);
    ui->actionTab->setEnabled(true);
    ui->actionAll->setEnabled(false);
    ui->scrollArea->show();
    ui->regendFrame->show();
    ui->regendLine->show();
    ui->tabWG->show();
}

void MainWindow::logViewerActionTriggered()
{
    LogViewer *logviewer = new LogViewer(configure.KGONSITE_HOME + "/logs");
    logviewer->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("KGom"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::actionConfigureWizardTriggered()
{
    wizard->setup(configure);
    wizard->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    log->write(configure.KGONSITE_HOME + "/logs/", "Configuration wizard opened.");
    wizard->show();
}

void MainWindow::showSysTime()
{
    QDateTime time = QDateTime::currentDateTimeUtc();
    time = convertKST(time);

    QString text = time.toString("hh:mm:ss");
    sysLN->display(text);

    if(eventMode == 1)
    {
        if(eventStartTime.toTime_t() + EVENT_DURATION < time.toTime_t())
        {
            eventMode = 0; 
            log->write(configure.KGONSITE_HOME + "/logs/", "---- Terminated this event ----");
        }
    }
}

void MainWindow::readConfigure(QString configFile)
{
    configure.localStaVT.clear(); configure.localStaCount = 0;
    configure.kissStaVT.clear(); configure.kissStaCount = 0;

    QFile file(configFile);
    if(!file.exists())
    {
        log->write(configure.KGONSITE_HOME + "/logs/", "Failed configuration. Parameter file doesn't exists.");
        exit(1);
    }
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(_line.startsWith(" ") || _line.startsWith("#"))
                continue;
            else if(_line.startsWith("LOCAL_STA") && _line.section("=",1,1) != "")
            {
                _STATION sta; sta.sta = _line.section("=",1,1).section(",",0,0);
                sta.chan = _line.section("=",1,1).section(",",1,1);
                sta.net = _line.section("=",1,1).section(",",2,2);
                sta.loc = _line.section("=",1,1).section(",",3,3);
                sta.lat = _line.section("=",1,1).section(",",4,4).toDouble();
                sta.lon = _line.section("=",1,1).section(",",5,5).toDouble();
                sta.elev = _line.section("=",1,1).section(",",6,6).toDouble();
                configure.localStaVT.push_back(sta);
                configure.localStaCount++;
            }
            else if(_line.startsWith("KISS_STA") && _line.section("=",1,1) != "")
            {
                _STATION sta; sta.sta = _line.section("=",1,1).section(",",0,0);
                sta.chan = _line.section("=",1,1).section(",",1,1);
                sta.net = _line.section("=",1,1).section(",",2,2);
                sta.loc = _line.section("=",1,1).section(",",3,3);
                sta.lat = _line.section("=",1,1).section(",",4,4).toDouble();
                sta.lon = _line.section("=",1,1).section(",",5,5).toDouble();
                sta.elev = _line.section("=",1,1).section(",",6,6).toDouble();
                configure.kissStaVT.push_back(sta);
                configure.kissStaCount++;
            }
            else if(_line.startsWith("EEW_AMQ_INFO") && _line.section("=",1,1) != "")
            {
                configure.eew_ip = _line.section("=",1,1).section(":", 0, 0);
                configure.eew_port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.eew_user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.eew_passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.eew_topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            /*
            else if(_line.startsWith("LOCAL_SERVER") && _line.section("=",1,1) != "")
            {
                configure.localServerIP = _line.section("=",1,1).section(":",0,0);
                configure.localServerPort = _line.section("=",1,1).section(":",1,1).toInt();
            }
            else if(_line.startsWith("KISS_SERVER") && _line.section("=",1,1) != "")
            {
                configure.kissServerIP = _line.section("=",1,1).section(":",0,0);
                configure.kissServerPort = _line.section("=",1,1).section(":",1,1).toInt();
            }
            else if(_line.startsWith("EEW_SERVER") && _line.section("=",1,1) != "")
            {
                configure.eewServerIP = _line.section("=",1,1).section(":",0,0);
                configure.eewServerPort = _line.section("=",1,1).section(":",1,1).toInt();
            }
            */
            else if(_line.startsWith("SOH_WARNING"))
            {
                configure.sohWarningEnabled = _line.section("=",1,1).section(",",0,0).toInt();
                configure.sohWarningScript = _line.section("=",1,1).section(",",1,1);
            }
            else if(_line.startsWith("EEW_WARNING"))
            {
                configure.eewWarningEnabled = _line.section("=",1,1).section(",",0,0).toInt();
                configure.eewWarningScript = _line.section("=",1,1).section(",",1,1);
            }
            else if(_line.startsWith("LEVEL1"))
            {
                configure.level1Enabled = _line.section("=",1,1).section(",",0,0).toInt();
                configure.level1NumSta = _line.section("=",1,1).section(",",1,1).toInt();
                configure.level1Script = _line.section("=",1,1).section(",",2,2);
            }
            else if(_line.startsWith("LEVEL2"))
            {
                configure.level2Enabled = _line.section("=",1,1).section(",",0,0).toInt();
                configure.level2NumSta = _line.section("=",1,1).section(",",1,1).toInt();
                configure.level2Script = _line.section("=",1,1).section(",",2,2);
            }
        }
        file.close();
    }
    log->write(configure.KGONSITE_HOME + "/logs/", "Succeed loading parameters. <" +
               configFile + ">");
}

void MainWindow::sohPBClicked()
{
    QObject *senderObj = sender();
    QString inst;
    if(senderObj->objectName().startsWith("KISS"))
        inst = "KISS";
    else
        inst = "LOCAL";

    int i = senderObj->objectName().right(1).toInt();

    if(inst == "KISS")
    {
        sohKissMon[i]->show();
    }
    else
    {
        sohLocalMon[i]->show();
    }
}

void MainWindow::setDiffTime()
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    now.setTimeSpec(Qt::UTC);
    now = convertKST(now);

    if(ui->timeLB->text() != "")
    {
        QString temp = ui->timeLB->text(); //"yyyy-MM-dd hh:mm:ss KST"
        temp = temp.left(19);
        QDateTime tt = QDateTime::fromString(temp, "yyyy-MM-dd hh:mm:ss");
        tt.setTimeSpec(Qt::UTC);

        int diffSec;

        QString diffStr;

        diffSec = tt.secsTo(now);

        int days = diffSec / 86400;
        int hours = (diffSec - days*86400) / 3600;
        int minutes = (diffSec - days*86400 - hours*3600) / 60;
        int seconds = diffSec - days*86400 - hours*3600 - 60*minutes;

        if (days>0)
            diffStr = " (" + QString::number(days) + " days and " + QString::number(hours) + " hours ago)";
        else if ((days==0)&&(hours>0))
            diffStr = " (" + QString::number(hours) + " hours and " + QString::number(minutes) + " minutes ago)";
        else if ((days==0)&&(hours==0)&&(minutes>0))
            diffStr = " (" + QString::number(minutes) + " minutes and " + QString::number(seconds) +  " seconds ago)";

        ui->timeLB2->setText(diffStr);
    }
}

void MainWindow::rvPickInfo(unsigned char *msg)
{
    _KGOnSite_Pick_t pick;
    memcpy(&pick, msg, sizeof(_KGOnSite_Pick_t));

    int lastEventID;

    QString conSCNL, rvSCNL;
    bool isMyStation = false;
    rvSCNL = QString::fromStdString(pick.sta) + "/" + QString::fromStdString(pick.chan) + "/" +
            QString::fromStdString(pick.net) + "/" + QString::fromStdString(pick.loc);

    for(int i=0;i<configure.localStaCount;i++)
    {
        conSCNL = configure.localStaVT.at(i).sta + "/" + configure.localStaVT.at(i).chan + "/" +
               configure.localStaVT.at(i).net + "/" + configure.localStaVT.at(i).loc;

        if(conSCNL.startsWith(rvSCNL))
        {
            isMyStation = true;
            break;
        }
    }

    for(int i=0;i<configure.kissStaCount;i++)
    {
        conSCNL = configure.kissStaVT.at(i).sta + "/" + configure.kissStaVT.at(i).chan + "/" +
               configure.kissStaVT.at(i).net + "/" + configure.kissStaVT.at(i).loc;

        if(conSCNL.startsWith(rvSCNL))
        {
            isMyStation = true;
            break;
        }
    }

    if(eventMode == 0 && isMyStation)
    {
        // get last event id
        QString query;
        lastEventID = getLastEvid();

        QDateTime eTime;
        eTime.setTime_t( (uint)pick.ttime );
        eTime.setTimeSpec(Qt::UTC);
        eTime = convertKST(eTime);

        query = "INSERT INTO event(evid, lddate) values (" +
                QString::number(lastEventID + 1) + ", '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";

        this->model->setQuery(query);

        log->write(configure.KGONSITE_HOME + "/logs/", "---- Started a new event ----");
        log->write(configure.KGONSITE_HOME + "/logs/", "Event ID: " + QString::number(lastEventID + 1));
        log->write(configure.KGONSITE_HOME + "/logs/", "Start Time(GMT): " + eTime.toString("yyyy/MM/dd hh:mm:ss"));
        log->write(configure.KGONSITE_HOME + "/logs/", "End Time(GMT): " + eTime.addSecs(10).toString("yyyy/MM/dd hh:mm:ss"));

        query = "INSERT INTO pickInfo("
                "evid, version, msg_type, "
                "sta, chan, net, loc, "
                "lat, lon, elev, ttime, "
                "polarity, weight, amplitude, period, "
                "dir, dfile, lddate) values (" +
                QString::number(lastEventID + 1) + ", 1, 'P', '" +
                QString::fromStdString(pick.sta) + "', '" + QString::fromStdString(pick.chan) + "', '" +
                QString::fromStdString(pick.net) + "', '" + QString::fromStdString(pick.loc) + "', " +
                QString::number(pick.lat, 'f', 4) + ", " + QString::number(pick.lon, 'f', 4) + ", " +
                QString::number(pick.elev, 'f', 1) + ", " + QString::number(pick.ttime, 'f', 0) + ", '" +
                QString(pick.polarity) + "', " + QString::number(pick.weight) + ", " +
                QString::number(pick.amplitude) + ", " + QString::number(pick.period, 'f', 2) + ", '" +
                QDate::currentDate().toString("yyyy/MMdd") + "', '" +
                QString::number(pick.ttime, 'f', 0) + QString::fromStdString(pick.sta) + ".dat', '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";

        log->write(configure.KGONSITE_HOME + "/logs/", "Detected a new PICK info. <" + rvSCNL + ">");

        this->model->setQuery(query);
    }
    else if(eventMode == 1 && isMyStation)
    {
        QString query;
        lastEventID = getLastEvid();

        query = "INSERT INTO pickInfo("
                "evid, version, msg_type, "
                "sta, chan, net, loc, "
                "lat, lon, elev, ttime, "
                "polarity, weight, amplitude, period, "
                "dir, dfile, lddate) values (" +
                QString::number(lastEventID) + ", 1, 'P', '" +
                QString::fromStdString(pick.sta) + "', '" + QString::fromStdString(pick.chan) + "', '" +
                QString::fromStdString(pick.net) + "', '" + QString::fromStdString(pick.loc) + "', " +
                QString::number(pick.lat, 'f', 4) + ", " + QString::number(pick.lon, 'f', 4) + ", " +
                QString::number(pick.elev, 'f', 1) + ", " + QString::number(pick.ttime, 'f', 0) + ", '" +
                QString(pick.polarity) + "', " + QString::number(pick.weight) + ", " +
                QString::number(pick.amplitude) + ", " + QString::number(pick.period, 'f', 2) + ", '" +
                QDate::currentDate().toString("yyyy/MMdd") + "', '" +
                QString::number(pick.ttime, 'f', 0) + QString::fromStdString(pick.sta) + ".dat', '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";

        log->write(configure.KGONSITE_HOME + "/logs/", "Detected a new PICK info. <" + rvSCNL + "> Completed inserting to Database");

        this->model->setQuery(query);
    }

    if(isMyStation)
        processEvent(lastEventID + 1);
    else
        log->write(configure.KGONSITE_HOME + "/logs/", "Detected a new PICK info. <" + rvSCNL + "> But this is not my station. Ignored");
}

void MainWindow::rvOnsiteInfo(unsigned char *msg)
{
    _KGOnSite_Info_t info;
    memcpy(&info, msg, sizeof(_KGOnSite_Info_t));

    QString conSCNL, rvSCNL;
    bool isMyStation = false;
    rvSCNL = QString::fromStdString(info.sta) + "/" + QString::fromStdString(info.chan) + "/" +
            QString::fromStdString(info.net) + "/" + QString::fromStdString(info.loc);

    for(int i=0;i<configure.localStaCount;i++)
    {
        conSCNL = configure.localStaVT.at(i).sta + "/" + configure.localStaVT.at(i).chan + "/" +
               configure.localStaVT.at(i).net + "/" + configure.localStaVT.at(i).loc;

        if(conSCNL.startsWith(rvSCNL))
        {
            isMyStation = true;
            break;
        }
    }

    for(int i=0;i<configure.kissStaCount;i++)
    {
        conSCNL = configure.kissStaVT.at(i).sta + "/" + configure.kissStaVT.at(i).chan + "/" +
               configure.kissStaVT.at(i).net + "/" + configure.kissStaVT.at(i).loc;

        if(conSCNL.startsWith(rvSCNL))
        {
            isMyStation = true;
            break;
        }
    }

    if(isMyStation)
    {
        if(info.alert < 0)                      info.alert = 0;
        if(info.intensity < 0)                  info.intensity = 0;
        if(info.intensity_uncertainty_low < 0)  info.intensity_uncertainty_low = 0;
        if(info.intensity_uncertainty_high < 0) info.intensity_uncertainty_high = 0;
        if(info.magnitude < 0)                  info.magnitude = 0;
        if(info.magnitude_uncertainty_low < 0)  info.magnitude_uncertainty_low = 0;
        if(info.magnitude_uncertainty_high < 0) info.magnitude_uncertainty_high = 0;

        QString query = "INSERT INTO onsiteInfo "
                        "(version, msg_type, "
                        "sta, chan, net, loc, "
                        "duration, type, ttime, disp_count, "
                        "displacement, disp_time, pvel, "
                        "pacc, tauc, pgv, "
                        "pgv_uncertainty_low, pgv_uncertainty_high, "
                        "alert, snr, log_disp_ratio, log_PdPv, dB_Pd, "
                        "intensity, intensity_uncertainty_low, intensity_uncertainty_high, "
                        "magnitude, magnitude_uncertainty_low, magnitude_uncertainty_high, "
                        "distance, distance_uncertainty_low, distance_uncertainty_high, lddate) values (1, 'I', '" +
                QString::fromStdString(info.sta) + "', '" + QString::fromStdString(info.chan) + "', '" +
                QString::fromStdString(info.net) + "', '" + QString::fromStdString(info.loc) + "', '" +
                QString(info.duration) + "', '" + QString(info.type) + "', " +
                QString::number(info.ttime, 'f', 0) + ", " + QString::number(info.disp_count, 'f', 6) + ", " +
                QString::number(info.displacement, 'f', 6) + ", " + QString::number(info.disp_time, 'f', 6) + ", " +
                QString::number(info.pvel, 'f', 6) + ", " + QString::number(info.pacc, 'f', 6) + ", " +
                QString::number(info.tauc, 'f', 6) + ", " + QString::number(info.pgv, 'f', 6) + ", " +
                QString::number(info.pgv_uncertainty_low, 'f', 6) + ", " + QString::number(info.pgv_uncertainty_high, 'f', 6) + ", " +
                QString::number(info.alert) + ", " + QString::number(info.snr, 'f', 6) + ", " +
                QString::number(info.log_disp_ratio, 'f', 6) + ", " + QString::number(info.log_PdPv, 'f', 6) + ", " + QString::number(info.dB_Pd, 'f', 6) + ", " +
                QString::number(info.intensity, 'f', 6) + ", " + QString::number(info.intensity_uncertainty_low, 'f', 6) + ", " + QString::number(info.intensity_uncertainty_high, 'f', 6) + ", " +
                QString::number(info.magnitude, 'f', 6) + ", " + QString::number(info.magnitude_uncertainty_low, 'f', 6) + ", " + QString::number(info.magnitude_uncertainty_high, 'f', 6) + ", " +
                QString::number(info.distance, 'f', 6) + ", " + QString::number(info.distance_uncertainty_low, 'f', 6) + ", " + QString::number(info.distance_uncertainty_high, 'f', 6) + ", '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";

        this->model->setQuery(query);

        log->write(configure.KGONSITE_HOME + "/logs/", "Detected a new ONSITE info. <" + rvSCNL + "> Completed inserting to Database");

        setEventTable(ui->startDE->date(), ui->endDE->date(), 1);
    }
    else
    {
        log->write(configure.KGONSITE_HOME + "/logs/", "Detected a new ONSITE info. <" + rvSCNL + "> But this is not my station. Ignored");
    }
}

void MainWindow::processEvent(int newEventID)
{
    if(eventMode == 0)
    {
        // if tray mode, then force openning main window.
        restoreAction->trigger();
        this->setWindowState(Qt::WindowActive);
        eventStartTime = QDateTime::currentDateTimeUtc();
        eventStartTime = convertKST(eventStartTime);
        bTimer->start(1000);
        eventMode = 1;
        ui->mainToolBar->actions().at(2)->setVisible(true);

        /*
        if( QMessageBox::warning(this,
                                  tr("Detected a New Event.") + " (EVENT ID : " + QString::number(newEventID) + ")",
                                  tr("You can push the <b>OK</b> button and stop blinking window.")))
        {
            ui->mainTW->setStyleSheet("QWidget { background-color: rgb(235, 235, 235); }");
            bTimer->stop();
        }
        */
    }
}

void MainWindow::stopBlinkPBClicked()
{
    bTimer->stop();
    ui->mainToolBar->actions().at(2)->setVisible(false);
    ui->mainTW->setStyleSheet("QWidget { background-color: rgb(235, 235, 235); }");
}

int MainWindow::getLastEvid()
{
    QString query = "SELECT max(evid) FROM event";
    this->model->setQuery(query);
    return this->model->record(0).value("max(evid)").toInt();
}

void MainWindow::blinkingWindow()
{
    QDateTime t = QDateTime::currentDateTimeUtc();
    t = convertKST(t);

    if(t.toTime_t() % 2 == 0)
        ui->mainTW->setStyleSheet("QWidget { background-color: rgb(235, 235, 235); }");
    else
        ui->mainTW->setStyleSheet("QWidget { background-color: rgb(239, 41, 41); }");
}

void MainWindow::rvWaveform(unsigned char *msg)
{

}

void MainWindow::rvSOH(unsigned char *msg)
{

}

void MainWindow::rvEEWInfo(_EEWInfo eewInfo)
{
    int lastEventID = getLastEvid();
    int eewEventID = eewInfo.eew_event_id;

    QString query;

    if(eewInfo.message_category == LIVE)
    {
        if(eewInfo.message_type == NEW)
        {
            if(eventMode == 0)
            {
                lastEventID = lastEventID + 1;

                query = "INSERT INTO event(evid, lddate) values (" +
                        QString::number(lastEventID) + ", '" +
                        QDate::currentDate().toString("yyyyMMdd") + "')";

                this->model->setQuery(query);
            }
        }
    }

    query = "INSERT INTO eewInfo "
                    "(evid , type , "
                    "eew_evid , version , message_category , message_type , "
                    "magnitude , magnitude_uncertainty , latitude , latitude_uncertainty , "
                    "longitude , longitude_uncertainty , depth , depth_uncertainty , "
                    "origin_time , origin_time_uncertainty , number_stations , "
                    "number_triggers , sent_flag , "
                    "sent_time , percentsta , misfit , status , lddate) values (" +
            QString::number(lastEventID) + ", 'E', " +
            QString::number(eewEventID) + ", " + QString::number(eewInfo.version) + ", " +
            QString::number(eewInfo.message_category) + ", " + QString::number(eewInfo.message_type) + ", " +
            QString::number(eewInfo.magnitude, 'f', 6) + ", " + QString::number(eewInfo.magnitude_uncertainty, 'f', 6) + ", " +
            QString::number(eewInfo.latitude, 'f', 6) + ", " + QString::number(eewInfo.latitude_uncertainty, 'f', 6) + ", " +
            QString::number(eewInfo.longitude, 'f', 6) + ", " + QString::number(eewInfo.longitude_uncertainty, 'f', 6) + ", " +
            QString::number(eewInfo.depth, 'f', 6) + ", " + QString::number(eewInfo.depth_uncertainty, 'f', 6) + ", " +
            QString::number(eewInfo.origin_time, 'f', 3) + ", " + QString::number(eewInfo.origin_time_uncertainty, 'f', 3) + ", " +
            QString::number(eewInfo.number_stations, 'f', 3) + ", " + QString::number(eewInfo.number_triggers, 'f', 3) + ", " +
            QString::number(eewInfo.sent_flag) + ", " + QString::number(eewInfo.sent_time, 'f', 3) + ", " +
            QString::number(eewInfo.percentsta, 'f', 3) + ", " + QString::number(eewInfo.misfit, 'f', 3) + ", " +
            QString::number(eewInfo.status) + ", '" +
            QDate::currentDate().toString("yyyyMMdd") + "')";

    this->model->setQuery(query);

    setEventTable(ui->startDE->date(), ui->endDE->date(), 1);

    /* for circle animation */
    double dist, azim;
    int rtn = geo_to_km(MYPOSITION_LAT, MYPOSITION_LON, eewInfo.latitude, eewInfo.longitude, &dist, &azim);

    timeP = myRound(dist / P_VEL, 1);
    timeS = myRound(dist / S_VEL, 1);

    QMetaObject::invokeMethod(this->rootObj, "addMyPositionMarker",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, MYPOSITION_LAT), Q_ARG(QVariant, MYPOSITION_LON));

    //qDebug() << MYPOSITION_LAT << MYPOSITION_LON << eewInfo.latitude << eewInfo.longitude << dist << timeP << timeS;

    aniLat = eewInfo.latitude;
    aniLon = eewInfo.longitude;
    count = 0;
    aniTimer->start(100);
    /**************************************************************/

    processEvent(lastEventID);

    log->write(configure.KGONSITE_HOME + "/logs/", "Detected a new Earthquake info. EEW Event ID:" +
               QString::number(eewEventID) + ", Version: " + QString::number(eewInfo.version) +
               ". Completed inserting to Database");
}

void MainWindow::showAnimation()
{
    timeP = myRound(timeP - 0.1, 1);
    timeS = myRound(timeS - 0.1, 1);

    double radiusP = count * P_VEL * 100;
    double radiusS = count * S_VEL * 100;

    //qDebug() << timeP << timeS << radiusP << radiusS;

    QMetaObject::invokeMethod(this->rootObj, "addCircleForAnimation",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, aniLat), Q_ARG(QVariant, aniLon),
                              Q_ARG(QVariant, radiusP), Q_ARG(QVariant, radiusS),
                              Q_ARG(QVariant, (int)timeS), Q_ARG(QVariant, "1"), Q_ARG(QVariant, "미세"));

    count++;

    if(timeS <= -4)
    {
        aniTimer->stop();
        QMetaObject::invokeMethod(this->rootObj, "removeItemForAnimation",
                                  Q_RETURN_ARG(QVariant, returnedValue));
    }
}
