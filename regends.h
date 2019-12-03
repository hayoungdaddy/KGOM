#ifndef SELECTVALUES_H
#define SELECTVALUES_H

#include <QString>
#include <QStringList>
#include <QColor>
#include <QTextCodec>
#include <cmath>

#define NUM_MAGLIST 4
static double magValue[NUM_MAGLIST] = {0, 3, 4.5, 6};
static QColor magColor[NUM_MAGLIST] = {QColor(29, 219, 22), QColor(255, 153, 0), QColor(255, 51, 51), QColor("#AA0000")};

#define NUM_INTENLIST 10
static int intenValue[NUM_INTENLIST] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static QColor intenColor[NUM_INTENLIST] = {QColor("#F6F6F6"), QColor("#bfccff"), QColor("#a0e6ff"),
                                           QColor("#80ffff"), QColor("#7aff93"), QColor("#ffff00"),
                                           QColor("#ffc800"), QColor("#ff9100"), QColor("#ff0000"),
                                           QColor("#c80000")};

#define INTEN1COLOR "#D3D7CF"
#define INTEN2COLOR "#BFCCFF"
#define INTEN3COLOR "#A0E6FF"
#define INTEN4COLOR "#80FFFF"
#define INTEN5COLOR "#7AFF93"
#define INTEN6COLOR "#FFFF00"
#define INTEN7COLOR "#FFC800"
#define INTEN8COLOR "#FF9100"
#define INTEN9COLOR "#FF0000"
#define INTEN10COLOR "#C80000"

#define NUM_SOHLIST 3
static int sohValue[NUM_SOHLIST] = {0, 300, 600};
static QColor sohColor[NUM_SOHLIST] = {QColor(115, 210, 22), QColor(252, 175, 62), QColor(239, 41, 41)};

static QString getMagText(double magd)
{
    int i;
    for(i=0;i<NUM_MAGLIST;i++)
    {
        if(magd < magValue[i])
            break;
        else
            continue;
    }

    if(i == 1)
            return "M<" + QString::number(magValue[i], 'f', 1);
    else if(i > NUM_MAGLIST -1)
        return "M≥" + QString::number(magValue[NUM_MAGLIST-1], 'f', 1);
    else
        return QString::number(magValue[i-1], 'f', 1) + "≤M<" + QString::number(magValue[i], 'f', 1);
}

static QColor getMagColor(double magd)
{
    int i;
    for(i=0;i<NUM_MAGLIST;i++)
    {
        if(magd < magValue[i])
            break;
        else
            continue;
    }

    return magColor[i-1];
}

static QString getIntenText(double intenD)
{
    int intenI = (int)intenD;
    if(intenI <= 0) intenI = 1;
    if(intenI >= 10) intenI = 10;

    return QString::number(intenValue[intenI-1]);
}

static QColor getIntenColor(double intenD)
{
    int intenI = (int)intenD;
    if(intenI <= 0) intenI = 1;
    if(intenI >= 10) intenI = 10;

    return intenColor[intenI-1];
}

static QString alertText(int i)
{
    if(i <= 0) i = 0;
    if(i >= 3) i = 3;
    QStringList alertList;
    alertList << "0" << "1" << "2" << "3";
    return alertList.at(i);
}

static QString alertColor(int i)
{
    if(i <= 0) i = 0;
    if(i >= 3) i = 3;
    QStringList alertColors;
    alertColors << "#CCCCCC" << "#46BFBD" << "#FDB45C" << "#F7464A";
    return alertColors.at(i);
}

static QString distText(double i)
{
    int distI;
    if(i > 150) distI = 0;
    else if(i <= 150 && i > 50) distI = 1;
    else if(i <= 50) distI = 2;

    QStringList distList;
    distList << "> 150Km " << "> 50Km" << "< 50Km";

    return distList.at(distI);
}

static QString distColor(double i)
{
    int distI;
    if(i > 150) distI = 0;
    else if(i <= 150 && i > 50) distI = 1;
    else if(i <= 50) distI = 2;

    QStringList distColors;
    distColors << "#CCFFCC" << "#FFFF33" << "#FF3333";
    return distColors.at(distI);
}

static QString getStringFromInten(double inten)
{
    QString str;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    if(inten < 4) str = codec->toUnicode("약함");
    else if(inten >= 4 && inten < 6) str = codec->toUnicode("강함");
    else if(inten >= 6) str = codec->toUnicode("격함");

    return str;
}

static int getIntenFromMag(double mag, double dist)
{
    double _myGal = 0;
    int intensity;

    if(dist <= 100)
    {
        if(dist < 10) dist = 10;

        _myGal = exp(
                    (0.1250737E+02 + 0.4874629E+00 * (mag - 6) + -0.2940726E-01 * pow((mag - 6), 2) + 0.1737204E-01 * pow((mag - 6), 3))
                            +(-0.1928185E-02 + 0.2251016E-03 * (mag - 6) + -0.6378615E-04 * pow((mag - 6), 2) + 0.6967121E-04 * pow((mag - 6), 3)) * dist
                            +(-0.5795112E+00 + 0.1138817E+00 * (mag - 6) + -0.1162326E-01 * pow((mag - 6), 2) + -0.3646674E-02 * pow((mag - 6), 3)) * log(dist)
                            +(-log(dist) - 0.5 * log(100))
            );
    }
    else
    {
        _myGal = exp(
                    (0.1250737E+02 + 0.4874629E+00 * (mag - 6) + -0.2940726E-01 * pow((mag - 6), 2) + 0.1737204E-01 * pow((mag - 6), 3))
                            +(-0.1928185E-02 + 0.2251016E-03 * (mag - 6) + -0.6378615E-04 * pow((mag - 6), 2) + 0.6967121E-04 * pow((mag - 6), 3)) * dist
                            +(-0.5795112E+00 + 0.1138817E+00 * (mag - 6) + -0.1162326E-01 * pow((mag - 6), 2) + -0.3646674E-02 * pow((mag - 6), 3)) * log(dist)
                            +(-log(100) - 0.5 * log(dist))
            );
    }

    if(_myGal <= 0.98) intensity = 1;
    else if(_myGal > 0.98 && _myGal <= 2.94) intensity = 2;
    else if(_myGal > 2.94 && _myGal <= 7.84) intensity = 3;
    else if(_myGal > 7.84 && _myGal <= 23.52) intensity = 4;
    else if(_myGal > 23.52 && _myGal <= 65.66) intensity = 5;
    else if(_myGal > 65.66 && _myGal <= 127.40) intensity = 6;
    else if(_myGal > 127.40 && _myGal <= 235.20) intensity = 7;
    else if(_myGal > 235.20 && _myGal <= 431.20) intensity = 8;
    else if(_myGal > 431.20 && _myGal <= 813.40) intensity = 9;
    else if(_myGal > 813.40) intensity = 10;

    return intensity;
}

#endif // SELECTVALUES_H
