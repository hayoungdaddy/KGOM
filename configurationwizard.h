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
    QTextCodec *codec;
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
    QTextCodec *codec;

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
    QTextCodec *codec;

private slots:
    void addKissStaPBClicked();
    void delKissStaPBClicked();
    void showKissStaList();
    void hideKissStaList();
};

class ConfigurationWizard : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationWizard(QWidget *parent = 0);
    ~ConfigurationWizard();

    QObject *rootObj;

    void setup(_CONFIGURE);

    _CONFIGURE con;

private:
    Ui::ConfigurationWizard *ui;

    QString logDir;
    WriteLog *log;

    QWizard *wizard;
    QVariant returnedValue;
    QTextCodec *codec;

private slots:
    void writeConfigureToFile();

signals:
    void resetMainWindow();
};

#endif // CONFIGURATIONWIZARD_H
