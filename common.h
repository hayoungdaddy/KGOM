#ifndef COMMON_H
#define COMMON_H

#include <QStringList>
#include <QString>
#include <QVector>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QTextCodec>
#include <QtMath>
#include <QtConcurrent>

#define KGOM_VERSION POSTECH_V2_0

#define MAX_LOCALSTA_NUM 6
#define MAX_KISSSTA_NUM 6
#define MAX_ONSITE_NUM MAX_LOCALSTA_NUM + MAX_KISSSTA_NUM

#define EVENT_DURATION 120
#define SEARCH_DURATION 31

#define STA_LEN 10
#define CHAN_LEN 5
#define NET_LEN 4
#define LOC_LEN 4
#define STR_MAX_LENGTH 30

#define SOH_INTERVAL 60                // seconds
#define STAINFO_INTERVAL 86400         // seconds
#define WAIT_4DELETE_NOT_SENT_INFO 60  // seconds

#define KIGAM_ONSITE 'K'
#define LOCAL_ONSITE 'L'

#define MSG_TYPE_SOH 'S'
#define MSG_TYPE_STA_SOH 'A'
#define MSG_TYPE_STA_OPTION 'T'
#define MSG_TYPE_KGONSITE_OPTION 'K'
#define MSG_TYPE_PICK 'P'
#define MSG_TYPE_KGONSITE_INFO 'I'
#define MSG_TYPE_MANAGE_MESSAGE 'M'
#define MSG_TYPE_GMPEAK_EVENT 'E'

#define RUN_MODE_AMQ 'A'
#define RUN_MODE_ZMQ 'Z'
#define RUN_MODE_UDP 'U'
#define RUN_MODE_TCP 'T'

#define ALERT_LOW_SNR -2
#define ALERT_S_PD -3
#define ALERT_DISP_THRESHOLD -4
#define ALERT_TAUC_THRESHOLD -5
#define ALERT_VEL_THRESHOLD -6
#define ALERT_LOGPDPV_LOWMIN -7
#define ALERT_LOGPDPV_LOWMAX -8

#define PI 3.14159265358979323846

#define MAX_QSCD_CNT 100
#define QSCD_LEN 120

#define NUM_LEGEND 10

static QString find_loc_program = "findLocC";

static QDateTime convertKST(QDateTime oriTime)
{
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
    int evid;

    char type;
    int eew_evid;
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
    int sent_flag;
    double sent_time;
    double percentsta;
    double misfit;
    int   status;
    QDate lddate;
} _EEWInfo ;

typedef struct _ksonsite_info_t
{
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

    int evid;
    char fromWhere;
    double lat;
    double lon;
    double elev;
} _KGOnSite_Info_t;

typedef struct _kgonsite_soh_t
{
  int version ;
  char msg_type ;                      // 'S' SOH

  char sta[STA_LEN];
  char chan[CHAN_LEN];
  char net[NET_LEN];
  char loc[LOC_LEN];

  double dtime ;                       // last received data time
  time_t started ;                     // started time
  time_t stime ;                       // current system time
  int interval;

  char named[STR_MAX_LENGTH];
  char comment[STR_MAX_LENGTH];
} _KGOnSite_SOH_t;

typedef struct _gmpeak_event_sta_t
{
    int version;
    char msg_type;

    char sta[STA_LEN];
    char chan[CHAN_LEN];
    char net[NET_LEN];
    char loc[LOC_LEN];

    float lat;
    float lon;

    time_t time;

    float maxZ;
    float maxN;
    float maxE;

    float maxH;
    float maxT;
} _KGKIIS_GMPEAK_EVENT_STA_t;

typedef struct _pga_detection
{
    int num;
    int num_threshold;
    float threshold1;
    float threshold2;
    int timeWindow;
    QList<_KGKIIS_GMPEAK_EVENT_STA_t> staPGAList;
    int condition;
} _PGA_DETECTION;

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

    int maxPGATime;
    float maxPGA;
} _STATION;

typedef struct _amqinfo
{
    QString ip;
    QString port;
    QString user;
    QString passwd;
    QString topic;
} _AMQINFO;

typedef struct _configure
{
    QString KGOM_HOME;
    QString configFileName;

    double myposition_lat;
    double myposition_lon;

    QVector<_STATION> localStaVT;
    QVector<_STATION> kissStaVT;

    // KIGAM ActiveMQ
    _AMQINFO kiss_eew_amq;
    _AMQINFO kiss_onsite_amq;
    _AMQINFO kiss_soh_amq;
    _AMQINFO kiss_pga_amq;

    // LOCAL ActiveMQ
    _AMQINFO local_onsite_amq;
    _AMQINFO local_soh_amq;
    _AMQINFO local_pga_amq;

    // Alert Level
    QString alarm_device_ip;
    int alarm_device_port;

    int mag_level1_alert_use;
    float mag_level1_alert_min_mag;
    float mag_level1_alert_max_mag;
    int mag_level1_alert_dist;

    int mag_level2_alert_use;
    float mag_level2_alert_min_mag;
    float mag_level2_alert_max_mag;
    int mag_level2_alert_dist;

    int pga_level1_alert_use;
    float pga_level1_threshold;

    int pga_level2_alert_use;
    float pga_level2_threshold;
    int pga_num_sta_threshold;
    int pga_time_window;

    double p_vel;
    double s_vel;
} _CONFIGURE;



typedef struct _qscd_struct
{
    int crc32 ;         // 4byte CRC Number
    char QFlag ;        // 1byte Quality Flag
    char DType ;        // 1byte Data Type
    char Reserved ;        // 1byte Unused
    char SSSSS[5] ;     // 5byte Station Code
    int time ;          // 4byte Data Time(EpochTime)
    float ZWMin ;     // 4byte U-D Windowed Minimum
    float ZWMax ;     // 4byte U-D Windowed Maximum
    float ZWAvg ;     // 4byte U-D Windowed Average
    float NWMin ;     // 4byte N-S Windowed Minimum
    float NWMax ;     // 4byte N-S Windowed Maximum
    float NWAvg ;     // 4byte N-S Windowed Average
    float EWMin ;     // 4byte E-W Windowed Minimum
    float EWMax ;     // 4byte E-W Windowed Maximum
    float EWAvg ;     // 4byte E-W Windowed Average
    float ZTMin ;     // 4byte U-D True Minimum
    float ZTMax ;     // 4byte U-D True Maximum
    float NTMin ;     // 4byte N-S True Minimum
    float NTMax ;     // 4byte N-S True Maximum
    float ETMin ;     // 4byte E-W True Minimum
    float ETMax ;     // 4byte E-W True Maximum
    float ZMax ;      // 4byte E-W Maximum
    float NMax ;      // 4byte N-S Maximum
    float EMax ;      // 4byte U-D Maximum
    float HPGA ;        // 4byte Horizontal PGA
    float TPGA ;        // 4byte Total PGA
    float ZSI ;       // 4byte U-D SI
    float NSI ;       // 4byte E-W SI
    float ESI ;       // 4byte N-S SI
    float HSI ;       // 4byte Horizontal SI
    float Correl ;    // 4byte Correlation
    char CN1;           // 1byte Channel Number 1
    char CN2;           // 1byte Channel Number 2
    char LO[2];     // 2byte Unused
} _QSCD_PACKET ;

typedef struct _qscd_for_multimap
{
    QString sta;
    QString net;
    float hpga;
} _QSCD_FOR_MULTIMAP;

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

static double getDistance(double lat1, double lon1, double lat2, double lon2)
{
    double dist, azim;
    int rtn = geo_to_km(lat1, lon1, lat2, lon2, &dist, &azim);
    return dist;
}

static     int     inten[NUM_LEGEND] = {    1,    2,      3,     4,     5,      6,      7,      8,      9,     10 };
static QString intenText[NUM_LEGEND] = {  "I",  "II", "III",  "IV",   "V",   "VI",  "VII", "VIII",   "IX",    "X" };
static  double  pgaValue[NUM_LEGEND] = { 0.97, 2.94,   4.90, 23.52, 65.66, 127.40, 235.20, 431.20, 813.40, 999999 };
static     int  pgaWidth[NUM_LEGEND] = {   12,   13,     14,    16,    18,     20,     20,     20,     20,     20 };
static  QColor  pgaColor[NUM_LEGEND] = { QColor(215, 236, 255), QColor(165, 221, 249), QColor(146, 208,  80), QColor(255, 255,   0), QColor(255, 192,   0),
                                       QColor(255,   0,   0), QColor(163,  39, 119), QColor( 99,  37,  35), QColor( 76,  38,   0), QColor(  0,   0,   0) };

static int getLegendIndex(double value)
{
    int legendIndex;

    double gal = myRound(value, 2);

    if( gal <= pgaValue[0] ) legendIndex = 0; // min
    else if( gal > pgaValue[8] ) legendIndex = 9; // max
    else
    {
        for(int i=1;i<9;i++) // 1 ~ 8
        {
            if(gal <= pgaValue[i])
            {
                legendIndex = i;
                break;
            }
        }
    }

    return legendIndex;
}

static unsigned int GetBits(unsigned x, int p, int n) { return (x >> (p-n+1)) & ~(~0 << n) ; }

static void SwapFloat(float *data)
{
    char temp;

    union {
        char c[4];
    } dat;

    memcpy( &dat, data, sizeof(float) );
    temp     = dat.c[0];
    dat.c[0] = dat.c[3];
    dat.c[3] = temp;
    temp     = dat.c[1];
    dat.c[1] = dat.c[2];
    dat.c[2] = temp;
    memcpy( data, &dat, sizeof(float) );
    return;
}

static void SwapInt(int *data)
{
    char temp;

    union {
        char c[4];
    } dat;

    memcpy( &dat, data, sizeof(int) );
    temp     = dat.c[0];
    dat.c[0] = dat.c[3];
    dat.c[3] = temp;
    temp     = dat.c[1];
    dat.c[1] = dat.c[2];
    dat.c[2] = temp;
    memcpy( data, &dat, sizeof(int) );
    return;
}

static QMultiMap<int, _QSCD_FOR_MULTIMAP> convertMMap(char *msg, QVector<_STATION> myStaVT)
{
    QMultiMap<int, _QSCD_FOR_MULTIMAP> mmap;
    _QSCD_PACKET QSCDBlock[MAX_QSCD_CNT];
    memcpy(&QSCDBlock[0], msg, sizeof(QSCDBlock));
    char sta[6];
    char net[3];
    memset(sta, 0x00, 5);
    memset(net, 0x00, 2);

    for(int i=0;i<MAX_QSCD_CNT;i++)
    {
        strncpy(sta, QSCDBlock[i].SSSSS, 5);
        strncpy(net, QSCDBlock[i].LO, 2);

        bool isMyStation = false;
        QString sc, sc2;

        sc2 = QString(sta) + "/" + QString(net).left(2);

        for(int j=0;j<myStaVT.count();j++)
        {
            sc = myStaVT.at(j).sta + "/" + myStaVT.at(j).net;

            if(sc.startsWith(sc2))
            {
                isMyStation = true;
                break;
            }
        }

        if(isMyStation == true)
        {
            _QSCD_FOR_MULTIMAP qfmm;
            qfmm.sta = QString(sta);
            qfmm.net = QString(net).left(2);
            SwapInt(&QSCDBlock[i].time);    // epoch time (GMT)
            SwapFloat(&QSCDBlock[i].HPGA);
            qfmm.hpga = QSCDBlock[i].HPGA;
            mmap.insert(QSCDBlock[i].time, qfmm);
        }
    }

    return mmap;
}



#endif // COMMON_H
