#include "configurationwizard.h"
#include "ui_configurationwizard.h"

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));

    QLabel *label = new QLabel(tr("This wizard will help you configure stations and set "
                               "for KGom."));
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

LocalStaRegistPage::LocalStaRegistPage(_CONFIGURE con, QObject *rootObj, QWidget *parent)
{
    thisObj = rootObj;
    logDir = con.KGONSITE_HOME + "/logs";
    log = new WriteLog;

    setTitle(tr("Do you have your own seismic stations?"));

    QLabel *subLabel = new QLabel;
    subLabel->setTextFormat(Qt::RichText);
    subLabel->setText(tr("These stations will be marked on the MapView using "
                      "\<img src=\":Icon/triangle-blue.png\" width=20 height=20 \> symbol."));
    //subLabel->setText("<img src=":/Icon/localMarger.png">");

    QFont font("Ubuntu", 9, QFont::Normal);

    QVBoxLayout *vl = new QVBoxLayout;
    vl->addWidget(subLabel);

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Preferred, QSizePolicy::Preferred);
    vl->addItem(spacer);

    for(int i=0;i<MAX_LOCALSTA_NUM;i++)
    {
        vl->addItem(spacer);

        nameLB[i] = new QLabel(tr("Local Station No. ") + QString::number(i + 1));
        lstaLB[i] = new QLineEdit; lstaLB[i]->setPlaceholderText(tr("Station")); lstaLB[i]->setClearButtonEnabled(true); lstaLB[i]->setFont(font);
        lchanLB[i] = new QLineEdit; lchanLB[i]->setPlaceholderText(tr("Channel")); lchanLB[i]->setClearButtonEnabled(false); lchanLB[i]->setFont(font);
        lchanLB[i]->setText("HGZ"); lchanLB[i]->setEnabled(false);
        lnetLB[i] = new QLineEdit; lnetLB[i]->setPlaceholderText(tr("Network")); lnetLB[i]->setClearButtonEnabled(true); lnetLB[i]->setFont(font);
        llocLB[i] = new QLineEdit; llocLB[i]->setPlaceholderText(tr("Location")); llocLB[i]->setClearButtonEnabled(true); llocLB[i]->setFont(font);
        llatLB[i] = new QLineEdit; llatLB[i]->setPlaceholderText(tr("Latitude")); llatLB[i]->setClearButtonEnabled(true); llatLB[i]->setFont(font);
        llonLB[i] = new QLineEdit; llonLB[i]->setPlaceholderText(tr("Longitude")); llonLB[i]->setClearButtonEnabled(true); llonLB[i]->setFont(font);
        lelevLB[i] = new QLineEdit; lelevLB[i]->setPlaceholderText(tr("Elevation (m)")); lelevLB[i]->setClearButtonEnabled(true); lelevLB[i]->setFont(font);
        ldepthLB[i] = new QLineEdit; ldepthLB[i]->setPlaceholderText(tr("Depth (m)")); ldepthLB[i]->setClearButtonEnabled(true); ldepthLB[i]->setFont(font);
        addPB[i] = new QPushButton(tr("ADD")); addPB[i]->setFont(font); addPB[i]->setObjectName("addPB" + QString::number(i));
        delPB[i] = new QPushButton(tr("DELETE")); delPB[i]->setFont(font); delPB[i]->setObjectName("delPB" + QString::number(i));
        connect(addPB[i], SIGNAL(clicked(bool)), this, SLOT(addLocalStaPBClicked()));
        connect(delPB[i], SIGNAL(clicked(bool)), this, SLOT(delLocalStaPBClicked()));

        registerField("localSta" + QString::number(i), lstaLB[i]);
        registerField("localChan" + QString::number(i), lchanLB[i]);
        registerField("localNet" + QString::number(i), lnetLB[i]);
        registerField("localLoc" + QString::number(i), llocLB[i]);
        registerField("localLat" + QString::number(i), llatLB[i]);
        registerField("localLon" + QString::number(i), llonLB[i]);
        registerField("localElev" + QString::number(i), lelevLB[i]);
        registerField("localDepth" + QString::number(i), ldepthLB[i]);

        vl->addWidget(nameLB[i]);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(lstaLB[i], i, 0);
        layout->addWidget(lchanLB[i], i, 1);
        layout->addWidget(lnetLB[i], i, 2);
        layout->addWidget(llocLB[i], i, 3);
        layout->addWidget(llatLB[i], i+1, 0);
        layout->addWidget(llonLB[i], i+1, 1);
        layout->addWidget(lelevLB[i], i+1, 2);
        layout->addWidget(ldepthLB[i], i+1, 3);
        layout->addWidget(addPB[i], i+2, 2);
        layout->addWidget(delPB[i], i+2, 3);

        vl->addLayout(layout);
    }

    vl->addStretch(1);
    setLayout(vl);

    for(int i=0;i<con.localStaCount;i++)
    {
        lstaLB[i]->setText(con.localStaVT.at(i).sta);
        lchanLB[i]->setText(con.localStaVT.at(i).chan);
        lnetLB[i]->setText(con.localStaVT.at(i).net);
        llocLB[i]->setText(con.localStaVT.at(i).loc);
        llatLB[i]->setText(QString::number(con.localStaVT.at(i).lat,'f', 6));
        llonLB[i]->setText(QString::number(con.localStaVT.at(i).lon,'f', 6));
        lelevLB[i]->setText(QString::number(con.localStaVT.at(i).elev,'f', 1));
        ldepthLB[i]->setText(QString::number(con.localStaVT.at(i).depth,'f',1));
        addPB[i]->setText("EDIT");

        QMetaObject::invokeMethod(thisObj, "addLocalStaMarker",
                                  Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, con.localStaVT.at(i).lat), Q_ARG(QVariant, con.localStaVT.at(i).lon),
                                  Q_ARG(QVariant, con.localStaVT.at(i).sta), Q_ARG(QVariant, i));
    }
}

void LocalStaRegistPage::addLocalStaPBClicked()
{
    QObject *senderObj = sender();
    int i = senderObj->objectName().right(1).toInt();

    QMetaObject::invokeMethod(thisObj, "addLocalStaMarker",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, llatLB[i]->text()), Q_ARG(QVariant, llonLB[i]->text()),
                              Q_ARG(QVariant, lstaLB[i]->text()), Q_ARG(QVariant, i));
    addPB[i]->setText(tr("EDIT"));
    log->write(logDir, "Added(Edited) " + lstaLB[i]->text() + " Local station.");
}

void LocalStaRegistPage::delLocalStaPBClicked()
{  
    if( QMessageBox::question( this,
                                tr("Delete this station."),
                                tr("Do you want to delete this station?"),
                                tr("Confirm"),
                                tr("Cancel"),
                                QString::null, 1, 1) )
    {
        return;
    }

    QObject *senderObj = sender();
    int i = senderObj->objectName().right(1).toInt();

    QMetaObject::invokeMethod(thisObj, "delLocalStaMarker",
                              Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, i));
    addPB[i]->setText(tr("ADD"));

    log->write(logDir, "Removed " + lstaLB[i]->text() + " Local station.");

    lstaLB[i]->clear();
    lnetLB[i]->clear();
    lchanLB[i]->clear();
    llocLB[i]->clear();
    llatLB[i]->clear();
    llonLB[i]->clear();
    lelevLB[i]->clear();
    ldepthLB[i]->clear();
}

KissStaRegistPage::KissStaRegistPage(_CONFIGURE con, QObject *rootObj, QWidget *parent)
{
    thisObj = rootObj;
    logDir = con.KGONSITE_HOME + "/logs";
    log = new WriteLog();

    setTitle(tr("Do you have public seismic stations?"));
    //setSubTitle(tr("If you don't have local stations, "          "then you must choose one or more KISS stations."));
    QLabel *subLabel = new QLabel;
    subLabel->setTextFormat(Qt::RichText);
    subLabel->setText(tr("These stations will be marked on the MapView using "
                      "\<img src=\":Icon/triangle-green.png\" width=20 height=20 \> symbol."));

    QFont font("Ubuntu", 9, QFont::Normal);

    QVBoxLayout *vl = new QVBoxLayout;
    QSpacerItem *spacerV = new QSpacerItem(20, 40, QSizePolicy::Preferred, QSizePolicy::Preferred);

    vl->addWidget(subLabel);
    //vl->addItem(spacerV);

    QHBoxLayout *hl = new QHBoxLayout;
    QLabel *temp = new QLabel;
    temp->setText(tr("Show or hide the public stations using buttons below."));

    vl->addWidget(temp);
    showPB = new QPushButton(tr("Show all public stations"));
    hidePB = new QPushButton(tr("Hide all public stations"));
    connect(showPB, SIGNAL(clicked(bool)), this, SLOT(showKissStaList()));
    connect(hidePB, SIGNAL(clicked(bool)), this, SLOT(hideKissStaList()));
    hl->addWidget(showPB); hl->addWidget(hidePB);
    vl->addLayout(hl);

    for(int i=0;i<MAX_KISSSTA_NUM;i++)
    {
        vl->addItem(spacerV);

        nameLB[i] = new QLabel(tr("Public Station No. ") + QString::number(i + 1));
        kstaLB[i] = new QLineEdit; kstaLB[i]->setPlaceholderText(tr("Station")); kstaLB[i]->setClearButtonEnabled(true); kstaLB[i]->setFont(font);
        knetLB[i] = new QLineEdit; klatLB[i] = new QLineEdit; klonLB[i] = new QLineEdit; kelevLB[i] = new QLineEdit;
        knetLB[i]->hide(); klatLB[i]->hide(); klonLB[i]->hide(); kelevLB[i]->hide();
        addPB[i] = new QPushButton(tr("ADD")); addPB[i]->setFont(font); addPB[i]->setObjectName("addPB" + QString::number(i));
        delPB[i] = new QPushButton(tr("DELETE")); delPB[i]->setFont(font); delPB[i]->setObjectName("delPB" + QString::number(i));
        connect(addPB[i], SIGNAL(clicked(bool)), this, SLOT(addKissStaPBClicked()));
        connect(delPB[i], SIGNAL(clicked(bool)), this, SLOT(delKissStaPBClicked()));

        registerField("kissSta" + QString::number(i), kstaLB[i]);
        registerField("kissNet" + QString::number(i), knetLB[i]);
        registerField("kissLat" + QString::number(i), klatLB[i]);
        registerField("kissLon" + QString::number(i), klonLB[i]);
        registerField("kissElev" + QString::number(i), kelevLB[i]);

        vl->addWidget(nameLB[i]);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(kstaLB[i], i, 0);
        layout->addWidget(addPB[i], i, 1);
        layout->addWidget(delPB[i], i, 2);

        vl->addLayout(layout);
    }

    vl->addStretch(1);
    setLayout(vl);

    setKISSStation(con.KGONSITE_HOME);

    for(int i=0;i<con.kissStaCount;i++)
    {
        kstaLB[i]->setText(con.kissStaVT.at(i).sta);
        net[i] = con.kissStaVT.at(i).net;
        lat[i] = con.kissStaVT.at(i).lat;
        lon[i] = con.kissStaVT.at(i).lon;
        elev[i] = con.kissStaVT.at(i).elev;
        knetLB[i]->setText(net[i]);
        klatLB[i]->setText(QString::number(lat[i], 'f', 6));
        klonLB[i]->setText(QString::number(lon[i], 'f', 6));
        kelevLB[i]->setText(QString::number(elev[i], 'f', 1));
        addPB[i]->setText(tr("EDIT"));

        QMetaObject::invokeMethod(thisObj, "addKissStaMarker",
                                  Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, con.kissStaVT.at(i).lat), Q_ARG(QVariant, con.kissStaVT.at(i).lon),
                                  Q_ARG(QVariant, con.kissStaVT.at(i).sta), Q_ARG(QVariant, i));
    }
}

void KissStaRegistPage::showKissStaList()
{
    QMetaObject::invokeMethod(thisObj, "showAllKissStaMarker", Q_RETURN_ARG(QVariant, returnedValue));
}

void KissStaRegistPage::hideKissStaList()
{
    QMetaObject::invokeMethod(thisObj, "hideAllKissStaMarker", Q_RETURN_ARG(QVariant, returnedValue));
}

void KissStaRegistPage::addKissStaPBClicked()
{
    QObject *senderObj = sender();
    int i = senderObj->objectName().right(1).toInt();

    for(int j=0;j<kissStaVector.count();j++)
    {
        //if(kissStaVector.at(j).sta.startsWith(kstaLB[i]->text()))
        if(kissStaVector.at(j).sta == kstaLB[i]->text())
        {
            net[i] = kissStaVector.at(j).net;
            lat[i] = kissStaVector.at(j).lat;
            lon[i] = kissStaVector.at(j).lon;
            elev[i] = kissStaVector.at(j).elev;

            knetLB[i]->setText(net[i]);
            klatLB[i]->setText(QString::number(lat[i], 'f', 6));
            klonLB[i]->setText(QString::number(lon[i], 'f', 6));
            kelevLB[i]->setText(QString::number(elev[i], 'f', 6));

            QMetaObject::invokeMethod(thisObj, "addKissStaMarker",
                                      Q_RETURN_ARG(QVariant, returnedValue),
                                      Q_ARG(QVariant, lat[i]), Q_ARG(QVariant, lon[i]),
                                      Q_ARG(QVariant, kstaLB[i]->text()), Q_ARG(QVariant, i));
            addPB[i]->setText(tr("EDIT"));
            log->write(logDir, "Added(Edited) " + kstaLB[i]->text() + " KISS station.");
            break;
        }
    }
}

void KissStaRegistPage::delKissStaPBClicked()
{
    if( QMessageBox::question( this,
                                tr("Delete this station."),
                                tr("Do you want to delete this station?"),
                                tr("Confirm"),
                                tr("Cancel"),
                                QString::null, 1, 1) )
    {
        return;
    }

    QObject *senderObj = sender();
    int i = senderObj->objectName().right(1).toInt();

    log->write(logDir, "Removed " + kstaLB[i]->text() + " KISS station.");

    kstaLB[i]->clear();
    knetLB[i]->clear();
    klatLB[i]->clear();
    klonLB[i]->clear();
    kelevLB[i]->clear();

    QMetaObject::invokeMethod(thisObj, "delKissStaMarker",
                              Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, i));
    addPB[i]->setText(tr("ADD"));
}

void KissStaRegistPage::setKISSStation(QString home)
{
    kissStaVector.clear();
    QFile file(home + "/params/KissStaList.txt");
    if(!file.exists())
    {
        log->write(home + "/logs/", "KISS Station List file doesn't exists. (" + home + "/params/KissStaList.txt)");
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
            else
            {
                _STATION sta; sta.sta = _line.section(" ",1,1);
                sta.net = _line.section(" ", 0, 0); sta.chan = "HGZ"; sta.loc = "--";
                sta.lat = _line.section(" ", 2, 2).toDouble();
                sta.lon = _line.section(" ", 3, 3).toDouble();
                sta.elev = _line.section(" ",4, 4).toDouble();
                kissStaVector.push_back(sta);
            }
        }
        file.close();
    }

    for(int i=0;i<kissStaVector.count();i++)
    {
        _STATION thisSta = kissStaVector.at(i);

        QMetaObject::invokeMethod(thisObj, "createAllKissStaMarker",
                                  Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, thisSta.lat), Q_ARG(QVariant, thisSta.lon),
                                  Q_ARG(QVariant, thisSta.sta));
    }
}

ServerSetPage::ServerSetPage(_CONFIGURE con, QWidget *parent)
{
    /*
    logDir = con.KGONSITE_HOME + "/logs";
    log = new WriteLog();

    setTitle(tr("Server Info. Registration"));
    setSubTitle(tr("Specify IP address and port about the Local KGOnsite server and KISS KGOnsite server, EEW server."));

    QFont font("Ubuntu", 9, QFont::Normal);

    QVBoxLayout *vl = new QVBoxLayout;
    QSpacerItem *spacerV = new QSpacerItem(20, 40, QSizePolicy::Preferred, QSizePolicy::Preferred);

    for(int i=0;i<3;i++)
    {
        vl->addItem(spacerV);

        nameLB[i] = new QLabel(tr("SERVER"));
        if(i==0) nameLB[i]->setText(tr("Local KGOnsite Server"));
        else if(i==1) nameLB[i]->setText(tr("KISS KGOnsite Server"));
        else if(i==2) nameLB[i]->setText(tr("EEW Server"));
        ipLB[i] = new QLabel(tr("IP : "));
        portLB[i] = new QLabel(tr("Port : "));
        sIPLE[i] = new QLineEdit; sIPLE[i]->setPlaceholderText(tr("IP address")); sIPLE[i]->setClearButtonEnabled(true); sIPLE[i]->setFont(font);
        sPortLE[i] = new QLineEdit; sPortLE[i]->setPlaceholderText(tr("Port")); sPortLE[i]->setClearButtonEnabled(true); sPortLE[i]->setFont(font);

        if(i == 0)
        {
            if(con.localServerIP != "")
                sIPLE[i]->setText(con.localServerIP);
            if(con.localServerPort != 0)
                sPortLE[i]->setText(QString::number(con.localServerPort));
        }
        else if(i == 1)
        {
            if(con.kissServerIP != "")
                sIPLE[i]->setText(con.kissServerIP);
            if(con.kissServerPort != 0)
                sPortLE[i]->setText(QString::number(con.kissServerPort));
        }
        else if(i == 2)
        {
            if(con.eewServerIP != "")
                sIPLE[i]->setText(con.eewServerIP);
            if(con.eewServerPort != 0)
                sPortLE[i]->setText(QString::number(con.eewServerPort));
        }

        registerField("serverIP" + QString::number(i), sIPLE[i]);
        registerField("serverPort" + QString::number(i), sPortLE[i]);

        vl->addWidget(nameLB[i]);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(ipLB[i], i, 0);
        layout->addWidget(portLB[i], i+1, 0);
        layout->addWidget(sIPLE[i], i, 1);
        layout->addWidget(sPortLE[i], i+1, 1);

        vl->addLayout(layout);
    }

    vl->addStretch(1);
    setLayout(vl);
    */
}

AlertSetPage::AlertSetPage(_CONFIGURE con, QWidget *parent)
{
    logDir = con.KGONSITE_HOME + "/logs";
    log = new WriteLog();

    //setTitle(tr("Configuration for Alert"));
    //setSubTitle(tr("Specify Standard of Alert."));
    setTitle(tr("Select the excutable files to be used for alert."));

    QFont font("Ubuntu", 9, QFont::Normal);

    QVBoxLayout *vl = new QVBoxLayout;
    QSpacerItem *spacerV = new QSpacerItem(20, 40, QSizePolicy::Preferred, QSizePolicy::Preferred);
    //QSpacerItem *spacerV2 = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);

    vl->addItem(spacerV);
    for(int i=0;i<4;i++)
    {
        enableCB[i] = new QCheckBox;
        nameLB[i] = new QLabel;
        if(i==0) nameLB[i]->setText(tr("Enable SOH Warning"));
        else if(i==1) nameLB[i]->setText(tr("Enable EEW Warning"));
        else if(i==2) nameLB[i]->setText(tr("Enable LEVEL 1 Warning"));
        else if(i==3) nameLB[i]->setText(tr("Enable LEVEL 2 Warning"));
        scriptFileLE[i] = new QLineEdit; scriptFileLE[i]->setPlaceholderText("script file"); scriptFileLE[i]->setMinimumWidth(300);
        scriptFileLE[i]->setClearButtonEnabled(true); scriptFileLE[i]->setFont(font);
        browPB[i] = new QPushButton; browPB[i]->setObjectName("BROWPB"+QString::number(i));
        browPB[i]->setText(tr("Browser"));
        previewPB[i] = new QPushButton; previewPB[i]->setObjectName("PREVIEW"+QString::number(i));
        previewPB[i]->setText(tr("Preview"));
        connect(browPB[i], SIGNAL(clicked(bool)), this, SLOT(browPBCLicked()));

        QHBoxLayout *lh = new QHBoxLayout;
        lh->addWidget(enableCB[i]);
        lh->addWidget(nameLB[i]);
        lh->addStretch(1);

        QHBoxLayout *lh2 = new QHBoxLayout;
        lh2->addSpacing(50);
        lh2->addWidget(scriptFileLE[i]);
        lh2->addWidget(browPB[i]);
        lh2->addWidget(previewPB[i]);
        lh2->addStretch(1);

        vl->addLayout(lh);
        vl->addLayout(lh2);

        registerField("alertEnabled" + QString::number(i), enableCB[i]);
        registerField("alertScript" + QString::number(i), scriptFileLE[i]);

        if(i == 2 || i == 3)
        {
            tagLabel[i-2] = new QLabel;
            tagLabel[i-2]->setText(tr("Minimum number of Onsite Info."));
            numStaSB[i-2] = new QSpinBox;
            numStaSB[i-2]->setRange(1, MAX_ONSITE_NUM);
            numStaSB[i-2]->setValue(1);
            if(i==3) numStaSB[i-2]->setValue(3);

            QHBoxLayout *lh3 = new QHBoxLayout;
            lh3->addSpacing(50);
            lh3->addWidget(tagLabel[i-2]);
            lh3->addWidget(numStaSB[i-2]);
            lh3->addStretch(1);

            vl->addLayout(lh3);

            registerField("alertNumSta" + QString::number(i-2), numStaSB[i-2]);
        }

        vl->addItem(spacerV);
    }
    vl->addStretch(1);
    setLayout(vl);

    setValue(con);
}

void AlertSetPage::setValue(_CONFIGURE con)
{
    if(con.sohWarningEnabled == 1)         enableCB[0]->setChecked(true);
    else         enableCB[0]->setChecked(false);
    if(con.sohWarningScript != "") scriptFileLE[0]->setText(con.sohWarningScript);

    if(con.eewWarningEnabled == 1) enableCB[1]->setChecked(true);
    else enableCB[1]->setChecked(false);
    if(con.eewWarningScript != "") scriptFileLE[1]->setText(con.eewWarningScript);

    if(con.level1Enabled == 1) enableCB[2]->setChecked(true);
    else enableCB[2]->setChecked(false);
    numStaSB[0]->setValue(con.level1NumSta);
    if(con.level1Script != "") scriptFileLE[2]->setText(con.level1Script);

    if(con.level2Enabled == 1) enableCB[3]->setChecked(true);
    else enableCB[3]->setChecked(false);
    numStaSB[1]->setValue(con.level2NumSta);
    if(con.level2Script != "") scriptFileLE[3]->setText(con.level2Script);
}

void AlertSetPage::browPBCLicked()
{
    QObject *senderObj = sender();
    int i = senderObj->objectName().right(1).toInt();

    QString scriptFile;
    scriptFile = QFileDialog::getOpenFileName(this, tr("Select a script file for alert"), "/home", "*.*");

    scriptFileLE[i]->setText(scriptFile);
}

ConfigurationWizard::ConfigurationWizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationWizard)
{
    ui->setupUi(this);

    log = new WriteLog();

    /* load a osm map */
    QQuickView *view = new QQuickView();
    QWidget *container = QWidget::createWindowContainer(view, this);
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    container->setFocusPolicy(Qt::TabFocus);
    view->setSource(QUrl(QStringLiteral("qrc:/Selectsta.qml")));
    ui->mapLO->addWidget(container);
    rootObj = view->rootObject();
    /*
    QMetaObject::invokeMethod(rootObjrootObj, "mapReset", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, configure.KGONSITE_HOME + "/params/maps"), Q_ARG(QVariant, "osm"));
                              */

    QMetaObject::invokeMethod(rootObj, "createLocalStaMarker", Q_RETURN_ARG(QVariant, returnedValue));
    QMetaObject::invokeMethod(rootObj, "createKissStaMarker", Q_RETURN_ARG(QVariant, returnedValue));
}

ConfigurationWizard::~ConfigurationWizard()
{
    delete ui;
}

void ConfigurationWizard::setup(_CONFIGURE configure)
{
    memcpy(&con, &configure, sizeof(_CONFIGURE));

    logDir = con.KGONSITE_HOME + "/logs";

    wizard = new QWizard();
    wizard->setWindowTitle(tr("Wizard"));
    wizard->setWizardStyle(QWizard::ModernStyle);

    wizard->addPage(new IntroPage);
    wizard->addPage(new LocalStaRegistPage(con, rootObj));
    wizard->addPage(new KissStaRegistPage(con, rootObj));
    //wizard->addPage(new ServerSetPage(con));
    wizard->addPage(new AlertSetPage(con));

    ui->vLO->addWidget(wizard);

    connect(wizard, SIGNAL(accepted()), this, SLOT(writeConfigureToFile()));
    connect(wizard, SIGNAL(rejected()), this, SLOT(accept()));
    connect(wizard, SIGNAL(destroyed(QObject*)), this, SLOT(accept()));
    //connect(wizard->CancelButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
}

void ConfigurationWizard::writeConfigureToFile()
{
    // check validation
    int nLocalSta = 0;
    int nKissSta = 0;

    for(int i=0;i<MAX_LOCALSTA_NUM;i++)
        if(wizard->field("localSta"+QString::number(i)).toString() != "")
            nLocalSta++;

    for(int i=0;i<MAX_KISSSTA_NUM;i++)
        if(wizard->field("kissSta"+QString::number(i)).toString() != "")
            nKissSta++;

    if(nLocalSta + nKissSta == 0)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("You should choose at least one station."));
        msgBox.exec();
        accept();
    }

    // write a configuration file
    QFile file(con.configFileName);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "#Local Station Info" << "\n";
        for(int i=0;i<MAX_LOCALSTA_NUM;i++)
        {
            if(wizard->field("localSta"+QString::number(i)).toString() != "")
            {
                stream << "LOCAL_STA=" +
                      wizard->field("localSta"+QString::number(i)).toString() + "," +
                      wizard->field("localChan"+QString::number(i)).toString() + "," +
                      wizard->field("localNet"+QString::number(i)).toString() + "," +
                      wizard->field("localLoc"+QString::number(i)).toString() + "," +
                      wizard->field("localLat"+QString::number(i)).toString() + "," +
                      wizard->field("localLon"+QString::number(i)).toString() + "," +
                      wizard->field("localElev"+QString::number(i)).toString() +
                      "\n";
            }
        }
        stream << "\n";
        stream << "#KISS Station Info" << "\n";
        for(int i=0;i<MAX_KISSSTA_NUM;i++)
        {
            if(wizard->field("kissSta"+QString::number(i)).toString() != "")
            {
                stream << "KISS_STA=" +
                      wizard->field("kissSta"+QString::number(i)).toString() + "," +
                      "HGZ," +
                      wizard->field("kissNet"+QString::number(i)).toString() + "," +
                      "--," +
                      wizard->field("kissLat"+QString::number(i)).toString() + "," +
                      wizard->field("kissLon"+QString::number(i)).toString() + "," +
                      wizard->field("kissElev"+QString::number(i)).toString() +
                      "\n";
            }
        }
        stream << "\n";
        stream << "#Server Info" << "\n";
        //stream << "EEW_AMQ_INFO=127.0.0.1:61616:elarms:h3ePs7bn:elarms.data.client";
        stream << "EEW_AMQ_INFO=" << con.eew_ip << ":" << con.eew_port << ":" << con.eew_user << ":" << con.eew_passwd << ":" << con.eew_topic << "\n";
        /*
        stream << "LOCAL_SERVER=" << wizard->field("serverIP0").toString() + ":" +
                  wizard->field("serverPort0").toString() + "\n";
        stream << "KISS_SERVER=" << wizard->field("serverIP1").toString() + ":" +
                  wizard->field("serverPort1").toString() + "\n";
        stream << "EEW_SERVER=" << wizard->field("serverIP2").toString() + ":" +
                  wizard->field("serverPort2").toString() + "\n";
                  */

        stream << "\n";
        stream << "#Alert Info" << "\n";

        if(wizard->field("alertEnabled0").toBool())
            stream << "SOH_WARNING=1," << wizard->field("alertScript0").toString() + "\n";
        else
            stream << "SOH_WARNING=0," << wizard->field("alertScript0").toString() + "\n";

        if(wizard->field("alertEnabled1").toBool())
            stream << "EEW_WARNING=1," << wizard->field("alertScript1").toString() + "\n";
        else
            stream << "EEW_WARNING=0," << wizard->field("alertScript1").toString() + "\n";

        if(wizard->field("alertEnabled2").toBool())
            stream << "LEVEL1=1," << wizard->field("alertNumSta0").toString() << "," << wizard->field("alertScript2").toString() + "\n";
        else
            stream << "LEVEL1=0," << wizard->field("alertNumSta0").toString() << "," << wizard->field("alertScript2").toString() + "\n";

        if(wizard->field("alertEnabled3").toBool())
            stream << "LEVEL2=1," << wizard->field("alertNumSta1").toString() << "," << wizard->field("alertScript3").toString() + "\n";
        else
            stream << "LEVEL2=0," << wizard->field("alertNumSta1").toString() << "," << wizard->field("alertScript3").toString() + "\n";


        file.close();
    }

    log->write(logDir, "Succeed saving parameters.");

    emit(resetMainWindow());

    log->write(logDir, "Configuration wizard closed.");
    accept();
}
