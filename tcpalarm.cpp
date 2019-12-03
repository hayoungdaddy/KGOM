#include "tcpalarm.h"

TcpAlarm::TcpAlarm()
{
    sock = -1;
    port = 0;
    address = "";
}


TcpAlarm::~TcpAlarm()
{
}

bool TcpAlarm::stop()
{

}

bool TcpAlarm::conn(string address, int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }

        qDebug()<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }

    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            qDebug()<<"Failed to resolve hostname\n";

            return false;
        }

        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];

            //qDebug()<<address<<" resolved to "<<inet_ntoa(*addr_list[i]);

            break;
        }
    }

    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    qDebug()<<"Connected\n";
    return true;
}

/**
   Send data to the connected host
*/
bool TcpAlarm::send_data(char *data, int len)
{
    //Send some data
    if( send(sock , data , len, 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    qDebug()<<"Data send\n";

    return true;
}

/**
   Receive data from the connected host
*/
int TcpAlarm::receive(char *a, int len=15)
{
    char buffer[len];
    int reply;

    //Receive a reply from the server
    if( reply = recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
    }

    memcpy( a, buffer, 10) ;
    return reply ;
}

ControlAlarm::ControlAlarm()
{
}

ControlAlarm::~ControlAlarm()
{
}

void ControlAlarm::setup(QString ip, int port)
{
    alarm.conn(ip.toStdString(), port);

    WCommand[3] = 2 ;
    RCommand[3] = 2 ;

    memset( WCommand+1, 0x00, 9 ) ;
    memset( RCommand+1, 0x00, 9 ) ;
    memset( RecvAck+1, 0x00, 9 ) ;

    alarm.send_data( RCommand, 10 ) ;
    alarm.receive( RecvAck, 10);

    if ( RecvAck[0] != 'A' )
    {
        qDebug() << " Couldn't receive any data, exit " << endl ;
    }
    else
    {
        qDebug() << " ########################## Received data";
    }
}

void ControlAlarm::blinkRED()
{
    WCommand[RLAMP] = LAMP_BLINK ;
    //WCommand[SOUND] = STYPE_B___ ;
    WCommand[SOUND] = STYPE_BB;
    alarm.send_data( WCommand, 10 ) ;
}

void ControlAlarm::blinkYELLOW()
{
    WCommand[YLAMP] = LAMP_BLINK ;
    //WCommand[SOUND] = STYPE_B___ ;
    WCommand[SOUND] = STYPE_BB;
    alarm.send_data( WCommand, 10 ) ;
}

void ControlAlarm::blinkBLUE()
{
    WCommand[BLAMP] = LAMP_BLINK ;
    //WCommand[SOUND] = STYPE_B___ ;
    WCommand[SOUND] = STYPE_BpBp;
    alarm.send_data( WCommand, 10 ) ;
}

void ControlAlarm::stopBlinkAll()
{
    memset( WCommand+1, 0x00, 9 ) ;
    alarm.send_data( WCommand, 10 ) ;
}