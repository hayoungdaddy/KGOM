#ifndef CONFIGURATIONWIZARD_H
#define CONFIGURATIONWIZARD_H

#include "common.h"
#include "writelog.h"

#include <QDialog>

#include <QQuickView>
#include <QtQuick>

#include <QWizard>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class ConfigurationWizard;
}

class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    IntroPage(QWidget *parent = 0);
private:
    QLabel *label;
};

class LocalStaRegistPage : public QWizardPage
{
    Q_OBJECT
public:
    LocalStaRegistPage(_CONFIGURE con, QObject *rootObj, QWidget *parent = 0);
private:
    QString logDir;
    WriteLog *log;
    QVariant returnedValue;
    QObject *thisObj;
    QLabel *nameLB[MAX_LOCALSTA_NUM];
    QLineEdit *lstaLB[MAX_LOCALSTA_NUM];
    QLineEdit *lchanLB[MAX_LOCALSTA_NUM];
    QLineEdit *lnetLB[MAX_LOCALSTA_NUM];
    QLineEdit *llocLB[MAX_LOCALSTA_NUM];
    QLineEdit *llatLB[MAX_LOCALSTA_NUM];
    QLineEdit *llonLB[MAX_LOCALSTA_NUM];
    QLineEdit *lelevLB[MAX_LOCALSTA_NUM];
    QLineEdit *ldepthLB[MAX_LOCALSTA_NUM];
    QPushButton *addPB[MAX_LOCALSTA_NUM];
    QPushButton *delPB[MAX_LOCALSTA_NUM];
private slots:
    void addLocalStaPBClicked();
    void delLocalStaPBClicked();
};

class KissStaRegistPage : public QWizardPage
{
    Q_OBJECT
public:
    KissStaRegistPage(_CONFIGURE con, QObject *rootObj, QWidget *parent = 0); 
private:
    QString logDir;
    WriteLog *log;
    QVariant returnedValue;
    QObject *thisObj;
    QLabel *nameLB[MAX_KISSSTA_NUM];
    QLineEdit *kstaLB[MAX_KISSSTA_NUM];
    QLineEdit *knetLB[MAX_KISSSTA_NUM];
    QLineEdit *klatLB[MAX_KISSSTA_NUM];
    QLineEdit *klonLB[MAX_KISSSTA_NUM];
    QLineEdit *kelevLB[MAX_KISSSTA_NUM];
    QPushButton *addPB[MAX_KISSSTA_NUM];
    QPushButton *delPB[MAX_KISSSTA_NUM];
    QPushButton *showPB;
    QPushButton *hidePB;
    double lat[MAX_KISSSTA_NUM], lon[MAX_KISSSTA_NUM], elev[MAX_KISSSTA_NUM];
    QString net[MAX_KISSSTA_NUM];
    QVector<_STATION> kissStaVector;
    void setKISSStation(QString);
private slots:
    void addKissStaPBClicked();
    void delKissStaPBClicked();
    void showKissStaList();
    void hideKissStaList();
};

class ServerSetPage : public QWizardPage
{
    Q_OBJECT
public:
    ServerSetPage(_CONFIGURE con, QWidget *parent = 0);
private:
    QString logDir;
    WriteLog *log;
    QLabel *nameLB[3];
    QLabel *ipLB[3];
    QLabel *portLB[3];
    QLineEdit *sIPLE[3];
    QLineEdit *sPortLE[3];
};

class AlertSetPage : public QWizardPage
{
    Q_OBJECT
public:
    AlertSetPage(_CONFIGURE con, QWidget *parent = 0);
private:
    QString logDir;
    WriteLog *log;
    QLabel *nameLB[4];
    QCheckBox *enableCB[4];
    QLabel *tagLabel[2];
    QSpinBox *numStaSB[2];
    QLineEdit *scriptFileLE[4];
    QPushButton *browPB[4];
    QPushButton *previewPB[4];

    void setValue(_CONFIGURE);
private slots:
    void browPBCLicked();
};

class ConfigurationWizard : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationWizard(QWidget *parent = 0);
    ~ConfigurationWizard();

    QObject *rootObj;

    void setup(_CONFIGURE configure);

    _CONFIGURE con;

private:
    QString logDir;
    WriteLog *log;
    Ui::ConfigurationWizard *ui;   

    QWizard *wizard;
    QVariant returnedValue;

private slots:
    void writeConfigureToFile();

signals:
    void resetMainWindow();
};

#endif // CONFIGURATIONWIZARD_H
