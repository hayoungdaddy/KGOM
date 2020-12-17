#ifndef RECVMESSAGE_H
#define RECVMESSAGE_H

#include "common.h"

#include <QThread>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

class RecvEEWMessage : public QThread
{
    Q_OBJECT
public:
    RecvEEWMessage(QWidget *parent = 0);
    ~RecvEEWMessage();

public:
    Connection *connection;
    Session *session;
    Destination *destination;
    MessageConsumer *consumer;
    QString ip;

    void setup(QString, QString, QString, QString, bool, bool);
    void cleanUp();
    void run();

signals:
    void _rvEEWInfo(_EEWInfo);
};

class RecvOnsiteMessage : public QThread
{
    Q_OBJECT
public:
    RecvOnsiteMessage(QWidget *parent = 0);
    ~RecvOnsiteMessage();

public:
    Connection *connection;
    Session *session;
    Destination *destination;
    MessageConsumer *consumer;
    QString ip;
    QVector<_STATION> stationVT;
    int isFromKIGAM;

    void setup(QString, QString, QString, QString, bool, bool, int);
    void updateStation(QVector<_STATION>);
    void cleanUp();
    void run();

signals:
    void _rvOnsiteInfo(_KGOnSite_Info_t);
};

class RecvPGAMessage : public QThread
{
    Q_OBJECT
public:
    RecvPGAMessage(QWidget *parent = 0);
    ~RecvPGAMessage();

public:
    Connection *connection;
    Session *session;
    Destination *destination;
    MessageConsumer *consumer;
    QString ip;

    void setup(QString, QString, QString, QString, bool, bool);
    void cleanUp();
    void run();

signals:
    void _rvPGAInfo(_KGKIIS_GMPEAK_EVENT_t);
};

class RecvSOHMessage : public QThread
{
    Q_OBJECT
public:
    RecvSOHMessage(QWidget *parent = 0);
    ~RecvSOHMessage();

public:
    Connection *connection;
    Session *session;
    Destination *destination;
    MessageConsumer *consumer;
    QVector<_STATION> stationVT;
    int isKissStation;

    void setup(QString, QString, QString, QString, bool, bool);
    void updateStation(QVector<_STATION>, int);
    void cleanUp();
    void run();

signals:
    void _rvSOHInfo(_KGOnSite_SOH_t, int, int);
};

class RecvRealTimePGAMessage : public QThread
{
    Q_OBJECT
public:
    RecvRealTimePGAMessage(QWidget *parent = 0);
    ~RecvRealTimePGAMessage();

public:
    Connection *connection;
    Session *session;
    Destination *destination;
    MessageConsumer *consumer;
    QVector<_STATION> stationVT;

    void setup(QString, QString, QString, QString, bool, bool);
    void updateStation(QVector<_STATION>);
    void cleanUp();
    void run();

private:
    QMultiMap<int, _QSCD_FOR_MULTIMAP> mmap;
    int count = 0;

signals:
    void _rvPGAMultiMap(QMultiMap<int, _QSCD_FOR_MULTIMAP>);
};

#endif // RECVMESSAGE_H
