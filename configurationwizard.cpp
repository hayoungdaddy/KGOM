#include "configurationwizard.h"
#include "ui_configurationwizard.h"

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    codec = QTextCodec::codecForName("utf-8");

    //setTitle(tr("Introduction"));
    setTitle(codec->toUnicode("소개"));

    //QLabel *label = new QLabel("This wizard will help you configure stations and set for KGOM."));
    QLabel *label = new QLabel(codec->toUnicode("이 마법사를 이용하여 지진관측소를 설정할 수 있습니다."));

    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

LocalStaRegistPage::LocalStaRegistPage(_CONFIGURE con, QObject *rootObj, QWidget *parent)
{
    codec = QTextCodec::codecForName("utf-8");

    thisObj = rootObj;
    logDir = con.KGOM_HOME + "/logs";
    log = new WriteLog;

    //setTitle("Do you have your own seismic stations?");
    setTitle(codec->toUnicode("로컬 지진 관측소를 설정합니다."));

    QLabel *subLabel = new QLabel;
    subLabel->setTextFormat(Qt::RichText);
    //subLabel->setText("These stations will be marked on the MapView using \<img src=\":Icon/localStaMarker.png\" width=20 height=20 \> symbol.");
    subLabel->setText(codec->toUnicode("로컬 관측소가 추가되면 좌측 지도에서 ") + "\<img src=\":Icon/localStaMarker.png\" width=20 height=20 \>" + codec->toUnicode("마크로 확인할 수 있습니다."));

    QFont font("Ubuntu", 9, QFont::Normal);

    QVBoxLayout *vl = new QVBoxLayout;
    vl->addWidget(subLabel);

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Preferred, QSizePolicy::Preferred);
    vl->addItem(spacer);

    for(int i=0;i<MAX_LOCALSTA_NUM;i++)
    {
        vl->addItem(spacer);

        //nameLB[i] = new QLabel("Local Station No. " + QString::number(i + 1));
        nameLB[i] = new QLabel(codec->toUnicode("관측소 번호 : ") + QString::number(i + 1));
        lstaLB[i] = new QLineEdit; lstaLB[i]->setPlaceholderText(tr("Station")); lstaLB[i]->setClearButtonEnabled(true); lstaLB[i]->setFont(font);
        lchanLB[i] = new QLineEdit; lchanLB[i]->setPlaceholderText(tr("Channel")); lchanLB[i]->setClearButtonEnabled(false); lchanLB[i]->setFont(font);
        lchanLB[i]->setText("HGZ"); lchanLB[i]->setEnabled(false);
        lnetLB[i] = new QLineEdit; lnetLB[i]->setPlaceholderText(tr("Network")); lnetLB[i]->setClearButtonEnabled(true); lnetLB[i]->setFont(font);
        llocLB[i] = new QLineEdit; llocLB[i]->setPlaceholderText(tr("Location")); llocLB[i]->setClearButtonEnabled(true); llocLB[i]->setFont(font);
        llatLB[i] = new QLineEdit; llatLB[i]->setPlaceholderText(tr("Latitude")); llatLB[i]->setClearButtonEnabled(true); llatLB[i]->setFont(font);
        llonLB[i] = new QLineEdit; llonLB[i]->setPlaceholderText(tr("Longitude")); llonLB[i]->setClearButtonEnabled(true); llonLB[i]->setFont(font);
        lelevLB[i] = new QLineEdit; lelevLB[i]->setPlaceholderText(tr("Elevation (m)")); lelevLB[i]->setClearButtonEnabled(true); lelevLB[i]->setFont(font);
        ldepthLB[i] = new QLineEdit; ldepthLB[i]->setPlaceholderText(tr("Depth (m)")); ldepthLB[i]->setClearButtonEnabled(true); ldepthLB[i]->setFont(font);
        //addPB[i] = new QPushButton("ADD");
        addPB[i] = new QPushButton(codec->toUnicode("추가"));
        addPB[i]->setFont(font); addPB[i]->setObjectName("addPB" + QString::number(i));
        //delPB[i] = new QPushButton("DELETE");
        delPB[i] = new QPushButton(codec->toUnicode("삭제"));
        delPB[i]->setFont(font); delPB[i]->setObjectName("delPB" + QString::number(i));
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

    for(int i=0;i<con.localStaVT.count();i++)
    {
        lstaLB[i]->setText(con.localStaVT.at(i).sta);
        lchanLB[i]->setText(con.localStaVT.at(i).chan);
        lnetLB[i]->setText(con.localStaVT.at(i).net);
        llocLB[i]->setText(con.localStaVT.at(i).loc);
        llatLB[i]->setText(QString::number(con.localStaVT.at(i).lat,'f', 6));
        llonLB[i]->setText(QString::number(con.localStaVT.at(i).lon,'f', 6));
        lelevLB[i]->setText(QString::number(con.localStaVT.at(i).elev,'f', 1));
        ldepthLB[i]->setText(QString::number(con.localStaVT.at(i).depth,'f',1));
        //addPB[i]->setText("EDIT");
        addPB[i]->setText(codec->toUnicode("수정"));

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
    //addPB[i]->setText("EDIT");
    addPB[i]->setText(codec->toUnicode("수정"));
    log->write(logDir, "Added(Edited) " + lstaLB[i]->text() + " Local station.");
}

void LocalStaRegistPage::delLocalStaPBClicked()
{  
    /*
    if( QMessageBox::question( this,
                                "Delete this station.",
                                "Do you want to delete this station?",
                                "Confirm",
                                "Cancel",
                                QString::null, 1, 1) )
                                */
    if( QMessageBox::question( this,
                                codec->toUnicode("이 관측소를 삭제합니다."),
                                codec->toUnicode("정말로 삭제하시겠습니까?"),
                                codec->toUnicode("확인"),
                                codec->toUnicode("취소"),
                                QString::null, 1, 1) )
    {
        return;
    }

    QObject *senderObj = sender();
    int i = senderObj->objectName().right(1).toInt();

    QMetaObject::invokeMethod(thisObj, "delLocalStaMarker",
                              Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, i));
    //addPB[i]->setText("ADD");
    addPB[i] = new QPushButton(codec->toUnicode("추가"));

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
    codec = QTextCodec::codecForName("utf-8");

    thisObj = rootObj;
    logDir = con.KGOM_HOME + "/logs";
    log = new WriteLog();

    //setTitle("Do you have public seismic stations?");
    setTitle(codec->toUnicode("공용 지진 관측소를 설정합니다."));
    QLabel *subLabel = new QLabel;
    subLabel->setTextFormat(Qt::RichText);
    //subLabel->setText("These stations will be marked on the MapView using \<img src=\":Icon/kissStaMarker.png\" width=20 height=20 \> symbol.");
    subLabel->setText(codec->toUnicode("공용 관측소가 추가되면 좌측 지도에서 ")
                      + "\<img src=\":Icon/kissStaMarker.png\" width=20 height=20\>"
                      + codec->toUnicode("마크로 확인할 수 있습니다."));

    QFont font("Ubuntu", 9, QFont::Normal);

    QVBoxLayout *vl = new QVBoxLayout;
    QSpacerItem *spacerV = new QSpacerItem(20, 40, QSizePolicy::Preferred, QSizePolicy::Preferred);

    vl->addWidget(subLabel);

    QHBoxLayout *hl = new QHBoxLayout;
    QLabel *temp = new QLabel;
    //temp->setText("Show or hide the public stations using buttons below.");
    temp->setText(codec->toUnicode("어떤 관측소가 있는지 보거나 숨기려면 아래 버튼을 이용하세요."));

    vl->addWidget(temp);
    //showPB = new QPushButton("Show all public stations");
    //hidePB = new QPushButton("Hide all public stations");
    showPB = new QPushButton(codec->toUnicode("모든 공용 관측소 보기"));
    hidePB = new QPushButton(codec->toUnicode("모든 공용 관측소 숨기기"));
    connect(showPB, SIGNAL(clicked(bool)), this, SLOT(showKissStaList()));
    connect(hidePB, SIGNAL(clicked(bool)), this, SLOT(hideKissStaList()));
    hl->addWidget(showPB); hl->addWidget(hidePB);
    vl->addLayout(hl);

    for(int i=0;i<MAX_KISSSTA_NUM;i++)
    {
        vl->addItem(spacerV);

        //nameLB[i] = new QLabel("Public Station No. " + QString::number(i + 1));
        nameLB[i] = new QLabel(codec->toUnicode("관측소 번호 : ") + QString::number(i + 1));
        kstaLB[i] = new QLineEdit; kstaLB[i]->setPlaceholderText(tr("Station")); kstaLB[i]->setClearButtonEnabled(true); kstaLB[i]->setFont(font);
        knetLB[i] = new QLineEdit; klatLB[i] = new QLineEdit; klonLB[i] = new QLineEdit; kelevLB[i] = new QLineEdit;
        knetLB[i]->hide(); klatLB[i]->hide(); klonLB[i]->hide(); kelevLB[i]->hide();
        //addPB[i] = new QPushButton("ADD");
        addPB[i] = new QPushButton(codec->toUnicode("추가"));
        addPB[i]->setFont(font); addPB[i]->setObjectName("addPB" + QString::number(i));
        //delPB[i] = new QPushButton("DELETE");
        delPB[i] = new QPushButton(codec->toUnicode("삭제"));
        delPB[i]->setFont(font); delPB[i]->setObjectName("delPB" + QString::number(i));
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

    setKISSStation(con.KGOM_HOME);

    for(int i=0;i<con.kissStaVT.count();i++)
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
        //addPB[i]->setText("EDIT");
        addPB[i]->setText(codec->toUnicode("수정"));

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
            //addPB[i]->setText(tr("EDIT"));
            addPB[i]->setText(codec->toUnicode("수정"));
            log->write(logDir, "Added(Edited) " + kstaLB[i]->text() + " KISS station.");
            break;
        }
    }
}

void KissStaRegistPage::delKissStaPBClicked()
{
    /*
    if( QMessageBox::question( this,
                                tr("Delete this station."),
                                tr("Do you want to delete this station?"),
                                tr("Confirm"),
                                tr("Cancel"),
                                QString::null, 1, 1) )
                                */
    if( QMessageBox::question( this,
                                codec->toUnicode("이 관측소를 삭제합니다."),
                                codec->toUnicode("정말로 삭제하시겠습니까?"),
                                codec->toUnicode("확인"),
                                codec->toUnicode("취소"),
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
    //addPB[i]->setText("ADD");
    addPB[i]->setText(codec->toUnicode("추가"));
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

ConfigurationWizard::ConfigurationWizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationWizard)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");

    log = new WriteLog();

    setWindowFlags(Qt::Drawer);

    /* load a osm map */
    QQuickView *view = new QQuickView();
    QWidget *container = QWidget::createWindowContainer(view, this);
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::TabFocus);
    view->setSource(QUrl(QStringLiteral("qrc:/Selectsta.qml")));
    ui->mapLO->addWidget(container);
    rootObj = view->rootObject();

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

    logDir = con.KGOM_HOME + "/logs";

    wizard = new QWizard();
    //wizard->setWindowTitle("Wizard");
    wizard->setWindowTitle(codec->toUnicode("설정 마법사"));
    wizard->setWizardStyle(QWizard::ModernStyle);
    wizard->setButtonText(QWizard::NextButton, codec->toUnicode("다음 >"));
    wizard->setButtonText(QWizard::BackButton, codec->toUnicode("< 이전"));
    wizard->setButtonText(QWizard::CancelButton, codec->toUnicode("취소"));
    wizard->setButtonText(QWizard::FinishButton, codec->toUnicode("완료"));

    wizard->addPage(new IntroPage);
    wizard->addPage(new LocalStaRegistPage(con, rootObj));
    wizard->addPage(new KissStaRegistPage(con, rootObj));

    ui->vLO->addWidget(wizard);

    connect(wizard, SIGNAL(accepted()), this, SLOT(writeConfigureToFile()));
    connect(wizard, SIGNAL(rejected()), this, SLOT(accept()));
    connect(wizard, SIGNAL(destroyed(QObject*)), this, SLOT(accept()));
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
        //msgBox.setText("You should choose at least one station.");
        msgBox.setText(codec->toUnicode("적어도 한개의 관측소는 선택해야 합니다."));
        msgBox.exec();
        accept();
    }

    // write a configuration file
    QFile file(con.configFileName);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "#My Position Info" << "\n";
        stream << "MYPOSITION_LAT=" << QString::number(con.myposition_lat, 'f', 4) << "\n";
        stream << "MYPOSITION_LON=" << QString::number(con.myposition_lon, 'f', 4) << "\n";

        stream << "\n";
        stream << "#Local Station Info" << "\n";
        for(int i=0;i<MAX_LOCALSTA_NUM;i++)
        {
            if(wizard->field("localSta"+QString::number(i)).toString() != "")
            {
                stream << "LOCAL_STA=" +
                      wizard->field("localSta"+QString::number(i)).toString() + ":" +
                      wizard->field("localChan"+QString::number(i)).toString() + ":" +
                      wizard->field("localNet"+QString::number(i)).toString() + ":" +
                      wizard->field("localLoc"+QString::number(i)).toString() + ":" +
                      wizard->field("localLat"+QString::number(i)).toString() + ":" +
                      wizard->field("localLon"+QString::number(i)).toString() + ":" +
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
                      wizard->field("kissSta"+QString::number(i)).toString() + ":" +
                      "HGZ:" +
                      wizard->field("kissNet"+QString::number(i)).toString() + ":" +
                      "--:" +
                      wizard->field("kissLat"+QString::number(i)).toString() + ":" +
                      wizard->field("kissLon"+QString::number(i)).toString() + ":" +
                      wizard->field("kissElev"+QString::number(i)).toString() +
                      "\n";
            }
        }

        stream << "\n";
        stream << "#LOCAL AMQ Server Info" << "\n";
        stream << "LOCAL_ONSITE_AMQ=" << con.local_onsite_amq.ip << ":" << con.local_onsite_amq.port << ":" << con.local_onsite_amq.user << ":" << con.local_onsite_amq.passwd << ":" << con.local_onsite_amq.topic << "\n";
        stream << "LOCAL_SOH_AMQ=" << con.local_soh_amq.ip << ":" << con.local_soh_amq.port << ":" << con.local_soh_amq.user << ":" << con.local_soh_amq.passwd << ":" << con.local_soh_amq.topic << "\n";
        stream << "LOCAL_PGA_AMQ=" << con.local_pga_amq.ip << ":" << con.local_pga_amq.port << ":" << con.local_pga_amq.user << ":" << con.local_pga_amq.passwd << ":" << con.local_pga_amq.topic << "\n";

        stream << "\n";
        stream << "#KIGAM AMQ Server Info" << "\n";
        stream << "KISS_EEW_AMQ=" << con.kiss_eew_amq.ip << ":" << con.kiss_eew_amq.port << ":" << con.kiss_eew_amq.user << ":" << con.kiss_eew_amq.passwd << ":" << con.kiss_eew_amq.topic << "\n";
        stream << "KISS_ONSITE_AMQ=" << con.kiss_onsite_amq.ip << ":" << con.kiss_onsite_amq.port << ":" << con.kiss_onsite_amq.user << ":" << con.kiss_onsite_amq.passwd << ":" << con.kiss_onsite_amq.topic << "\n";
        stream << "KISS_SOH_AMQ=" << con.kiss_soh_amq.ip << ":" << con.kiss_soh_amq.port << ":" << con.kiss_soh_amq.user << ":" << con.kiss_soh_amq.passwd << ":" << con.kiss_soh_amq.topic << "\n";
        stream << "KISS_PGA_AMQ=" << con.kiss_pga_amq.ip << ":" << con.kiss_pga_amq.port << ":" << con.kiss_pga_amq.user << ":" << con.kiss_pga_amq.passwd << ":" << con.kiss_pga_amq.topic << "\n";

        stream << "\n";
        stream << "#Velocity Info" << "\n";
        stream << "P_VEL=" << QString::number(con.p_vel, 'f', 2) << "\n";
        stream << "S_VEL=" << QString::number(con.s_vel, 'f', 2) << "\n";

        stream << "\n";
        stream << "#Alert Level" << "\n";
        stream << "ALARM_DEVICE=" << con.alarm_device_ip << ":" << QString::number(con.alarm_device_port) << "\n";

        stream << "USE_MAG_ALERT=" << QString::number(con.mag_alert_use) << "\n";

        stream << "MAG_LEVEL1=" << QString::number(con.mag_level1_alert_min_mag, 'f', 1)
               << ":" << QString::number(con.mag_level1_alert_max_mag, 'f', 1) << ":" << QString::number(con.mag_level1_alert_dist) << "\n";
        stream << "MAG_LEVEL2=" << QString::number(con.mag_level2_alert_min_mag, 'f', 1)
               << ":" << QString::number(con.mag_level2_alert_max_mag, 'f', 1) << ":" << QString::number(con.mag_level2_alert_dist) << "\n";

        stream << "USE_INTEN_ALERT=" << QString::number(con.inten_alert_use) << "\n";

        stream << "INTEN_LEVEL1=" << QString::number(con.inten_level1_threshold) << "\n";
        stream << "INTEN_LEVEL2=" << QString::number(con.inten_level2_threshold) << "\n";

        stream << "NUM_STA_FOR_PGA=" << QString::number(con.pga_num_sta_threshold) << "\n";
        stream << "PGA_TIME_WINDOW=" << QString::number(con.pga_time_window) << "\n";

        stream << "\n";

        file.close();
    }

    log->write(logDir, "Succeed saving parameters.");

    emit(resetMainWindow());

    log->write(logDir, "Configuration wizard window closed.");
    accept();
}
