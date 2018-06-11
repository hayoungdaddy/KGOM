#include "getwavefromew.h"

GetWaveFromEW::GetWaveFromEW(QString sta, QString chan, QString net, QString loc, QString ringName, QWidget *parent)
{
    SHM_INFO        region;
    long            RingKey;
    MSG_LOGO        getlogo[NLOGO], logo;
    long            gotsize;
    char            msg[MAX_TRACEBUF_SIZ];
    char            *getSta, *getComp, *getNet, *getLoc, *inRing;
    char            wildSta, wildComp, wildNet, wildLoc;
    unsigned char   Type_Mseed;
    unsigned char   Type_TraceBuf, Type_TraceBuf2;
    unsigned char   Type_TraceComp, Type_TraceComp2;
    unsigned char   InstWildcard, ModWildcard;
    short           *short_data;
    int             *long_data;
    TRACE2_HEADER   *trh;
    char            orig_datatype[3];
    char            stime[256];
    char            etime[256];
    int             dataflag;
    int             i;
    int             rc;
    int             verbose = 0;
    int             nLogo = NLOGO;
    static unsigned char InstId;
    char            ModName[MAX_MOD_STR];
    char            *modid_name;
    unsigned char   sequence_number = 0;
    int             statistics_flag;
    time_t          monitor_start_time = 0;
    double          start_time, end_time;
    int             seconds_to_live;
    unsigned long   packet_total=0;
    unsigned long   packet_total_size=0;
    SCNL_LAST_SCNL_PACKET_TIME scnl_time[MAX_NUM_SCNL];
    int n_scnl_time = 0;
    MSRecord *msr = NULL;

    seconds_to_live = 0;
    dataflag = 0;
    statistics_flag = 0;

    trh  = (TRACE2_HEADER *) msg;
    long_data  =  (int *)( msg + sizeof(TRACE2_HEADER) );
    short_data = (short *)( msg + sizeof(TRACE2_HEADER) );

    verbose = 1;
    dataflag = 1;

    strcpy(inRing, ringName.toLatin1().constData());

    getSta = getComp = getNet = getLoc = "";
    wildSta = wildComp = wildNet = wildLoc = 1;

    strcpy(getSta, sta.toLatin1().constData());
    wildSta  = IsWild(getSta);
    strcpy(getComp, chan.toLatin1().constData());
    wildComp = IsWild(getComp);
    strcpy(getNet, net.toLatin1().constData());
    wildNet = IsWild(getNet);
    strcpy(getLoc, loc.toLatin1().constData());
    wildLoc = IsWild(getLoc);

    if ((RingKey = GetKey( inRing )) == -1 )
    {
        /* log->write
        fprintf( stderr, "Invalid RingName; exiting!\n" );
        exit( -1 );
        */

        return;
    }
    tport_attach( &region, RingKey );

    /* Look up local installation id
    *****************************/
    if ( GetLocalInst( &InstId ) != 0 )
    {
        /*
        fprintf(stderr, "sniffwave: error getting local installation id; exiting!\n" );
        exit( -1 );
        */
        return;
    }

    /* Specify logos to get
    ***********************/
    if ( GetType( "TYPE_MSEED", &Type_Mseed ) != 0 )
    {
        //fprintf(stderr, "%s: WARNING: Invalid message type <TYPE_MSEED>! Missing from earthworm.d or earthworm_global.d\n", argv[0] );
        Type_Mseed = TYPE_NOTUSED;
    }
    if ( GetType( "TYPE_TRACEBUF", &Type_TraceBuf ) != 0 )
    {
        //fprintf(stderr, "%s: Invalid message type <TYPE_TRACEBUF>! Missing from earthworm.d or earthworm_global.d\n", argv[0] );
        exit( -1 );
        return;
    }
    if ( GetType( "TYPE_TRACE_COMP_UA", &Type_TraceComp ) != 0 )
    {
        //fprintf(stderr, "%s: Invalid message type <TYPE_TRACE_COMP_UA>! Missing from earthworm.d or earthworm_global.d\n", argv[0] );
        exit( -1 );
        return;
    }
    if ( GetType( "TYPE_TRACEBUF2", &Type_TraceBuf2 ) != 0 )
    {
        //fprintf(stderr, "%s: Invalid message type <TYPE_TRACEBUF2>! Missing from earthworm.d or earthworm_global.d\n", argv[0] );
        exit( -1 );
        return;
    }
    if ( GetType( "TYPE_TRACE2_COMP_UA", &Type_TraceComp2 ) != 0 )
    {
        //fprintf(stderr,"%s: Invalid message type <TYPE_TRACE2_COMP_UA>! Missing from earthworm.d or earthworm_global.d\n", argv[0] );
        exit( -1 );
        return;
    }
    if ( GetModId( "MOD_WILDCARD", &ModWildcard ) != 0 )
    {
        //fprintf(stderr, "%s: Invalid moduleid <MOD_WILDCARD>! Missing from earthworm.d or earthworm_global.d\n", argv[0] );
        exit( -1 );
        return;
    }
    if ( GetInst( "INST_WILDCARD", &InstWildcard ) != 0 )
    {
        //fprintf(stderr, "%s: Invalid instid <INST_WILDCARD>! Missing from earthworm.d or earthworm_global.d\n", argv[0] );
        exit( -1 );
        return;
    }

    for( i=0; i<nLogo; i++ )
    {
        getlogo[i].instid = InstWildcard;
        getlogo[i].mod    = ModWildcard;
    }
    getlogo[0].type = Type_Mseed;
    getlogo[1].type = Type_TraceBuf;
    getlogo[2].type = Type_TraceComp;
    if (nLogo >= 4)
    {     /* if nLogo=5 then include TraceBuf2s */
        getlogo[3].type = Type_TraceBuf2;
        getlogo[4].type = Type_TraceComp2;
    }

    /* Flush the ring
    *****************/
    while ( tport_copyfrom( &region, getlogo, nLogo, &logo, &gotsize,
                            (char *)&msg, MAX_TRACEBUF_SIZ, &sequence_number ) != GET_NONE )
    {
        packet_total++;
        packet_total_size+=gotsize;
    }
    //fprintf( stderr, "sniffwave: inRing flushed %ld packets of %ld bytes total.\n", packet_total, packet_total_size);

    if (seconds_to_live > 0)
    {
        monitor_start_time = time(0);
        packet_total = packet_total_size = 0;
        start_time = 0.0;
    }






    while ( (!(seconds_to_live > 0) ||
             (time(0)-monitor_start_time < seconds_to_live)) &&
             tport_getflag( &region ) != TERMINATE )

    {
        rc = tport_copyfrom( &region, getlogo, nLogo,
                   &logo, &gotsize, msg, MAX_TRACEBUF_SIZ, &sequence_number );

        if ( rc == GET_NONE )
        {
          sleep_ew( 200 );
          continue;
        }

        if ( rc == GET_TOOBIG )
        {
          fprintf( stderr, "sniffwave: retrieved message too big (%ld) for msg\n",
            gotsize );
          continue;
        }

        if ( rc == GET_NOTRACK )
          fprintf( stderr, "sniffwave: Tracking error.\n");

        if ( rc == GET_MISS_LAPPED )
          fprintf( stderr, "sniffwave: Got lapped on the ring.\n");

        if ( rc == GET_MISS_SEQGAP )
          fprintf( stderr, "sniffwave: Gap in sequence numbers\n");

        if ( rc == GET_MISS )
            fprintf( stderr, "sniffwave: Missed messages\n");

        /* Check SCNL of the retrieved message */
        if (Type_Mseed != TYPE_NOTUSED && logo.type == Type_Mseed)
        {
            /* Unpack record header and not data samples */
            if ( msr_unpack (msg, gotsize, &msr, dataflag|statistics_flag, verbose) != MS_NOERROR)
            {
                fprintf (stderr, "Error parsing mseed record\n");
                continue;
            }

            /* Print record information */
            msr_print (msr, verbose);

            msr_free (&msr);
            continue;
        }

        if ( (wildSta  || (strcmp(getSta,trh->sta)  ==0)) &&
                 (wildComp || (strcmp(getComp,trh->chan)==0)) &&
                 (wildNet  || (strcmp(getNet,trh->net)  ==0)) &&
                 (((logo.type == Type_TraceBuf2 ||
                    logo.type == Type_TraceComp2) &&
                 (wildLoc  || (strcmp(getLoc,trh->loc) == 0))) ||
                 ( (logo.type == Type_TraceBuf ||
                     logo.type == Type_TraceComp))))
        {
            strcpy(orig_datatype, trh->datatype);
            /*
            if(WaveMsg2MakeLocal( trh ) < 0)
            {
                char scnl[20], dt[3];
                scnl[0] = 0;
                strcat( scnl, trh->sta);
                strcat( scnl, ".");
                strcat( scnl, trh->chan);
                strcat( scnl, ".");
                strcat( scnl, trh->net);
                strcat( scnl, ".");
                strcat( scnl, trh->loc);

                for ( i=0; i<15; i++ )
                {
                    if ( !isalnum(scnl[i]) && !ispunct(scnl[i]))
                        scnl[i] = ' ';
                }
                strncpy( dt, trh->datatype, 2 );
                for ( i=0; i<2; i++ )
                {
                    if ( !isalnum(dt[i]) && !ispunct(dt[i]))
                        dt[i] = ' ';
                }
                dt[i] = 0;
                fprintf(stderr, "WARNING: WaveMsg2MakeLocal rejected tracebuf.  Discard (%s).\n", scnl );
                fprintf(stderr, "\tdatatype=[%s]\n", dt);
                continue;
            }
            */

            if (seconds_to_live > 0)
            {
                if (start_time == 0.0)
                    start_time = trh->starttime;
                end_time = trh->endtime;
                packet_total++;
                packet_total_size += gotsize;
            }

            /* TODO add a flag to output gap message or not */
            /* Check for gap and in case output message */
            check_for_gap_overlap_and_output_message(trh, scnl_time, &n_scnl_time);

            datestr23 (trh->starttime, stime, 256);
            datestr23 (trh->endtime,   etime, 256);

            if( logo.type == Type_TraceBuf2 || logo.type == Type_TraceComp2)
            {
                fprintf( stdout, "%5s.%s.%s.%s (0x%x 0x%x) ",
                           trh->sta, trh->chan, trh->net, trh->loc, trh->version[0], trh->version[1] );
            }
            else
            {
                fprintf( stdout, "%5s.%s.%s ", trh->sta, trh->chan, trh->net );
            }

            fprintf( stdout, "%d %s %3d ", trh->pinno, orig_datatype, trh->nsamp);

            if (trh->samprate < 1.0)
            { /* more decimal places for slower sample rates */
                fprintf( stdout, "%6.4f", trh->samprate);
            }
            else
            {
                fprintf( stdout, "%5.1f", trh->samprate);
            }
            fprintf( stdout, " %s (%.4f) %s (%.4f) ", stime, trh->starttime, etime, trh->endtime);

            switch (trh->version[1])
            {
                case TRACE2_VERSION1:
                    //fprintf( stdout, "0x%02x 0x%02x ", CAST_QUALITY(trh->quality[0]), CAST_QUALITY(trh->quality[1]) );
                break;
                case TRACE2_VERSION11:
                    fprintf( stdout, "%6.4f ",
                                 ((TRACE2X_HEADER *)trh)->x.v21.conversion_factor);
                break;
            }
            if (verbose)
            {
                if (InstId == logo.instid)
                {
                    modid_name = GetModIdName(logo.mod);
                    if (modid_name != NULL)
                        sprintf(ModName, "%s", modid_name);
                    else
                        sprintf(ModName, "UnknownLocalMod");
                }
                else
                {
                    sprintf( ModName, "UnknownRemoteMod");
                }
                fprintf( stdout, "i%d=%s m%d=%s t%d=%s len%4ld",
                         (int)logo.instid, GetInstName(logo.instid), (int)logo.mod,
                         ModName, (int)logo.type, GetTypeName(logo.type), gotsize );
            }
            else
            {
                fprintf( stdout, "i%d m%d t%d len%4ld", (int)logo.instid, (int)logo.mod, (int)logo.type, gotsize );
            }

            /* Compute and print latency for data and packet */
            print_data_feed_latencies(trh, scnl_time, &n_scnl_time);

            fprintf( stdout, "\n");
            fflush (stdout);

            if (dataflag == 1)
            {
                if( logo.type == Type_TraceBuf2 || logo.type == Type_TraceBuf )
                {
                    if( (strcmp (trh->datatype, "s2")==0) || (strcmp (trh->datatype, "i2")==0) )
                    {
                        for ( i = 0; i < trh->nsamp; i++ )
                        {
                            fprintf ( stdout, "%6hd ", *(short_data+i) );
                            if(i%10==9) fprintf ( stdout, "\n" );
                        }
                    }
                    else if ( (strcmp (trh->datatype, "s4")==0) || (strcmp (trh->datatype, "i4")==0) )
                    {
                        for ( i = 0; i < trh->nsamp; i++ )
                        {
                            fprintf ( stdout, "%6d ", *(long_data+i) );
                            if(i%10==9) fprintf ( stdout, "\n" );
                        }
                    }
                    else
                    {
                        fprintf (stdout, "Unknown datatype %s\n", trh->datatype);
                    }
                }
                else
                {
                    fprintf (stdout, "Data values compressed\n");
                }
                fprintf (stdout, "\n");
                fflush (stdout);
            }

            if (statistics_flag == 1)
            {
                if( logo.type == Type_TraceBuf2 || logo.type == Type_TraceBuf )
                {
                    long int max=0;
                    long int min=0;
                    double avg=0.0;
                    short short_value;
                    int long_value;

                    if ( (strcmp (trh->datatype, "s2")==0) || (strcmp (trh->datatype, "i2")==0) )
                    {
                        for ( i = 0; i < trh->nsamp; i++ )
                        {
                            short_value = *(short_data+i);
                            if (short_value > max || max == 0)
                            {
                                max = short_value;
                            }
                            if (short_value < min || min == 0)
                            {
                                min = short_value;
                            }
                            avg += short_value;
                        }
                        avg = avg / trh->nsamp;
                        fprintf(stdout, "Raw Data statistics max=%ld min=%ld avg=%f\n", max, min, avg);
                        fprintf(stdout, "DC corrected statistics max=%f min=%f spread=%ld\n\n",
                                    (double)(max - avg), (double)(min - avg), labs(max - min));
                    }
                    else if ( (strcmp (trh->datatype, "s4")==0) || (strcmp (trh->datatype, "i4")==0) )
                    {
                        for ( i = 0; i < trh->nsamp; i++ )
                        {
                            long_value = *(long_data+i);
                            if (long_value > max || max == 0)
                            {
                                max=long_value;
                            }
                            if (long_value < min || min == 0)
                            {
                                min=long_value;
                            }
                            avg += long_value;
                        }
                        avg = avg / trh->nsamp;
                        fprintf(stdout, "Raw Data statistics max=%ld min=%ld avg=%f\n", max, min, avg);
                        fprintf(stdout, "DC corrected statistics max=%f min=%f spread=%ld\n\n",
                                   (double)(max - avg), (double)(min - avg),
                                    labs(max - min));
                    }
                    else
                    {
                        fprintf (stdout, "Unknown datatype %s\n", trh->datatype);
                    }
                }
                else
                {
                    fprintf (stdout, "Data values compressed\n");
                }
                fflush (stdout);
            } /* end of statistics_flag if */
        } /* end of process this tracebuf if */
    } /* end of while loop */

    if (seconds_to_live > 0 && start_time > 0.0)
    {
        datestr23 (start_time, stime, 256);
        datestr23 (end_time,   etime, 256);
        //fprintf(stdout, "Sniffed %s for %d seconds:\n", argv[1], seconds_to_live);
        fprintf(stdout, "\tStart Time of first packet:  %s\n", stime);
        fprintf(stdout, "\t   End Time of last packet:  %s\n", etime);
        fprintf(stdout, "\t           Seconds of data:  %f\n", end_time - start_time);
        fprintf(stdout, "\t             Bytes of data:  %ld\n", packet_total_size);
        fprintf(stdout, "\t Number of Packets of data:  %ld\n", packet_total);
    }

    if (seconds_to_live > 0 && start_time == 0.0)
    {
        //fprintf(stdout, "Sniffed %s for %d seconds and found no packets matching desired SCN[L] filter.\n", argv[1], seconds_to_live);
    }

    exit (0);
    return;
}

GetWaveFromEW::~GetWaveFromEW()
{
}

int GetWaveFromEW::IsWild( char *str )
{
    if( (strcmp(str,"wild")==0)  ) return 1;
    if( (strcmp(str,"WILD")==0)  ) return 1;
    if( (strcmp(str,"*")   ==0)  ) return 1;
    return 0;
}

int GetWaveFromEW::search_scnl_time(SCNL_LAST_SCNL_PACKET_TIME *scnl_time, int *n_scnl_time,
                                    char *sta, char *chan, char *net, char *loc)
{
    char str_SCNL[MAX_LEN_STR_SCNL];
    int i = 0;
    int found = 0;

    /* initialize a SCNL string from current packet */
    snprintf(str_SCNL, MAX_LEN_STR_SCNL, "%s.%s.%s.%s", sta, chan, net, loc);

    /* search for SCNL string in scnl array */
    i = 0;
    while(i < *n_scnl_time &&  !found)
    {
        if(strcmp(str_SCNL, scnl_time[i].scnl) == 0)
        {
            found = 1;
        }
        else
        {
            i++;
        }
    }

    if(!found)
    {
        /* just add if space is available */
        if(*n_scnl_time < MAX_NUM_SCNL-1)
        {
            strncpy(scnl_time[*n_scnl_time].scnl, str_SCNL, MAX_LEN_STR_SCNL);
            scnl_time[*n_scnl_time].last_scnl_time = -1.0;
            scnl_time[*n_scnl_time].last_packet_time = -1.0;
            (*n_scnl_time)++;
        }
        else
        {
            /* TODO error message */
            i = -1;
            fprintf(stderr, "sniffwave: error: number of channels for gaps and latencies exceeded MAX_NUM_SCNL (%d).\n                  SCNL %s will not be monitored.",
                    MAX_NUM_SCNL, str_SCNL);
        }
    }

    return i;
}

int GetWaveFromEW::check_for_gap_overlap_and_output_message(TRACE2_HEADER  *trh, SCNL_LAST_SCNL_PACKET_TIME *scnl_time,
                                                            int *n_scnl_time)
{
    int ret = 0;
    int i = 0;
    char st1[256], st2[256];
    double diff_time;
    double SCNL_TIME_DIFF_TOLLERANCE;
    double t1, t2;

    /* Get index for SCNL */
    i = search_scnl_time(scnl_time, n_scnl_time, trh->sta, trh->chan, trh->net, trh->loc);

    if(i < 0)
    {
        /* Error message has been printed by search_scnl_time() */
        ret = i;
    }
    else
    {
        /* Skips the check the first time for the current scnl */
        if(scnl_time[i].last_scnl_time > 0.0)
        {
            t1 = scnl_time[i].last_scnl_time;
            t2 = trh->starttime;

            /* compare to last scnl time */
            SCNL_TIME_DIFF_TOLLERANCE = 2.0 * (1.0 / (double) trh->samprate);
            diff_time =  t2 - t1;

            if(fabs(diff_time) > SCNL_TIME_DIFF_TOLLERANCE)
            {
                /* check for a gap or an overlap */
                if(diff_time > 0.0)
                {
                        /* it is a gap */
                }
                else
                {
                    t1 = trh->starttime;
                    /* it is an overlap */
                    if(scnl_time[i].last_scnl_time > (trh->endtime + (1.0 / (double) trh->samprate)))
                    {
                        t2 = trh->endtime + (1.0 / (double) trh->samprate);
                    }
                    else
                    {
                        t2 = scnl_time[i].last_scnl_time;
                    }
                    diff_time = t1 - t2;
                }

                /* Convert double epochs to string */
                datestr23 (t1, st1, 256);
                datestr23 (t2, st2, 256);

                /* output message for gap or overlap */
                fprintf(stdout, "%15s %8s of %6.2fs        %s (%.4f) %s (%.4f)\n",
                        scnl_time[i].scnl, (diff_time > 0.0) ? "gap" : "overlap", fabs(diff_time), st1, t1, st2, t2);

            }
        }
        /* Save last time for current scnl */
        scnl_time[i].last_scnl_time = trh->endtime + (1.0 / (double) trh->samprate);
    }
    return ret;
}

void GetWaveFromEW::print_data_feed_latencies(TRACE2_HEADER *trh, SCNL_LAST_SCNL_PACKET_TIME *scnl_time,
                                              int *n_scnl_time)
{
    double d_now;
    double d_latency_second;
    double d_packet_latency_second = 0.0;
    int i = 0;

    /* Get current time */
    hrtime_ew( &d_now );

    /* Compute latency */
    d_latency_second = d_now - trh->endtime;

    /* Get index for SCNL */
    i = search_scnl_time(scnl_time, n_scnl_time, trh->sta, trh->chan, trh->net, trh->loc);

    if(i < 0)
    {
        /* Error message has been printed by search_scnl_time() */
    }
    else
    {
        if(scnl_time[i].last_packet_time > 0.0)
        {
            /* Compute packet latency */
            d_packet_latency_second = d_now - scnl_time[i].last_packet_time;
        }
        scnl_time[i].last_packet_time = d_now;
    }

    /* Print Data and Feed latencies */
    //fprintf( stdout, " [D:%4.1fs F:%4.1fs]", d_latency_second, d_packet_latency_second);
}
