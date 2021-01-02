#include "recvmessage.h"

RecvEEWMessage::RecvEEWMessage(QWidget *parent)
{
}

RecvEEWMessage::~RecvEEWMessage()
{
    cleanUp();
}

void RecvEEWMessage::setup(QString qBrokerURI, QString qID, QString qPW, QString qTopic, bool useTopic, bool clientAck)
{
    auto_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory( qBrokerURI.toStdString() ));

    if(qID == "" || qPW == "")
        connection = connectionFactory->createConnection();
    else
        connection = connectionFactory->createConnection(qID.toStdString(), qPW.toStdString());

    connection->start();

    // Create a Session
    if( clientAck ) {
        session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
    } else {
        session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
    }

    useTopic = true;

    // Create the destination (Topic or Queue)
    if( useTopic ) {
        destination = session->createTopic( qTopic.toStdString() );
    } else {
        destination = session->createQueue( qTopic.toStdString() );
    }

    consumer = session->createConsumer (destination);
}

void RecvEEWMessage::run()
{
    while (this->isRunning())
    {
        if(this->isInterruptionRequested())
            this->quit();

        auto_ptr<Message> message (consumer->receive() );
        if( message.get() != nullptr )
        {
            const TextMessage *m = dynamic_cast< const TextMessage* >( message.get() );
            QString xmlbody = QString::fromStdString(m->getText());

            _EEWInfo _eewinfo;

            xmlbody = xmlbody.simplified();
            xmlbody.replace(QString("\n"), QString(""));

            QXmlStreamReader xml(xmlbody);
            while(!xml.atEnd() && !xml.hasError())
            {
                xml.readNext();
                if(xml.name() == "event_message")
                {
                    QXmlStreamAttributes attributes = xml.attributes();
                    if(attributes.value("category").toString().startsWith("live"))
                        _eewinfo.message_category = LIVE;
                    else
                        _eewinfo.message_category = TEST;

                    if(attributes.value("message_type").toString().startsWith("new"))
                        _eewinfo.message_type = NEW;
                    else if(attributes.value("message_type").toString().startsWith("update"))
                        _eewinfo.message_type = UPDATE;
                    else
                        _eewinfo.message_type = DELETE;

                    _eewinfo.version = attributes.value("version").toInt();
                }
                else if(xml.name() == "core_info")
                {
                    QXmlStreamAttributes attributes = xml.attributes();
                    if(attributes.hasAttribute("id"))
                        _eewinfo.eew_evid = attributes.value("id").toInt();
                }
                else if(xml.name() == "mag")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.magnitude = temp.toDouble();
                }
                else if(xml.name() == "mag_uncer")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.magnitude_uncertainty = temp.toDouble();
                }
                else if(xml.name() == "lat")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.latitude = temp.toDouble();
                }
                else if(xml.name() == "lat_uncer")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.latitude_uncertainty = temp.toDouble();
                }
                else if(xml.name() == "lon")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.longitude = temp.toDouble();
                }
                else if(xml.name() == "lon_uncer")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.longitude_uncertainty = temp.toDouble();
                }
                else if(xml.name() == "depth")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.depth = temp.toDouble();
                }
                else if(xml.name() == "depth_uncer")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.depth_uncertainty = temp.toDouble();
                }
                else if(xml.name() == "orig_time")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                    {
                        QDateTime dt = QDateTime::fromString(temp, "yyyy-MM-ddThh:mm:ss.zzzZ");
                        dt.setTimeSpec(Qt::UTC);
                        _eewinfo.origin_time = dt.toTime_t();
                        QString msec = "0." + dt.toString("zzz");
                        _eewinfo.origin_time = _eewinfo.origin_time + msec.toDouble();
                    }
                }
                else if(xml.name() == "orig_time_uncer")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.origin_time_uncertainty = temp.toDouble();
                }
                else if(xml.name() == "num_stations")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.number_stations = temp.toDouble();
                }
                else if(xml.name() == "num_triggers")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.number_triggers = temp.toDouble();
                }
                else if(xml.name() == "sent_flag")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    //qDebug() << temp;
                    if(!temp.startsWith(" "))
                    {
                        if(temp.startsWith("0"))
                            _eewinfo.sent_flag = true;
                        else
                            _eewinfo.sent_flag = false;
                    }
                }
                else if(xml.name() == "sent_time")
                {
                    xml.readNext();
                    QString temp = xml.text().toString();
                    if(!temp.startsWith(" "))
                        _eewinfo.sent_time = temp.toDouble();
                }
            }

            emit _rvEEWInfo(_eewinfo);
        }
    }
}

void RecvEEWMessage::cleanUp()
{
    if (connection != nullptr)
        connection->close();

    delete destination;
    destination = nullptr;
    delete consumer;
    consumer = nullptr;
    delete session;
    session = nullptr;
    delete connection;
}


RecvOnsiteMessage::RecvOnsiteMessage(QWidget *parent)
{
}

RecvOnsiteMessage::~RecvOnsiteMessage()
{
    cleanUp();
}

void RecvOnsiteMessage::setup(QString qBrokerURI, QString qID, QString qPW, QString qTopic, bool useTopic, bool clientAck, int fromWhere)
{
    auto_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory( qBrokerURI.toStdString() ));

    if(qID == "" || qPW == "")
        connection = connectionFactory->createConnection();
    else
        connection = connectionFactory->createConnection(qID.toStdString(), qPW.toStdString());

    connection->start();

    isFromKIGAM = fromWhere;    // 0 is local, 1 is kigam (kiss)

    // Create a Session
    if( clientAck ) {
        session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
    } else {
        session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
    }

    useTopic = true;

    // Create the destination (Topic or Queue)
    if( useTopic ) {
        destination = session->createTopic( qTopic.toStdString() );
    } else {
        destination = session->createQueue( qTopic.toStdString() );
    }

    consumer = session->createConsumer (destination);
}

void RecvOnsiteMessage::updateStation(QVector<_STATION> staVT)
{
    stationVT = staVT;
}

void RecvOnsiteMessage::run()
{
    while (this->isRunning())
    {
        if(this->isInterruptionRequested())
            this->quit();

        auto_ptr<Message> message ( consumer->receive() );
        if( message.get() != nullptr )
        {
            const BytesMessage *b = dynamic_cast< const BytesMessage* >( message.get() );
            char *msg = (char *)b->getBodyBytes();

            _KGOnSite_Info_t _myOnsite;
            memcpy(&_myOnsite, msg, sizeof(_myOnsite));
            if(_myOnsite.msg_type == MSG_TYPE_KGONSITE_INFO)
            {
                QString sc, sc2;
                sc2 = QString(_myOnsite.sta) + "/" + QString(_myOnsite.net);

                for(int i=0;i<stationVT.count();i++)
                {
                    sc = stationVT.at(i).sta + "/" + stationVT.at(i).net;
                    if(sc.startsWith(sc2))
                    {
                        if(isFromKIGAM == 1) _myOnsite.fromWhere = 'K';
                        else if(isFromKIGAM == 0) _myOnsite.fromWhere = 'L';
                        _myOnsite.lat = stationVT.at(i).lat;
                        _myOnsite.lon = stationVT.at(i).lon;
                        _myOnsite.elev = stationVT.at(i).elev;
                        emit _rvOnsiteInfo(_myOnsite);
                        break;
                    }
                }
            }
            free(msg);
        }
    }
}

void RecvOnsiteMessage::cleanUp()
{
    if (connection != nullptr)
        connection->close();

    delete destination;
    destination = nullptr;
    delete consumer;
    consumer = nullptr;
    delete session;
    session = nullptr;
    delete connection;
}


RecvSOHMessage::RecvSOHMessage(QWidget *parent)
{
}

RecvSOHMessage::~RecvSOHMessage()
{
    cleanUp();
}

void RecvSOHMessage::setup(QString qBrokerURI, QString qID, QString qPW, QString qTopic, bool useTopic, bool clientAck)
{
    auto_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory( qBrokerURI.toStdString() ));

    if(qID == "" || qPW == "")
        connection = connectionFactory->createConnection();
    else
        connection = connectionFactory->createConnection(qID.toStdString(), qPW.toStdString());

    connection->start();

    // Create a Session
    if( clientAck ) {
        session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
    } else {
        session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
    }

    useTopic = true;

    // Create the destination (Topic or Queue)
    if( useTopic ) {
        destination = session->createTopic( qTopic.toStdString() );
    } else {
        destination = session->createQueue( qTopic.toStdString() );
    }

    consumer = session->createConsumer (destination);
}

void RecvSOHMessage::updateStation(QVector<_STATION> staVT, int kiss)
{
    stationVT = staVT;
    isKissStation = kiss;
}

void RecvSOHMessage::run()
{
    while (this->isRunning())
    {
        if(this->isInterruptionRequested())
            this->quit();

        auto_ptr<Message> message ( consumer->receive() );
        if( message.get() != nullptr )
        {
            const BytesMessage *b = dynamic_cast< const BytesMessage* >( message.get() );
            char *msg = (char *)b->getBodyBytes();

            _KGOnSite_SOH_t soh;
            memcpy(&soh, msg, sizeof(soh));

            bool isMyStation = false;
            int index = 0;
            QString sc, sc2;

            sc2 = QString(soh.sta) + "/" + QString(soh.net);

            for(int i=0;i<stationVT.count();i++)
            {
                sc = stationVT.at(i).sta + "/" + stationVT.at(i).net;

                if(sc.startsWith(sc2))
                {
                    isMyStation = true;
                    index = i;
                    break;
                }
            }

            if(isMyStation == true)
                emit _rvSOHInfo(soh, isKissStation, index);

            free(msg);
        }
    }
}

void RecvSOHMessage::cleanUp()
{
    if (connection != nullptr)
        connection->close();

    delete destination;
    destination = nullptr;
    delete consumer;
    consumer = nullptr;
    delete session;
    session = nullptr;
    delete connection;
}

RecvQSCD20Message::RecvQSCD20Message(QWidget *parent)
{
}

RecvQSCD20Message::~RecvQSCD20Message()
{
    cleanUp();
}

void RecvQSCD20Message::setup(QString qBrokerURI, QString qID, QString qPW, QString qTopic, bool useTopic, bool clientAck)
{
    auto_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory( qBrokerURI.toStdString() ) );

    if(qID == "" || qPW == "")
    {
        connection = connectionFactory->createConnection();
    }
    else
    {
        connection = connectionFactory->createConnection(qID.toStdString(), qPW.toStdString());
    }

    connection->start();

    // Create a Session
    session = connection->createSession( Session::AUTO_ACKNOWLEDGE );

    useTopic = true;

    // Create the destination (Topic or Queue)
    if( useTopic ) {
        destination = session->createTopic( qTopic.toStdString() );
    } else {
        destination = session->createQueue( qTopic.toStdString() );
    }
    consumer = session->createConsumer (destination);
}

void RecvQSCD20Message::updateStation(QVector<_STATION> staVT)
{
    stationVT = staVT;
}

void RecvQSCD20Message::run()
{
    while (this->isRunning())
    {
        if(this->isInterruptionRequested())
            this->quit();

        auto_ptr<Message> message( consumer->receive() );
        if( message.get() != NULL )
        {
            const BytesMessage* bytes_message = dynamic_cast< const BytesMessage* >( message.get() );
            char *msg = (char *)bytes_message->getBodyBytes();

            QFuture<QMultiMap<int, _QSCD_FOR_MULTIMAP>> future = QtConcurrent::run(convertMMap, msg, stationVT);
            future.waitForFinished();

            emit _rvPGAMultiMap(future.result());
            mmap.clear();
            free(msg);
        }
    }
}

void RecvQSCD20Message::cleanUp()
{
    if (connection != nullptr)
        connection->close();

    delete destination;
    destination = nullptr;
    delete consumer;
    consumer = nullptr;
    delete session;
    session = nullptr;
    delete connection;
}
