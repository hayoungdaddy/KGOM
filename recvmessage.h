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

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

class RecvEEWMessage : public QThread
{
    Q_OBJECT
public:
    RecvEEWMessage(QString qBrokerURI, QString qID, QString qPW, QString qTopic, bool useTopic, bool clientAck, QWidget *parent = 0);
    ~RecvEEWMessage();

public:
    ActiveMQConnectionFactory* connectionFactory;
    Connection *connection;
    Session *session;
    Destination *destination;
    MessageConsumer *consumer;
    QString ip;

    void cleanUp();
    void run();

private:

signals:
    void _rvEEWInfo(_EEWInfo);
};

class RecvMessageUDP : public QObject
{
    Q_OBJECT
public:
    RecvMessageUDP();
    ~RecvMessageUDP();

    void setup(QString, int);

private:
    QUdpSocket *rvMsgSock;
    int rvMsgSock_port;
    QHostAddress localhost;
    void processDatagram(QByteArray);

private slots:
    void rvMsg();

signals:
signals:
    void _rvPickInfo(unsigned char *);
    void _rvOnsiteInfo(unsigned char *);
};

#endif // RECVMESSAGE_H
