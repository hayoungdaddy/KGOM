#include "configuration.h"
#include "ui_configuration.h"

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");

    log = new WriteLog();

    setWindowFlags( Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowSystemMenuHint);

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
    ui->kigamAmqIpLE->setText(con.kigam_amq_ip);
    ui->kigamAmqPortLE->setText(con.kigam_amq_port);
    ui->kigamAmqIdLE->setText(con.kigam_amq_user);
    ui->kigamAmqPwLE->setText(con.kigam_amq_passwd);
    ui->kigamAmqEewTopicLE->setText(con.kigam_eew_topic);
    ui->kigamAmqPickTopicLE->setText(con.kigam_onsite_topic);
    ui->kigamAmqSohTopicLE->setText(con.kigam_soh_topic);
    ui->localAmqIpLE->setText(con.local_amq_ip);
    ui->localAmqPortLE->setText(con.local_amq_port);
    ui->localAmqIdLE->setText(con.local_amq_user);
    ui->localAmqPwLE->setText(con.local_amq_passwd);
    ui->localAmqPickTopicLE->setText(con.local_onsite_topic);
    ui->localAmqPGATopicLE->setText(con.local_pga_topic);
    ui->localAmqSohTopicLE->setText(con.local_soh_topic);
    ui->pVelLE->setText(QString::number(con.p_vel, 'f', 2));
    ui->sVelLE->setText(QString::number(con.s_vel, 'f', 2));
    //if(con.soh_alert_use==0) ui->sohAlertUseCB->setChecked(false); else ui->sohAlertUseCB->setChecked(true);
    //ui->sohAlertSecLE->setText(QString::number(con.soh_alert_sec));
    if(con.level1_alert_use==0) ui->level1AlertUseCB->setChecked(false); else ui->level1AlertUseCB->setChecked(true);
    ui->level1AlertMinMagLE->setText(QString::number(con.level1_alert_min_mag, 'f', 1));
    ui->level1AlertMaxMagLE->setText(QString::number(con.level1_alert_max_mag, 'f', 1));
    ui->level1AlertDistLE->setText(QString::number(con.level1_alert_dist));
    if(con.level2_alert_use==0) ui->level2AlertUseCB->setChecked(false); else ui->level2AlertUseCB->setChecked(true);
    ui->level2AlertMinMagLE->setText(QString::number(con.level2_alert_min_mag, 'f', 1));
    ui->level2AlertMaxMagLE->setText(QString::number(con.level2_alert_max_mag, 'f', 1));
    ui->level2AlertDistLE->setText(QString::number(con.level2_alert_dist));
}

void Configuration::writeConfigureToFile()
{
    // to do : check validation

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
        stream << "LOCAL_AMQ_INFO=" << ui->localAmqIpLE->text() << ":" << ui->localAmqPortLE->text()
               << ":" << ui->localAmqIdLE->text() << ":" << ui->localAmqPwLE->text() << "\n";
        stream << "LOCAL_ONSITE_TOPIC=" << ui->localAmqPickTopicLE->text() << "\n";
        stream << "LOCAL_PGA_TOPIC=" << ui->localAmqPGATopicLE->text() << "\n";
        stream << "LOCAL_SOH_TOPIC=" << ui->localAmqSohTopicLE->text() << "\n";

        stream << "\n";
        stream << "#KIGAM AMQ Server Info" << "\n";
        stream << "KIGAM_AMQ_INFO=" << ui->kigamAmqIpLE->text() << ":" << ui->kigamAmqPortLE->text()
               << ":" << ui->kigamAmqIdLE->text() << ":" << ui->kigamAmqPwLE->text() << "\n";
        stream << "KIGAM_EEW_TOPIC=" << ui->kigamAmqEewTopicLE->text() << "\n";
        stream << "KIGAM_ONSITE_TOPIC=" << ui->kigamAmqPickTopicLE->text() << "\n";
        stream << "KIGAM_SOH_TOPIC=" << ui->kigamAmqSohTopicLE->text() << "\n";

        stream << "\n";
        stream << "#Velocity Info" << "\n";
        stream << "P_VEL=" << ui->pVelLE->text() << "\n";
        stream << "S_VEL=" << ui->sVelLE->text() << "\n";

        stream << "\n";
        stream << "#Alert Level" << "\n";
        stream << "ALARM_DEVICE=" << con.alarm_device_ip << ":" << QString::number(con.alarm_device_port) << "\n";
        //stream << "SOH_ALERT=";
        //if(ui->sohAlertUseCB->isChecked()) stream << "1";
        //else stream << "0";
        //stream << ":" << ui->sohAlertSecLE->text() << "\n";
        stream << "LEVEL1=";
        if(ui->level1AlertUseCB->isChecked()) stream << "1";
        else stream << "0";
        stream << ":" << ui->level1AlertMinMagLE->text()
               << ":" << ui->level1AlertMaxMagLE->text() << ":" << ui->level1AlertDistLE->text() << "\n";
        stream << "LEVEL2=";
        if(ui->level2AlertUseCB->isChecked()) stream << "1";
        else stream << "0";
        stream << ":" << ui->level2AlertMinMagLE->text()
               << ":" << ui->level2AlertMaxMagLE->text() << ":" << ui->level2AlertDistLE->text() << "\n";
        stream << "\n";

        file.close();
    }

    log->write(logDir, "Succeed saving parameters.");

    emit(resetMainWindow());

    log->write(logDir, "Configuration window closed.");
    accept();
}
