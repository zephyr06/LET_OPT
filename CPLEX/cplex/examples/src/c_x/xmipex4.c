/* --------------------------------------------------------------------------
 * File: xmipex4.c
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2007, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

/* xmipex4.c - Reading in and optimizing a problem using a callback
              to log or interrupt or CPXXsetterminate to interrupt */

/* To run this example, command line arguments are required.
      xmipex4   filename option
                       where option is one of
                          t to use the time-limit-gap callback
                          l to use the logging callback
                          a to use the aborter

   Example:
       xmipex4  mexample.mps l
*/

/* Bring in the CPLEX function declarations and the C library
   header file stdio.h with the following single include. */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string and character functions,
   math functions and malloc */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Define the structures used to communicate info to the callbacks */

struct timeliminfo {
    double timestart;
    double timelim;
    double acceptablegap;
    int    aborted;
};
typedef struct timeliminfo TIMELIMINFO, *TIMELIMINFOptr;

struct loginfo {
   double timestart;
   double dettimestart;
   double lastincumbent;
   CPXCNT lastlog;
   CPXDIM numcols;
};
typedef struct loginfo LOGINFO, *LOGINFOptr;


/* Include declarations for functions in this program */

static void
   free_and_null (char **ptr),
   usage (char *progname);

static int CPXPUBLIC
   timelimcallback (CPXCENVptr env, void *cbdata, int wherefrom,
                    void *cbhandle);

static int CPXPUBLIC
   logcallback (CPXCENVptr env, void *cbdata, int wherefrom,
                void *cbhandle);


int
main (int argc, char *argv[])
{
   int     uselogcallback = 0;
   LOGINFO myloginfo;

   int         usetimelimcallback = 0;
   TIMELIMINFO mytimeliminfo;

   int          useterminate = 0;
   volatile int terminator;

   CPXENVptr env = NULL;
   CPXLPptr  lp = NULL;
   int       solstat;
   int       status = 0;

   /* Check the command line arguments */

   if (( argc != 3 )                                         ||
       ( strchr ("lta", argv[2][0]) == NULL )  ) {
      usage (argv[0]);
      goto TERMINATE;
   }

   switch (argv[2][0]) {
      case 'l':
         uselogcallback = 1;
         break;
      case 't':
         usetimelimcallback = 1;
         break;
      case 'a':
         useterminate = 1;
         break;
      default:
         break;
   }

   /* Initialize the CPLEX environment */

   env = CPXXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXXgeterrorstring will produce the text of
      the error message.  Note that CPXXopenCPLEX produces no output,
      so the only way to see the cause of the error is to use
      CPXXgeterrorstring.  For other CPLEX routines, the errors will
      be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON.  */

   if ( env == NULL ) {
      char  errmsg[CPXMESSAGEBUFSIZE];
      fprintf (stderr, "Could not open CPLEX environment.\n");
      CPXXgeterrorstring (env, status, errmsg);
      fprintf (stderr, "%s", errmsg);
      goto TERMINATE;
   }

   /* Turn on output to the screen */

   status = CPXXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status ) {
      fprintf (stderr,
               "Failure to turn on screen indicator, error %d.\n", status);
      goto TERMINATE;
   }

   /* Create the problem, using the filename as the problem name */

   lp = CPXXcreateprob (env, &status, argv[1]);

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of
      failure, an error message will have been written to the error
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPXPARAM_ScreenOutput causes the error message to
      appear on stdout.  Note that most CPLEX routines return
      an error code to indicate the reason for failure.   */

   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }

   /* Now read the file, and copy the data into the created lp */

   status = CPXXreadcopyprob (env, lp, argv[1], NULL);
   if ( status ) {
      fprintf (stderr, "Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }


   if ( usetimelimcallback ) {
      double t;
      status = CPXXgettime (env, &t);
      if ( status ) {
         fprintf (stderr, "Failed to initialize timer.\n");
         goto TERMINATE;
      }
      mytimeliminfo.acceptablegap = 10.0;
      mytimeliminfo.aborted       = 0;
      mytimeliminfo.timestart     = t;
      mytimeliminfo.timelim       = 1.0;

      status = CPXXsetinfocallbackfunc (env, timelimcallback, &mytimeliminfo);
      if ( status ) {
         fprintf (stderr, "Failed to set time limit callback function.\n");
         goto TERMINATE;
      }
   }
   else if ( uselogcallback ) {
      /* Set overall node limit in case callback conditions are not met */
      status = CPXXsetcntparam (env, CPXPARAM_MIP_Limits_Nodes, 5000);
      if ( status ) goto TERMINATE;

      status = CPXXgettime (env, &myloginfo.timestart);
      if ( status ) {
         fprintf (stderr, "Failed to query time.\n");
         goto TERMINATE;
      }
      status = CPXXgetdettime (env, &myloginfo.dettimestart);
      if ( status ) {
         fprintf (stderr, "Failed to query deterministic time.\n");
         goto TERMINATE;
      }
      myloginfo.numcols       = CPXXgetnumcols (env, lp);
      myloginfo.lastincumbent = CPXXgetobjsen (env, lp) * 1e+35;
      myloginfo.lastlog       = -10000;
      status = CPXXsetinfocallbackfunc (env, logcallback, &myloginfo);
      if ( status ) {
         fprintf (stderr, "Failed to set logging callback function.\n");
         goto TERMINATE;
      }
      /* Turn off CPLEX logging */
      status = CPXXsetintparam (env, CPXPARAM_MIP_Display, 0);
      if ( status )  goto TERMINATE;
   }
   else if ( useterminate) {
      status = CPXXsetterminate (env, &terminator);
      if ( status ) {
         fprintf (stderr, "Failed to set terminator.\n");
         goto TERMINATE;
      }
      /* Typically, you would pass the terminator variable to
         another thread or pass it to an interrupt handler,
         and  monitor for some event to occur.  When it does,
         set terminator to a non-zero value.
 
         To illustrate its use without creating a thread or
         an interrupt handler, terminate immediately by setting
         terminator before the solve.
      */
      terminator = 1;
   }

   /* Optimize the problem and obtain solution. */

   status = CPXXmipopt (env, lp);

   if ( status ) {
      fprintf (stderr, "Failed to optimize MIP.\n");
      goto TERMINATE;
   }

   solstat = CPXXgetstat (env, lp);
   printf ("Solution status %d.\n", solstat);

TERMINATE:

   /* Free up the problem as allocated by CPXXcreateprob, if necessary */

   if ( lp != NULL ) {
      int xstatus = CPXXfreeprob (env, &lp);
      if ( xstatus ) {
         fprintf (stderr, "CPXXfreeprob failed, error code %d.\n", xstatus);
         status = xstatus;
      }
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      int xstatus = CPXXcloseCPLEX (&env);

      /* Note that CPXXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */

      if ( status ) {
         char  errmsg[CPXMESSAGEBUFSIZE];
         fprintf (stderr, "Could not close CPLEX environment.\n");
         CPXXgeterrorstring (env, status, errmsg);
         fprintf (stderr, "%s", errmsg);
         status = xstatus;
      }
   }
 
   return (status);

}  /* END main */


/* Spend at least timelim seconds on optimization, but once
   this limit is reached, quit as soon as the solution is acceptable */

static int CPXPUBLIC
timelimcallback (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle)
{
   int status = 0;

   TIMELIMINFOptr info = (TIMELIMINFOptr) cbhandle;
   int            hasincumbent = 0;

   status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_MIP_FEAS, &hasincumbent);
   if ( status )  goto TERMINATE;

   if ( !info->aborted  &&  hasincumbent ) {
      double gap;
      double timenow;

      status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                   CPX_CALLBACK_INFO_MIP_REL_GAP, &gap);
      if ( status )  goto TERMINATE;

      /* Turn the gap into a percentage */
      gap *= 100.0;

      status = CPXXgettime (env, &timenow);
      if ( status )  goto TERMINATE;

      if ( timenow - info->timestart > info->timelim  &&
           gap < info->acceptablegap                    ) {
         fprintf (stderr,
     "Good enough solution at time %.2fsec, gap = %g%%, quitting\n",
                  timenow - info->timestart, gap);

         /* callback may be called again during the clean up phase after the
            abort has been issued, so remember that abort processing has
            already occurred. */

         info->aborted = 1;

         /* nonzero status causes abort */

         status = 1;
      }
   }

TERMINATE:

   return (status);

} /* END timelimcallback */


/* Log new incumbents if they are at better than the old by a
   relative tolerance of 1e-5; also log progress info every
   100 nodes. */

static int CPXPUBLIC
logcallback (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle)
{
   int status = 0;

   LOGINFOptr info = (LOGINFOptr) cbhandle;
   int        hasincumbent = 0;
   int        newincumbent = 0;
   CPXCNT     nodecnt;
   CPXCNT     nodesleft;
   double     objval;
   double     bound;
   double     *x = NULL;


   status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODE_COUNT_LONG, &nodecnt);
   if ( status )  goto TERMINATE;

   status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODES_LEFT_LONG, &nodesleft);
   if ( status )  goto TERMINATE;

   status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_MIP_FEAS, &hasincumbent);
   if ( status )  goto TERMINATE;

   if ( hasincumbent ) {
      status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                   CPX_CALLBACK_INFO_BEST_INTEGER, &objval);
      if ( status )  goto TERMINATE;

      if ( fabs(info->lastincumbent - objval) > 1e-5*(1.0 + fabs(objval)) ) {
         newincumbent = 1;
         info->lastincumbent = objval;
      }
   }

   if ( nodecnt >= info->lastlog + 100  ||  newincumbent ) {
      double walltime;
      double dettime;

      status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                   CPX_CALLBACK_INFO_BEST_REMAINING, &bound);
      if ( status )  goto TERMINATE;

      if ( !newincumbent )  info->lastlog = nodecnt;

      status = CPXXgettime (env, &walltime);
      if ( status )  goto TERMINATE;

      status = CPXXgetdettime (env, &dettime);
      if ( status )  goto TERMINATE;

      printf ("Time = %.2f  Dettime = %.2f  Nodes = %lld(%lld)  "
              "Best objective = %g",
              walltime - info->timestart, dettime - info->dettimestart,
              nodecnt, nodesleft, bound);
      if ( hasincumbent )  printf ("  Incumbent objective = %g\n", objval);
      else                 printf ("\n");
   }

   if ( newincumbent ) {
      int j;
      CPXDIM numcols = info->numcols;

      x = malloc (numcols*sizeof(*x));
      if ( x == NULL ) {
         status = CPXERR_NO_MEMORY;
         goto TERMINATE;
      }
      status = CPXXgetcallbackincumbent (env, cbdata, wherefrom,
                                        x, 0, numcols-1);
      if ( status )  goto TERMINATE;

      printf ("New incumbent values:\n");
      for (j = 0; j < numcols; j++) {
         if ( fabs(x[j]) > 1e-6 ) {
            printf ("  Column %d:  %g\n", j, x[j]);
         }
      }
   }

TERMINATE:

   free_and_null ((char **) &x);
   return (status);

} /* END logcallback */


/* This simple routine frees up the pointer *ptr, and sets *ptr to NULL */

static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */


static void
usage (char *progname)
{
   fprintf (stderr, "Usage: %s filename option\n", progname);
   fprintf (stderr, "   where option is one of \n");
   fprintf (stderr, "      t to use the time-limit-gap callback\n");
   fprintf (stderr, "      l to use the logging callback\n");
   fprintf (stderr, "      a to use the aborter\n");
   fprintf (stderr, "   where filename is a file with extension \n");
   fprintf (stderr, "      MPS, SAV, or LP (lower case is allowed)\n");
   fprintf (stderr, " Exiting...\n");
} /* END usage */
