#ifndef COMMON_H
#define COMMON_H

#include <QStringList>
#include <QString>
#include <QVector>
#include <QColor>
#include <QDateTime>
#include <QDebug>

#define MAX_LOCALSTA_NUM 6
#define MAX_KISSSTA_NUM 6
#define MAX_ONSITE_NUM MAX_LOCALSTA_NUM + MAX_KISSSTA_NUM

#define EVENT_DURATION 120

#define DEFAULT_READ_EVENTS_FOR_DAY 350

#define KGONSITE_VERSION 1   // 2017/05

#define STA_LEN 10
#define CHAN_LEN 5
#define NET_LEN 4
#define LOC_LEN 4

#define MSG_TYPE_SOH 'S'
#define MSG_TYPE_STA_OPTION 'T'
#define MSG_TYPE_KGONSITE_OPTION 'K'
#define MSG_TYPE_PICK 'P'
#define MSG_TYPE_KGONSITE_INFO 'I'

#define P_VEL 6 // 6km/s
#define S_VEL 4 // 4km/s

//#define MYPOSITION_LAT 36.8
//#define MYPOSITION_LON 127.2
#define MYPOSITION_LAT 37.3419
#define MYPOSITION_LON 127.9195

#define PI 3.14159265358979323846

static QDateTime convertKST(QDateTime oriTime)
{
    oriTime.setTimeSpec(Qt::UTC);
    oriTime = oriTime.addSecs(3600 * 9);
    return oriTime;
}

static QDateTime convertUTC(QDateTime oriTime)
{
    oriTime = oriTime.addSecs( - (3600 * 9) );
    return oriTime;
}

enum nudMessageType {NEW, UPDATE, DELETE};
enum MessageCategory {LIVE, TEST};

typedef struct _eewinfo
{
  QString kgom_event_id;
  char type;
  int eew_event_id;
  int version;
  enum MessageCategory message_category;
  enum nudMessageType message_type;
  double magnitude;
  double magnitude_uncertainty;
  double latitude;
  double latitude_uncertainty;
  double longitude;
  double longitude_uncertainty;
  double depth;
  double depth_uncertainty;
  double origin_time;
  double origin_time_uncertainty;
  double number_stations;
  double number_triggers; // Seong
  bool sent_flag;
  double sent_time;

  double percentsta;
  double misfit;
  int   status;
} _EEWInfo ;

typedef struct _kgonsite_pick_t
{
  QString kgom_event_id;
  int version;

  char msg_type;                    // 'P' Option

  char sta[STA_LEN];
  char chan[CHAN_LEN];
  char net[NET_LEN];
  char loc[LOC_LEN];

  double lat;
  double lon;
  double elev;

  double ttime;                         // Trigger time
  char   polarity;
  int    weight;
  int    amplitude;
  double period;

  //char dir[64];
  //char dfile[32];

  char dummy[50];

} _KGOnSite_Pick_t;

typedef struct _ksonsite_info_t
{
  QString kgom_event_id;
  int version;

  char msg_type;                    // 'K' KGOnSite

  char sta[STA_LEN];
  char chan[CHAN_LEN];
  char net[NET_LEN];
  char loc[LOC_LEN];

  char duration;                       // 1,2 and 3 second

  char type;                          // 'L' : LOW, 'H' : High'

  double ttime;                        // Trigger time

  float disp_count ;                   // displacement count
  float displacement ;                 // displacement
  double disp_time ;                   // displacement time

  float pvel;                         // peak velocity
  float pacc;                         // peak acceleration
  float tauc;                         // tauc

  float pgv;                          // pgv
  float pgv_uncertainty_low;          // low pgv uncertainty
  float pgv_uncertainty_high;         // high pgv uncertainty

  int alert;

  float snr;
  float log_disp_ratio ;              // log displacement ratio
  float log_PdPv;                     // log10 ( displacement / pvel )
  float dB_Pd;                        // db_Pd

  float intensity;                    // intensity
  float intensity_uncertainty_low;    // low intensity uncertainty
  float intensity_uncertainty_high;   // high intensity uncertainty

  float magnitude;                    // magnitude
  float magnitude_uncertainty_low;    // low magnitude uncertainty
  float magnitude_uncertainty_high;   // high magnitude uncertainty

  float distance;                     // distance
  float distance_uncertainty_low;     // low distance uncertainty
  float distance_uncertainty_high;    // high distance uncertainty

  char dummy[50];

} _KGOnSite_Info_t;

typedef struct _report
{
    _KGOnSite_Pick_t pick;
    QVector<_KGOnSite_Info_t> infos;
} _REPORT;

typedef struct _station
{
    QString sta;
    QString chan;
    QString net;
    QString loc;

    double lat;
    double lon;
    double elev;
    double depth;
} _STATION;

typedef struct _configure
{
    QString KGONSITE_HOME;
    QString configFileName;

    QVector<_STATION> localStaVT;
    QVector<_STATION> kissStaVT;
    int localStaCount = 0;
    int kissStaCount = 0;

    /*
    QString localServerIP;
    QString kissServerIP;
    QString eewServerIP;
    int localServerPort;
    int kissServerPort;
    int eewServerPort;
    */

    /* ActiveMQ */
    QString eew_ip, eew_port, eew_user, eew_passwd, eew_topic;
    QString ampIP;
    int amqPort;

    int sohWarningEnabled;
    int eewWarningEnabled;
    int level1Enabled;
    int level1NumSta;
    int level2Enabled;
    int level2NumSta;
    QString sohWarningScript;
    QString eewWarningScript;
    QString level1Script;
    QString level2Script;

} _CONFIGURE;

static double myRound(double n, unsigned int c)
{
    double marge = pow(10, c);
    double up = n * marge;
    double ret = round(up) / marge;
    return ret;
}

static int geo_to_km(double lat1,double lon1,double lat2,double lon2,double* dist,double* azm)
{
    double a, b;
    double semi_major=a=6378.160;
    double semi_minor=b=6356.775;
    double torad, todeg;
    double aa, bb, cc, dd, top, bottom, lambda12, az, temp;
    double v1, v2;
    double fl, e, e2, eps, eps0;
    double b0, x2, y2, z2, z1, u1p, u2p, xdist;
    double lat1rad, lat2rad, lon1rad, lon2rad;
    double coslon1, sinlon1, coslon2, sinlon2;
    double coslat1, sinlat1, coslat2, sinlat2;
    double tanlat1, tanlat2, cosazm, sinazm;

    double c0, c2, c4, c6;

    double c00=1.0, c01=0.25, c02=-0.046875, c03=0.01953125;
    double c21=-0.125, c22=0.03125, c23=-0.014648438;
    double c42=-0.00390625, c43=0.0029296875;
    double c63=-0.0003255208;

    if( lat1 == lat2 && lon1 == lon2 ) {
        *azm = 0.0;
        *dist= 0.0;
        return(1);
    }

    torad = PI / 180.0;
    todeg = 1.0 / torad;
    fl = ( a - b ) / a;
    e2 = 2.0*fl - fl*fl;
    e  = sqrt(e2);
    eps = e2 / ( 1.0 - e2);

    temp=lat1;
    if(temp == 0.) temp=1.0e-08;
    lat1rad=torad*temp;
    lon1rad=torad*lon1;

    temp=lat2;
    if(temp == 0.) temp=1.0e-08;
    lat2rad=torad*temp;
    lon2rad=torad*lon2;

    coslon1 = cos(lon1rad);
    sinlon1 = sin(lon1rad);
    coslon2 = cos(lon2rad);
    sinlon2 = sin(lon2rad);
    tanlat1 = tan(lat1rad);
    tanlat2 = tan(lat2rad);
    sinlat1 = sin(lat1rad);
    coslat1 = cos(lat1rad);
    sinlat2 = sin(lat2rad);
    coslat2 = cos(lat2rad);

    v1 = a / sqrt( 1.0 - e2*sinlat1*sinlat1 );
    v2 = a / sqrt( 1.0 - e2*sinlat2*sinlat2 );
    aa = tanlat2 / ((1.0+eps)*tanlat1);
    bb = e2*(v1*coslat1)/(v2*coslat2);
    lambda12 = aa + bb;
    top = sinlon2*coslon1 - coslon2*sinlon1;
    bottom = lambda12*sinlat1-coslon2*coslon1*sinlat1-sinlon2*sinlon1*sinlat1;
    az = atan2(top,bottom)*todeg;
    if( az < 0.0 ) az = 360 + az;
    *azm = az;
    az = az * torad;
    cosazm = cos(az);
    sinazm = sin(az);

    if( lat2rad < 0.0 ) {
        temp = lat1rad;
        lat1rad = lat2rad;
        lat2rad = temp;
        temp = lon1rad;
        lon1rad = lon2rad;
        lon2rad = temp;

        coslon1 = cos(lon1rad);
        sinlon1 = sin(lon1rad);
        coslon2 = cos(lon2rad);
        sinlon2 = sin(lon2rad);
        tanlat1 = tan(lat1rad);
        tanlat2 = tan(lat2rad);
        sinlat1 = sin(lat1rad);
        coslat1 = cos(lat1rad);
        sinlat2 = sin(lat2rad);
        coslat2 = cos(lat2rad);

        v1 = a / sqrt( 1.0 - e2*sinlat1*sinlat1 );
        v2 = a / sqrt( 1.0 - e2*sinlat2*sinlat2 );

        aa = tanlat2 / ((1.0+eps)*tanlat1);
        bb = e2*(v1*coslat1)/(v2*coslat2);
        lambda12 = aa + bb;

        top = sinlon2*coslon1 - coslon2*sinlon1;
        bottom =lambda12*sinlat1-coslon2*coslon1*sinlat1-
            sinlon2*sinlon1*sinlat1;
        az = atan2(top,bottom);
        cosazm = cos(az);
        sinazm = sin(az);

    }

    eps0 = eps * ( coslat1*coslat1*cosazm*cosazm + sinlat1*sinlat1 );
    b0 = (v1/(1.0+eps0)) * sqrt(1.0+eps*coslat1*coslat1*cosazm*cosazm);

    x2 = v2*coslat2*(coslon2*coslon1+sinlon2*sinlon1);
    y2 = v2*coslat2*(sinlon2*coslon1-coslon2*sinlon1);
    z2 = v2*(1.0-e2)*sinlat2;
    z1 = v1*(1.0-e2)*sinlat1;

    c0 = c00 + c01*eps0 + c02*eps0*eps0 + c03*eps0*eps0*eps0;
    c2 =       c21*eps0 + c22*eps0*eps0 + c23*eps0*eps0*eps0;
    c4 =                  c42*eps0*eps0 + c43*eps0*eps0*eps0;
    c6 =                                  c63*eps0*eps0*eps0;

    bottom = cosazm*sqrt(1.0+eps0);
    u1p = atan2(tanlat1,bottom);

    top = v1*sinlat1+(1.0+eps0)*(z2-z1);
    bottom = (x2*cosazm-y2*sinlat1*sinazm)*sqrt(1.0+eps0);
    u2p = atan2(top,bottom);

    aa = c0*(u2p-u1p);
    bb = c2*(sin(2.0*u2p)-sin(2.0*u1p));
    cc = c4*(sin(4.0*u2p)-sin(4.0*u1p));
    dd = c6*(sin(6.0*u2p)-sin(6.0*u1p));

    xdist = fabs(b0*(aa+bb+cc+dd));
    *dist = xdist;
    return(1);
}


#endif // COMMON_H
