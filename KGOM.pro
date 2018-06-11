#-------------------------------------------------
#
# Project created by QtCreator 2017-07-31T23:54:22
#
#-------------------------------------------------

QT       += core gui quick network sql printsupport charts xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KGOM
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    sohmonitor.cpp \
    writelog.cpp \
    qcustomplot.cpp \
    reportviewer.cpp \
    alertdialog.cpp \
    configurationwizard.cpp \
    distancedialog.cpp \
    intensitydialog.cpp \
    magdialog.cpp \
    tabinfo.cpp \
    onsitechart.cpp \
    listinfo.cpp \
    logviewer.cpp \
    recvmessage.cpp \
    getwavefromew.cpp \
    eewinfo.cpp

HEADERS += \
        mainwindow.h \
    common.h \
    sohmonitor.h \
    writelog.h \
    qcustomplot.h \
    reportviewer.h \
    alertdialog.h \
    configurationwizard.h \
    distancedialog.h \
    intensitydialog.h \
    magdialog.h \
    tabinfo.h \
    onsitechart.h \
    listinfo.h \
    recvmessage.h \
    logviewer.h \
    selectvalues.h \
    getwavefromew.h \
    eewinfo.h

FORMS += \
        mainwindow.ui \
    sohmonitor.ui \
    reportviewer.ui \
    alertdialog.ui \
    configurationwizard.ui \
    distancedialog.ui \
    intensitydialog.ui \
    magdialog.ui \
    tabinfo.ui \
    onsitechart.ui \
    listinfo.ui \
    logviewer.ui \
    eewinfo.ui

RESOURCES += \
    KGOM.qrc

TRANSLATIONS = kgomtr_ko.ts

LIBS += -lactivemq-cpp
INCLUDEPATH += /usr/local/include/activemq-cpp-3.9.4
INCLUDEPATH += /usr/local/apr/include/apr-1/


LIBS += -L/home/sysop/earthworm_7.9/lib/ -lew -lmseed
#LIBS += /home/sysop/earthworm_7.9/lib/libmseed.a
INCLUDEPATH += /home/sysop/earthworm_7.9/include
