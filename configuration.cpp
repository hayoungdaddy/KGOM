#include "configuration.h"
#include "ui_configuration.h"

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");

    log = new WriteLog();

    setWindowFlags(Qt::Drawer);

    connect(ui->savePB, SIGNAL(clicked(bool)), this, SLOT(writeConfigureToFile()));
}

Configuration::~Configuration()
{
    delete ui;
}

void Configuration::setup(_CONFIGURE configure)
{
    memcpy(&con, &configure, sizeof(_CONFIGURE));

    logDir = con.KGOM_HOME + "/logs";

    ui->myLatLE->setText(QString::number(con.myposition_lat, 'f', 4));
    ui->myLonLE->setText(QString::number(con.myposition_lon, 'f', 4));
    ui->pVelLE->setText(QString::number(con.p_vel, 'f', 2));
    ui->sVelLE->setText(QString::number(con.s_vel, 'f', 2));

    if(con.mag_alert_use==0) ui->magAlertUseCB->setChecked(false); else ui->magAlertUseCB->setChecked(true);

    ui->level1AlertMinMagLE->setText(QString::number(con.mag_level1_alert_min_mag, 'f', 1));
    ui->level1AlertMaxMagLE->setText(QString::number(con.mag_level1_alert_max_mag, 'f', 1));
    ui->level1AlertDistLE->setText(QString::number(con.mag_level1_alert_dist));

    ui->level2AlertMinMagLE->setText(QString::number(con.mag_level2_alert_min_mag, 'f', 1));
    ui->level2AlertMaxMagLE->setText(QString::number(con.mag_level2_alert_max_mag, 'f', 1));
    ui->level2AlertDistLE->setText(QString::number(con.mag_level2_alert_dist));

    if(con.inten_alert_use==0) ui->intenAlertUseCB->setChecked(false); else ui->intenAlertUseCB->setChecked(true);

    ui->level1AlertINTENLE->setText(QString::number(con.inten_level1_threshold));
    ui->level2AlertINTENLE->setText(QString::number(con.inten_level2_threshold));

    ui->numStaPGALE->setText(QString::number(con.pga_num_sta_threshold));
    ui->pgaTimeWindowLE->setText(QString::number(con.pga_time_window));
}

void Configuration::writeConfigureToFile()
{
/*
#My Position Info
MYPOSITION_LAT=35.8714
MYPOSITION_LON=128.6019

#Local Station Info

#KISS Station Info
KISS_STA=GKP1:HGZ:KG:--:35.889300:128.606050:56.0
KISS_STA=DAU:HGZ:KS:--:35.885600:128.618800:9.0
KISS_STA=DAG2:HGZ:KS:--:35.768500:128.897000:294.0
KISS_STA=CIGB:HGZ:KS:--:36.039900:128.381300:74.000000
KISS_STA=YOCB:HGZ:KS:--:35.977100:128.951100:143.000000
KISS_STA=CHR:HGZ:KS:--:35.544000:128.491700:115.000000

#LOCAL AMQ Server Info
LOCAL_ONSITE_AMQ=::::
LOCAL_SOH_AMQ=::::
LOCAL_PGA_AMQ=::::

#KIGAM AMQ Server Info
KISS_EEW_AMQ=210.98.8.82:61616:elarms:h3ePs7bn:elarms.data.client
KISS_ONSITE_AMQ=172.31.100.137:61616:::KSEIS.KGOnsite_Pick
KISS_SOH_AMQ=172.31.100.137:61616:::KSEIS.KGOnSite_SOH
KISS_PGA_AMQ=172.310.100.137:61616:::KGKIIS.KISS.QSCDBLOCK

#Velocity Info
P_VEL=5.50
S_VEL=3.50

#Alert Level
ALARM_DEVICE=:0
USE_ALARM_SOUND=1
USE_MAG_ALERT=1
MAG_LEVEL1=2.0:4.0:100
MAG_LEVEL2=4.0:999.0:9999
USE_INTEN_ALERT=1
INTEN_LEVEL1=3
INTEN_LEVEL2=5
NUM_STA_FOR_PGA=3
PGA_TIME_WINDOW=10

*/

    // write a configuration file
    QFile file(con.configFileName);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "#My Position Info" << "\n";
        stream << "MYPOSITION_LAT=" << ui->myLatLE->text() << "\n";
        stream << "MYPOSITION_LON=" << ui->myLonLE->text() << "\n";

        stream << "\n";
        stream << "#Local Station Info" << "\n";
        for(int i=0;i<con.localStaVT.count();i++)
        {
            stream << "LOCAL_STA=" << con.localStaVT.at(i).sta << ":" << con.localStaVT.at(i).chan << ":" << con.localStaVT.at(i).net
                   << ":" << con.localStaVT.at(i).loc << ":" << QString::number(con.localStaVT.at(i).lat, 'f', 6) << ":" << QString::number(con.localStaVT.at(i).lon, 'f', 6)
                   << ":" << QString::number(con.localStaVT.at(i).elev, 'f', 1) << "\n";
        }

        stream << "\n";
        stream << "#KISS Station Info" << "\n";
        for(int i=0;i<con.kissStaVT.count();i++)
        {
            stream << "KISS_STA=" << con.kissStaVT.at(i).sta << ":" << con.kissStaVT.at(i).chan << ":" << con.kissStaVT.at(i).net
                   << ":" << con.kissStaVT.at(i).loc << ":" << QString::number(con.kissStaVT.at(i).lat, 'f', 6) << ":" << QString::number(con.kissStaVT.at(i).lon, 'f', 6)
                   << ":" << QString::number(con.kissStaVT.at(i).elev, 'f', 1) << "\n";
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
        stream << "P_VEL=" << ui->pVelLE->text() << "\n";
        stream << "S_VEL=" << ui->sVelLE->text() << "\n";

        stream << "\n";
        stream << "#Alert Level" << "\n";
        stream << "ALARM_DEVICE=" << con.alarm_device_ip << ":" << QString::number(con.alarm_device_port) << "\n";
        stream << "USE_ALARM_SOUND=" << QString::number(con.use_alarm_sound) << "\n";
        stream << "USE_MAG_ALERT=";
        if(ui->magAlertUseCB->isChecked()) stream << "1" << "\n";
        else stream << "0" << "\n";

        stream << "MAG_LEVEL1=";
        stream << ui->level1AlertMinMagLE->text()
               << ":" << ui->level1AlertMaxMagLE->text() << ":" << ui->level1AlertDistLE->text() << "\n";
        stream << "MAG_LEVEL2=";
        stream << ui->level2AlertMinMagLE->text()
               << ":" << ui->level2AlertMaxMagLE->text() << ":" << ui->level2AlertDistLE->text() << "\n";

        stream << "USE_INTEN_ALERT=";
        if(ui->intenAlertUseCB->isChecked()) stream << "1" << "\n";
        else stream << "0" << "\n";

        stream << "INTEN_LEVEL1=";
        stream << ui->level1AlertINTENLE->text() << "\n";
        stream << "INTEN_LEVEL2=";
        stream << ui->level2AlertINTENLE->text() << "\n";

        stream << "NUM_STA_FOR_PGA=" << ui->numStaPGALE->text() << "\n";
        stream << "PGA_TIME_WINDOW=" << ui->pgaTimeWindowLE->text() << "\n";

        stream << "\n";

        file.close();
    }

    log->write(logDir, "Succeed saving parameters.");

    emit(resetMainWindow());

    log->write(logDir, "Configuration window closed.");
    accept();
}
