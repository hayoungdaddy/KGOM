#ifndef SELECTVALUES_H
#define SELECTVALUES_H

#include <QString>
#include <QStringList>
#include <QColor>
#include <QTextCodec>
#include <cmath>


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

#endif // SELECTVALUES_H
