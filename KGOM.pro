#-------------------------------------------------
#
# Project created by QtCreator 2018-06-25T01:31:18
#
#-------------------------------------------------

QT       += core gui quick network sql printsupport charts xml location

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KGOM
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
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
    writelog.cpp \
    eventlist.cpp \
    configurationwizard.cpp \
    logviewer.cpp \
    recvmessage.cpp \
    sohmonitor.cpp \
    eewinfo.cpp \
    searchform.cpp \
    onsiteinfo.cpp \
    detailview.cpp \
    eewdetailtable.cpp \
    onsitedetailtable.cpp \
    aboutthis.cpp \
    configuration.cpp \
    tcpalarm.cpp \
    pgainfo.cpp \
    pgadetailtable.cpp

HEADERS += \
        mainwindow.h \
    common.h \
    writelog.h \
    eventlist.h \
    configurationwizard.h \
    logviewer.h \
    recvmessage.h \
    sohmonitor.h \
    eewinfo.h \
    searchform.h \
    regends.h \
    onsiteinfo.h \
    detailview.h \
    eewdetailtable.h \
    onsitedetailtable.h \
    aboutthis.h \
    configuration.h \
    tcpalarm.h \
    pgainfo.h \
    pgadetailtable.h

FORMS += \
        mainwindow.ui \
    eventlist.ui \
    configurationwizard.ui \
    logviewer.ui \
    sohmonitor.ui \
    eewinfo.ui \
    searchform.ui \
    onsiteinfo.ui \
    detailview.ui \
    eewdetailtable.ui \
    onsitedetailtable.ui \
    aboutthis.ui \
    configuration.ui \
    pgainfo.ui \
    pgadetailtable.ui

RESOURCES += \
    KGOM.qrc

LIBS += -lactivemq-cpp
INCLUDEPATH += /usr/local/include/activemq-cpp-3.9.4
INCLUDEPATH += /usr/local/apr/include/apr-1/
