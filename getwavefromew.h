#ifndef GETWAVEFROMEW_H
#define GETWAVEFROMEW_H

#define sema_t int
#define mutex_t pthread_mutex_t
#define SOCKET int
#define thr_ret void

#define NLOGO 5
#define MAX_LEN_STR_SCNL 30
#define MAX_NUM_SCNL 6
#define TYPE_NOTUSED 254

#include <QThread>
#include <QString>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

extern "C" {
//#include <platform.h>
#include <transport.h>
#include <swap.h>
#include <time_ew.h>
#include <trace_buf.h>
#include <earthworm.h>
#include <libmseed.h>
}

typedef struct
{
    char scnl[MAX_LEN_STR_SCNL];
    double last_scnl_time;
    double last_packet_time;
} SCNL_LAST_SCNL_PACKET_TIME;


class GetWaveFromEW : public QThread
{
public:
    GetWaveFromEW(QString sta, QString chan, QString net, QString loc, QString ringName, QWidget *parent = 0);
    ~GetWaveFromEW();

private:
    int IsWild( char *str );
    int search_scnl_time(SCNL_LAST_SCNL_PACKET_TIME *scnl_time, int *n_scnl_time,
                         char *sta, char *chan, char *net, char *loc);
    int check_for_gap_overlap_and_output_message(TRACE2_HEADER  *trh, SCNL_LAST_SCNL_PACKET_TIME *scnl_time,
                                                 int *n_scnl_time);
    void print_data_feed_latencies(TRACE2_HEADER *trh, SCNL_LAST_SCNL_PACKET_TIME *scnl_time, int *n_scnl_time);

};

#endif // GETWAVEFROMEW_H
