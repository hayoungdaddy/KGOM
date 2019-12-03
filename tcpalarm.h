#ifndef TCPALARM_H
#define TCPALARM_H

#include <iostream>    //cout
#include <stdio.h>     //printf
#include <string.h>    //strlen
#include <string>      //string
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h>     //hostent
#include <unistd.h>

#include <QString>
#include <QDebug>

using namespace std;

#define RLAMP 2
#define YLAMP 3
#define BLAMP 4
#define SOUND 7

#define LAMP_ON    0x02
#define LAMP_BLINK 0x01
#define LAMP_OFF   0x00

#define STYPE_BB      0x01
#define STYPE_BpBp    0x02
#define STYPE_BppB    0x03
#define STYPE_pppp    0x04
#define STYPE_B___    0x05


class TcpAlarm
{
public:
    TcpAlarm();
    ~TcpAlarm();

    bool stop();
    bool conn(string, int);
    bool send_data(char *, int);
    int  receive(char *, int);

private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
};

class ControlAlarm
{
public:
    ControlAlarm();
    ~ControlAlarm();

    TcpAlarm alarm;

    char WCommand[10]={0x57,} ;
    char RCommand[10]={0x52,} ;
    char RecvAck[10]={0x00,} ;

    void setup(QString, int);
    void blinkRED();
    void blinkYELLOW();
    void blinkBLUE();
    void stopBlinkAll();
};

#endif // TCPALARM_H
