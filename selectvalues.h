#ifndef SELECTVALUES_H
#define SELECTVALUES_H

#include <QString>
#include <QStringList>
#include <QColor>

#define NUM_INTENLIST 10
#define NUM_ALERTLIST 3
#define NUM_MAGLIST 3
#define NUM_DISTLIST 2

static QString intenText(double i)
{
    int intenI = (int)i;
    if(intenI <= 0) intenI = 1;
    if(intenI >= 10) intenI = 10;

    QStringList intenList;
    intenList << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10+";

    return intenList.at(intenI);
}

static QString intenColor(double i)
{
    int intenI = (int)i;
    if(intenI <= 0) intenI = 1;
    if(intenI >= 10) intenI = 10;

    QStringList intenColors;
    intenColors << "" << "#F6F6F6" << "#bfccff" << "#a0e6ff" << "#80ffff" << "#7aff93"
                << "#ffff00" << "#ffc800" << "#ff9100" << "#ff0000" << "#c80000";
    return intenColors.at(intenI);
}

static QString intenSubText(double i)
{
    int intenI = (int)i;
    if(intenI <= 0) intenI = 1;
    if(intenI >= 10) intenI = 10;

    QStringList intenSubList;
    intenSubList << ""
                 << "No Damage\n(no shaking)"                  << "No Damage\n(weak shaking)"
                 << "No Damage\n(weak shaking)"                 << "No Damage\n(light shaking)"
                 << "Very Light Damage\n(moderate shaking)"                 << "Light Damage\n(strong shaking)"
                 << "Moderate Damage\n(very strong shaking)"                 << "Moderate-Heavy Damage\n(severe shaking)"
                 << "Heavy Damage\n(violent shaking)"                 << "Very Heavy Damage\n(extreme shaking)";
    return intenSubList.at(intenI);
}

static QString alertText(int i)
{
    if(i <= 0) i = 0;
    if(i >= 3) i = 3;
    QStringList alertList;
    alertList << "0" << "1" << "2" << "3";
    return alertList.at(i);
}

static QString alertSubText(int i)
{
    if(i <= 0) i = 0;
    if(i >= 3) i = 3;
    QStringList alertSubList;
    alertSubList << "No\nDamage" << "Far\nDamage" << "Local\nDamage" << "Local & Far\nDamage";
    return alertSubList.at(i);
}

static QString alertColor(int i)
{
    if(i <= 0) i = 0;
    if(i >= 3) i = 3;
    QStringList alertColors;
    alertColors << "#CCCCCC" << "#46BFBD" << "#FDB45C" << "#F7464A";
    return alertColors.at(i);
}

/*
magList << "Small" << "Medium" << "Moderate" << "Large";
magColors << "#CCFFCC" << "#FFFF33" << "#FF9900" << "#FF3333";
magSubList << "M < 3" << "3 < M < 5" << "5 < M < 6.5" << "M > 6.5";
*/

static QString magText(double i)
{
    int magI;
    if(i < 4) magI = 0;
    else if(i >= 4 && i < 5) magI = 1;
    else if(i >= 5) magI = 2;

    QStringList magList;
    magList << "< 4" << "4 ~ 5" << "> 5";
    return magList.at(magI);
}

static QString magColor(double i)
{
    int magI;
    if(i < 4) magI = 0;
    else if(i >= 4 && i < 5) magI = 1;
    else if(i >= 5) magI = 2;

    QStringList magColors;
    magColors << "#CCFFCC" << "#FFFF33" << "#FF9900";
    return magColors.at(magI);
}

/*
distList << "Far" << "Medium" << "Near";
distColors << "#CCFFCC" << "#FF3333" << "#FFFF33";
distSubList << "KM > 150" << "50 < KM < 150" << "KM < 50";
*/

static QString distText(double i)
{
    int distI;
    if(i > 50) distI = 0;
    else if(i <= 50) distI = 1;

    QStringList distList;
    distList << "> 50Km" << "< 50Km";

    return distList.at(distI);
}

static QString distColor(double i)
{
    int distI;
    if(i > 50) distI = 0;
    else if(i <= 50) distI = 1;

    QStringList distColors;
    distColors << "#CCFFCC" << "#FFFF33";
    return distColors.at(distI);
}

#endif // SELECTVALUES_H
