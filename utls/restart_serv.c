/*
 * restart_srv is a tuxedo client that restart server using MIB calls
 *
 * compilation :
 * buildclient -v -o restart_serv -f restart_serv.c
 *
 * environnement
 * FLDTBLDIR=$TUXDIR/udataobj
 * FIELDTBLS=tpadm,evt_mib
 *
 * Return codes
 *      -3  CRITICAL ERROR when restarting servers
 *      -2  ERROR when creating list of servers
 *      -1  Context error
 *       0  Nothing restarted
 *      >0  Number of restarted servers
 */
 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <atmi.h>
#include <fml32.h>
#include <tpadm.h>
#include <evt_mib.h>
#include <libgen.h>

#define MAX_INST   300
#define ERR_GENE    -1
#define ERR_GET     -2
#define ERR_SET     -3

#ifdef WIN32
#define PATH_DELIM '\\'
#else
#define PATH_DELIM '/'
#endif

struct oneInst  {
    char grp[30];
    char srvn[32];
    long id;
    long nbd;
};
struct oneInst myInst[MAX_INST];
TPCONTEXT_T  ctxt;

void print_usage( char *pgm )
{
    fprintf(stdout, "usage : %s [-P ubb_pwd] [OPTIONS] [ADD]\n", pgm);
    fprintf(stdout, "This Tuxedo client calls the TMIB to restart server instances.\n");
    fprintf(stdout, "When several to restart, they are restarted one by one.\n");
    fprintf(stdout, "  primary OPTIONS :\n");
    fprintf(stdout, "      -g groupe_name\n");
    fprintf(stdout, "      -i server instance\n");
    fprintf(stdout, "      -s servername\n");
    fprintf(stdout, "      -o sequence number\n");
    fprintf(stdout, "  ADDitional options :\n");
    fprintf(stdout, "      -N number of request done\n");  
    fprintf(stdout, "      -C   Check only servers will not be restarted\n");
    fprintf(stdout, "  At least one primary option is required.\n");
    fprintf(stdout, "  Examples:\n");
    fprintf(stdout, "    %s -g GRP1 -s ARTIMPP_UDB \n", pgm);
    fprintf(stdout, "     >> restart servers ARTIMMP from group GRP1\n", pgm);
    fprintf(stdout, "    %s -s ARTIBMP_UDB -N 100 \n", pgm);
    fprintf(stdout, "     >> restart servers ARTIBMP that have been called at least 100 times\n\n", pgm);
}

int getServers (char *groupe, char *srvname, long srvid, long seqid, long nbd)
{
    char  ope[8];    /* GET or GETNEXT */
    char  tmp[72];   /* SERVERNAME is full path in */
    char  *Pch;
    long  occ, mor, nb;
    long  flags, len;
    int   i, cur;
    FBFR32 *bget;
	
    occ = mor = nb = cur = 0;

    if (tpsetctxt (ctxt, 0) < 0) {
        fprintf(stderr, "Tuxedo cannot set context -GET- %s\n",
                      tpstrerror(tperrno));
        return (0);
    }

    bget = (FBFR32 *)tpalloc("FML32", NULL, Fneeded32(6, 1024));
    if (bget == NULL) {
        fprintf(stderr, "Tuxedo allocation FML32 for GET fails : <%d> %s\n", 
                      tperrno, tpstrerror(tperrno));
        return (-1);
    }
	
    /* Init before first request */
    if (Finit32(bget, Fneeded32(6, 1024)) == -1) {
       fprintf(stderr, "Error when INIT FML32 GET buffer : <%d> %s\n",
                     Ferror32, Fstrerror32(Ferror32));
       tpfree ((char *) bget);
       return (-1);
    }  

    do {
        /* get all the active servers based on criteria provided */
        if (nb == 0)
            strcpy (ope, "GET");
        else
            strcpy (ope, "GETNEXT");
	
        if (Fchg32(bget, TA_OPERATION, 0, ope, 0) == -1) {
            fprintf(stderr, "Error when Set Field TA_OPERATION : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bget);
            return (-1);
        }
        if (Fchg32(bget, TA_CLASS, 0, "T_SERVER", 0) == -1) {
            fprintf(stderr, "Error when Set Field TA_CLASS : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bget);
            return (-1);
        }    
        /* Only the ACTIVE servers, T_SERVER contains all .. MAX in UBB */
        if (Fchg32(bget, TA_STATE, 0, "ACT", 0) == -1) {
            fprintf(stderr, "Error when Set Field TA_STATE : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bget);
            return (-1);
        }
        if (*groupe != '\0') {
            if (Fchg32(bget, TA_SRVGRP, 0, groupe, 0) == -1) {
                fprintf(stderr, "Error when Set Field TA_SRVGRP : <%d> %s\n",
                              Ferror32, Fstrerror32(Ferror32));
                tpfree ((char *) bget);
                return (-1);
            }
        }
        /* Set fields based on options used */
        if (*srvname != '\0') {
            if (Fchg32(bget, TA_SERVERNAME, 0, srvname, 0) == -1) {
                fprintf(stderr, "Error when Set Field TA_SERVERNAME : <%d> %s\n",
                              Ferror32, Fstrerror32(Ferror32));
                tpfree ((char *) bget);
                return (-1);
            }	   
        }
        if (srvid != 0) {
            len = sizeof (srvid);
            if (Fchg32(bget, TA_SRVID, 0, (char *)&srvid , len) == -1) {
                fprintf(stderr, "Error when Set Field TA_SRVID : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
                tpfree ((char *) bget);
                return (-1);
            } 
        }
        if (seqid != 0) {
            len = sizeof (seqid);
            if (Fchg32(bget, TA_SEQUENCE, 0, (char *)&seqid , len) == -1) {
                fprintf(stderr, "Error when Set Field TA_SEQUENCE : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
                tpfree ((char *) bget);
                return (-1);
            } 
        }
	
        if (nbd) {      /* Need MIB_LOCAL to get metrics  */
            flags = MIB_LOCAL;
            len = sizeof (flags);
            if (Fchg32(bget, TA_FLAGS, 0, (char *)&flags , len) == -1) {
                fprintf(stderr, "Error when Set Field TA_FLAGS : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
                tpfree ((char *) bget);
                return (-1);
            } 
        }
	
	flags = TPSIGRSTRT;
        if (tpcall(".TMIB", (char *)bget, 0, (char **)&bget, &len, flags) == -1) {
            fprintf(stderr, "Error when GET data from TMIB : <%d> %s\n", 
                          tperrno, tpstrerror(tperrno));
            tpfree ((char *) bget);
            return (-1);
        }

        Fprint32( bget);

        len = sizeof (occ);
        if (Fget32(bget, TA_OCCURS, 0, &occ, &len) == -1) {
            fprintf (stderr, "No Field TA_OCCURS : <%d> %s\n", 
                           Ferror32, Fstrerror32(Ferror32));
            fprintf (stdout, " Content of FML32 buffer received \n");
            Fprint32(bget);
            tpfree ((char *) bget);
            return (-1);
        }
		
        if (occ) {
            for (i= 0; i < occ; i++) {
                len = 30;
                cur= nb+i;

                /* Get the SRVGRP & SRVID  for all of them */
                if (Fget32(bget, TA_SRVGRP, i, myInst[cur].grp, &len) == -1) {
                  fprintf (stderr, "Loop %d: fails get TA_SRVGRP : <%d> %s\n", 
                              cur, Ferror32, Fstrerror32(Ferror32));
                  tpfree ((char *) bget);
                  return (-1);
               }	
               len = 32;
               if (Fget32(bget, TA_SERVERNAME, i, tmp, &len) == -1) {
                  fprintf (stderr, "Loop %d: fails get TA_SERVERNAME : <%d> %\n", 
                                 cur, Ferror32, Fstrerror32(Ferror32));
                  tpfree ((char *) bget);
                  return (-1);
               }
               /*  TA_SERVERNAME contains PGM with full path  */
               strcpy (myInst[cur].srvn, "");
               Pch = (char *) strrchr (tmp, PATH_DELIM);
               if (Pch == NULL)
                  strcat (myInst[cur].srvn, tmp);
               else
                  strcat (myInst[cur].srvn, ++Pch);		
               len = sizeof (myInst[cur].id);
               if (Fget32(bget, TA_SRVID, i, &myInst[cur].id, &len) == -1) {
                  fprintf (stderr, "Loop %d: fails get TA_SRVID : <%d> %s\n",
                                 cur, Ferror32, Fstrerror32(Ferror32));
                  tpfree ((char *) bget);
                  return(-1);
               }
               fprintf (stdout, "Loop %d [%s] [%s] [%d]\n",
                        cur, myInst[cur].grp, myInst[cur].srvn, myInst[cur].id);
           } /* for */
        }
        nb += occ;
		
        len = sizeof (mor);
        if (Fget32(bget, TA_MORE, 0, &mor, &len) == -1) {
            fprintf (stderr, "No Field TA_MORE : <%d> %s\n", 
                           Ferror32, Fstrerror32(Ferror32));
            fprintf (stdout, " Content of FML32 buffer received \n");
            Fprint32(bget);
            tpfree ((char *) bget);
            return (-1);
        }
		
    } while (mor);

    tpfree ((char *) bget);
    /* Return number of servers found */
    return (nb);
}

int restartServer ( char *pgm , long nb)
{
    long  flags, len;
    int   i, done;
    FBFR32 *bset;

    if (tpsetctxt (ctxt, 0) < 0) {
        fprintf(stderr, "Tuxedo cannot set context -SET- %s\n",
                      tpstrerror(tperrno));
        return (0);
    }
	
    /* Allocate buffer to update server state  */
    if ((bset = (FBFR32 *)tpalloc("FML32", NULL, Fneeded32(10, 1024))) == NULL){ 
        fprintf(stderr, "Tuxedo allocation FML32 for SET fails : <%d> %s\n", 
                      Ferror32, Fstrerror32(Ferror32));
        return(-1);
    }

    done = 0;
	
    /* Stop and start each instance get from Tuxedo MIB */
    for (i=0; i < nb; i++) {
        flags=0;
        fprintf(stdout, "%s CMD tmshutdown -g %s  -i %d\n", 
                      pgm, myInst[i].grp, myInst[i].id);
        Finit32(bset, Fneeded32(10, 1024));
        if (Finit32(bset, Fneeded32(10, 1024)) == -1) {
            fprintf(stderr, "Error when INIT FML32 SET buffer -INA- : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            if (i)
                return (i);
            else 
                return (-1);
        } 
        if (Fchg32(bset, TA_OPERATION, 0, "SET", 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_OPERATION : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        if (Fchg32(bset, TA_CLASS, 0, "T_SERVER", 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_CLASS : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        if (Fchg32(bset, TA_SRVGRP, 0, myInst[i].grp , 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_SRVGRP : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        if (Fchg32(bset, TA_SRVID, 0, (char *)&myInst[i].id, 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_SRVID : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        if (Fchg32(bset, TA_STATE, 0, "INA", 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_STATE : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        
        if (tpcall(".TMIB", (char *)bset, 0, (char **)&bset, &len, flags) == -1) {
            fprintf(stderr, "loop %d: fails to call Tuxedo for INA: <%d> %s\n",
                          i, tperrno, tpstrerror(tperrno));
            tpfree ((char *) bset);
            if (i)
               return (i);
            else 
               return (-1);
        }

        fprintf(stdout, "%s CMD = tmboot -g %s  -i %d\n", 
                      pgm, myInst[i].grp, myInst[i].id);
        Finit32(bset, Fneeded32(10, 1024));
        if (Finit32(bset, Fneeded32(10, 1024)) == -1) {
            fprintf(stderr, "Error when INIT FML32 SET buffer -ACT- : <%d> %s\n",
                          Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            if (i)
                return (i);
            else 
                return (-1);
        } 
        if (Fchg32(bset, TA_OPERATION, 0, "SET", 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_OPERATION : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        if (Fchg32(bset, TA_CLASS, 0, "T_SERVER", 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_CLASS : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        if (Fchg32(bset, TA_SRVGRP, 0, myInst[i].grp , 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_SRVGRP : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }
        if (Fchg32(bset, TA_SRVID, 0, (char *)&myInst[i].id, 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_SRVID : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        } 
        if (Fchg32(bset, TA_SERVERNAME, 0, myInst[i].srvn, 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_SERVERNAME : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        } 
        if (Fchg32(bset, TA_STATE, 0, "ACT", 0) == -1) {
            fprintf (stderr, "Loop %d: fails chg TA_STATE : <%d> %s\n",
                           i, Ferror32, Fstrerror32(Ferror32));
            tpfree ((char *) bset);
            return (i);
        }

        if (tpcall(".TMIB", (char *)bset, 0, (char **)&bset, &len, flags) == -1) {
            fprintf(stderr, "loop %d: fails to call Tuxedo for ACT: <%d> %s\n", 
                          i, tperrno, tpstrerror(tperrno));
            tpfree ((char *) bset);
            if (i)
               return (i);
            else 
               return (-1);
        }
        done++;
    }

    return (done);
}

int main(int argc, char *argv[])
{
    char  groupe[30], srvname[32], pwd[32];
    char  *pgm;
    long  srvid, seqid, nbd;
    long  nbfound, nbresta;
    int   opt, test, ok, err;
    TPINIT *tpinfo;
    extern char  *optarg;
    extern int   optind, opterr;   /* used by getopt line parser */

    /* pgm = basename ( argv[0] ); */
    pgm = (char *) strrchr (argv[0], PATH_DELIM);
    if (pgm == NULL)
        pgm = argv[0];
    else
        pgm++;		

    if (argc < 2) {
        print_usage( pgm );
        exit(ERR_GENE); /* EXIT_FAILURE | EXIT_SUCCESS */
    }

    memset (groupe, '\0', sizeof (groupe));
    memset (srvname, '\0', sizeof (srvname));
    memset (pwd, '\0', sizeof (pwd));
    srvid = seqid = nbd = 0;
    nbfound = nbresta = 0;
    test  = ok = err = 0;
    opt = '\0';
	
    while ((opt = getopt(argc, argv, "g:i:o:s:N:P:C")) != -1) {
       switch(opt) {
         case 'g': strcpy (groupe, optarg); 
                 fprintf (stdout, ">> get servers in GROUPE %s\n", groupe);
                 ok++;
                  break;
         case 'i': srvid = atoi (optarg);
                 fprintf (stdout, ">> get servers which SRVID is %d\n", srvid);
                 ok++;
                 break;
         case 'o': seqid = atoi (optarg);
                 fprintf (stdout, ">> get servers with SEQUENCE %d\n", seqid);
                 ok++;
                 break;
         case 's': strcpy (srvname, optarg);
                 fprintf (stdout, ">> get servers which name is %s\n", srvname);
                 ok++;
                 break;
         case 'N': nbd = atoi (optarg);
                 if (nbd < 1)
                    fprintf (stdout, " N lower than 1 is not relevant\n");
                 else 
                    fprintf (stdout, ">> only servers at least called %d times\n", nbd);
                 break;
         case 'P': strcpy (pwd, optarg);
                 break;
         case 'C': test = 1;
                 fprintf (stdout, ">> get servers only - NO RESTART TO DO\n", seqid);
                 break;
         default : fprintf (stderr, "Unknown option ... EXITING \n");
                  err=1;
       } /* end switch */
    }

    if ((!ok) || (err)) {
        print_usage( pgm );
        exit(ERR_GENE);
    } 

    if((tpinfo = (TPINIT *) tpalloc("TPINIT", NULL, TPINITNEED(0))) == NULL) {
        fprintf(stderr, "Fails tpalloc TPINIT : <%d> %s\n", 
                      tperrno, tpstrerror(tperrno));
        exit(ERR_GENE);
    }

    if (pwd[0] != '\0') {
       strcpy(tpinfo->passwd, pwd);
    }
    strcpy(tpinfo->usrname, "tpsysadm"); 
    strcpy(tpinfo->cltname, "tpsysadm");
    tpinfo->flags=TPMULTICONTEXTS;

    if (tpinit(tpinfo) == -1) {
        fprintf(stderr, "Tuxedo init fails : <%d> %s\n", 
                      tperrno, tpstrerror(tperrno));
        tpfree ((char *) tpinfo);
        exit(ERR_GENE);
    }

    if (tpgetctxt(&ctxt, 0) < 0) {
        fprintf(stderr, "Tuxedo can't retreive context: %s\n",
                      tpstrerror(tperrno));
        tpfree ((char *) tpinfo);
        exit(ERR_GENE);
    }

    fprintf (stdout, "INF groupe %s, servname %s, id %d, seq %d, nb %d, test %d\n", groupe, srvname, srvid, seqid, nbd, test);

    nbfound = getServers (groupe, srvname, srvid, seqid , nbd);
    if (nbfound < 0 ) {
        fprintf (stderr, "ERR: when get servers, restart not initiated !!\n");
        tpfree ((char *)tpinfo);
        tpterm ();
        return(ERR_GET);
    }

    if (!nbfound){
        fprintf (stdout, "No server found with your criteria, check UBB\n");
    }else{
        fprintf (stdout, "With your criteria %d server(s) will be restarted\n", nbfound);
    }
	
    /* Don't do the set when test */
    if (test)
        exit (0);

    nbresta = restartServer ( pgm , nbfound);
    if (nbresta < 0) {
       fprintf (stderr, "ERR:  !!\n");
       tpfree ((char *)tpinfo);
       tpterm ();
       return(ERR_SET);
    }

    tpfree ((char *)tpinfo);
    tpterm();
    exit(nbresta);
}

