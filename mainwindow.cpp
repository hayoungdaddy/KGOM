#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QtConcurrent/qtconcurrentrun.h"

MainWindow::MainWindow(QString configFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    activemq::library::ActiveMQCPP::initializeLibrary();

    // This line is essential
    qRegisterMetaType<_EEWInfo>("_EEWInfo");
    qRegisterMetaType<_KGOnSite_Info_t>("_KGOnSite_Info_t");
    qRegisterMetaType<_KGOnSite_SOH_t>("_KGOnSite_SOH_t");
    qRegisterMetaType<_KGKIIS_GMPEAK_EVENT_t>("_KGKIIS_GMPEAK_EVENT_t");
    qRegisterMetaType<_KGKIIS_GMPEAK_EVENT_STA_t>("_KGKIIS_GMPEAK_EVENT_STA_t");
    qRegisterMetaType< QMultiMap<int,_QSCD_FOR_MULTIMAP> >("QMultiMap<int,_QSCD_FOR_MULTIMAP>");

    // Setup threads for receiving data
    krecveew = new RecvEEWMessage(this);
    krecvOnsite = new RecvOnsiteMessage(this);
    krecvSoh = new RecvSOHMessage(this);
    krecvPGA = new RecvRealTimePGAMessage(this);
    lrecvOnsite = new RecvOnsiteMessage(this);
    lrecvPGA = new RecvRealTimePGAMessage(this);
    lrecvSoh = new RecvSOHMessage(this);

    codec = QTextCodec::codecForName("utf-8");

    // Setup Environment
    configure.KGOM_HOME = QProcessEnvironment::systemEnvironment().value("KGOM_HOME");
    configure.configFileName = configFile;

    // Decoration GUI
    decorationGUI();

    // Setup wizards
    log = new WriteLog();
    log->write(configure.KGOM_HOME + "/logs/", "======================================================");
    log->write(configure.KGOM_HOME + "/logs/", "KGOM Started.");

    wizard = new ConfigurationWizard(this);
    wizard->hide();
    connect(wizard, SIGNAL(resetMainWindow()), this, SLOT(setup()));

    detailview = new DetailView(this);
    detailview->hide();

    aboutthis = new AboutThis(configure.KGOM_HOME, this);
    aboutthis->hide();

    configuration = new Configuration(this);
    configuration->hide();
    connect(configuration, SIGNAL(resetMainWindow()), this, SLOT(setup()));

    searchform = new SearchForm(this);
    searchform->hide();
    sfMagIndex = 0;
    sfMinMag = 0;
    sfMaxMag = 0;
    sfDateIndex = 0;
    sfNEvents = 10;
    sfNDays = 0;
    sfEndDate = QDate::currentDate();
    sfStartDate = sfEndDate.addDays(-30);
    connect(ui->searchEventPB, SIGNAL(clicked(bool)), this, SLOT(showSearchWindow()));
    connect(searchform, SIGNAL(sendSearchOptionsToMainWindow(int, double, double, int, int, int, QDate, QDate)),
            this, SLOT(recvSearchOptions(int, double, double, int, int, int, QDate, QDate)));

    // Setup Timers
    blinkTimer = new QTimer;
    blinkTimer->stop();
    connect(blinkTimer, SIGNAL(timeout()), this, SLOT(blinkingWindow()));
    blinkCount = 0;

    aniTimer = new QTimer;
    aniTimer->stop();
    connect(aniTimer, SIGNAL(timeout()), this, SLOT(showAnimation()));

    QTimer *systemTimer = new QTimer;
    connect(systemTimer, SIGNAL(timeout()), this, SLOT(doRepeatWork()));
    systemTimer->start(1000);
    sysLN = new QLCDNumber();
    sysLN->setDigitCount(8);
    sysLN->setFixedWidth(130);
    sysLN->setPalette(Qt::black);
    sysLN->setStyleSheet("color:black;");

    QTimer *diffTimer = new QTimer(this);
    diffTimer->start(1000);
    connect(diffTimer, SIGNAL(timeout()), this, SLOT(setDiffTime()));

    // Setup Tray mode
    createTrayActions();
    createTrayIcon();
    QIcon icon(configure.KGOM_HOME + "/params/images/PCIcon.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->show();

    // Setup Action
    ui->actionWizard->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    ui->actionConfigure->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    ui->actionViewLogs->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    ui->actionAboutThis->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    connect(ui->actionWizard, &QAction::triggered, this, &MainWindow::configureWizardActionTriggered);
    connect(ui->actionConfigure, &QAction::triggered, this, &MainWindow::configurationActionTriggered);
    connect(ui->actionViewLogs, &QAction::triggered, this, &MainWindow::logViewerActionTriggered);
    connect(ui->actionAboutThis, &QAction::triggered, this, &MainWindow::aboutthisActionTriggered);

    // Load a osm map & Setup a Map for EVENTS tab
    QQuickView *eventsView = new QQuickView();
    eMapContainer = QWidget::createWindowContainer(eventsView, this);
    //eventsView->setResizeMode(QQuickView::SizeViewToRootObject);
    eventsView->setResizeMode(QQuickView::SizeRootObjectToView);
    eMapContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    eMapContainer->setFocusPolicy(Qt::TabFocus);
    eMapContainer->setStyleSheet("background-color: black;");
    eventsView->setSource(QUrl(QStringLiteral("qrc:/Viewmap.qml")));
    ui->eventsMapLO->addWidget(eMapContainer);
    eRootObj = eventsView->rootObject();
    QMetaObject::invokeMethod(this->eRootObj, "clearMap", Q_RETURN_ARG(QVariant, eReturnedValue));
    QMetaObject::invokeMethod(this->eRootObj, "createMyPositionMarker", Q_RETURN_ARG(QVariant, eReturnedValue));
    QMetaObject::invokeMethod(this->eRootObj, "createCurrentEventMarker", Q_RETURN_ARG(QVariant, eReturnedValue));

    QObject::connect(this->eRootObj, SIGNAL(sendIDSignal(QString, QString, QString)), this, SLOT(_qmlSignalfromEMap(QString, QString, QString)));

    // Load a osm map & Setup a Map for ALERT tab
    QQuickView *alertView = new QQuickView();
    aMapContainer = QWidget::createWindowContainer(alertView, this);
    alertView->setResizeMode(QQuickView::SizeRootObjectToView);
    aMapContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    aMapContainer->setFocusPolicy(Qt::TabFocus);
    aMapContainer->setStyleSheet("background-color: black;");
    alertView->setSource(QUrl(QStringLiteral("qrc:/Viewmap.qml")));
    ui->alertMapLO->addWidget(aMapContainer);
    aRootObj = alertView->rootObject();
    QMetaObject::invokeMethod(this->aRootObj, "clearMap", Q_RETURN_ARG(QVariant, aReturnedValue));
    QMetaObject::invokeMethod(this->aRootObj, "createEEWStarMarker", Q_RETURN_ARG(QVariant, aReturnedValue));
    QMetaObject::invokeMethod(this->aRootObj, "createMyPositionMarker", Q_RETURN_ARG(QVariant, aReturnedValue));

    // Load a osm map & Setup a Map for REALTIME tab
    QQuickView *realTimeView = new QQuickView();
    rMapContainer = QWidget::createWindowContainer(realTimeView, this);
    realTimeView->setResizeMode(QQuickView::SizeRootObjectToView);
    rMapContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rMapContainer->setFocusPolicy(Qt::TabFocus);
    rMapContainer->setStyleSheet("background-color: black;");
    realTimeView->setSource(QUrl(QStringLiteral("qrc:/Viewmap.qml")));
    ui->pgaMapLO->addWidget(rMapContainer);
    rRootObj = realTimeView->rootObject();
    QMetaObject::invokeMethod(this->rRootObj, "clearMap", Q_RETURN_ARG(QVariant, rReturnedValue));
    QMetaObject::invokeMethod(this->rRootObj, "createMyPositionMarker", Q_RETURN_ARG(QVariant, rReturnedValue));

    // Open DB
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(configure.KGOM_HOME + "/data/KGOM.db");
    if(!db.open())
    {
        QMessageBox msgBox;
        msgBox.setText(db.lastError().text());
        log->write(configure.KGOM_HOME + "/logs/", "Failed openning a database. (" +
                   configure.KGOM_HOME + "/data/KGOM.db)");
        msgBox.exec();
        exit(1);
    }
    log->write(configure.KGOM_HOME + "/logs/", "Succeed opening a database. <" +
               configure.KGOM_HOME + "/data/KGOM/db>");

    this->eventModel = new QSqlQueryModel();
    this->onsiteModel = new QSqlQueryModel();
    this->eewModel = new QSqlQueryModel();
    this->pgaModel = new QSqlQueryModel();

    // Setup SOH widgets
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

    // Setup a ToolBar
    QLabel *sysTimeLB = new QLabel;
    //sysTimeLB->setText("System Time (KST) : ");
    sysTimeLB->setText(codec->toUnicode(" 시스템 시간 (KST) "));

    QWidget *empty2 = new QWidget();
    empty2->setFixedWidth(50);
    //stopBlinkPB = new QPushButton("Stop Blinking Window");
    stopBlinkPB = new QPushButton(codec->toUnicode("이벤트 확인"));
    stopBlinkPB->setFixedWidth(200);
    stopBlinkPB->setStyleSheet("background-color: red; color: white; font: bold 18px;");
    connect(stopBlinkPB, SIGNAL(clicked(bool)), this, SLOT(stopBlinkPBClicked()));

    QWidget *empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

    ui->mainToolBar->addWidget(sysTimeLB);
    ui->mainToolBar->addWidget(sysLN);
    ui->mainToolBar->addWidget(empty2);
    ui->mainToolBar->addWidget(stopBlinkPB);
    ui->mainToolBar->addWidget(empty);
    ui->mainToolBar->actions().at(3)->setVisible(false);

    connect(ui->viewDetailPB, SIGNAL(clicked(bool)), this, SLOT(showViewDetail()));

    setup();

    controlAlarm = new ControlAlarm();
    if(configure.alarm_device_ip != "")
        controlAlarm->setup(configure.alarm_device_ip, configure.alarm_device_port);

    //ui->replayPB->hide();
    connect(ui->replayPB, SIGNAL(clicked(bool)), this, SLOT(eventReplayPBClicked()));
}

MainWindow::~MainWindow()
{
    if(krecveew->isRunning())
        krecveew->requestInterruption();
    if(krecvOnsite->isRunning())
        krecvOnsite->requestInterruption();
    if(krecvSoh->isRunning())
        krecvSoh->requestInterruption();
    if(krecvPGA->isRunning())
        krecvPGA->requestInterruption();
    if(lrecvOnsite->isRunning())
        lrecvOnsite->requestInterruption();
    if(lrecvPGA->isRunning())
        lrecvPGA->requestInterruption();
    if(lrecvSoh->isRunning())
        lrecvSoh->requestInterruption();

    log->write(configure.KGOM_HOME + "/logs/", "This Program terminated.");

    delete ui;
}

void MainWindow::showViewDetail()
{
    detailview->show();
}

void MainWindow::decorationGUI()
{
    ui->noEventLB->hide();

    QPixmap pm(65,65);
    pm.fill(QColor(243, 243, 243));
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setFont(QFont("Open Sans", 10, QFont::Bold));
    p.setPen(QPen(getMagColor(2)));
    p.setBrush(QBrush(getMagColor(2)));
    p.drawEllipse(0, 0, 65, 65);
    p.setPen(QPen(Qt::black));
    p.drawText(pm.rect(), Qt::AlignCenter, getMagText(2));
    ui->mag1LB->setPixmap(pm);
    ui->mag1LB2->setPixmap(pm);
    p.setPen(QPen(getMagColor(4)));
    p.setBrush(QBrush(getMagColor(4)));
    p.drawEllipse(0, 0, 65, 65);
    p.setPen(QPen(Qt::black));
    p.drawText(pm.rect(), Qt::AlignCenter, getMagText(4));
    ui->mag2LB->setPixmap(pm);
    ui->mag2LB2->setPixmap(pm);
    p.setPen(QPen(getMagColor(5)));
    p.setBrush(QBrush(getMagColor(5)));
    p.drawEllipse(0, 0, 65, 65);
    p.setPen(QPen(Qt::white));
    p.drawText(pm.rect(), Qt::AlignCenter, getMagText(5));
    ui->mag3LB->setPixmap(pm);
    ui->mag3LB2->setPixmap(pm);
    p.setPen(QPen(getMagColor(7)));
    p.setBrush(QBrush(getMagColor(7)));
    p.drawEllipse(0, 0, 65, 65);
    p.setPen(QPen(Qt::white));
    p.drawText(pm.rect(), Qt::AlignCenter, getMagText(7));
    ui->mag4LB->setPixmap(pm);
    ui->mag4LB2->setPixmap(pm);

    QPixmap searchPX(configure.KGOM_HOME + "/params/images/searchIcon.png");
    QIcon searchIC(searchPX);
    ui->searchEventPB->setIcon(searchIC);
    ui->searchEventPB->setIconSize(QSize(30, 30));
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    //qDebug() << eMapContainer->width() << eMapContainer->height();
    //eMapContainer->adjustSize();

    //QMetaObject::invokeMethod(this->eRootObj, "refreshMyPositionMarker", Q_RETURN_ARG(QVariant, eReturnedValue));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        /*
        QMessageBox::information(this, "KGOM",
                                 "The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry.");
                                    */
        QMessageBox::information(this, "KGOM",
                                 codec->toUnicode("시스템 트레이에서 프로그램이 계속 작동중입니다. "
                                                  "만약 프로그램 종료를 원한다면 우측 상단 아이콘 메뉴에서 <종료>를 선택하세요."));
        hide();
        event->ignore();
    }
}

void MainWindow::recvSearchOptions(int magIndex, double minMag, double maxMag, int dateIndex, int nEvents, int nDays, QDate sDate, QDate eDate)
{
    sfMagIndex = magIndex;
    sfMinMag = minMag;
    sfMaxMag = maxMag;
    sfDateIndex = dateIndex;
    sfNEvents = nEvents;
    sfNDays = nDays;
    sfStartDate = sDate;
    sfEndDate = eDate;
    if(sfMagIndex == 0)
        setEventsTab(0, 0, sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 1)
        setEventsTab(magValue[0], magValue[1], sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 2)
        setEventsTab(magValue[1], magValue[2], sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 3)
        setEventsTab(magValue[2], magValue[3], sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 4)
        setEventsTab(magValue[3], 999, sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 5)
        setEventsTab(sfMinMag, sfMaxMag, sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
}

void MainWindow::eventReplayPBClicked()
{
    // /home/sysop/KGOM/data/KGOM.db 183 35.8714 128.60186 6.0 4.0
    QString cmd;
    cmd = configure.KGOM_HOME + "/bin/KGOM_Replayer " + configure.KGOM_HOME + "/data/KGOM.db" + " " + ui->evidLB->text().section(":", 1, 1) + " " +
            QString::number(configure.myposition_lat, 'f', 6) + " " + QString::number(configure.myposition_lon, 'f', 6) + " " +
            QString::number(configure.p_vel, 'f', 4) + " " + QString::number(configure.s_vel, 'f', 4) + " " + configure.KGOM_HOME + "/data/INTENSITY" + " &";

    system(cmd.toLatin1().constData());
}

void MainWindow::createTrayActions()
{
    //minimizeAction = new QAction(tr("M&inimize"), this);
    minimizeAction = new QAction(codec->toUnicode("최소화"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    //maximizeAction = new QAction(tr("Maxi&mize"), this);
    maximizeAction = new QAction(codec->toUnicode("최대화"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    //restoreAction = new QAction(tr("&Restore"), this);
    restoreAction = new QAction(codec->toUnicode("복귀"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    //quitAction = new QAction(tr("&Quit"), this);
    quitAction = new QAction(codec->toUnicode("종료"), this);
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

void MainWindow::configureWizardActionTriggered()
{
    wizard->setup(configure);
    log->write(configure.KGOM_HOME + "/logs/", "Configuration wizard window opened.");
    wizard->show();
}

void MainWindow::configurationActionTriggered()
{
    configuration->setup(configure);
    log->write(configure.KGOM_HOME + "/logs/", "Configuration window opened.");
    configuration->show();
}

void MainWindow::logViewerActionTriggered()
{
    LogViewer *logviewer = new LogViewer(configure.KGOM_HOME + "/logs");
    logviewer->show();
}

void MainWindow::aboutthisActionTriggered()
{
    aboutthis->show();
}

void MainWindow::doRepeatWork()
{
    // show system time
    //QDateTime time = QDateTime::currentDateTimeUtc();
    QDateTime timeUTC = QDateTime::currentDateTimeUtc();
    QDateTime dataTimeUTC = timeUTC.addSecs(- DATA_TIME_DIFF);
    QDateTime timeKST;
    timeKST = convertKST(timeUTC);

    QString text = timeKST.toString("hh:mm:ss");
    sysLN->display(text);

    if(text.right(5).startsWith("00:00") || text.right(5).startsWith("30:00"))
        log->write(configure.KGOM_HOME + "/logs/", "I'm alive.");

    if(eventMode == 1)
    {
        if(eventStartTimeUTC.toTime_t() + EVENT_DURATION < timeUTC.toTime_t())
        {
            eventMode = 0;
            maxMag = 0;
            log->write(configure.KGOM_HOME + "/logs/", "---- Terminated this event ----");

            ui->mainTW->setTabEnabled(0, true);
            ui->mainTW->setCurrentIndex(1);

            if(aniTimer->isActive())
                aniTimer->stop();

            QMetaObject::invokeMethod(this->aRootObj, "removeItemForAnimation", Q_RETURN_ARG(QVariant, aReturnedValue));

            // make and put into database pgaInfos for max PGA
            for(int i=0;i<configure.kissStaVT.count();i++)
            {
                int evid;
                QString query;
                evid = getLastEvid();

                if(configure.kissStaVT.at(i).maxPGA != 0)
                {
                    query = "INSERT INTO pgaInfo "
                            "(evid, version, msg_type, sta, chan, net, loc, lat, lon, target_chan, e_time, time, maxZ, maxN, maxE, maxH, maxT, lddate) values (" +
                            QString::number(evid) + ", 1, 'E', '" +
                            QString(configure.kissStaVT.at(i).sta) + "', '" + QString(configure.kissStaVT.at(i).chan) + "', '" +
                            QString(configure.kissStaVT.at(i).net) + "', '" + QString(configure.kissStaVT.at(i).loc) + "', " +
                            QString::number(configure.kissStaVT.at(i).lat, 'f', 4) + ", " + QString::number(configure.kissStaVT.at(i).lon, 'f', 4) + ", 'H', " +
                            QString::number(timeUTC.toTime_t(), 'f', 0) + ", " +
                            QString::number(configure.kissStaVT.at(i).maxPGATime, 'f', 0) + ", -1, -1, -1, " +
                            QString::number(configure.kissStaVT.at(i).maxPGA, 'f', 4) + ", -1, '" +
                            QDate::currentDate().toString("yyyyMMdd") + "')";

                    this->pgaModel->setQuery(query);
                }
            }

            // reset maxPGA for realTime PGA
            for(int i=0;i<configure.kissStaVT.count();i++)
            {
                _STATION sta = configure.kissStaVT.at(i);
                sta.maxPGA = 0;
                sta.maxPGATime = 0;
                configure.kissStaVT.replace(i, sta);
            }

            setEventsTab(0, 0, 0, 10, 0, QDate::currentDate(), QDate::currentDate());
            searchform->setCheckBox(0, 0);
        }
    }

    // for RealTime PGA
    // read PGA
    mutex.lock();
    QList<_QSCD_FOR_MULTIMAP> pgaList = pgaHouse.values(dataTimeUTC.toTime_t());
    mutex.unlock();

    resetStaCircleOnMap();

    // reset a pga table
    for(int j=0;j<configure.kissStaVT.count();j++)
    {
        ui->pgaTW->setItem(j, 2, new QTableWidgetItem("None"));
        ui->pgaTW->item(j, 2)->setTextAlignment(Qt::AlignCenter);
        ui->pgaTW->item(j, 2)->setTextColor(Qt::black);
        ui->pgaTW->item(j, 2)->setBackgroundColor(Qt::white);
    }

    if(pgaList.size() != 0)
    {
        QDateTime dataTimeKST = convertKST(dataTimeUTC);
        //dataTimeKST = convertKST(dataTimeKST);
        ui->dataTimeLN->display(dataTimeKST.toString("hh:mm:ss"));

        // display
        int regendIndex;

        for(int i=0;i<pgaList.size();i++)
        {
            for(int j=0;j<configure.kissStaVT.count();j++)
            {
                if(pgaList.at(i).net.startsWith(configure.kissStaVT.at(j).net) &&
                        pgaList.at(i).sta.startsWith(configure.kissStaVT.at(j).sta))
                {
                    QFuture<int> future = QtConcurrent::run(getRegendIndex, pgaList.at(i).hpga);
                    future.waitForFinished();
                    regendIndex = future.result();

                    // for a map
                    QMetaObject::invokeMethod(this->rRootObj, "changeSizeAndColorForStaCircle", Q_RETURN_ARG(QVariant, rReturnedValue),
                                              Q_ARG(QVariant, j), Q_ARG(QVariant, pgaWidth[regendIndex]*2),
                                              Q_ARG(QVariant, pgaColor[regendIndex]),
                                              Q_ARG(QVariant, 1));

                    // for a pga table
                    ui->pgaTW->setItem(j, 2, new QTableWidgetItem(QString::number(pgaList.at(i).hpga, 'f', 4)));
                    ui->pgaTW->item(j, 2)->setBackgroundColor(pgaColor[regendIndex]);
                    if(regendIndex <= 17)
                        ui->pgaTW->item(j, 2)->setTextColor(Qt::black);
                    else
                        ui->pgaTW->item(j, 2)->setTextColor(Qt::white);

                    // keep pga values
                    if(eventMode == 1)
                    {
                        int evid;
                        evid = getLastEvid();
                        QString path, fileName;
                        path = configure.KGOM_HOME + "/data/PGA/" + dataTimeUTC.toString("yyyy/MM/") + QString::number(evid) + "/";
                        fileName = path + "/" + pgaList.at(i).net + "_" + pgaList.at(i).sta;
                        QDir evtFilePathD(path);
                        if(!evtFilePathD.exists())
                            evtFilePathD.mkpath(".");

                        QFile pgaFile(fileName);
                        pgaFile.open(QIODevice::WriteOnly | QIODevice::Append);
                        QTextStream out(&pgaFile);
                        out << QString::number(dataTimeUTC.toTime_t()) << " " << QString::number(pgaList.at(i).hpga, 'f', 4) << "\n";
                        pgaFile.close();

                        if(configure.kissStaVT.at(j).maxPGA < pgaList.at(i).hpga)
                        {
                            _STATION sta = configure.kissStaVT.at(j);
                            sta.maxPGA = pgaList.at(i).hpga;
                            sta.maxPGATime = dataTimeUTC.toTime_t();
                            configure.kissStaVT.replace(j, sta);
                        }
                    }

                    break;
                }
            }
        }

        for(int i=0;i<ui->pgaTW->rowCount();i++)
        {
            for(int j=0;j<ui->pgaTW->columnCount();j++)
            {
                ui->pgaTW->item(i, j)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    // remove mmap
    QMultiMap<int, _QSCD_FOR_MULTIMAP>::iterator iter;

    if(!pgaHouse.isEmpty())
    {
        mutex.lock();
        QMultiMap<int, _QSCD_FOR_MULTIMAP>::iterator untilIter;
        untilIter = pgaHouse.lowerBound(dataTimeUTC.toTime_t() - 10);

        for(iter = pgaHouse.begin() ; untilIter != iter;)
        {
            QMultiMap<int, _QSCD_FOR_MULTIMAP>::iterator thisIter;
            thisIter = iter;
            iter++;
            pgaHouse.erase(thisIter);
        }
        mutex.unlock();
    }
}

void MainWindow::setupPGATable()
{
    ui->pgaTW->setRowCount(0);
    ui->pgaTW->setFixedHeight(configure.kissStaVT.count() * 30 + 26);

    for(int i=0;i<configure.kissStaVT.count();i++)
    {
        ui->pgaTW->setRowCount(ui->pgaTW->rowCount()+1);
        ui->pgaTW->setItem(ui->pgaTW->rowCount()-1, 0, new QTableWidgetItem(configure.kissStaVT.at(i).net));
        ui->pgaTW->setItem(ui->pgaTW->rowCount()-1, 1, new QTableWidgetItem(configure.kissStaVT.at(i).sta));
        ui->pgaTW->setItem(ui->pgaTW->rowCount()-1, 2, new QTableWidgetItem("None"));
    }
}

void MainWindow::resetStaCircleOnMap()
{
    for(int i=0;i<configure.kissStaVT.count();i++)
    {
        QMetaObject::invokeMethod(this->rRootObj, "changeSizeAndColorForStaCircle", Q_RETURN_ARG(QVariant, rReturnedValue),
                                  Q_ARG(QVariant, i), Q_ARG(QVariant, 10*2),
                                  Q_ARG(QVariant, "white"), Q_ARG(QVariant, 1));
    }
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
        sohKissMon[i]->show();
    else
        sohLocalMon[i]->show();
}

void MainWindow::blinkingWindow()
{
    QDateTime t = QDateTime::currentDateTimeUtc();
    t = convertKST(t);

    QString style = "background-color: " + blinkColor.name();

    if(blinkCount == 0)
    {
        ui->mainTW->setStyleSheet(style);
        blinkCount = 1;

        QString cmd = "play "  + configure.KGOM_HOME + "/bin/alert.oga &> /dev/null &";
        system(cmd.toLatin1().constData());
    }
    else if(blinkCount == 1)
    {
        ui->mainTW->setStyleSheet("background-color: rgb(235, 235, 235);");
        blinkCount = 0;   
    }
}

void MainWindow::stopBlinkPBClicked()
{
    QString style = "QTabWidget::pane {\n    border: 1px white;\n}\n\nQTabWidget::tab-bar {\n    left: 20px; \n}\n\nQTabBar::tab {\n	"
                    "min-width: 15ex;\n}\n\nQTabBar::tab:selected {\n    "
                    "background: rgb(235, 235, 235);\n	color: blue;\n}\n\nQTabBar::tab:!selected {\n    "
                    "background: white;\n}\n\nQWidget {\n	background-color: rgb(235, 235, 235);\n}\n\n"
                    "QWidget { background-color: rgb(235, 235, 235); }";

    blinkTimer->stop();
    if(configure.alarm_device_ip != "")
    {
        controlAlarm->setup(configure.alarm_device_ip, configure.alarm_device_port);
        controlAlarm->stopBlinkAll();
    }
    ui->mainToolBar->actions().at(3)->setVisible(false);
    ui->mainTW->setStyleSheet(style);
    blinkCount = 0;
}

void MainWindow::showSearchWindow()
{
    searchform->setDateEdit(QDate::currentDate().addDays(-SEARCH_DURATION), QDate::currentDate());
    if(searchform->isHidden())
        searchform->show();
}

void MainWindow::setDiffTime()
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    //now.setTimeSpec(Qt::UTC);
    now = convertKST(now);

    if(ui->timeLB->text() != "")
    {
        QString temp = ui->timeLB->text(); //"yyyy-MM-dd hh:mm:ss KST"
        temp = temp.left(19);
        QDateTime tt = QDateTime::fromString(temp, "yyyy-MM-dd hh:mm:ss");
        //tt.setTimeSpec(Qt::UTC);

        int diffSec;

        QString diffStr;

        diffSec = tt.secsTo(now);

        int days = diffSec / 86400;
        int hours = (diffSec - days*86400) / 3600;
        int minutes = (diffSec - days*86400 - hours*3600) / 60;
        int seconds = diffSec - days*86400 - hours*3600 - 60*minutes;

        /*
        if (days>0)
            diffStr = " (" + QString::number(days) + " days and " + QString::number(hours) + " hours ago)";
        else if ((days==0)&&(hours>0))
            diffStr = " (" + QString::number(hours) + " hours and " + QString::number(minutes) + " minutes ago)";
        else if ((days==0)&&(hours==0)&&(minutes>0))
            diffStr = " (" + QString::number(minutes) + " minutes and " + QString::number(seconds) +  " seconds ago)";
            */
        if (days>0)
            diffStr = " (" + QString::number(days) + codec->toUnicode(" 일 ") + QString::number(hours) + codec->toUnicode(" 시간 전)");
        else if ((days==0)&&(hours>0))
            diffStr = " (" + QString::number(hours) + codec->toUnicode(" 시간 ") + QString::number(minutes) + codec->toUnicode(" 분 전)");
        else if ((days==0)&&(hours==0)&&(minutes>0))
            diffStr = " (" + QString::number(minutes) + codec->toUnicode(" 분 ") + QString::number(seconds) + codec->toUnicode(" 초 전)");

        ui->timeLB2->setText(diffStr);
    }
}

void MainWindow::readConfigure(QString configFile)
{
    configure.localStaVT.clear();
    configure.kissStaVT.clear();

    QFile file(configFile);
    if(!file.exists())
    {
        log->write(configure.KGOM_HOME + "/logs/", "Failed configuration. Parameter file doesn't exists.");
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
            else if(_line.startsWith("MYPOSITION_LAT") && _line.section("=",1,1) != "")
                configure.myposition_lat = _line.section("=", 1, 1).toDouble();
            else if(_line.startsWith("MYPOSITION_LON") && _line.section("=",1,1) != "")
                configure.myposition_lon = _line.section("=", 1, 1).toDouble();
            else if(_line.startsWith("LOCAL_STA") && _line.section("=",1,1) != "")
            {
                _STATION sta; sta.sta = _line.section("=",1,1).section(":",0,0);
                sta.chan = _line.section("=",1,1).section(":",1,1);
                sta.net = _line.section("=",1,1).section(":",2,2);
                sta.loc = _line.section("=",1,1).section(":",3,3);
                sta.lat = _line.section("=",1,1).section(":",4,4).toDouble();
                sta.lon = _line.section("=",1,1).section(":",5,5).toDouble();
                sta.elev = _line.section("=",1,1).section(":",6,6).toDouble();
                configure.localStaVT.push_back(sta);
            }
            else if(_line.startsWith("KISS_STA") && _line.section("=",1,1) != "")
            {
                _STATION sta; sta.sta = _line.section("=",1,1).section(":",0,0);
                sta.chan = _line.section("=",1,1).section(":",1,1);
                sta.net = _line.section("=",1,1).section(":",2,2);
                sta.loc = _line.section("=",1,1).section(":",3,3);
                sta.lat = _line.section("=",1,1).section(":",4,4).toDouble();
                sta.lon = _line.section("=",1,1).section(":",5,5).toDouble();
                sta.elev = _line.section("=",1,1).section(":",6,6).toDouble();
                sta.maxPGATime = 0; sta.maxPGA = 0;
                configure.kissStaVT.push_back(sta);
            }
            else if(_line.startsWith("LOCAL_ONSITE_AMQ") && _line.section("=",1,1) != "::::" && _line.section("=",1,1) != "")
            {
                configure.local_onsite_amq.ip = _line.section("=",1,1).section(":", 0, 0);
                configure.local_onsite_amq.port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.local_onsite_amq.user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.local_onsite_amq.passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.local_onsite_amq.topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            else if(_line.startsWith("LOCAL_SOH_AMQ") && _line.section("=",1,1) != "::::" && _line.section("=",1,1) != "")
            {
                configure.local_soh_amq.ip = _line.section("=",1,1).section(":", 0, 0);
                configure.local_soh_amq.port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.local_soh_amq.user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.local_soh_amq.passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.local_soh_amq.topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            else if(_line.startsWith("LOCAL_PGA_AMQ") && _line.section("=",1,1) != "::::" && _line.section("=",1,1) != "")
            {
                configure.local_pga_amq.ip = _line.section("=",1,1).section(":", 0, 0);
                configure.local_pga_amq.port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.local_pga_amq.user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.local_pga_amq.passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.local_pga_amq.topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            else if(_line.startsWith("KISS_EEW_AMQ") && _line.section("=",1,1) != "::::" && _line.section("=",1,1) != "")
            {
                configure.kiss_eew_amq.ip = _line.section("=",1,1).section(":", 0, 0);
                configure.kiss_eew_amq.port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.kiss_eew_amq.user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.kiss_eew_amq.passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.kiss_eew_amq.topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            else if(_line.startsWith("KISS_ONSITE_AMQ") && _line.section("=",1,1) != "::::" && _line.section("=",1,1) != "")
            {
                configure.kiss_onsite_amq.ip = _line.section("=",1,1).section(":", 0, 0);
                configure.kiss_onsite_amq.port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.kiss_onsite_amq.user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.kiss_onsite_amq.passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.kiss_onsite_amq.topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            else if(_line.startsWith("KISS_SOH_AMQ") && _line.section("=",1,1) != "::::" && _line.section("=",1,1) != "")
            {
                configure.kiss_soh_amq.ip = _line.section("=",1,1).section(":", 0, 0);
                configure.kiss_soh_amq.port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.kiss_soh_amq.user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.kiss_soh_amq.passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.kiss_soh_amq.topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            else if(_line.startsWith("KISS_PGA_AMQ") && _line.section("=",1,1) != "::::" && _line.section("=",1,1) != "")
            {
                configure.kiss_pga_amq.ip = _line.section("=",1,1).section(":", 0, 0);
                configure.kiss_pga_amq.port = _line.section("=", 1, 1).section(":", 1, 1);
                configure.kiss_pga_amq.user = _line.section("=", 1, 1).section(":", 2, 2);
                configure.kiss_pga_amq.passwd = _line.section("=", 1, 1).section(":", 3, 3);
                configure.kiss_pga_amq.topic = _line.section("=", 1, 1).section(":", 4, 4);
            }
            else if(_line.startsWith("ALARM_DEVICE"))
            {
                configure.alarm_device_ip = _line.section("=", 1, 1).section(":", 0, 0);
                configure.alarm_device_port = _line.section("=", 1, 1).section(":", 1, 1).toInt();
            }
            else if(_line.startsWith("LEVEL1") && _line.section("=",1,1) != "")
            {
                configure.level1_alert_use = _line.section("=", 1, 1).section(":", 0, 0).toInt();
                configure.level1_alert_min_mag = _line.section("=", 1, 1).section(":", 1, 1).toFloat();
                configure.level1_alert_max_mag = _line.section("=", 1, 1).section(":", 2, 2).toFloat();
                configure.level1_alert_dist = _line.section("=", 1, 1).section(":", 3, 3).toInt();
            }
            else if(_line.startsWith("LEVEL2") && _line.section("=",1,1) != "")
            {
                configure.level2_alert_use = _line.section("=", 1, 1).section(":", 0, 0).toInt();
                configure.level2_alert_min_mag = _line.section("=", 1, 1).section(":", 1, 1).toFloat();
                configure.level2_alert_max_mag = _line.section("=", 1, 1).section(":", 2, 2).toFloat();
                configure.level2_alert_dist = _line.section("=", 1, 1).section(":", 3, 3).toInt();
            }
            else if(_line.startsWith("P_VEL"))
                configure.p_vel = _line.section("=",1,1).toDouble();
            else if(_line.startsWith("S_VEL"))
                configure.s_vel = _line.section("=",1,1).toDouble();
        }
        file.close();
    }
    log->write(configure.KGOM_HOME + "/logs/", "Succeed loading parameters. <" +
               configFile + ">");
}

void MainWindow::createStaCircleOnRealTimePGAMap()
{
    // create staCircle
    QMetaObject::invokeMethod(this->rRootObj, "clearMap", Q_RETURN_ARG(QVariant, rReturnedValue));
    for(int i=0;i<configure.kissStaVT.count();i++)
    {
        QMetaObject::invokeMethod(this->rRootObj, "createStaCircle", Q_RETURN_ARG(QVariant, rReturnedValue),
                                  Q_ARG(QVariant, i), Q_ARG(QVariant, configure.kissStaVT.at(i).lat),
                                  Q_ARG(QVariant, configure.kissStaVT.at(i).lon), Q_ARG(QVariant, 10*2), Q_ARG(QVariant, "white"),
                                  Q_ARG(QVariant, configure.kissStaVT.at(i).net + "/" + configure.kissStaVT.at(i).sta),
                                  Q_ARG(QVariant, 1));
    }
}

void MainWindow::createActionsOnToolbar()
{
    // Delete all SOH widgets before resetup
    for(int i=0;i<MAX_LOCALSTA_NUM;i++)
        ui->mainToolBar->removeAction(sohLocalWA[i]);
    for(int i=0;i<MAX_KISSSTA_NUM;i++)
        ui->mainToolBar->removeAction(sohKissWA[i]);

    QString style = "background-color: " + sohColor[2].name();

    for(int i=0;i<configure.localStaVT.count();i++)
    {
        sohLocalPB[i]->setText(configure.localStaVT.at(i).sta);
        sohLocalPB[i]->setFixedWidth(60); sohLocalPB[i]->setFixedHeight(25);
        sohLocalPB[i]->setStyleSheet(style);
        sohLocalWA[i]->setDefaultWidget(sohLocalPB[i]);
        ui->mainToolBar->addAction(sohLocalWA[i]);

        sohLocalPB[i]->setObjectName("LOCAL" + QString::number(i));
        sohLocalMon[i]->setup(configure.localStaVT.at(i));
        sohLocalMon[i]->hide();
        connect(sohLocalPB[i], SIGNAL(clicked(bool)), this, SLOT(sohPBClicked()));
        connect(sohLocalMon[i], SIGNAL(sendSOHtoMainWindow(int, QString, QString)),
                this, SLOT(recvSOHfromWG(int, QString, QString)));
    }

    for(int i=0;i<configure.kissStaVT.count();i++)
    {
        sohKissPB[i]->setText(configure.kissStaVT.at(i).sta);
        sohKissPB[i]->setFixedWidth(60); sohKissPB[i]->setFixedHeight(25);
        sohKissPB[i]->setStyleSheet(style);
        sohKissWA[i]->setDefaultWidget(sohKissPB[i]);
        ui->mainToolBar->addAction(sohKissWA[i]);

        sohKissPB[i]->setObjectName("KISS" + QString::number(i));
        sohKissMon[i]->setup(configure.kissStaVT.at(i));
        sohKissMon[i]->hide();
        connect(sohKissPB[i], SIGNAL(clicked(bool)), this, SLOT(sohPBClicked()));
        connect(sohKissMon[i], SIGNAL(sendSOHtoMainWindow(int, QString, QString)),
                                       this, SLOT(recvSOHfromWG(int, QString, QString)));
    }
}

void MainWindow::showAnimation()
{
    //qDebug() << remainSecSRelative;
    remainSecPRelative = myRound(remainSecPRelative - 0.1, 1);
    remainSecSRelative = myRound(remainSecSRelative - 0.1, 1);

    double radiusP = (myRound(remainSecPAbsolute - remainSecPRelative, 1)) * configure.p_vel * 1000;
    double radiusS = (myRound(remainSecSAbsolute - remainSecSRelative, 1)) * configure.s_vel * 1000;

    int intenI = Intensity.toInt();
    QString tColor;
    if(intenI == 5) tColor = "black";
    else tColor = "black";
    //else tColor = "white";
    //qDebug() << "intenI " << intenI;
    QMetaObject::invokeMethod(this->aRootObj, "addCircleForAnimation",
                              Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, aniLat), Q_ARG(QVariant, aniLon),
                              Q_ARG(QVariant, radiusP), Q_ARG(QVariant, radiusS),
                              Q_ARG(QVariant, (int)remainSecSRelative),
                              Q_ARG(QVariant, Intensity),
                              Q_ARG(QVariant, Vibration),
                              Q_ARG(QVariant, tColor),
                              Q_ARG(QVariant, getIntenColor(intenI).name()));
    //aniCount++;

    if(remainSecSRelative >= 0 && remainSecSRelative <= 10 && blinkTimer->interval() == 1000)
        blinkTimer->setInterval(500);

    if(remainSecSRelative <= 0 && blinkTimer->interval() == 500)
        blinkTimer->setInterval(1000);

    if(remainSecSRelative <= -10)
        aniTimer->stop();
}

int MainWindow::getLastEvid()
{
    QString query = "SELECT max(evid) FROM event";
    this->eventModel->setQuery(query);
    return this->eventModel->record(0).value("max(evid)").toInt();
}

void MainWindow::alerting(double mag, double dist)
{
    if(configure.level1_alert_use == 1)
    {
        if(mag >= configure.level1_alert_min_mag && mag < configure.level1_alert_max_mag && dist <= configure.level1_alert_dist)
        {
            if(configure.alarm_device_ip != "")
            {
                controlAlarm->setup(configure.alarm_device_ip, configure.alarm_device_port);
                controlAlarm->blinkYELLOW();
                log->write(configure.KGOM_HOME + "/logs/", "Yellow Alarm Worked");
            }
        }
    }
    if(configure.level2_alert_use == 1)
    {
        if(mag >= configure.level2_alert_min_mag && mag < configure.level2_alert_max_mag && dist <= configure.level2_alert_dist)
        {
            if(configure.alarm_device_ip != "")
            {
                controlAlarm->setup(configure.alarm_device_ip, configure.alarm_device_port);
                controlAlarm->blinkRED();
                log->write(configure.KGOM_HOME + "/logs/", "Red Alarm Worked");
            }
        }
    }
}

void MainWindow::getEventInfo(int evid)
{
    QString evidS = QString::number(evid);
    QString query;

    QVector<_KGOnSite_Info_t> onsiteInfos;
    QVector<_EEWInfo> eewInfos;
    QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaInfos;
    QString pgaChannel;
    int pgaTime;

    query = "select * from pgaInfo where evid =" + evidS;
    this->pgaModel->setQuery(query);

    if(this->pgaModel->rowCount() > 0)
    {
        for(int i=0;i<this->pgaModel->rowCount();i++)
        {
            _KGKIIS_GMPEAK_EVENT_STA_t info;
            info.version = this->pgaModel->record(i).value("version").toInt();
            char *temp;
            temp = (char*) malloc(sizeof(char));
            strcpy(temp, this->pgaModel->record(i).value("msg_type").toString().toLatin1().constData());
            info.msg_type = temp[0];
            strcpy(info.sta, this->pgaModel->record(i).value("sta").toString().toLatin1().constData());
            strcpy(info.chan, this->pgaModel->record(i).value("chan").toString().toLatin1().constData());
            strcpy(info.net, this->pgaModel->record(i).value("net").toString().toLatin1().constData());
            strcpy(info.loc, this->pgaModel->record(i).value("loc").toString().toLatin1().constData());
            info.lat   = this->pgaModel->record(i).value("lat").toDouble();
            info.lon   = this->pgaModel->record(i).value("lon").toDouble();
            pgaChannel = this->pgaModel->record(i).value("target_chan").toString();
            pgaTime    = this->pgaModel->record(i).value("e_time").toInt();
            info.time  = this->pgaModel->record(i).value("time").toInt();
            info.maxZ = this->pgaModel->record(i).value("maxZ").toFloat();
            info.maxN = this->pgaModel->record(i).value("maxN").toFloat();
            info.maxE = this->pgaModel->record(i).value("maxE").toFloat();
            info.maxH = this->pgaModel->record(i).value("maxH").toFloat();
            info.maxT = this->pgaModel->record(i).value("maxT").toFloat();
            pgaInfos.push_back(info);
            free(temp);
        }
    }

    query = "select * from onsiteInfo where evid =" + evidS;
    this->onsiteModel->setQuery(query);

    if(this->onsiteModel->rowCount() > 0) // input last onsiteinfo only to infos.
    {
        for(int i=0;i<this->onsiteModel->rowCount();i++)
        {
            _KGOnSite_Info_t info;
            info.version = this->onsiteModel->record(i).value("version").toInt();
            char *temp;
            temp = (char*) malloc(sizeof(char));
            strcpy(temp, this->onsiteModel->record(i).value("msg_type").toString().toLatin1().constData());
            info.msg_type = temp[0];
            strcpy(info.sta, this->onsiteModel->record(i).value("sta").toString().toLatin1().constData());
            strcpy(info.chan, this->onsiteModel->record(i).value("chan").toString().toLatin1().constData());
            strcpy(info.net, this->onsiteModel->record(i).value("net").toString().toLatin1().constData());
            strcpy(info.loc, this->onsiteModel->record(i).value("loc").toString().toLatin1().constData());
            strcpy(temp, this->onsiteModel->record(i).value("duration").toString().toLatin1().constData());
            info.duration = temp[0];
            strcpy(temp, this->onsiteModel->record(i).value("type").toString().toLatin1().constData());
            info.type = temp[0];
            info.ttime = this->onsiteModel->record(i).value("ttime").toDouble();
            info.disp_count = this->onsiteModel->record(i).value("disp_count").toFloat();
            info.displacement = this->onsiteModel->record(i).value("displacement").toFloat();
            info.disp_time = this->onsiteModel->record(i).value("disp_time").toDouble();
            info.pgv = this->onsiteModel->record(i).value("pgv").toFloat();
            info.pgv_uncertainty_low = this->onsiteModel->record(i).value("pgv_uncertainty_low").toFloat();
            info.pgv_uncertainty_high = this->onsiteModel->record(i).value("pgv_uncertainty_high").toFloat();
            info.alert = this->onsiteModel->record(i).value("alert").toInt();
            info.snr = this->onsiteModel->record(i).value("snr").toFloat();
            info.log_disp_ratio = this->onsiteModel->record(i).value("log_disp_ratio").toFloat();
            info.log_PdPv = this->onsiteModel->record(i).value("log_PdPv").toFloat();
            info.dB_Pd = this->onsiteModel->record(i).value("dB_Pd").toFloat();
            info.intensity = this->onsiteModel->record(i).value("intensity").toFloat();
            info.intensity_uncertainty_low = this->onsiteModel->record(i).value("intensity_uncertainty_low").toFloat();
            info.intensity_uncertainty_high = this->onsiteModel->record(i).value("intensity_uncertainty_high").toFloat();
            info.magnitude = this->onsiteModel->record(i).value("magnitude").toFloat();
            info.magnitude_uncertainty_low = this->onsiteModel->record(i).value("magnitude_uncertainty_low").toFloat();
            info.magnitude_uncertainty_high = this->onsiteModel->record(i).value("magnitude_uncertainty_high").toFloat();
            info.distance = this->onsiteModel->record(i).value("distance").toFloat();
            info.distance_uncertainty_low = this->onsiteModel->record(i).value("distance_uncertainty_low").toFloat();
            info.distance_uncertainty_high = this->onsiteModel->record(i).value("distance_uncertainty_high").toFloat();
            info.evid = evid;
            strcpy(temp, this->onsiteModel->record(i).value("fromWhere").toString().toLatin1().constData());
            info.fromWhere = temp[0];
            info.lat = this->onsiteModel->record(i).value("lat").toDouble();
            info.lon = this->onsiteModel->record(i).value("lon").toDouble();
            info.elev = this->onsiteModel->record(i).value("elev").toDouble();
            onsiteInfos.push_back(info);
            free(temp);
        }
    }

    query = "SELECT * FROM eewInfo WHERE evid = " + evidS + " ORDER BY rowid DESC";
    this->eewModel->setQuery(query);

    if(this->eewModel->rowCount() > 0)
    {
        for(int i=0;i<this->eewModel->rowCount();i++)
        {
            _EEWInfo eewInfo;
            eewInfo.evid = this->eewModel->record(i).value("evid").toInt();
            char *temp;
            temp = (char*) malloc(sizeof(char));
            strcpy(temp, this->eewModel->record(i).value("type").toString().toLatin1().constData());
            eewInfo.type = temp[0];
            eewInfo.eew_evid = this->eewModel->record(i).value("eew_evid").toInt();
            eewInfo.version = this->eewModel->record(i).value("version").toInt();
            int mc = this->eewModel->record(i).value("message_category").toInt();
            if(mc == 0) eewInfo.message_category = LIVE;
            else eewInfo.message_category = TEST;
            int mt = this->eewModel->record(i).value("message_type").toInt();
            if(mt == 0) eewInfo.message_type = NEW;
            else if(mt == 1) eewInfo.message_type = UPDATE;
            else eewInfo.message_type = DELETE;
            eewInfo.magnitude = this->eewModel->record(i).value("magnitude").toDouble();
            eewInfo.magnitude_uncertainty = this->eewModel->record(i).value("magnitude_uncertainty").toDouble();
            eewInfo.latitude = this->eewModel->record(i).value("latitude").toDouble();
            eewInfo.latitude_uncertainty = this->eewModel->record(i).value("latitude_uncertainty").toDouble();
            eewInfo.longitude = this->eewModel->record(i).value("longitude").toDouble();
            eewInfo.longitude_uncertainty = this->eewModel->record(i).value("longitude_uncertainty").toDouble();
            eewInfo.depth = this->eewModel->record(i).value("depth").toDouble();
            eewInfo.depth_uncertainty = this->eewModel->record(i).value("depth_uncertainty").toDouble();
            eewInfo.origin_time = this->eewModel->record(i).value("origin_time").toDouble();
            eewInfo.origin_time_uncertainty = this->eewModel->record(i).value("origin_time_uncertainty").toDouble();
            eewInfo.number_stations = this->eewModel->record(i).value("number_stations").toDouble();
            eewInfo.number_triggers = this->eewModel->record(i).value("number_triggers").toDouble();
            eewInfo.sent_flag = this->eewModel->record(i).value("sent_flag").toInt();
            eewInfo.sent_time = this->eewModel->record(i).value("sent_time").toDouble();
            eewInfo.percentsta = this->eewModel->record(i).value("percentsta").toDouble();
            eewInfo.misfit = this->eewModel->record(i).value("misfit").toDouble();
            eewInfo.sent_flag = this->eewModel->record(i).value("sent_flag").toInt();
            free(temp);
            eewInfos.push_back(eewInfo);
        }
    }

    setAlertTab(onsiteInfos, eewInfos, pgaChannel, pgaTime, pgaInfos, evidS);
    detailview->setup(onsiteInfos, eewInfos, pgaChannel, pgaTime, pgaInfos, evidS, configure.KGOM_HOME);
}

void MainWindow::setAlertTab(QVector<_KGOnSite_Info_t> onsiteInfos, QVector<_EEWInfo> eewInfos,
                             QString pgaChannel, int pgaTime, QVector<_KGKIIS_GMPEAK_EVENT_STA_t> pgaInfos, QString evid)
{
    QLayoutItem *child;

    while ((child = ui->alertListVLO->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    QMetaObject::invokeMethod(this->aRootObj, "clearMap", Q_RETURN_ARG(QVariant, aReturnedValue));

    double minLat = 999, maxLat = 0;
    double minLon = 999, maxLon = 0;

    ui->evidLB->setText("Event ID:" + evid);

    QString query = "SELECT lddate FROM event WHERE evid=" +  evid;
    this->eventModel->setQuery(query);
    drawIntensityOnMap(this->eventModel->record(0).value("lddate").toString(), evid);;

    if(eewInfos.count() != 0)
    {
        QDateTime ttimeUTC;
        QDateTime ttimeKST;
        ttimeUTC.setTimeSpec(Qt::UTC);
        ttimeUTC.setTime_t(eewInfos.first().origin_time);

        ttimeKST = convertKST(ttimeUTC);
        ui->timeLB->setText(ttimeKST.toString("yyyy-MM-dd hh:mm:ss ") +"KST");

        double dist = getDistance(eewInfos.first().latitude, eewInfos.first().longitude, configure.myposition_lat, configure.myposition_lon);

        EewInfo *eew = new EewInfo;
        eew->setup(eewInfos.first(), QString::number(dist, 'f', 0), configure.myposition_lat, configure.myposition_lon);
        ui->alertListVLO->addWidget(eew);

        if(eewInfos.first().latitude < minLat) minLat = eewInfos.first().latitude;
        if(eewInfos.first().latitude > maxLat) maxLat = eewInfos.first().latitude;
        if(eewInfos.first().longitude < minLon) minLon = eewInfos.first().longitude;
        if(eewInfos.first().longitude > maxLon) maxLon = eewInfos.first().longitude;

        drawEEWOnMap(eewInfos.first());
        ui->replayPB->show();
    }

    if(onsiteInfos.count() != 0)
    {
        // sort infos,  using ttime/sta/net
        QVector<_KGOnSite_Info_t> tempInfos;

        for(int i=0;i<onsiteInfos.count();i++)
        {
            _KGOnSite_Info_t info = onsiteInfos.at(i);
            if(tempInfos.isEmpty())
                tempInfos.push_back(info);

            int isNewOrUpdate;  // new = 0, update = 1
            int index;
            for(int j=0;j<tempInfos.count();j++)
            {
                _KGOnSite_Info_t tempinfo = tempInfos.at(j);
                if(QString(info.sta).startsWith(QString(tempinfo.sta)) &&
                        info.ttime == tempinfo.ttime &&
                        QString(info.net).startsWith(QString(tempinfo.net)))
                {
                    isNewOrUpdate = 1;
                    index = j;
                    break;
                }
                isNewOrUpdate = 0;
            }
            if(isNewOrUpdate == 0)      // is new
                tempInfos.push_back(info);
            else                        // is update
                tempInfos.replace(index, info);
        }

        for(int i=0;i<tempInfos.count();i++)
        {
            if(i == 0 && eewInfos.count() == 0)
            {
                QDateTime ttimeUTC, ttimeKST;
                ttimeUTC.setTimeSpec(Qt::UTC);
                ttimeUTC.setTime_t(tempInfos.at(i).ttime);

                ttimeKST = convertKST(ttimeUTC);
                ui->timeLB->setText(ttimeKST.toString("yyyy-MM-dd hh:mm:ss ") +"KST");
            }

            OnsiteInfo *onsiteinfo = new OnsiteInfo;
            onsiteinfo->setup(tempInfos.at(i));
            ui->alertListVLO->addWidget(onsiteinfo);

            if(QString(tempInfos.at(i).fromWhere).startsWith("K"))
            {
                drawOnsiteOnMap(1, tempInfos.at(i));
                /*
                int j;
                for(j=0;j<configure.kissStaVT.count();j++)
                {
                    if(configure.kissStaVT.at(j).sta.startsWith(QString(tempInfos.at(i).sta)) &&
                            configure.kissStaVT.at(j).net.startsWith(QString(tempInfos.at(i).net)))
                        break;
                }
                drawOnsiteOnMap(1, tempInfos.at(i), j);
                */

                if(tempInfos.at(i).lat < minLat) minLat = tempInfos.at(i).lat;
                if(tempInfos.at(i).lat > maxLat) maxLat = tempInfos.at(i).lat;
                if(tempInfos.at(i).lon < minLon) minLon = tempInfos.at(i).lon;
                if(tempInfos.at(i).lon > maxLon) maxLon = tempInfos.at(i).lon;
            }
            else if(QString(tempInfos.at(i).fromWhere).startsWith("L"))
            {
                drawOnsiteOnMap(0, tempInfos.at(i));
                /*
                int j;
                for(j=0;j<configure.localStaVT.count();j++)
                {
                    if(configure.localStaVT.at(j).sta.startsWith(QString(tempInfos.at(i).sta)) &&
                            configure.localStaVT.at(j).net.startsWith(QString(tempInfos.at(i).net)))
                        break;
                }
                drawOnsiteOnMap(0, tempInfos.at(i), j);
                */

                if(tempInfos.at(i).lat < minLat) minLat = tempInfos.at(i).lat;
                if(tempInfos.at(i).lat > maxLat) maxLat = tempInfos.at(i).lat;
                if(tempInfos.at(i).lon < minLon) minLon = tempInfos.at(i).lon;
                if(tempInfos.at(i).lon > maxLon) maxLon = tempInfos.at(i).lon;
            }
        }
    }

    if(pgaInfos.count() != 0)
    {
        PgaInfo *pgainfo = new PgaInfo;
        pgainfo->setup(pgaChannel, pgaTime, pgaInfos);
        ui->alertListVLO->addWidget(pgainfo);

        for(int i=0;i<pgaInfos.count();i++)
        {
            drawPGAOnMap(pgaChannel, pgaInfos.at(i));
            if(pgaInfos.at(i).lat < minLat) minLat = pgaInfos.at(i).lat;
            if(pgaInfos.at(i).lat > maxLat) maxLat = pgaInfos.at(i).lat;
            if(pgaInfos.at(i).lon < minLon) minLon = pgaInfos.at(i).lon;
            if(pgaInfos.at(i).lon > maxLon) maxLon = pgaInfos.at(i).lon;
        }
    }

    ui->alertListVLO->addStretch(1);

    if(configure.myposition_lat < minLat) minLat = configure.myposition_lat;
    if(configure.myposition_lat > maxLat) maxLat = configure.myposition_lat;
    if(configure.myposition_lon < minLon) minLon = configure.myposition_lon;
    if(configure.myposition_lon > maxLon) maxLon = configure.myposition_lon;

    QMetaObject::invokeMethod(this->aRootObj, "addMyPositionMarker",
                              Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, configure.myposition_lat), Q_ARG(QVariant, configure.myposition_lon));

    double dist = getDistance(minLat, minLon, maxLat, maxLon);
    QMetaObject::invokeMethod(this->aRootObj, "adjustMap", Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, (minLat+maxLat)/2), Q_ARG(QVariant, (minLon+maxLon)/2), Q_ARG(QVariant, dist));  
}

void MainWindow::drawIntensityOnMap(QString yearMonth, QString evid)
{
    QString fileName = configure.KGOM_HOME + "/data/INTENSITY/" + yearMonth.left(4) + "/" + yearMonth.mid(4, 2) + "/" + evid + "/Intensity.dat";

    int numPath = 0, numLine = 0;
    QVector<double> lat;
    QVector<double> lon;
    QVector<double> inten;

    QFile file(fileName);

    if(!file.exists())
    {
        log->write(configure.KGOM_HOME + "/logs/", "This event doesn't have a Intensity file.");
        return;
    }
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;

        line = stream.readLine();
        _line = line.simplified();
        numPath = _line.section(" ", 0, 0).toInt();
        numLine = _line.section(" ", 1, 1).toInt();

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(!_line.startsWith(" "))
            {
                lon.push_back(_line.section(" ", 0, 0).toDouble());
                lat.push_back(_line.section(" ", 1, 1).toDouble());
                inten.push_back(_line.section(" ", 2, 2).toDouble());
            }
        }
        file.close();
    }

    for(int i=0;i<numPath;i++)
    {
        QVariantList latP;
        QVariantList lonP;
        QVariantList intenP;
        QVariantList colorP;

        for(int j=i*numLine;j<=(i+1)*numLine-1;j++)
        {
            latP.push_back(lat.at(j));
            lonP.push_back(lon.at(j));
            intenP.push_back(inten.at(j));

            int intenI = inten.at(j);
            colorP.push_back(getIntenColor(intenI).name());
        }

        QMetaObject::invokeMethod(this->aRootObj, "addPolygon", Q_RETURN_ARG(QVariant, aReturnedValue),
                                  Q_ARG(QVariant, latP), Q_ARG(QVariant, lonP), Q_ARG(QVariant, colorP), Q_ARG(QVariant, numLine));
    }
}

void MainWindow::adjustRealTimePGAMap()
{
    double minLat = 999, maxLat = 0, minLon = 999, maxLon = 0;
    for(int i=0;i<configure.kissStaVT.count();i++)
    {
        if(configure.kissStaVT.at(i).lat < minLat) minLat = configure.kissStaVT.at(i).lat;
        if(configure.kissStaVT.at(i).lat > maxLat) maxLat = configure.kissStaVT.at(i).lat;
        if(configure.kissStaVT.at(i).lon < minLon) minLon = configure.kissStaVT.at(i).lon;
        if(configure.kissStaVT.at(i).lon > maxLon) maxLon = configure.kissStaVT.at(i).lon;
    }

    if(configure.myposition_lat < minLat) minLat = configure.myposition_lat;
    if(configure.myposition_lat > maxLat) maxLat = configure.myposition_lat;
    if(configure.myposition_lon < minLon) minLon = configure.myposition_lon;
    if(configure.myposition_lon > maxLon) maxLon = configure.myposition_lon;

    QMetaObject::invokeMethod(this->rRootObj, "addMyPositionMarker",
                              Q_RETURN_ARG(QVariant, rReturnedValue),
                              Q_ARG(QVariant, configure.myposition_lat), Q_ARG(QVariant, configure.myposition_lon));

    double dist = getDistance(minLat, minLon, maxLat, maxLon);
    QMetaObject::invokeMethod(this->rRootObj, "adjustMap", Q_RETURN_ARG(QVariant, rReturnedValue),
                              Q_ARG(QVariant, (minLat+maxLat)/2), Q_ARG(QVariant, (minLon+maxLon)/2), Q_ARG(QVariant, dist));
}

void MainWindow::drawEEWOnMap(_EEWInfo eewInfo)
{
    QString tempMag = QString::number(eewInfo.magnitude, 'f', 1);

    QMetaObject::invokeMethod(this->aRootObj, "addEEWStarMarker",
                              Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, eewInfo.latitude), Q_ARG(QVariant, eewInfo.longitude),
                              Q_ARG(QVariant, tempMag));
}

void MainWindow::drawPGAOnMap(QString chan, _KGKIIS_GMPEAK_EVENT_STA_t pgaInfo)
{
    double lat, lon;
    lat = pgaInfo.lat;
    lon = pgaInfo.lon;
    QString pgaS;

    if(chan.startsWith("Z")) pgaS = QString::number(pgaInfo.maxZ, 'f', 4);
    else if(chan.startsWith("N")) pgaS = QString::number(pgaInfo.maxN, 'f', 4);
    else if(chan.startsWith("E")) pgaS = QString::number(pgaInfo.maxE, 'f', 4);
    else if(chan.startsWith("H")) pgaS = QString::number(pgaInfo.maxH, 'f', 4);
    else if(chan.startsWith("T")) pgaS = QString::number(pgaInfo.maxT, 'f', 4);

    QString text = QString(pgaInfo.sta) + "\n" + pgaS + " gal";

    int net;    // 1 : KISS, 0 : LOCAL
    if(QString(pgaInfo.net).startsWith("KG") || QString(pgaInfo.net).startsWith("KS"))
        net = 1;
    else
        net = 0;

    QMetaObject::invokeMethod(this->aRootObj, "addStaMarker",
                              Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, net),
                              Q_ARG(QVariant, lat), Q_ARG(QVariant, lon),
                              Q_ARG(QVariant, text));
}

void MainWindow::drawOnsiteOnMap(int net, _KGOnSite_Info_t info)
{
    double radius = info.distance * 1000;
    //QString text = "Alert:" + QString::number(info.alert) + ", Dist.:" + QString::number(info.distance,'f',1) + "Km";

    double lat, lon;
    lat = info.lat;
    lon = info.lon;

    QMetaObject::invokeMethod(this->aRootObj, "addStaMarker",
                              Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, net),
                              Q_ARG(QVariant, lat), Q_ARG(QVariant, lon),
                              Q_ARG(QVariant, info.sta));


    QMetaObject::invokeMethod(this->aRootObj, "addCircle",
                              Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, lat), Q_ARG(QVariant, lon),
                              Q_ARG(QVariant, radius));

/*
    QMetaObject::invokeMethod(this->aRootObj, "addText",
                              Q_RETURN_ARG(QVariant, aReturnedValue),
                              Q_ARG(QVariant, pick.lat), Q_ARG(QVariant, pick.lon),
                              Q_ARG(QVariant, text));
                              */
}

void MainWindow::setup()
{
    eventMode = 0;
    maxMag = 0;
    readConfigure(configure.configFileName);
    createActionsOnToolbar();
    createStaCircleOnRealTimePGAMap();

    // Define a consumer
    if(!krecveew->isRunning() && configure.kiss_eew_amq.topic != "")
    {
        QString failover = "failover:(tcp://" + configure.kiss_eew_amq.ip + ":" + configure.kiss_eew_amq.port + ")";
        krecveew->setup(failover, configure.kiss_eew_amq.user, configure.kiss_eew_amq.passwd, configure.kiss_eew_amq.topic, true, false);
        connect(krecveew, SIGNAL(_rvEEWInfo(_EEWInfo)), this, SLOT(rvEEWInfo(_EEWInfo)));
        krecveew->start();
    }

    if(!configure.kissStaVT.isEmpty())
    {
        if(!krecvOnsite->isRunning() && configure.kiss_onsite_amq.topic != "")
        {
            QString failover = "failover:(tcp://" + configure.kiss_onsite_amq.ip + ":" + configure.kiss_onsite_amq.port + ")";
            krecvOnsite->setup(failover, configure.kiss_onsite_amq.user, configure.kiss_onsite_amq.passwd, configure.kiss_onsite_amq.topic, true, false, 1);
            connect(krecvOnsite, SIGNAL(_rvOnsiteInfo(_KGOnSite_Info_t)), this, SLOT(rvOnsiteInfo(_KGOnSite_Info_t)));
            krecvOnsite->start();
        }

        if(!krecvSoh->isRunning() && configure.kiss_soh_amq.topic != "")
        {
            QString failover = "failover:(tcp://" + configure.kiss_soh_amq.ip + ":" + configure.kiss_soh_amq.port + ")";
            krecvSoh->setup(failover, configure.kiss_soh_amq.user, configure.kiss_soh_amq.passwd, configure.kiss_soh_amq.topic, true, false);
            connect(krecvSoh, SIGNAL(_rvSOHInfo(_KGOnSite_SOH_t, int, int)), this, SLOT(rvSOHInfo(_KGOnSite_SOH_t, int, int)));
            krecvSoh->start();
        }

        // RealTime PGA
        if(!krecvPGA->isRunning() && configure.kiss_pga_amq.topic != "")
        {
            QString failover = "failover:(tcp://" + configure.kiss_pga_amq.ip + ":" + configure.kiss_pga_amq.port + ")";
            krecvPGA->setup(failover, configure.kiss_pga_amq.user, configure.kiss_pga_amq.passwd, configure.kiss_pga_amq.topic, true, false);
            connect(krecvPGA, SIGNAL(_rvPGAMultiMap(QMultiMap<int, _QSCD_FOR_MULTIMAP>)), this, SLOT(rvPGAMultiMap(QMultiMap<int, _QSCD_FOR_MULTIMAP>)));
            krecvPGA->start();
        }
        else if(configure.kiss_pga_amq.topic == "")
        {
            ui->mainTW->removeTab(2);
        }

        krecvPGA->updateStation(configure.kissStaVT);
        krecvOnsite->updateStation(configure.kissStaVT);
        krecvSoh->updateStation(configure.kissStaVT, 1);
    }
    else
        ui->mainTW->removeTab(2);

    if(!configure.localStaVT.isEmpty())
    {
        // Define a consumer
        if(!lrecvOnsite->isRunning() && configure.local_onsite_amq.topic != "")
        {
            QString failover = "failover:(tcp://" + configure.local_onsite_amq.ip + ":" + configure.local_onsite_amq.port + ")";
            lrecvOnsite->setup(failover, configure.local_onsite_amq.user, configure.local_onsite_amq.passwd, configure.local_onsite_amq.topic, true, false, 0);
            connect(lrecvOnsite, SIGNAL(_rvOnsiteInfo(_KGOnSite_Info_t)), this, SLOT(rvOnsiteInfo(_KGOnSite_Info_t)));
            lrecvOnsite->start();
        }

        if(!lrecvPGA->isRunning() && configure.local_pga_amq.topic != "")
        {
            QString failover = "failover:(tcp://" + configure.local_pga_amq.ip + ":" + configure.local_pga_amq.port + ")";
            lrecvPGA->setup(failover, configure.local_pga_amq.user, configure.local_pga_amq.passwd, configure.local_pga_amq.topic, true, false);
            connect(lrecvPGA, SIGNAL(_rvPGAMultiMap(QMultiMap<int, _QSCD_FOR_MULTIMAP>)), this, SLOT(rvPGAMultiMap(QMultiMap<int, _QSCD_FOR_MULTIMAP>)));
            //connect(lrecvPGA, SIGNAL(_rvPGAInfo(_KGKIIS_GMPEAK_EVENT_t)), this, SLOT(rvPGAInfo(_KGKIIS_GMPEAK_EVENT_t)));
            lrecvPGA->start();
        }

        if(!lrecvSoh->isRunning() && configure.local_soh_amq.topic != "")
        {
            QString failover = "failover:(tcp://" + configure.local_soh_amq.ip + ":" + configure.local_soh_amq.port + ")";
            lrecvSoh->setup(failover, configure.local_soh_amq.user, configure.local_soh_amq.passwd, configure.local_soh_amq.topic, true, false);
            connect(lrecvSoh, SIGNAL(_rvSOHInfo(_KGOnSite_SOH_t, int, int)), this, SLOT(rvSOHInfo(_KGOnSite_SOH_t, int, int)));
            lrecvSoh->start();
        }

        lrecvPGA->updateStation(configure.localStaVT);
        lrecvOnsite->updateStation(configure.localStaVT);
        lrecvSoh->updateStation(configure.localStaVT, 0);
    }

    QLayoutItem *child;

    while ((child = ui->eventsListVLO->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    // Setup dates on table
    if(sfMagIndex == 0)
        setEventsTab(0, 0, sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 1)
        setEventsTab(magValue[0], magValue[1], sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 2)
        setEventsTab(magValue[1], magValue[2], sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 3)
        setEventsTab(magValue[2], magValue[3], sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 4)
        setEventsTab(magValue[3], 999, sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);
    else if(sfMagIndex == 5)
        setEventsTab(sfMinMag, sfMaxMag, sfDateIndex, sfNEvents, sfNDays, sfStartDate, sfEndDate);

    ui->mainTW->setCurrentIndex(0);

    setupPGATable();
    adjustRealTimePGAMap();
}

void MainWindow::_qmlSignalfromEMap(QString ids, QString lats, QString lons)
{
    eventClicked(ids, lats, lons);
}

void MainWindow::eventClicked(QString ids, QString lats, QString lons)
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    QList<QWidget*>::iterator it;

    QMetaObject::invokeMethod(this->eRootObj, "moveCurrentEventMarker",
                              Q_RETURN_ARG(QVariant, eReturnedValue),
                              Q_ARG(QVariant, ids),
                              Q_ARG(QVariant, lats),
                              Q_ARG(QVariant, lons));

    for (it = widgets.begin(); it != widgets.end(); it++)
    {
        if((*it)->objectName() == lastObjectName)
        {

            (*it)->setStyleSheet("color:black; background-color:white;");
        }
    }

    for (it = widgets.begin(); it != widgets.end(); it++)
    {
        if((*it)->objectName() == ids)  // here, objectName is not working!!
        {
            (*it)->setStyleSheet("color:black; background-color:#33FFFF;");
            lastObjectName = ids;
        }
    }
}

void MainWindow::eventDoubleClicked(QString evidS)
{
    getEventInfo(evidS.section(":", 1, 1).toInt());
    ui->mainTW->setCurrentIndex(1);
}

void MainWindow::setEventsTab(double sMag, double eMag, int dateIndex, int nEvents, int nDays, QDate sDate, QDate eDate)
{
    QLayoutItem *child;

    while ((child = ui->eventsListVLO->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    QMetaObject::invokeMethod(this->eRootObj, "clearMap", Q_RETURN_ARG(QVariant, eReturnedValue));
    QMetaObject::invokeMethod(this->eRootObj, "addMyPositionMarker",
                              Q_RETURN_ARG(QVariant, eReturnedValue),
                              Q_ARG(QVariant, configure.myposition_lat), Q_ARG(QVariant, configure.myposition_lon));

    QString query;

    if(dateIndex == 0) query = "SELECT * FROM event ORDER BY rowid DESC LIMIT " + QString::number(nEvents);
    else if(dateIndex == 1)
    {
        QDate sd, ed;
        ed = QDate::currentDate();
        sd = ed.addDays(-nDays);
        query = "SELECT * FROM event WHERE lddate >= " + sd.toString("yyyyMMdd") +
                " AND lddate <= " + ed.toString("yyyyMMdd") + " ORDER BY rowid DESC";
    }
    else if(dateIndex == 2)
    {
        query = "SELECT * FROM event WHERE lddate >= " + sDate.toString("yyyyMMdd") +
                " AND lddate <= " + eDate.toString("yyyyMMdd") + " ORDER BY rowid DESC";
    }

    this->eventModel->setQuery(query);  // get event list

    double minLat = 999, maxLat = 0;
    double minLon = 999, maxLon = 0;

    QVector<_EEWInfo> eewInfos;

    for(int i=0;i<this->eventModel->rowCount();i++)
    {
        _EEWInfo eewInfo;
        eewInfo.evid = this->eventModel->record(i).value("evid").toInt();

        if(sMag == 0 && eMag == 0)
            query = "SELECT * FROM eewInfo WHERE evid = " + this->eventModel->record(i).value("evid").toString() + " ORDER BY rowid DESC LIMIT 1";
        else
        {
            query = "SELECT * FROM eewInfo WHERE magnitude > " + QString::number(sMag, 'f', 1) + " AND magnitude <= " + QString::number(eMag, 'f', 1) +
                    " AND evid = " + this->eventModel->record(i).value("evid").toString() + " ORDER BY rowid DESC LIMIT 1";
        }
        this->eewModel->setQuery(query);  

        if(this->eewModel->rowCount() > 0)
        {
            eewInfo.magnitude = this->eewModel->record(0).value("magnitude").toDouble();
            eewInfo.latitude = this->eewModel->record(0).value("latitude").toDouble();
            eewInfo.longitude = this->eewModel->record(0).value("longitude").toDouble();
            eewInfo.origin_time = this->eewModel->record(0).value("origin_time").toDouble();

            if(eewInfo.latitude < minLat) minLat = eewInfo.latitude;
            if(eewInfo.latitude > maxLat) maxLat = eewInfo.latitude;
            if(eewInfo.longitude < minLon) minLon = eewInfo.longitude;
            if(eewInfo.longitude > maxLon) maxLon = eewInfo.longitude;

            double dist = getDistance(eewInfo.latitude, eewInfo.longitude, configure.myposition_lat, configure.myposition_lon);

            EventList *el = new EventList(0, eewInfo, codec->toUnicode("거리(관심지점)  \n") + QString::number(dist, 'f', 0) + "Km ", configure.KGOM_HOME);
            ui->eventsListVLO->addWidget(el);
            eewInfos.push_back(eewInfo);

            el->setObjectName(QString::number(eewInfo.evid));
            connect(el, SIGNAL(clicked(QString, QString, QString)), this, SLOT(eventClicked(QString, QString, QString)));
            connect(el, SIGNAL(doubleClicked(QString)), this, SLOT(eventDoubleClicked(QString)));

            QMetaObject::invokeMethod(this->eRootObj, "addEventCircle",
                                      Q_RETURN_ARG(QVariant, eReturnedValue),
                                      Q_ARG(QVariant, eewInfo.latitude), Q_ARG(QVariant, eewInfo.longitude),
                                      Q_ARG(QVariant, getMagColor(eewInfo.magnitude).name()),
                                      Q_ARG(QVariant, eewInfo.evid));

            continue;
        }

        if(sMag == 0 && eMag == 0)
            query = "SELECT * FROM onsiteInfo WHERE evid = " + this->eventModel->record(i).value("evid").toString();
        else
        {
            query = "SELECT * FROM onsiteInfo WHERE magnitude > " + QString::number(sMag, 'f', 1) + " AND magnitude <= " + QString::number(eMag, 'f', 1) +
                    " AND evid = " + this->eventModel->record(i).value("evid").toString();
        }
        this->onsiteModel->setQuery(query);

        if(this->onsiteModel->rowCount() > 0)
        {
            int maxDuration = 0;
            for(int j=0;j<this->onsiteModel->rowCount();j++)
            {
                if(this->onsiteModel->record(j).value("duration").toInt() > maxDuration)
                    maxDuration = this->onsiteModel->record(j).value("duration").toInt();
            }

            for(int j=0;j<this->onsiteModel->rowCount();j++)
            {
                if(this->onsiteModel->record(j).value("duration").toInt() == maxDuration)
                {
                    eewInfo.magnitude = this->onsiteModel->record(j).value("magnitude").toDouble();
                    eewInfo.latitude = this->onsiteModel->record(j).value("lat").toDouble();
                    eewInfo.longitude = this->onsiteModel->record(j).value("lon").toDouble();
                    eewInfo.origin_time = this->onsiteModel->record(j).value("ttime").toDouble();

                    if(eewInfo.latitude < minLat) minLat = eewInfo.latitude;
                    if(eewInfo.latitude > maxLat) maxLat = eewInfo.latitude;
                    if(eewInfo.longitude < minLon) minLon = eewInfo.longitude;
                    if(eewInfo.longitude > maxLon) maxLon = eewInfo.longitude;

                    double dist = getDistance(eewInfo.latitude, eewInfo.longitude, configure.myposition_lat, configure.myposition_lon);

                    EventList *el = new EventList(0, eewInfo, codec->toUnicode("거리(관심지점)  \n") + QString::number(dist, 'f', 0) + "Km ", configure.KGOM_HOME);
                    ui->eventsListVLO->addWidget(el);
                    eewInfos.push_back(eewInfo);

                    el->setObjectName(QString::number(eewInfo.evid));
                    connect(el, SIGNAL(clicked(QString, QString, QString)), this, SLOT(eventClicked(QString, QString, QString)));
                    connect(el, SIGNAL(doubleClicked(QString)), this, SLOT(eventDoubleClicked(QString)));

                    QMetaObject::invokeMethod(this->eRootObj, "addEventCircle",
                                              Q_RETURN_ARG(QVariant, eReturnedValue),
                                              Q_ARG(QVariant, eewInfo.latitude), Q_ARG(QVariant, eewInfo.longitude),
                                              Q_ARG(QVariant, getMagColor(eewInfo.magnitude).name()),
                                              Q_ARG(QVariant, eewInfo.evid));

                    break;
                }
            }
            continue;
        }

        //find pga
        if(sMag == 0 && eMag == 0)
        {
            query = "SELECT * FROM pgaInfo WHERE evid = " + this->eventModel->record(i).value("evid").toString();
            this->pgaModel->setQuery(query);

            if(this->pgaModel->rowCount() > 0)
            {
                if(this->pgaModel->record(0).value("target_chan").toString().startsWith("Z"))
                    eewInfo.magnitude = this->pgaModel->record(0).value("maxZ").toDouble();
                else if(this->pgaModel->record(0).value("target_chan").toString().startsWith("N"))
                    eewInfo.magnitude = this->pgaModel->record(0).value("maxN").toDouble();
                else if(this->pgaModel->record(0).value("target_chan").toString().startsWith("E"))
                    eewInfo.magnitude = this->pgaModel->record(0).value("maxE").toDouble();
                else if(this->pgaModel->record(0).value("target_chan").toString().startsWith("H"))
                    eewInfo.magnitude = this->pgaModel->record(0).value("maxH").toDouble();
                else if(this->pgaModel->record(0).value("target_chan").toString().startsWith("T"))
                    eewInfo.magnitude = this->pgaModel->record(0).value("maxT").toDouble();

                eewInfo.latitude = this->pgaModel->record(0).value("lat").toDouble();
                eewInfo.longitude = this->pgaModel->record(0).value("lon").toDouble();
                eewInfo.origin_time = this->pgaModel->record(0).value("time").toDouble();

                if(eewInfo.latitude < minLat) minLat = eewInfo.latitude;
                if(eewInfo.latitude > maxLat) maxLat = eewInfo.latitude;
                if(eewInfo.longitude < minLon) minLon = eewInfo.longitude;
                if(eewInfo.longitude > maxLon) maxLon = eewInfo.longitude;

                double dist = getDistance(eewInfo.latitude, eewInfo.longitude, configure.myposition_lat, configure.myposition_lon);

                EventList *el = new EventList(1, eewInfo, codec->toUnicode("거리(관심지점)  \n") + QString::number(dist, 'f', 0) + "Km ", configure.KGOM_HOME);
                ui->eventsListVLO->addWidget(el);
                eewInfos.push_back(eewInfo);

                el->setObjectName(QString::number(eewInfo.evid));
                connect(el, SIGNAL(clicked(QString, QString, QString)), this, SLOT(eventClicked(QString, QString, QString)));
                connect(el, SIGNAL(doubleClicked(QString)), this, SLOT(eventDoubleClicked(QString)));

                QMetaObject::invokeMethod(this->eRootObj, "addEventCircle",
                                          Q_RETURN_ARG(QVariant, eReturnedValue),
                                          Q_ARG(QVariant, eewInfo.latitude), Q_ARG(QVariant, eewInfo.longitude),
                                          Q_ARG(QVariant, getMagColor(eewInfo.magnitude).name()),
                                          Q_ARG(QVariant, eewInfo.evid));
            }
        }
    }

    if(eewInfos.count() != 0)
    {
        ui->noEventLB->hide();
        ui->eventsListVLO->addStretch(1);

        if(configure.myposition_lat < minLat) minLat = configure.myposition_lat;
        if(configure.myposition_lat > maxLat) maxLat = configure.myposition_lat;
        if(configure.myposition_lon < minLon) minLon = configure.myposition_lon;
        if(configure.myposition_lon > maxLon) maxLon = configure.myposition_lon;

        double dist = getDistance(minLat, minLon, maxLat, maxLon);
        QMetaObject::invokeMethod(this->eRootObj, "adjustMap", Q_RETURN_ARG(QVariant, eReturnedValue),
                                  Q_ARG(QVariant, (minLat+maxLat)/2), Q_ARG(QVariant, (minLon+maxLon)/2), Q_ARG(QVariant, dist));

        //ui->sumLB->setText(QString::number(eewInfos.count()) + " earthquakes in map");
        ui->sumLB->setText(codec->toUnicode("지진목록 (") + QString::number(eewInfos.count()) + codec->toUnicode("개)"));
        QDateTime min, max;
        max.setTime_t(eewInfos.first().origin_time);
        min.setTime_t(eewInfos.last().origin_time);
        ui->sumDLB->setText(codec->toUnicode("기간 : ") + min.toString("yyyy.MM.dd") + " ~ " + max.toString("yyyy.MM.dd") + " KST");

        lastObjectName = QString::number(eewInfos.first().evid);
        getEventInfo(eewInfos.first().evid);
    }
    else
    {
        ui->sumLB->setText(codec->toUnicode("지진목록 (0개)"));
        ui->sumDLB->setText("");
        ui->noEventLB->show();
    }
}

void MainWindow::rvOnsiteInfo(_KGOnSite_Info_t info)
{ 
    int evid;
    QString query;
    if(eventMode == 0)
        evid = getLastEvid() + 1;
    else
        evid = getLastEvid();

    QString rvSCNL = QString(info.sta) + "/" + QString(info.chan) + "/" +
            QString(info.net) + "/" + QString(info.loc) + " " + QString::number(info.ttime, 'f', 0);

    if(info.alert < 0)                      info.alert = 0;
    if(info.intensity < 0)                  info.intensity = 0;
    if(info.intensity_uncertainty_low < 0)  info.intensity_uncertainty_low = 0;
    if(info.intensity_uncertainty_high < 0) info.intensity_uncertainty_high = 0;
    if(info.magnitude < 0)                  info.magnitude = 0;
    if(info.magnitude_uncertainty_low < 0)  info.magnitude_uncertainty_low = 0;
    if(info.magnitude_uncertainty_high < 0) info.magnitude_uncertainty_high = 0;
    if(info.log_disp_ratio == NULL) info.log_disp_ratio = 0;
    info.log_disp_ratio = 0;

    query = "INSERT INTO onsiteInfo "
                    "(evid, fromWhere, lat, lon, depth, "
                    "version, msg_type, "
                    "sta, chan, net, loc, "
                    "duration, type, ttime, disp_count, "
                    "displacement, disp_time, pvel, "
                    "pacc, tauc, pgv, "
                    "pgv_uncertainty_low, pgv_uncertainty_high, "
                    "alert, snr, log_disp_ratio, log_PdPv, dB_Pd, "
                    "intensity, intensity_uncertainty_low, intensity_uncertainty_high, "
                    "magnitude, magnitude_uncertainty_low, magnitude_uncertainty_high, "
                    "distance, distance_uncertainty_low, distance_uncertainty_high, lddate) values (" +
            QString::number(evid) + ", '" + QString(info.fromWhere) + "', " +
            QString::number(info.lat, 'f', 4) + ", " + QString::number(info.lon, 'f', 4) + ", " + QString::number(info.elev, 'f', 2) +
            ", 1, 'I', '" +
            QString(info.sta) + "', '" + QString(info.chan) + "', '" +
            QString(info.net) + "', '" + QString(info.loc) + "', '" +
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

    this->onsiteModel->setQuery(query);

    if(this->onsiteModel->lastError().isValid())
    {
        qDebug() << this->onsiteModel->lastError();
        log->write(configure.KGOM_HOME + "/logs/", "Detected a new ONSITE info. <" + rvSCNL + ">. But sql insert query error.");
        return;
    }

    log->write(configure.KGOM_HOME + "/logs/", "Detected a new ONSITE info. <" + rvSCNL + ">");

    double dist = getDistance(info.lat, info.lon, configure.myposition_lat, configure.myposition_lon);

    if(eventMode == 0)
    {
        query = "INSERT INTO event(evid, lddate) values (" +
                QString::number(evid) + ", '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";
        this->eventModel->setQuery(query);

        eventStartTimeUTC = QDateTime::currentDateTimeUtc();
        eventStartTimeKST = convertKST(eventStartTimeUTC);

        blinkTimer->start(1000);
        eventMode = 1;
        maxMag = info.magnitude;
        alerting(info.magnitude, dist);
        blinkColor.setNamedColor(getMagColor(info.magnitude).name());
        ui->mainToolBar->actions().at(3)->setVisible(true);

        log->write(configure.KGOM_HOME + "/logs/", "---- Started a new event ----");
        log->write(configure.KGOM_HOME + "/logs/", "Event ID: " + QString::number(evid));
        log->write(configure.KGOM_HOME + "/logs/", "Start Time(KST): " + eventStartTimeKST.toString("yyyy/MM/dd hh:mm:ss"));
        log->write(configure.KGOM_HOME + "/logs/", "End Time(KST): " + eventStartTimeKST.addSecs(EVENT_DURATION).toString("yyyy/MM/dd hh:mm:ss"));

        restoreAction->triggered();
        this->showMaximized();
        this->activateWindow();
        this->raise();
        ui->mainTW->setCurrentIndex(1);
        ui->mainTW->setTabEnabled(0, false);
    }
    else
    {
        if(maxMag <= info.magnitude)
        {
            maxMag = info.magnitude;
            //alerting(info.magnitude, dist);
            blinkColor.setNamedColor(getMagColor(info.magnitude).name());
        }
    }

    getEventInfo(evid);
}

void MainWindow::rvPGAInfo(_KGKIIS_GMPEAK_EVENT_t pgaInfos)
{
    int evid;
    QString query;
    if(eventMode == 0)
        evid = getLastEvid() + 1;
    else
        evid = getLastEvid();

    double avgLat = 0, avgLon = 0;

    for(int i=0;i<pgaInfos.nsta;i++)
    {
        query = "INSERT INTO pgaInfo "
                "(evid, version, msg_type, sta, chan, net, loc, lat, lon, target_chan, e_time, time, maxZ, maxN, maxE, maxH, maxT, lddate) values (" +
                QString::number(evid) + ", " + QString::number(pgaInfos.stainfo[i].version) + ", '" + QString(pgaInfos.stainfo[i].msg_type) + "', '" +
                QString(pgaInfos.stainfo[i].sta) + "', '" + QString(pgaInfos.stainfo[i].chan) + "', '" +
                QString(pgaInfos.stainfo[i].net) + "', '" + QString(pgaInfos.stainfo[i].loc) + "', " +
                QString::number(pgaInfos.stainfo[i].lat, 'f', 4) + ", " + QString::number(pgaInfos.stainfo[i].lon, 'f', 4) + ", '" +
                QString(pgaInfos.target_chan) + "', " + QString::number(pgaInfos.e_time, 'f', 0) + ", " +
                QString::number(pgaInfos.stainfo[i].time, 'f', 0) + ", " + QString::number(pgaInfos.stainfo[i].maxZ, 'f', 4) + ", " +
                QString::number(pgaInfos.stainfo[i].maxN, 'f', 4) + ", " + QString::number(pgaInfos.stainfo[i].maxE, 'f', 4) + ", " +
                QString::number(pgaInfos.stainfo[i].maxH, 'f', 4) + ", " + QString::number(pgaInfos.stainfo[i].maxT, 'f', 4) + ", '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";

        this->pgaModel->setQuery(query);

        avgLat = avgLat + pgaInfos.stainfo[i].lat;
        avgLon = avgLon + pgaInfos.stainfo[i].lon;
    }

    if(this->pgaModel->lastError().isValid())
    {
        qDebug() << this->pgaModel->lastError();
        log->write(configure.KGOM_HOME + "/logs/", "Detected a new PGA info. But sql insert query error.");
        return;
    }

    log->write(configure.KGOM_HOME + "/logs/", "Detected a new PGA info.");

    double dist = getDistance(avgLat / pgaInfos.nsta, avgLon / pgaInfos.nsta, configure.myposition_lat, configure.myposition_lon);

    if(eventMode == 0)
    {
        query = "INSERT INTO event(evid, lddate) values (" +
                QString::number(evid) + ", '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";
        this->eventModel->setQuery(query);

        eventStartTimeUTC = QDateTime::currentDateTimeUtc();
        eventStartTimeKST = convertKST(eventStartTimeUTC);

        blinkTimer->start(1000);
        eventMode = 1;

        ui->mainToolBar->actions().at(3)->setVisible(true);

        log->write(configure.KGOM_HOME + "/logs/", "---- Started a new event ----");
        log->write(configure.KGOM_HOME + "/logs/", "Event ID: " + QString::number(evid));
        log->write(configure.KGOM_HOME + "/logs/", "Start Time(KST): " + eventStartTimeKST.toString("yyyy/MM/dd hh:mm:ss"));
        log->write(configure.KGOM_HOME + "/logs/", "End Time(KST): " + eventStartTimeKST.addSecs(EVENT_DURATION).toString("yyyy/MM/dd hh:mm:ss"));

        this->showMaximized();
        this->activateWindow();
        this->raise();
        ui->mainTW->setCurrentIndex(1);
        ui->mainTW->setTabEnabled(0, false);
        alerting(900, dist);
        blinkColor.setNamedColor(getMagColor(990).name());
    }

    getEventInfo(evid);
}

void MainWindow::rvEEWInfo(_EEWInfo eewInfo)
{
    int evid;
    int needAnimation = 0;
    QString query;
    QString rvSCNL = QString::number(eewInfo.magnitude, 'f', 1) + "/" + QString::number(eewInfo.latitude, 'f', 4) + "/" +
            QString::number(eewInfo.longitude, 'f', 4) + "/" + QString::number(eewInfo.origin_time, 'f', 0);

    if(eventMode == 0)
    {
        evid = getLastEvid() + 1;
        query = "INSERT INTO event(evid, lddate) values (" +
                QString::number(evid) + ", '" +
                QDate::currentDate().toString("yyyyMMdd") + "')";
        this->eventModel->setQuery(query);
        needAnimation = 1;
    }
    else
    {
        evid = getLastEvid();

        query = "SELECT * FROM eewInfo WHERE evid = " + QString::number(evid);
        this->eewModel->setQuery(query);

        if(this->eewModel->rowCount() > 0) // eventMode = 1, update eewInfo
        {
            if(eewInfo.eew_evid != this->eewModel->record(0).value("evid").toInt())
                return;
            else
                needAnimation = 0;
        }
        else
            needAnimation = 1;
    }

    query = "INSERT INTO eewInfo "
                    "(evid , type , "
                    "eew_evid , version , message_category , message_type , "
                    "magnitude , magnitude_uncertainty , latitude , latitude_uncertainty , "
                    "longitude , longitude_uncertainty , depth , depth_uncertainty , "
                    "origin_time , origin_time_uncertainty , number_stations , "
                    "number_triggers , sent_flag , "
                    "sent_time , percentsta , misfit , status , lddate) values (" +
            QString::number(evid) + ", 'E', " +
            QString::number(eewInfo.eew_evid) + ", " + QString::number(eewInfo.version) + ", " +
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

    this->eewModel->setQuery(query);
    log->write(configure.KGOM_HOME + "/logs/", "Detected a new Network EEW info. <" + rvSCNL + ">");

    double dist = getDistance(eewInfo.latitude, eewInfo.longitude, configure.myposition_lat, configure.myposition_lon);

    if(eventMode == 0)
    {
        eventStartTimeUTC = QDateTime::currentDateTimeUtc();
        eventStartTimeKST = convertKST(eventStartTimeUTC);

        log->write(configure.KGOM_HOME + "/logs/", "---- Started a new event ----");
        log->write(configure.KGOM_HOME + "/logs/", "Event ID: " + QString::number(evid));
        log->write(configure.KGOM_HOME + "/logs/", "Start Time(KST): " + eventStartTimeKST.toString("yyyy/MM/dd hh:mm:ss"));
        log->write(configure.KGOM_HOME + "/logs/", "End Time(KST): " + eventStartTimeKST.addSecs(EVENT_DURATION).toString("yyyy/MM/dd hh:mm:ss"));

        blinkTimer->start(1000);
        eventMode = 1;
        maxMag = eewInfo.magnitude;
        alerting(eewInfo.magnitude, dist);
        blinkColor.setNamedColor(getMagColor(eewInfo.magnitude).name());
        ui->mainToolBar->actions().at(3)->setVisible(true);

        this->showMaximized();
        this->activateWindow();
        this->raise();
        ui->mainTW->setCurrentIndex(1);
        ui->mainTW->setTabEnabled(0, false); 
    }
    else
    {
        if(maxMag <= eewInfo.magnitude)
        {
            maxMag = eewInfo.magnitude;
            blinkColor.setNamedColor(getMagColor(eewInfo.magnitude).name());
        }
    }

    remainSecPAbsolute = (myRound(dist / configure.p_vel, 1));
    remainSecSAbsolute = (myRound(dist / configure.s_vel, 1));

    endTimeP.setTimeSpec(Qt::UTC);
    endTimeS.setTimeSpec(Qt::UTC);
    endTimeP.setTime_t(eewInfo.origin_time + remainSecPAbsolute);
    endTimeS.setTime_t(eewInfo.origin_time + remainSecSAbsolute);
    QDateTime now = QDateTime::currentDateTimeUtc();
    remainSecPRelative = endTimeP.toTime_t() - now.toTime_t();
    remainSecSRelative = endTimeS.toTime_t() - now.toTime_t();
    //remainSecPRelative = remainSecPAbsolute;
    //remainSecSRelative = remainSecSAbsolute;

    //qDebug() << endTimeS.toTime_t() << now.toTime_t() << remainSecSRelative << dist << remainSecSAbsolute;
    //1533104593 1533104598 4.29497e+09 18.076 4.5
    // 1533116482 1533116247 235 281.019 70.3

    if(now.toTime_t() >= endTimeP.toTime_t())
        remainSecPRelative = -10;
    if(now.toTime_t() >= endTimeS.toTime_t())
        remainSecSRelative = -10;

    if(needAnimation == 1)
    {
        //aniCount = 0;
        aniTimer->start(100);
    }

    aniLat = eewInfo.latitude;
    aniLon = eewInfo.longitude;

    Intensity = QString::number(getIntenFromMag(eewInfo.magnitude, dist));
    Vibration = getStringFromInten(getIntenFromMag(eewInfo.magnitude, dist));

    getEventInfo(evid);
}

void MainWindow::rvSOHInfo(_KGOnSite_SOH_t soh, int isKissStation, int index)
{
    if(isKissStation == 0)
        sohLocalMon[index]->update(soh);
    else if(isKissStation == 1)
        sohKissMon[index]->update(soh);
}

void MainWindow::recvSOHfromWG(int status, QString sta, QString net)
{
    QString style = "background-color: " + sohColor[status].name();

    if(net.startsWith("KS") || net.startsWith("KG"))
    {
        for(int i=0;i<configure.kissStaVT.count();i++)
        {
            if(configure.kissStaVT.at(i).sta.startsWith(sta))
            {
                sohKissPB[i]->setStyleSheet(style);
                return;
            }
        }
    }

    for(int i=0;i<configure.localStaVT.count();i++)
    {
        if(configure.localStaVT.at(i).sta.startsWith(sta))
        {
            sohLocalPB[i]->setStyleSheet(style);
            return;
        }
    }
}

void MainWindow::rvPGAMultiMap(QMultiMap<int, _QSCD_FOR_MULTIMAP> mmFromAMQ)
{
    QMapIterator<int, _QSCD_FOR_MULTIMAP> i(mmFromAMQ);
    mutex.lock();
    while(i.hasNext())
    {
        i.next();
        pgaHouse.insert(i.key(), i.value());
    }
    mutex.unlock();
}

