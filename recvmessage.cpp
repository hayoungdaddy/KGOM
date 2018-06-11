#include "recvmessage.h"

RecvEEWMessage::RecvEEWMessage(QString qBrokerURI, QString qID, QString qPW, QString qTopic, bool useTopic, bool clientAck, QWidget *parent)
{
    activemq::library::ActiveMQCPP::initializeLibrary();

    connectionFactory = new ActiveMQConnectionFactory(qBrokerURI.toStdString());
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

RecvEEWMessage::~RecvEEWMessage()
{
    cleanUp();
}

void RecvEEWMessage::run()
{
    while (this->isRunning())
    {
        TextMessage *m = (TextMessage*) consumer->receive();
        QString xmlbody = QString::fromStdString(m->getText());
        QDateTime time = QDateTime::currentDateTimeUtc();
        //qDebug() << time.toString("mm:ss") << " " << xmlbody;

        _EEWInfo _eewinfo;

        /*
         "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n
            <event_message alg_vers=\"1.0\" category=\"live\" instance=\"Evt4AMQ2AMQ\" message_type=\"new\" orig_sys=\"elarms\" timestamp=\"2018-05-22T18:49:26.885Z\" version=\"0\">\n\n
                <core_info id=\"15935\">\n
                    <mag units=\"Mw\">2.2808</mag>\n
                    <mag_uncer units=\"Mw\">0.5262</mag_uncer>\n
                    <lat units=\"deg\">34.9360</lat>\n
                    <lat_uncer units=\"deg\">0.1840</lat_uncer>\n
                    <lon units=\"deg\">127.0459</lon>\n
                    <lon_uncer units=\"deg\">0.1840</lon_uncer>\n
                    <depth units=\"km\">8.0000</depth>\n
                    <depth_uncer units=\"km\">5.0000</depth_uncer>\n
                    <orig_time units=\"UTC\">2018-05-22T18:49:19.294Z</orig_time>\n
                    <orig_time_uncer units=\"sec\">4.4830</orig_time_uncer>\n
                    <likelihood>0.8909</likelihood>\n
                    <num_stations>3</num_stations>\n
                    <num_triggers>-48949624</num_triggers>\n
                    <sent_flag>0</sent_flag>\n
                    <sent_time>0.0000</sent_time>\n
                    <percentsta>0.0000</percentsta>\n
                    <misfit_rms>-336706446282276808225657573530520480000000000000000000000000000000000000000000000000000000000000000\u001A\u001A\u001A\u001A\u001A\u001A@</misfit_rms>\n
                    <misfit_ave>0.0000</misfit_ave>\n
                    <status>-49406116</status>\n
                </core_info>\n\n
            </event_message>\n"
         */

        xmlbody = xmlbody.simplified();
        xmlbody.replace(QString("\n"), QString(""));

        //qDebug() << xmlbody;

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
                    _eewinfo.eew_event_id = attributes.value("id").toInt();
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
                    // 2018-05-22T18:49:19.294Z
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
                qDebug() << temp;
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

        qDebug() << "===========";
        qDebug() << _eewinfo.message_category;
        qDebug() << _eewinfo.message_type;
        qDebug() << _eewinfo.version;
        qDebug() << QString::number(_eewinfo.eew_event_id);
        qDebug() << QString::number(_eewinfo.magnitude, 'f', 2);
        qDebug() << QString::number(_eewinfo.magnitude_uncertainty, 'f', 2);
        qDebug() << QString::number(_eewinfo.latitude, 'f', 4);
        qDebug() << QString::number(_eewinfo.latitude_uncertainty, 'f', 4);
        qDebug() << QString::number(_eewinfo.longitude, 'f', 2);
        qDebug() << QString::number(_eewinfo.longitude_uncertainty, 'f', 4);
        qDebug() << QString::number(_eewinfo.depth, 'f', 2);
        qDebug() << QString::number(_eewinfo.depth_uncertainty, 'f', 2);
        qDebug() << QString::number(_eewinfo.origin_time, 'f', 3);
        QDateTime tt;
        tt.setTime_t(_eewinfo.origin_time);
        QString msec = QString::number(_eewinfo.origin_time, 'f', 3).section(".", 1, 1);
        tt = tt.addMSecs(msec.toInt());
        qDebug() << tt.toString("yyyy/MM/dd hh:mm:ss.zzz");
        qDebug() << QString::number(_eewinfo.origin_time_uncertainty, 'f', 4);
        qDebug() << QString::number(_eewinfo.number_stations);
        qDebug() << QString::number(_eewinfo.number_triggers);
        qDebug() << _eewinfo.sent_flag;
        qDebug() << QString::number(_eewinfo.sent_time, 'f', 4);

        emit _rvEEWInfo(_eewinfo);
    }
}

void RecvEEWMessage::cleanUp()
{
    if (connection != NULL)
        connection->close();

    delete destination;
    destination = NULL;
    delete consumer;
    consumer = NULL;
    delete session;
    session = NULL;
    delete connection;

    activemq::library::ActiveMQCPP::shutdownLibrary();
}


RecvMessageUDP::RecvMessageUDP()
{
    rvMsgSock = new QUdpSocket(this);
}

RecvMessageUDP::~RecvMessageUDP()
{
    rvMsgSock->close();
}

void RecvMessageUDP::setup(QString hostaddress, int port)
{
    localhost.setAddress("127.0.0.1");
    rvMsgSock_port = port;

    if(rvMsgSock->bind( localhost, rvMsgSock_port ))
    {
        connect(rvMsgSock, SIGNAL(readyRead()), this, SLOT(rvMsg()));
    }
    else
        qWarning("rv EEW Sock bind Failure");
}

void RecvMessageUDP::rvMsg()
{
    while(rvMsgSock->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = rvMsgSock->receiveDatagram();
        QByteArray replyData = datagram.data();
        processDatagram(replyData);
    }
}

void RecvMessageUDP::processDatagram(QByteArray datagram)
{
    unsigned char *msg;
    int length = datagram.length();
    msg = (unsigned char *)malloc(length);
    memcpy(msg, (unsigned char *)datagram.data(), length);

    if(length == sizeof(_KGOnSite_Pick_t)) // pick info
    {
        emit _rvPickInfo(msg);
    }
    else if(length == sizeof(_KGOnSite_Info_t)) // onsite info
    {
        emit _rvOnsiteInfo(msg);
    }
}
