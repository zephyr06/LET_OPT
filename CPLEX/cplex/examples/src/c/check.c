/* --------------------------------------------------------------------------
 * File: check.c
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 1997, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

/* Compile and link this file with your application if the diagnostic
   output associated with turning on the datacheck parameter is not
   sufficient to diagnose the problem.  This program is designed to
   work with both the new 64 bit and previous 32 bit CPLEX C APIs.
   See the comments in cplexcheck.h for compilation instructions for
   the different APIs.  */

#include <stdarg.h>
#include <ilcplex/cplexcheck.h>
#if CPLEXX_NAMES
#define CPXgetchannels      CPXXgetchannels
#define CPXgetnumcols       CPXXgetnumcols
#define CPXmsgstr           CPXXmsgstr
#define CPXflushstdchannels CPXXflushstdchannels
#define CPXgetnumrows       CPXXgetnumrows
#define CPXgetintparam      CPXXgetintparam
#define CPXsetintparam      CPXXsetintparam
#define CPXgetrngval        CPXXgetrngval
#define CPXgetobjsen        CPXXgetobjsen
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define XABS(x)        fabs((x))


#define BIGREAL        1.0E+75

#define EPSZERO        1.0E-10

#define TRUE    1
#define FALSE   0

#define SUCCEED 0
#define FAIL    1

#define MAXERRCNT  20
#define MAXWARNCNT  5
#define MAXNAMELEN 255

static int
checkinit    (CPXCENVptr env, CPXCLPptr lp, const char *routinename),
   checkdata    (CPXCENVptr env, CPXDIM numcols, CPXDIM numrows, int objsen,
                 const double *obj, const double *rhs, const char *sense,
                 const CPXNNZ *matbeg, const CPXDIM *matcnt, const CPXDIM
                 *matind, const double *matval, const double *lb,
                 const double *ub, const double *rngval,
                 char **colname, char **rowname),
   checkvaldata (CPXCENVptr env, CPXCLPptr lp, CPXNNZ cnt, CPXDIM rows,
                 CPXDIM cols, const CPXDIM *rowind, const CPXDIM *colind,
                 const double *values, const char *rowindname,
                 const char *colindname, const char *valuesname,
                 int testfordups),
   checknan     (CPXCENVptr env, const double *dx, CPXNNZ len,
                 const char *arrayname, CPXCHANNELptr errorchan,
                 CPXCHANNELptr reschan),
   checkmatval  (CPXCENVptr env, const CPXNNZ *matbeg, const CPXDIM *matcnt,
                 const double *matval, CPXDIM numcols, const char *arrayname,
                 CPXCHANNELptr errorchan, CPXCHANNELptr reschan),
   checkmalloc  (void),
   checksizes   (CPXCENVptr env, CPXDIM numcols, CPXDIM numrows,
                 CPXCHANNELptr errorchan),
   checkrows    (CPXCENVptr env, CPXDIM numcols, CPXDIM numrows,
                 const double *rhs, const char *sense, const CPXNNZ *matbeg,
                 const CPXDIM *matcnt, const double *rngval,
                 CPXCHANNELptr errorchan, CPXCHANNELptr warnchan,
                 CPXCHANNELptr reschan),
   checkcols    (CPXCENVptr env, CPXDIM numcols, CPXDIM numrows,
                 const double *obj, const CPXNNZ *matbeg, const CPXDIM *matcnt,
                 const CPXDIM *matind, const double *matval, const double *lb,
                 const double *ub, int *zeroind_p, double *maxval_p,
                 double *minval_p, CPXCHANNELptr errorchan,
                 CPXCHANNELptr warnchan, CPXCHANNELptr reschan),
   checknames   (CPXCENVptr env, CPXDIM numcols, CPXDIM numrows,
                 char **colname, char **rowname,
                 CPXCHANNELptr errorchan, CPXCHANNELptr warnchan,
                 CPXCHANNELptr reschan);

/* Convenience function to output a formatted message to a channel. */
static void
display (CPXCHANNELptr channel, const char *format, ...)
{
   /* None of the messages we print are longer than 1024 bytes */
   char msg[1024];
   va_list argp;

   va_start (argp, format);
   vsprintf (msg, format, argp);
   va_end (argp);
   CPXmsgstr (channel, msg);
} /* END display */


#ifdef CHECK_NAME_CONFLICT
static int
checknameconflicts (CPXCHANNELptr errorchan, CPXCHANNELptr warnchan,
                    CPXCHANNELptr reschan);
#endif



int CPXPUBLIC
CPXcheckcopylpwnames (CPXCENVptr   env,
                      CPXCLPptr    lp,
                      CPXDIM       numcols,
                      CPXDIM       numrows,
                      int          objsen,
                      const double *obj,
                      const double *rhs,
                      const char   *sense,
                      const CPXNNZ *matbeg,
                      const CPXDIM *matcnt,
                      const CPXDIM *matind,
                      const double *matval,
                      const double *lb,
                      const double *ub,
                      const double *rngval,
                      char         **colname,
                      char         **rowname)
{
   int status;

   status = checkinit (env, lp, "CPXcheckcopylpwnames");
   if ( !status ) {
      status = checkdata (env, numcols, numrows, objsen, obj,
                          rhs, sense, matbeg, matcnt, matind, matval,
                          lb, ub, rngval, colname, rowname);
   }
   return (status);

} /* END cpxcheckcopylpwnames */


int CPXPUBLIC
CPXcheckcopylp (CPXCENVptr   env,
                CPXCLPptr    lp,
                CPXDIM       numcols,
                CPXDIM       numrows,
                int          objsen,
                const double *obj,
                const double *rhs,
                const char   *sense,
                const CPXNNZ *matbeg,
                const CPXDIM *matcnt,
                const CPXDIM *matind,
                const double *matval,
                const double *lb,
                const double *ub,
                const double *rngval)
{
   int status;

   status = checkinit (env, lp, "CPXcheckcopylp");
   if ( !status ) {
      status = checkdata (env, numcols, numrows, objsen, obj,
                          rhs, sense, matbeg, matcnt, matind, matval,
                          lb, ub, rngval, NULL, NULL);
   }
   return (status);

} /* END cpxcheckcopylp */


int CPXPUBLIC
CPXcheckcopyctype (CPXCENVptr env,
                   CPXCLPptr  lp,
                   const char *ctype)
{
   int           status = SUCCEED;
   CPXDIM        j, numcols;
   CPXCHANNELptr errorchan = NULL;

   status = checkinit (env, lp, "CPXcheckcopyctype");
   if ( status )  goto TERMINATE;

   /* Get the CPLEX standard channels; all output is through
      these channels  */

   CPXgetchannels (env, NULL, NULL, &errorchan, NULL);

   numcols = CPXgetnumcols (env, lp);

   if ( ctype != NULL ) {
      for (j = 0; j < numcols; j++) {
         if ( ctype[j] != CPX_CONTINUOUS &&
              ctype[j] != CPX_BINARY     &&
              ctype[j] != CPX_INTEGER    &&
              ctype[j] != CPX_SEMICONT   &&
              ctype[j] != CPX_SEMIINT      ) {
            display (errorchan,
                     "ctype must be '%c', '%c', '%c', '%c', or '%c'; ",
                     CPX_CONTINUOUS, CPX_BINARY, CPX_INTEGER,
                     CPX_SEMICONT, CPX_SEMIINT);
            display (errorchan,
                     "entry %lld is '%c'.\n", (long long) j, ctype[j]);
            status = FAIL;
            goto TERMINATE;
         }
      }
   }

TERMINATE:

   return (status);

} /* END cpxcheckcopyctype */


int CPXPUBLIC
CPXcheckcopysos (CPXCENVptr   env,
                 CPXCLPptr    lp,
                 CPXDIM       numsos,
                 CPXNNZ       numsosnz,
                 const char   *sostype,
                 const CPXNNZ *sosbeg,
                 const CPXDIM *sosind,
                 const double *soswt,
                 char         **sosname)
{
   int           warncnt, status = SUCCEED;
   CPXDIM        i, minsoslen, maxsoslen, numcols;
   CPXNNZ        j, k, count;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan  = NULL;
   CPXCHANNELptr reschan   = NULL;
   char          sosindstr[32], soswtstr[32];

   status = checkinit (env, lp, "CPXcheckcopysos");
   if ( status )  goto TERMINATE;

   /* Get the CPLEX standard channels; all output is through
      these channels  */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   /* Check for NULL pointers */

   if ( sostype == NULL ||
        sosbeg  == NULL ||
        sosind  == NULL ||
        soswt   == NULL  ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s%s",
               (sostype == NULL ? "sostype\n": ""),
               (sosbeg  == NULL ? "sosbeg\n": ""),
               (sosind  == NULL ? "sosind\n": ""),
               (soswt   == NULL ? "soswt\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   numcols = CPXgetnumcols (env, lp);

   /* Check numsos, numsosnz for valid values. */

   if ( numsos > numcols*(numcols - 1)/2 || numsos < 0 ) {
      display (errorchan, "Invalid value for numsos: %d.\n", numsos);
      if ( numsos > numcols*(numcols - 1)/2 )
         display (errorchan,
                  "This number of distinct SOSs can be at most (%lld).\n",
                  (long long) (numcols*(numcols - 1)/2));
      else
         display (errorchan, "This value must be nonnegative.\n");

      status = FAIL;
      goto TERMINATE;
   }

   if ( numsosnz > numsos*numcols || numsosnz < 0 ) {

      /* Only issue warning since if numsosnz too big, duplicate
         entries exist; terminate after user sees duplicates output. */

      display (warnchan, "Invalid value for numsosnz: %d.\n", numsosnz);
      if ( numsosnz < 0 )
         display (errorchan, "This value must be nonnegative.\n");
      else
         display (errorchan,
                  "Each of the %lld SOSs can contain at most (%lld)"
                  " variables.\n",
                  (long long) numsos, (long long) numcols);
   }

   /* Check that sosbeg is in ascending order, has correct values relative
      to numsosnz. */

   maxsoslen = 0;
   minsoslen = numcols;
   count     = 0;
   for (i = 0; i < numsos-1; i++) {
      CPXDIM const soslen = sosbeg[i+1] - sosbeg[i];
      if ( soslen < 0 ) {
         display (errorchan,
                  "Components of sosbeg must be ascending, ");
         display (errorchan,
                  "but sosbeg[%lld] (%lld) > sosbeg[%lld] (%lld).\n",
                  (long long) i, (long long) sosbeg[i], (long long)i+1,
                  (long long) sosbeg[i+1]);
         status = FAIL;
         goto TERMINATE;
      }
      maxsoslen = soslen > maxsoslen ? soslen : maxsoslen;
      minsoslen = soslen < minsoslen ? soslen : minsoslen;
      count += soslen;
   }
   display (reschan, "Number of entries in largest SOS: %lld.\n",
            (long long) maxsoslen);
   display (reschan, "Number of entries in smallest SOS: %lld.\n",
            (long long) minsoslen);
   count += (numsosnz - sosbeg[numsos-1]);
   if ( count != numsosnz ) {
      display (warnchan, "Value of numsosnz (%lld) does not match ",
               (long long) numsosnz);
      display (warnchan, "number of entries in sosbeg array (%lld).\n",
               (long long) count);
   }

   if ( numsos > 0 ) {
      if ( sosbeg[0] < 0 ) {
         display (errorchan, "Negative entry in sosbeg: sosbeg[0] = %lld\n",
                  (long long) sosbeg[0]);
         status = FAIL;
         goto TERMINATE;
      }
      if ( sosbeg[numsos-1] >= numsosnz ) {
         display (errorchan,
                  "Invalid value in sosbeg: sosbeg[%lld] = %lld\n",
                  (long long) numsos-1, (long long) sosbeg[numsos-1]);
         display (errorchan, "This value must be < numsosnz, which is %lld.\n",
                  (long long) numsosnz);
         status = FAIL;
         goto TERMINATE;
      }
   }

   /* Check sostype array; each item must contain '1' or '2'. */

   for (i = 0; i < numsos; i++) {
      if ( sostype[i] != CPX_TYPE_SOS1 && sostype[i] != CPX_TYPE_SOS2 ) {
         display (errorchan, "Invalid value in sostype: sostype[%lld] = %c.\n",
                  (long long) i, sostype[i]);
         status = FAIL;
         goto TERMINATE;
      }
   }

   /* Loop through each individual SOS set.  Check for duplicate entries
      in sosind, invalid entries in soswt. */

   count   = numsosnz;
   warncnt = 0;
   for (i = numsos-1; i >= 0; i--) {
      count -= sosbeg[i];
      if ( count <= 1 ) {
         warncnt++;
         if ( warncnt <= MAXWARNCNT )
            display (warnchan, "SOS set %lld contains <= 1 elements.\n",
                     (long long) i);

      }

      sprintf (sosindstr, "(sosind + %lld)", (long long) sosbeg[i]);
      sprintf (soswtstr, "(soswt + %lld)", (long long) sosbeg[i]);

      status = checkvaldata (env, lp, count, 0, numcols, NULL,
                             &sosind[sosbeg[i]], NULL, NULL,
                             sosindstr, soswtstr, TRUE);
      if ( status ) goto TERMINATE;

      /* Also test for non-unique reference values in each SOS. */

      for (j = 0; j < count; j++) {
         for (k = sosbeg[i] + j + 1; k < sosbeg[i] + count; k++) {
            if ( fabs(soswt[sosbeg[i] + j] - soswt[k]) < EPSZERO ) {
               display (errorchan, "Duplicate entries in SOS %lld; ",
                        (long long) i);
               display (errorchan, "sosval[%lld] = %g, sosval[%lld] = %g.\n",
                        (long long) (sosbeg[i] + j), soswt[sosbeg[i] + j],
                        (long long) k, soswt[k]);
               status = FAIL;
               goto TERMINATE;
            }
	 }
      }
      count = sosbeg[i];
   }
   if ( warncnt > MAXWARNCNT )
      display (warnchan,
               "%d short SOS warnings not printed.\n", warncnt - MAXWARNCNT);

   /* Check the soswt array for unrepresentable values. */

   status = checknan (env, soswt, numsosnz, "soswt", errorchan, reschan);
   if ( status ) goto TERMINATE;

   /* Check names */

   warncnt = 0;
   if ( sosname ) {
      for (i = 0; i < numsos; i++) {
         if ( strlen(sosname[i]) > MAXNAMELEN ) {
            warncnt++;
            if ( warncnt <= MAXWARNCNT ) {
               display (warnchan,
                        "Warning:  Name for SOS %lld (%s) exceeds %d characters.\n",
                        (long long) i, sosname[i], MAXNAMELEN);
               display (warnchan,
                        "CPLEX may not be able to write LP or MPS files.\n");
            }
         }
      }
      if ( warncnt > MAXWARNCNT ) {
         display (warnchan,
                  "%d SOS name length warnings not printed.\n",
                  warncnt - MAXWARNCNT);
      }
      CPXflushstdchannels (env);
   }

   CPXflushstdchannels (env);
TERMINATE:

   return (status);

} /* END cpxcheckcopysos */


int CPXPUBLIC
CPXcheckaddcols (CPXCENVptr   env,
                 CPXCLPptr    lp,
                 CPXDIM       ccnt,
                 CPXNNZ       nzcnt,
                 const double *obj,
                 const CPXNNZ *cmatbeg,
                 const CPXDIM *cmatind,
                 const double *cmatval,
                 const double *lb,
                 const double *ub,
                 char         **colname)
{
   int           status = SUCCEED;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan  = NULL;
   CPXCHANNELptr reschan   = NULL;
   CPXDIM        i, rows;
   CPXNNZ        k, collen;
   char          buffer[64];

   status = checkinit (env, lp, "CPXcheckaddcols");
   if ( status )  goto TERMINATE;

   /* Here, we get the CPLEX standard channels.  Alternatively, you
      you create your own, or set reschan, warnchan, errorchan, to be
      a channel you want to use. */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   /* Check for NULL pointers */

   if ( cmatbeg == NULL ||
        cmatind == NULL ||
        cmatval == NULL   ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s",
               (cmatbeg == NULL ? "cmatbeg\n": ""),
               (cmatind == NULL ? "cmatind\n": ""),
               (cmatval == NULL ? "cmatval\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   status = checknan (env, obj, ccnt, "obj", errorchan, reschan);
   if ( status )  goto TERMINATE;
   if ( obj == NULL ) {
      display (warnchan, "Warning: obj array is NULL.\n");
      display (warnchan,
               "All added columns will have objective coefficient of 0.\n");
   }
   status = checknan (env, lb, ccnt, "lb", errorchan, reschan);
   if ( status )  goto TERMINATE;
   if ( lb == NULL ) {
      display (warnchan, "Warning: lower bound array is NULL.\n");
      display (warnchan,
               "All added columns will have lower bounds of 0.\n");
   }
   status = checknan (env, ub, ccnt, "ub", errorchan, reschan);
   if ( status )  goto TERMINATE;
   if ( ub == NULL ) {
      display (warnchan, "Warning: upper bound array is NULL.\n");
      display (warnchan,
               "All added columns will have infinite upper bounds.\n");
   }

   /* Verify that components of cmatbeg are in ascending order */

   for (i = 0; i < ccnt - 1; i++) {
      if ( cmatbeg[i] > cmatbeg[i+1] ) {
         display (errorchan,
                  "Components of cmatbeg must be ascending, ");
         display (errorchan,
                  "but cmatbeg[%lld] (%lld) > cmatbeg[%lld] (%lld).\n",
                  (long long) i, (long long) cmatbeg[i], (long long) (i+1),
                  (long long) cmatbeg[i+1]);
         status = FAIL;
         goto TERMINATE;
      }
   }
   if ( ccnt > 0 ) {
      if ( cmatbeg[0] < 0 ) {
         display (errorchan,
                  "Negative entry in cmatbeg: cmatbeg[0] = %lld\n",
                  (long long) cmatbeg[0]);
         status = FAIL;
         goto TERMINATE;
      }
      if ( cmatbeg[ccnt-1] > nzcnt ) {
         display (errorchan,
                  "Invalid value in cmatbeg: cmatbeg[%lld] = %lld\n",
                  (long long) ccnt-1, (long long) cmatbeg[ccnt-1]);
         display (errorchan, "This value must be < nzcnt, which is %lld.\n",
                  (long long) nzcnt);
         status = FAIL;
         goto TERMINATE;
      }
   }
   else if ( ccnt < 0 ) {
      display (errorchan, "Negative value for ccnt (%lld).\n",
               (long long) ccnt);
      status = FAIL;
      goto TERMINATE;
   }

   /* Test the column oriented data structures for errors. */

   rows = CPXgetnumrows (env, lp);
   for (i = ccnt - 1, k = nzcnt; i >= 0; i--) {
      collen = k - cmatbeg[i];
      k      = cmatbeg[i];
      sprintf (buffer, "cmatval for column %lld", (long long) i);
      status = checkvaldata (env, lp, collen, rows, 0,
                             &cmatind[cmatbeg[i]], NULL,
                             &cmatval[cmatbeg[i]], "cmatind", NULL,
                             buffer, TRUE);
      if ( status ) {
         display (errorchan, "Error detected in column %lld.\n",
                  (long long) i);
         goto TERMINATE;

      }
   }

TERMINATE:

   return (status);

} /* END cpxcheckaddcols */


int CPXPUBLIC
CPXcheckaddrows (CPXCENVptr env,
                 CPXCLPptr lp,
                 CPXDIM ccnt,
                 CPXDIM rcnt,
                 CPXNNZ nzcnt,
                 const double *rhs,
                 const char *sense,
                 const CPXNNZ *rmatbeg,
                 const CPXDIM *rmatind,
                 const double *rmatval,
                 char **colname,
                 char **rowname)
{
   int           status = SUCCEED;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan  = NULL;
   CPXCHANNELptr reschan   = NULL;
   int           oldparamval;
   CPXDIM        i, colstot;
   CPXNNZ        k, rowlen;
   char          buffer[64];

   status = checkinit (env, lp, "CPXcheckaddrows");
   if ( status )  goto TERMINATE;

   /* Here, we get the CPLEX standard channels.  Alternatively, you
      you create your own, or set reschan, warnchan, errorchan, to be
      a channel you want to use. */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   /* Check for NULL pointers */

   if ( rmatbeg == NULL ||
        rmatind == NULL ||
        rmatval == NULL   ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s",
               (rmatbeg == NULL ? "rmatbeg\n": ""),
               (rmatind == NULL ? "rmatind\n": ""),
               (rmatval == NULL ? "rmatval\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   status = checknan (env, rhs, rcnt, "rhs", errorchan, reschan);
   if ( status )  goto TERMINATE;
   if ( rhs == NULL ) {
      display (warnchan, "Warning: rhs array is NULL; ");
      display (warnchan, "all added rows will have right hand sides of 0.\n");
   }

   /* Check for invalid values in the sense array.
      Suppress output from CPXgetrngval if no ranged rows exist. */

   CPXgetintparam (env, CPXPARAM_ScreenOutput, &oldparamval);
   CPXsetintparam ((CPXENVptr) env, CPXPARAM_ScreenOutput, CPX_OFF);
   CPXsetintparam ((CPXENVptr) env, CPXPARAM_ScreenOutput, oldparamval);
   if ( sense != NULL ) {
      for (i = 0; i < rcnt; i++) {
         if ( sense[i] != 'L' &&
              sense[i] != 'G' &&
              sense[i] != 'E' &&
              sense[i] != 'R'   ) {
            display (errorchan,
                     "Rows sense must be 'L', 'G', 'E', or 'R':  ");
            display (errorchan,
                     "Entry %lld is '%c'.\n", (long long) i, sense[i]);
            status = FAIL;
            goto TERMINATE;
         }
      }
   }
   else {
      display (warnchan, "Warning: sense array is NULL; ");
      display (warnchan, "all added rows will be considered equalities.\n");
   }


   /* Verify that components of rmatbeg are in ascending order */

   for (i = 0; i < rcnt - 1; i++) {
      if ( rmatbeg[i] > rmatbeg[i+1] ) {
         display (errorchan,
                  "Components of rmatbeg must be ascending, ");
         display (errorchan,
                  "but rmatbeg[%lld] (%lld) > rmatbeg[%lld] (%lld).\n",
                  (long long) i, (long long) rmatbeg[i], (long long) (i+1),
                  (long long) rmatbeg[i+1]);
         status = FAIL;
         goto TERMINATE;
      }
   }

   /* Make sure rmatbeg has no negative entries, and that nzcnt is
      reasonable. Since we know that rmatbeg is in ascending order
      when we get here, just look at rmatbeg[0] and rmatbeg[rcnt-1]. */

   if ( rcnt > 0 ) {
      if ( rmatbeg[0] < 0 ) {
         display (errorchan,
                  "Negative entry in rmatbeg: rmatbeg[0] = %lld\n",
                  (long long) rmatbeg[0]);
         status = FAIL;
         goto TERMINATE;
      }
      if ( rmatbeg[rcnt-1] > nzcnt ) {
         display (errorchan,
                  "Invalid value in rmatbeg: rmatbeg[%lld] = %lld\n",
                  (long long) (rcnt-1), (long long) rmatbeg[rcnt-1]);
         display (errorchan, "This value must be < nzcnt, which is %lld.\n",
                  (long long) nzcnt);
         status = FAIL;
         goto TERMINATE;
      }
   }
   else if ( rcnt < 0 ) {
      display (errorchan, "Negative value for rcnt (%lld).\n",
               (long long) rcnt);
      status = FAIL;
      goto TERMINATE;
   }

   /* Test the row oriented data structures for errors. */

   colstot = CPXgetnumcols (env, lp) + ccnt;
   for (i = rcnt - 1, k = nzcnt; i >= 0; i--) {
      rowlen = k - rmatbeg[i];
      k      = rmatbeg[i];
      sprintf (buffer, "rmatval for row %lld", (long long) i);
      status = checkvaldata (env, lp, rowlen, 0, colstot, NULL,
                             &rmatind[rmatbeg[i]], &rmatval[rmatbeg[i]],
                             NULL, "rmatind", buffer, TRUE);
      if ( status ) {
         display (errorchan, "Error detected in row %lld.\n", (long long) i);
         goto TERMINATE;

      }
   }

TERMINATE:

   return (status);

} /* END cpxcheckaddcols */


int CPXPUBLIC
CPXcheckchgcoeflist (CPXCENVptr   env,
                     CPXCLPptr    lp,
                     CPXNNZ       numcoefs,
                     const CPXDIM *rowlist,
                     const CPXDIM *collist,
                     const double *vallist)
{
   int           status = SUCCEED;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan  = NULL;
   CPXCHANNELptr reschan   = NULL;
   CPXDIM        j, k, rows, cols;
   CPXNNZ        i, i1;
   CPXDIM        *rowchgcnt = NULL;
   CPXNNZ        *rowchgbeg = NULL;
   CPXDIM        *colchgind = NULL;
   CPXNNZ        *colchgprm = NULL;
   CPXDIM        *colchgcnt = NULL;

   status = checkinit (env, lp, "CPXcheckchgcoeflist");
   if ( status )  goto TERMINATE;

   /* Here, we get the CPLEX standard channels.  Alternatively, you
      you create your own, or set reschan, warnchan, errorchan, to be
      a channel you want to use. */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   /* Check for NULL pointers */

   if ( rowlist == NULL ||
        collist == NULL ||
        vallist == NULL   ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s",
               (rowlist == NULL ? "rowlist\n": ""),
               (collist == NULL ? "collist\n": ""),
               (vallist == NULL ? "vallist\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   /* Check for invalid values in the indices arrays. */

   rows = CPXgetnumrows (env, lp);
   cols = CPXgetnumcols (env, lp);

   rowchgcnt = (CPXDIM *) calloc(rows, sizeof(*rowchgcnt));
   rowchgbeg = (CPXNNZ *) calloc(rows, sizeof(*rowchgbeg));
   colchgind = (CPXDIM *) calloc(numcoefs, sizeof(*colchgind));
   colchgprm = (CPXNNZ *) calloc(numcoefs, sizeof(*colchgprm));
   colchgcnt = (CPXDIM *) calloc(cols, sizeof(*colchgcnt));
   if ( rowchgcnt == NULL || rowchgbeg == NULL ||
        colchgind == NULL || colchgprm == NULL || colchgcnt == NULL ) {
      display (errorchan,
               "Work vector mallocs failed in CPXcheckchgcoeflist.\n");
      status = FAIL;
      goto TERMINATE;
   }

   for (i = 0; i < numcoefs; i++) {
      k = rowlist[i];
      if ( k < 0 ) {
         display (errorchan,
                  "Error: entry rowlist[%lld] is negative (%lld).\n",
                  (long long) i, (long long) k);
         status = FAIL;
         goto TERMINATE;
      }
      if ( k >= rows ) {
         display (errorchan,
                  "Error: entry rowlist[%lld] (%lld) invalid for number of "
                  "rows (%lld).\n", (long long) i, (long long) k,
                  (long long) rows);
         status = FAIL;
         goto TERMINATE;
      }
      rowchgcnt[k]++;

      k = collist[i];
      if ( k < 0 ) {
         display (errorchan,
                  "Error: entry collist[%lld] is negative (%lld).\n",
                  (long long) i, (long long) k);
         status = FAIL;
         goto TERMINATE;
      }
      if ( k >= cols ) {
         display (errorchan,
                  "Error: entry collist[%lld] (%lld) invalid for number of "
                  "columns (%lld).\n", (long long) i, (long long) k,
                  (long long) cols);
         status = FAIL;
         goto TERMINATE;
      }
   }

   /* Check for duplicates in the indices arrays.  From the last loop,
      the rowchgcnt array contains counts for the # of coefficients
      changed in each row of the matrix.  Use this to construct a list
      of column indices for the changes in each row (i.e. the colchgind
      array below is the collist array sorted by rows).  Then test for
      duplicate column indices among each row that had more than one
      coefficient changed.  */

   for (k = 1; k < rows; k++)
      rowchgbeg[k] = rowchgbeg[k-1] + rowchgcnt[k-1];

   for (i = 0; i < numcoefs; i++) {
      j = collist[i];
      k = rowlist[i];
      colchgind[rowchgbeg[k]] = j;
      colchgprm[rowchgbeg[k]] = i;
      rowchgbeg[k]++;
   }

   rowchgbeg[0] = 0;
   for (i = 1; i < rows; i++)
      rowchgbeg[i] = rowchgbeg[i-1] + rowchgcnt[i-1];


   for (i = 0; i < numcoefs; i++) {
      k = rowlist[i];
      if ( rowchgcnt[k] > 1 ) { /* Need to check for a duplicate. */

         /* For each row that has more than one change, count the
            number of occurrences for the column indices associated
            with the coefficient changes in that row.  Any column
            index that appears more than once means a particular
            matrix element was changed more than once.  The code
            flags this as a warning, since in some cases it may be
            convenient to generate repeated changes.  Once we have
            tested a row, set it's # of changes to 0 so we don't
            reexamine it later. */

         for (i1 = rowchgbeg[k]; i1 < rowchgbeg[k] + rowchgcnt[k]; i1++)
            colchgcnt[colchgind[i1]]++;
         for (i1 = rowchgbeg[k] + rowchgcnt[k] - 1; i1 >= rowchgbeg[k];
              i1--) {
            if ( colchgcnt[colchgind[i1]] > 1 ) {
               display (warnchan, "Warning: the same matrix entry was ");
               display (warnchan, "changed more than once.\n");
               display (warnchan, "Rowlist[%lld] = %lld, "
                        "collist[%lld] = %lld.\n",
                        (long long) colchgprm[i1], (long long) rowlist[i],
                        (long long) colchgprm[i1], (long long) collist[i]);
	    }
	 }
         for (i1 = rowchgbeg[k]; i1 < rowchgbeg[k] + rowchgcnt[k]; i1++)
            colchgcnt[colchgind[i1]] = 0;

         rowchgcnt[k] = 0;
      }
   }


   /* Check for NANs in the values array. */

   status = checknan (env, vallist, numcoefs, "vallist",
                      errorchan, reschan);

TERMINATE:

   if ( rowchgcnt )  free ((char *) rowchgcnt);
   if ( rowchgbeg )  free ((char *) rowchgbeg);
   if ( colchgind )  free ((char *) colchgind);
   if ( colchgprm )  free ((char *) colchgprm);
   if ( colchgcnt )  free ((char *) colchgcnt);

   return (status);

} /* END cpxcheckchgcoeflist */


int CPXPUBLIC
CPXcheckvals (CPXCENVptr   env,
              CPXCLPptr    lp,
              CPXNNZ       cnt,
              const CPXDIM *rowind,
              const CPXDIM *colind,
              const double *values)
{
   int status = SUCCEED;
   CPXDIM rows, cols;

   status = checkinit (env, lp, "CPXcheckvals");
   if ( status )  goto TERMINATE;

   rows = CPXgetnumrows (env, lp);
   cols = CPXgetnumcols (env, lp);
   status = checkvaldata (env, lp, cnt, rows, cols, rowind, colind, values,
                          "rowind", "colind", "values", TRUE);

TERMINATE:

   return (status);

} /* END cpxcheckvals */


int CPXPUBLIC
CPXcheckcopyqpsep (CPXCENVptr   env,
                   CPXCLPptr    lp,
                   const double *qsepvec)
{
   int           status = SUCCEED;
   CPXDIM        i, numcols = 0, objsen = 0;
   double        qminval, qmaxval, t, dsen;
   const char    *senstr, *signstr;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan = NULL;
   CPXCHANNELptr reschan = NULL;

   status = checkinit (env, lp, "CPXcheckcopyqpsep");
   if ( status )  goto TERMINATE;

   /* Get the CPLEX standard channels; all output is through
      these channels  */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   /* Check for NULL pointers */

   if ( qsepvec == NULL ) {
      display (errorchan, "The qsepvec array is NULL.\n");
      status = FAIL;
      goto TERMINATE;
   }
   objsen = CPXgetobjsen (env, lp);
   numcols = CPXgetnumcols (env, lp);

   display (reschan, "Checking QP (separable) problem data:\n");

   qminval = BIGREAL;
   qmaxval = 0.0;

   senstr  = objsen == 1 ? "minimizing" : "maximizing";
   signstr = objsen == 1 ? "Negative"   : "Positive";
   dsen    = (double) objsen;
   for (i = 0; i < numcols; i++) {
      t = dsen * qsepvec[i];
      if ( t < 0 ) {
         display (errorchan, "%s entry in qsepvec when %s: ",
                  signstr, senstr);
         display (errorchan, "qsepvec[%lld] = %18.10e\n",
                  (long long) i, qsepvec[i]);
         status = FAIL;
         goto TERMINATE;
      }
      else {
         if ( t < qminval )  qminval = t;
         if ( t > qmaxval )  qmaxval = t;
      }
   }

   display (reschan, "Columns                %6lld\n", (long long) numcols);

   display (reschan,
            "Minimum absolute qsepvec value %18.10e.\n",
            qminval);
   display (reschan,
            "Maximum absolute qsepvec value %18.10e.\n",
            qmaxval);

   CPXflushstdchannels (env);

   status = checknan (env, qsepvec, numcols, "qsepvec", errorchan, reschan);
   if ( status )  goto TERMINATE;

TERMINATE:

   CPXflushstdchannels (env);

   return (status);

} /* END cpxcheckcopyqpsep */


int CPXPUBLIC
CPXcheckcopyquad (CPXCENVptr   env,
                  CPXCLPptr    lp,
                  const CPXNNZ *qmatbeg,
                  const CPXDIM *qmatcnt,
                  const CPXDIM *qmatind,
                  const double *qmatval)
{
   int           status = SUCCEED, errcnt = 0;
   int           zeroind = FALSE;
   CPXDIM        j;
   CPXDIM        numcols = 0;
   CPXDIM        *iwork = NULL;
   CPXNNZ        k;
   CPXNNZ        nzcnt = 0;
   double        qminval, qmaxval;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan = NULL;
   CPXCHANNELptr reschan = NULL;

   status = checkinit (env, lp, "CPXcheckcopyquad");
   if ( status == FAIL )  goto TERMINATE;

   /* Get the CPLEX standard channels; all output is through
      these channels  */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   numcols = CPXgetnumcols (env, lp);

   /* Check for NULL pointers */

   if ( qmatbeg == NULL ||
        qmatcnt == NULL ||
        qmatind == NULL ||
        qmatval == NULL   ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s%s",
               (qmatbeg == NULL ? "qmatbeg\n": ""),
               (qmatcnt == NULL ? "qmatcnt\n": ""),
               (qmatind == NULL ? "qmatind\n": ""),
               (qmatval == NULL ? "qmatval\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   display (reschan, "Checking Q matrix (non-separable):\n");

   /* Verify that components of qmatbeg are in ascending order */

   for (j = 0; j < numcols - 1; j++) {
      if ( qmatbeg[j] > qmatbeg[j+1] ) {
         display (errorchan,
                  "Components of qmatbeg must be ascending, ");
         display (errorchan,
                  "but qmatbeg[%lld] (%lld) > qmatbeg[%lld] (%lld).\n",
                  (long long) j, (long long) qmatbeg[j], (long long) (j+1),
                  (long long) qmatbeg[j+1]);
         status = FAIL;
         goto TERMINATE;
      }
   }

   /* Make sure qmatbeg has no negative entries. Since we know that
      qmatbeg is in ascending order when we get here, just look at
      qmatbeg[0] */

   if (( numcols > 0 ) && ( qmatbeg[0] < 0 )) {
      display (errorchan,
               "Negative entry in qmatbeg: qmatbeg[0] = %lld\n",
               (long long) qmatbeg[0]);
      status = FAIL;
      goto TERMINATE;
   }

   /* Check for invalid column counts and overlapping columns */

   iwork = (CPXDIM *) calloc(numcols, sizeof(*iwork));
   if ( iwork == NULL ) {
      display (errorchan, "Work vector malloc failed in checkquaddata.\n");
      status = FAIL;
      goto TERMINATE;
   }

   qmaxval = 0.0;
   qminval = BIGREAL;
   nzcnt   = 0;
   for (j = 0; j < numcols; j++) {
      if ( qmatcnt[j] < 0 ) {
         display (errorchan,
                  "Count of entries in column %lld is negative (%lld).\n",
                  (long long) j, (long long) qmatcnt[j]);
         status = FAIL;
         goto TERMINATE;
      }
      if ( qmatcnt[j] > numcols ) {
         display (errorchan,
                  "Count of entries in column %lld (%lld) > columns (%lld).\n",
                  (long long)j, (long long) qmatcnt[j], (long long) numcols);
         status = FAIL;
         goto TERMINATE;
      }
      if ( (j < numcols-1)                         &&
           (qmatbeg[j] + qmatcnt[j] > qmatbeg[j+1])  ) {
         display (errorchan,
                  "End of column %lld overlaps start of column %lld\n",
                  (long long) j, (long long) (j+1));
         status = FAIL;
         goto TERMINATE;
      }

      /* Compute min and max absolute Q matrix elements. Also,
         check validity of entries in qmatind */

      for (k = qmatbeg[j]; k < qmatbeg[j] + qmatcnt[j]; k++) {
         if ( qmatind[k] < 0 ) {
            display (errorchan,
                     "Entry qmatind[%lld] is negative (%lld).\n",
                     (long long) k, (long long) qmatind[k]);
            status = FAIL;
            goto TERMINATE;
         }
         if ( qmatind[k] >= numcols ) {
            display (errorchan,
                     "Entry qmatind[%lld] (%lld) invalid for number of "
                     "columns (%lld).\n", (long long) k,
                     (long long) qmatind[k], (long long) numcols);
            status = FAIL;
            goto TERMINATE;
         }
         iwork[qmatind[k]]++;
         if ( !qmatval[k] )  zeroind = TRUE;
         else                nzcnt++;
         if ( qmatval[k]  &&  XABS(qmatval[k]) < qminval ) {
            qminval = XABS(qmatval[k]);
         }
         if ( XABS(qmatval[k]) > qmaxval ) {
            qmaxval = XABS(qmatval[k]);
         }
      }

      /* Check for duplicate row indices in a single column */

      for (k = qmatbeg[j]; k < qmatbeg[j] + qmatcnt[j]; k++) {
         if ( iwork[qmatind[k]] > 1) {
            display (errorchan, "Duplicate row entry in qmatind: ");
            display (errorchan, "column %lld, row %lld.\n",
                     (long long) j, (long long) qmatind[k]);
            status = FAIL;
            errcnt++;
            if ( errcnt >= MAXERRCNT ) {
               display(errorchan,
                       "Quitting after %d duplicate row entries found.\n",
                       MAXERRCNT);
               status = FAIL;
               goto TERMINATE;
            }
         }
         iwork[qmatind[k]] = 0;
      }
   }
   if ( status )  goto TERMINATE;

   if ( numcols > 0 ) {
      status = checkmatval (env, qmatbeg, qmatcnt, qmatval, numcols,
                            "qmatval", errorchan, reschan);
      if ( status )  goto TERMINATE;
   }

   display (reschan, "Columns                  %6lld\n", (long long) numcols);
   display (reschan, "Q Matrix nonzeros        %6lld\n", (long long) nzcnt);
   display (reschan,
            "Minimum absolute nonzero Q matrix    value %18.10e.\n",
            qminval);
   display (reschan,
            "Maximum absolute         Q matrix    value %18.10e.\n",
            qmaxval);
   if ( zeroind ) {
      display (warnchan, "Warning:  Matrix contains zero entries.\n");
   }

TERMINATE:

   CPXflushstdchannels (env);

   if ( iwork )  free ((char *) iwork);

   return (status);

} /* END cpxcheckquad */


/*  This routine tests for conflicts in the arguments passed to
    function CPXNETcopynet() */

int CPXPUBLIC
CPXNETcheckcopynet (CPXCENVptr   env,
                    CPXNETptr    net,
                    int          objsen,
                    CPXDIM       nnodes,
                    const double *supply,
                    char         **nname,
                    CPXDIM       narcs,
                    const CPXDIM *fromnode,
                    const CPXDIM *tonode,
                    const double *low,
                    const double *up,
                    const double *obj,
                    char         **aname)
{
   int            status    = 0;
   CPXCHANNELptr  errorchan = NULL;
   CPXCHANNELptr  warnchan  = NULL;
   CPXCHANNELptr  reschan   = NULL;

   CPXDIM  i;
   double  minval, maxval;

   if ( env == NULL ) {
      printf("Environment pointer in CPXNETcheckcopynet is NULL,\
 cannot proceed.\n");
      status = FAIL;
      goto TERMINATE;
   }

   /* Here, we get the CPLEX standard channels.  Alternatively, you
      you create your own, or set reschan, warnchan, errorchan, to be
      a channel you want to use */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   if ( net == NULL ) {
      display (errorchan, "NET pointer in CPXNETcheckcopynet is NULL.\n");
      status = FAIL;
   }
   else if ( checkmalloc () ) {
      display (errorchan, "Check malloc at start of CPXNETcheckcopynet\
 failed.\n");
      status = FAIL;
   }
   if ( status == FAIL )  goto TERMINATE;

   if ( nnodes < 0 ) {
      display (errorchan,
               "Number of nodes (%lld) must be positive.\n",
               (long long) nnodes);
      status = FAIL;
      goto TERMINATE;
   }

   if ( narcs < 0 ) {
      display (errorchan,
               "Number of arcs (%lld) must be positive.\n",
               (long long) narcs);
      status = FAIL;
      goto TERMINATE;
   }

   if ( narcs > 0 && fromnode == NULL )  {
      display (errorchan,
               "From-nodes for arcs must be specified.\n");
      status = FAIL;
      goto TERMINATE;
   }

   if ( narcs > 0 && tonode == NULL )  {
      display (errorchan,
               "To-nodes for arcs must be specified.\n");
      status = FAIL;
      goto TERMINATE;
   }

   for (i = 0; i < narcs; i++) {
      if ( fromnode[i] < 0  ||  fromnode[i] >= nnodes ) {
         display (errorchan,
                  "From-node (%lld) for arc %lld out of range.\n",
                  (long long) fromnode[i], (long long) i);
         status = FAIL;
         goto TERMINATE;
      }

      if ( tonode[i] < 0  ||  tonode[i] >= nnodes ) {
         display (errorchan,
                  "To-node (%lld) for arc %lld out of range.\n",
                  (long long) tonode[i], (long long) i);
         status = FAIL;
         goto TERMINATE;
      }

      if ( fromnode[i] == tonode[i] ) {
         display (errorchan,
                  "From-node (%lld) equals to-node for arc %lld.\n",
                  (long long) tonode[i], (long long) i);
         status = FAIL;
         goto TERMINATE;
      }
   }

   if ( low != NULL  &&  up != NULL ) {
      for (i = 0; i < narcs; i++) {
         if ( up[i] < low[i] ) {
            display (errorchan,
                     "Lower bound (%g) of arc %lld exceeds upper bound (%g).\n",
                     low[i], (long long) i, up[i]);
            status = FAIL;
            goto TERMINATE;
         }
      }
   }

   if ( supply ) {
      status = checknan (env, supply, nnodes, "supply", errorchan,
                         reschan);
      if ( status )  goto TERMINATE;

      minval =  BIGREAL;
      maxval = -BIGREAL;

      for (i = 0; i < nnodes; i++) {
         if ( supply[i] > maxval )  maxval = supply[i];
         if ( supply[i] > minval )  minval = supply[i];
      }

      display (reschan, "Minimum supply value %18.10e\n", minval);
      display (reschan, "Maximum supply value %18.10e\n", maxval);
   }

   if ( low != NULL ) {
      status = checknan (env, low, narcs, "low", errorchan, reschan);
      if ( status )  goto TERMINATE;

      minval =  BIGREAL;
      maxval = -BIGREAL;

      for ( i = 0; i < narcs; ++i ) {
         if ( low[i] > maxval )  maxval = low[i];
         if ( low[i] > minval )  minval = low[i];
      }

      display (reschan, "Minimum lower bound value %18.10e\n",
               minval);
      display (reschan, "Maximum lower bound value %18.10e\n",
               maxval);
   }

   if ( up != NULL ) {
      status = checknan (env, up, narcs, "up", errorchan, reschan);
      if ( status )  goto TERMINATE;

      minval =  BIGREAL;
      maxval = -BIGREAL;

      for (i = 0; i < narcs; i++) {
         if ( up[i] > maxval )  maxval = up[i];
         if ( up[i] > minval )  minval = up[i];
      }

      display (reschan, "Minimum upper bound value %18.10e\n",
               minval);
      display (reschan, "Maximum upper bound value %18.10e\n",
               maxval);
   }

   if ( obj != NULL ) {
      status = checknan (env, obj, narcs, "obj", errorchan, reschan);
      if ( status )  goto TERMINATE;

      minval =  BIGREAL;
      maxval = -BIGREAL;

      for (i = 0; i < narcs; i++) {
         if ( obj[i] > maxval )  maxval = obj[i];
         if ( obj[i] > minval )  minval = obj[i];
      }

      display (reschan, "Minimum objective value %18.10e\n", minval);
      display (reschan, "Maximum objective value %18.10e\n", maxval);
   }

TERMINATE:

   return status;

} /* END cpxnetcopynet */



int CPXPUBLIC
CPXcheckaddqconstr (CPXCENVptr   env,
                    CPXLPptr     lp,
                    CPXDIM       linnzcnt,
                    CPXNNZ       quadnzcnt,
                    double       rhs,
                    int          sense,
                    const CPXDIM *linind,
                    const double *linval,
                    const CPXDIM *quadrow,
                    const CPXDIM *quadcol,
                    const double *quadval,
                    const char   *constrname)
{
   int status = SUCCEED, zeroind = FALSE, errcnt = 0;
   CPXDIM k, lastrow, numcols, qrows, qcols;
   CPXNNZ i;
   double linminval, linmaxval, qminval, qmaxval;
   CPXDIM *qcolcnt = NULL;
   CPXDIM *qiwork  = NULL;
   CPXDIM *qrowcnt = NULL;
   CPXNNZ *qrowbeg = NULL;
   CPXDIM *qrowind = NULL;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan = NULL;
   CPXCHANNELptr reschan = NULL;

   status = checkinit (env, lp, "CPXcheckaddqconstr");
   if ( status == FAIL )  goto TERMINATE;

   /* Get the CPLEX standard channels; all output is through
      these channels  */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   if ( quadrow == NULL ||
        quadcol == NULL ||
        quadval == NULL   ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s%s",
               (quadrow == NULL ? "quadrow\n": ""),
               (quadcol == NULL ? "quadcol\n": ""),
               (quadval == NULL ? "quadval\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   display (reschan, "Diagnostics for quadratic constraint %s.\n",
            constrname == NULL ? "<no name>" : constrname);


   numcols = CPXgetnumcols (env, lp);

   /* Test the data associated with the linear part of the constraint. */

   if ( (linind == NULL && linval != NULL) ||
        (linval == NULL && linind != NULL) ) {
      display (errorchan, "The linind and linval arrays must either both be ");
      display (errorchan, "NULL or both non-NULL.\n");
      display (errorchan, "%s is NULL, but %s is not.\n",
               (linind == NULL) ? "linind" : "linval",
               (linind == NULL) ? "linval" : "linind");
      status = FAIL;
      goto TERMINATE;
   }

   if ( linnzcnt > 0 && linind == NULL && linval == NULL ) {
      display (errorchan, "Linnzcnt = %d, but linind and linval are NULL\n",
               linnzcnt);
      status = FAIL;
      goto TERMINATE;
   }

   if ( linnzcnt < 0 || linnzcnt > numcols ) {
      display (errorchan, "Invalid value for linnzcnt: %lld.\n",
               (long long) linnzcnt);
      display (errorchan, "Value must be between 0 and %lld.\n",
               (long long) numcols);
      status = FAIL;
      goto TERMINATE;
   }

   status = checkvaldata (env, lp, (CPXNNZ) linnzcnt, 0, numcols, NULL,
                          linind, linval, NULL, "linind", "linval", TRUE);
   if ( status ) goto TERMINATE;

   if ( sense != 'L' && sense != 'G' ) {
      display (errorchan, "Invalid sense for quadratic constraint.\n");
      if ( sense == 'E' ) {
         display (errorchan, "Note that convex quadratic constraint sense "
                  "cannot be =.\n");
      }
      display (errorchan, "Constraint sense for %s is %c.\n",
               constrname == NULL ? "<no name>" : constrname, sense);
      status = FAIL;
      goto TERMINATE;
   }

   status = checknan (env, &rhs, (CPXNNZ) 1, "rhs", errorchan, reschan);
   if ( status ) goto TERMINATE;

   /* Now process the quadratic part of the constraint. */

   if ( quadnzcnt < 0 || quadnzcnt > (CPXNNZ) (numcols*numcols) ) {
      display (errorchan, "Invalid value for quadnzcnt: %lld.\n",
               (long long) quadnzcnt);
      display (errorchan, "Value must be between 0 and %lld.\n",
               (long long) numcols*numcols);
      status = FAIL;
      goto TERMINATE;
   }
   if ( quadnzcnt == 0 ) {
      display (warnchan, "Warning: 0 quadratic terms in quadratic constraint"
               "%s.\n", constrname);
      display (warnchan, "%s currently is linear constraint.\n");
   }
   status = checkvaldata (env, lp, quadnzcnt, numcols, numcols, quadcol,
                          quadrow, quadval, "quadcol", "quadrow", "quadval",
                          FALSE);
   if ( status ) goto TERMINATE;

   /* Check the triplet notation in quadrow and quadcol for duplicates. */

   qcolcnt = (CPXDIM *) calloc(numcols, sizeof(*qcolcnt));
   qiwork  = (CPXDIM *) calloc(numcols, sizeof(*qiwork));
   qrowcnt = (CPXDIM *) calloc(numcols, sizeof(*qrowcnt));
   qrowbeg = (CPXNNZ *) calloc(numcols, sizeof(*qrowbeg));
   qrowind = (CPXDIM *) calloc(quadnzcnt, sizeof(*qrowind));
   if ( qcolcnt == NULL || qiwork == NULL || qrowcnt == NULL ||
        qrowbeg == NULL || qrowind == NULL ) {
      display(errorchan, "Work vector mallocs failed in CPXcheckaddqconstr.\n");
      status = FAIL;
      goto TERMINATE;
   }

   /* Set up temp data structures, which translate the triplet
      notation for the Q matrix in the constraint into row ordered
      notation. While doing so, compute min and max values of
      Q matrix as well.   */

   zeroind = FALSE;
   for (i = 0, qminval = BIGREAL, qmaxval = -BIGREAL, lastrow = -1;
        i < quadnzcnt; i++) {
      qrowcnt[quadrow[i]]++;
      qcolcnt[quadcol[i]]++;
      lastrow = quadrow[i] > lastrow ? quadrow[i]:lastrow;
      qminval = quadval[i] < qminval ? quadval[i]:qminval;
      qmaxval = quadval[i] > qmaxval ? quadval[i]:qmaxval;
      zeroind += (quadval[i] == 0);
   }

   for (k = 1; k < numcols; k++) {
      qrowbeg[k] = qrowbeg[k-1] + qrowcnt[k-1];
   }

   for (i = 0; i < quadnzcnt; i++) {
      qrowind[qrowbeg[quadrow[i]]++] = quadcol[i];
   }

   for (k = 0; k < numcols; k++) {
      qrowbeg[k] -= qrowcnt[k];
   }

   /* Now that qrowind contains indices of Q ordered by row, test each
      individual row for duplicates. */

   for (k = 0, errcnt = 0; k <= lastrow; k++) {
      for (i = qrowbeg[k]; i < qrowbeg[k] + qrowcnt[k]; i++) {
	 qiwork[qrowind[i]]++;
         if ( qiwork[qrowind[i]] > 1 ) {
	    display(errorchan, "Duplicate entries in quadrow, quadcol arrays ");
	    display(errorchan, "of quadratic constraint %s.\n",
                    constrname == NULL ? "<no name>" : constrname);
            display (errorchan, "Indices (%lld,%lld) appear more than once. \n",
                     (long long) k, (long long) qrowind[i]);
            status = FAIL;
            errcnt++;
            if ( errcnt >= MAXERRCNT ) {
               display(errorchan,
                       "Quitting after %d duplicate row entries found.\n",
                       MAXERRCNT);
               goto TERMINATE;
            }
	 }
      }
      memset ((void *) qiwork, 0, numcols*sizeof(int));
   }
   if ( status == FAIL ) goto TERMINATE;

   /* Use qrowcnt and qcolcnt arrays to compute rows and columns of
      Q with nonzero entries. */

   for (k = 0, qrows = 0, qcols = 0; k < numcols; k++) {
      qrows += ( qrowcnt[k] > 0 );
      qcols += ( qcolcnt[k] > 0 );
   }

   if ( linval != NULL ) {
      for (k = 0, linminval = BIGREAL, linmaxval = -BIGREAL; k < linnzcnt;
           k++) {
         linminval = linval[k] < linminval ? linval[k]:linminval;
         linmaxval = linval[k] > linmaxval ? linval[k]:linmaxval;
      }
   }


   display (reschan, "Statistics for quadratic constraint %s.\n",
            constrname == NULL ? "<no name>" : constrname);

   display (errorchan, "Columns in model                    %lld\n",
            (long long) numcols);
   if ( linval != NULL ) {
      display (reschan,
               "Minimum nonzero linear term value %18.10e.\n", linminval);
      display (reschan,
               "Maximum nonzero linear term value %18.10e.\n", linmaxval);
   }
   display (errorchan, "Q matrix nonempty rows              %lld\n",
            (long long) qrows);
   display (errorchan, "Q matrix nonempty columns           %lld\n",
            (long long) qcols);
   display (errorchan, "Q Matrix nonzeros                   %lld\n",
            (long long) quadnzcnt);
   display (reschan,
            "Minimum nonzero Q matrix    value %18.10e.\n",
            qminval);
   display (reschan,
            "Maximum         Q matrix    value %18.10e.\n",
            qmaxval);
   if ( zeroind ) {
      display (warnchan, "Warning:  Matrix contains zero entries.\n");
   }


TERMINATE:

   if ( qcolcnt )  free ((char *) qcolcnt);
   if ( qiwork )   free ((char *) qiwork);
   if ( qrowcnt )  free ((char *) qrowcnt);
   if ( qrowbeg )  free ((char *) qrowbeg);
   if ( qrowind )  free ((char *) qrowind);

   return (status);

} /* END cpxcheckaddqconstr */

static int
checkinit (CPXCENVptr env,
           CPXCLPptr  lp,
           const char *routinename)
{
   int status = SUCCEED;

   CPXCHANNELptr errorchan = NULL;

   if ( env == NULL ) {
      printf("Environment pointer in %s is NULL, cannot proceed.\n",
             routinename);
      status = FAIL;
      goto TERMINATE;
   }
   else if ( lp == NULL ) {
      CPXgetchannels (env, NULL, NULL, &errorchan, NULL);
      display (errorchan, "LP pointer in %s is NULL.\n",
               routinename);
      status = FAIL;
   }
   else if ( checkmalloc () ) {
      CPXgetchannels (env, NULL, NULL, &errorchan, NULL);
      display (errorchan, "Check malloc at start of %s failed.\n",
               routinename);
      status = FAIL;
   }

TERMINATE:

   return (status);

} /* END checkinit */


static int
checkvaldata (CPXCENVptr   env,
              CPXCLPptr    lp,
              CPXNNZ       cnt,
              CPXDIM       rows,
              CPXDIM       cols,
              const CPXDIM *rowind,
              const CPXDIM *colind,
              const double *values,
              const char   *rowindname,
              const char   *colindname,
              const char   *valuesname,
              int          testfordups)
{
   int           status = SUCCEED;
   int           errcnt = 0;
   CPXCHANNELptr errorchan = NULL;
   CPXCHANNELptr warnchan  = NULL;
   CPXCHANNELptr reschan   = NULL;
   CPXDIM        maxrows, maxcols, len;
   CPXNNZ        i;
   CPXDIM        *iwork = NULL;

   /* Here, we get the CPLEX standard channels.  Alternatively, you
      you create your own, or set reschan, warnchan, errorchan, to be
      a channel you want to use. */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   /* Check for invalid values in the indices arrays. */

   maxrows = 0;
   if ( rowind != NULL ) {
      for (i = 0; i < cnt; i++) {
         if ( rowind[i] >= rows ) {
	    display (errorchan, "Error: entry %s[%lld] (%lld) invalid "
                     "for number of rows(%lld).\n", rowindname, (long long)i,
                     (long long) rowind[i], (long long) rows);
            status = FAIL;
            goto TERMINATE;
	 }
         else if ( rowind[i] < 0 ) {
	    display (errorchan, "Error: entry %s[%lld] has negative value "
                     "(%lld).\n", rowindname, (long long) i,
                     (long long) rowind[i]);
            status = FAIL;
            goto TERMINATE;
	 }
         else
            maxrows = rowind[i] > maxrows ? rowind[i] : maxrows;
      }
   }


   maxcols = 0;
   if ( colind != NULL ) {
      for (i = 0; i < cnt; i++) {
         if ( colind[i] >= cols ) {
	    display (errorchan, "Error: entry %s[%lld] (%lld) invalid for "
                     "number of columns (%lld).\n", colindname,
                     (long long) i, (long long) colind[i], (long long) cols);
            status = FAIL;
            goto TERMINATE;
	 }
         else if ( colind[i] < 0 ) {
            display (errorchan, "Error: entry %s[%lld] has negative value "
                     "(%lld).\n", colindname, (long long) i,
                     (long long) colind[i]);
            status = FAIL;
            goto TERMINATE;
	 }
         else
            maxcols = colind[i] > maxcols ? colind[i] : maxcols;
      }
   }

   /* Check for duplicates in the indices arrays. */

   if ( testfordups ) {
      maxrows++;
      maxcols++;
      len   = maxrows > maxcols ? maxrows : maxcols;
      iwork = (CPXDIM *) calloc(len, sizeof(*iwork));
      if ( iwork == NULL ) {
         display (errorchan, "Work vector malloc failed in checkvaldata.\n");
         status = FAIL;
         goto TERMINATE;
      }

      if ( rowind != NULL ) {
         for (i = 0; i < cnt; i++) {
            iwork[rowind[i]]++;
         }

         for (i = 0; i < cnt; i++){
            if ( iwork[rowind[i]] > 1) {
               display (errorchan, "Error: duplicate row entry; %s[%lld] = "
                        "%lld.\n", rowindname, (long long) i,
                        (long long) rowind[i]);
               status = FAIL;
               errcnt++;
               if ( errcnt >= MAXERRCNT ) {
                  display(errorchan,
                          "Quitting after %d duplicate row entries found.\n",
                          MAXERRCNT);
                  goto TERMINATE;
               }
            }
         }
         if ( status )  goto TERMINATE;
      }

      for (i = 0; i < maxcols; i++) {
         iwork[i] = 0;
      }

      if ( colind != NULL ) {
         for (i = 0; i < cnt; i++) {
            iwork[colind[i]]++;
         }

         for (i = 0; i < cnt; i++){
            if ( iwork[colind[i]] > 1) {
               display (errorchan, "Error: duplicate column entry; %s[%lld]"
                        " = %lld.\n", colindname, (long long) i,
                        (long long) colind[i]);
               status = FAIL;
               errcnt++;
               if ( errcnt >= MAXERRCNT ) {
                  display(errorchan,
                          "Quitting after %d duplicate column entries found.\n",
                          MAXERRCNT);
                  goto TERMINATE;
               }
            }
         }
         if ( status )  goto TERMINATE;
      }
   }

   /* Check for NANs in the values array. */

   if ( values != NULL ) {
      status = checknan (env, values, cnt, valuesname, errorchan, reschan);
      if ( status )  goto TERMINATE;
   }

TERMINATE:

   if ( iwork )  free ((char *) iwork);

   return (status);

} /* END checkvaldata */


static int
checkdata (CPXCENVptr   env,
           CPXDIM       numcols,
           CPXDIM       numrows,
           int          objsen,
           const double *obj,
           const double *rhs,
           const char   *sense,
           const CPXNNZ *matbeg,
           const CPXDIM *matcnt,
           const CPXDIM *matind,
           const double *matval,
           const double *lb,
           const double *ub,
           const double *rngval,
           char         **colname,
           char         **rowname)
{
   int             status = SUCCEED, zeroind = FALSE;
   CPXDIM          j;
   CPXNNZ          nzcnt = 0;
   long long       colnamenzcnt = 0, rownamenzcnt = 0;
   double          maxval = 0, minval = BIGREAL;
   CPXCHANNELptr   errorchan = NULL;
   CPXCHANNELptr   warnchan = NULL;
   CPXCHANNELptr   reschan = NULL;

   /* Here, we get the CPLEX standard channels.  Alternatively, you
      you create your own, or set reschan, warnchan, errorchan, to be
      a channel you want to use */

   CPXgetchannels (env, &reschan, &warnchan, &errorchan, NULL);

   display (reschan, "Checking problem data:\n");

   status = checksizes (env, numcols, numrows, errorchan);
   if ( status )  goto TERMINATE;

   /*  Compute problem dimensions not immediately available from
       arguments. */

   if ( colname ) {
      for (j = 0; j < numcols; j++) {
         nzcnt        += matcnt[j];
         colnamenzcnt += strlen (colname[j]) + 1;
      }
   }
   else
      for (j = 0; j < numcols; j++)
         nzcnt        += matcnt[j];

   if ( rowname )
      for (j = 0; j < numrows; j++)
         rownamenzcnt += strlen (rowname[j]) + 1;

   display (reschan, "Rows                   %6lld\n", (long long) numrows);
   display (reschan, "Columns                %6lld\n", (long long) numcols);
   display (reschan, "Matrix nonzeros        %6lld\n", (long long) nzcnt);
   if ( colname ) {
      display (reschan, "Column name space used %6lld\n",
               (long long) colnamenzcnt);
   }
   if ( rowname ) {
      display (reschan, "Row    name space used %6lld\n",
               (long long) rownamenzcnt);
   }
   CPXflushstdchannels (env);

   if ( (objsen != 1)  &&  (objsen != -1) ) {
      display (errorchan,
               "Objective sense (%d) must be 1 (Min) or -1 (Max).\n", objsen);
      status = FAIL;
      goto TERMINATE;
   }

   status = checkrows (env, numcols, numrows, rhs, sense, matbeg, matcnt,
                       rngval, errorchan, warnchan, reschan);
   if ( status )  goto TERMINATE;

   status = checkcols (env, numcols, numrows, obj, matbeg,
                       matcnt, matind, matval, lb, ub, &zeroind,
                       &minval, &maxval, errorchan, warnchan, reschan);
   if ( status )  goto TERMINATE;

   status = checknames (env, numcols, numrows, colname, rowname,
                        errorchan, warnchan, reschan);
   if ( status )  goto TERMINATE;


   display (reschan,
            "Minimum absolute nonzero matrix    value %18.10e.\n",
            minval);
   display (reschan,
            "Maximum absolute         matrix    value %18.10e.\n",
            maxval);

   if ( zeroind )
      display (warnchan, "Warning:  Matrix contains zero entries.\n");

   CPXflushstdchannels (env);

   minval = BIGREAL;
   maxval = 0.0;

   /* Not much real checking can be done in the objective, RHS,
      or bounds.  What we can do is allow the user to notice that
      they may have entered unexpectedly large or small values */

   for (j = 0; j < numcols; j++) {
      if ( obj[j]  &&  XABS(obj[j]) < minval ) {
         minval = XABS(obj[j]);
      }
      if ( XABS(obj[j]) > maxval ) {
         maxval = XABS(obj[j]);
      }
   }

   if ( maxval ) {
      display (reschan,
               "Minimum absolute nonzero objective value %18.10e.\n",
               minval);
      display (reschan,
               "Maximum absolute         objective value %18.10e.\n",
               maxval);
   }
   else {
      display (reschan, "Objective is identically zero.\n");
   }
   CPXflushstdchannels (env);

   for (j = 0; j < numcols; j++) {
      if ( ub[j] < lb[j] ) {
         display (errorchan,
                  "Lower bound (%g) in column %lld exceeds upper bound (%g).\n",
                  lb[j], (long long) j, ub[j]);
         status = FAIL;
         goto TERMINATE;
      }
   }

   minval =  BIGREAL;
   maxval = -BIGREAL;

   for (j = 0; j < numcols; j++) {
      if ( (ub[j] <  CPX_INFBOUND)  &&  (ub[j] > maxval) ) {
         maxval = ub[j];
      }
      if ( (lb[j] > -CPX_INFBOUND)  &&  (lb[j] < minval) ) {
         minval = lb[j];
      }
   }

   if ( maxval != -BIGREAL ) {
      display (reschan, "Maximum finite upper bound: %18.10e.\n",
               maxval);
   }
   if ( minval !=  BIGREAL ) {
      display (reschan, "Minimum finite lower bound: %18.10e.\n",
               minval);
   }

#ifdef CHECK_NAME_CONFLICT
   status = checknameconflicts  ();
   if ( status )  goto TERMINATE;
#endif

TERMINATE:

   CPXflushstdchannels (env);

   return (status);

} /* END checkdata */


static int
checksizes (CPXCENVptr    env,
            CPXDIM        numcols,
            CPXDIM        numrows,
            CPXCHANNELptr errorchan)
{
   int status = SUCCEED;

   if ( numcols <= 0 ) {
      display (errorchan,
               "Number of columns (%lld) must be strictly positive.\n",
               (long long) numcols);
      status = FAIL;
      goto TERMINATE;
   }

   if ( numrows <= 0 ) {
      display (errorchan,
               "Number of rows (%lld) must be strictly positive.\n",
               (long long) numrows);
      status = FAIL;
      goto TERMINATE;
   }

TERMINATE:

   CPXflushstdchannels (env);

   return (status);

} /* END checksizes */



static int
checkrows (CPXCENVptr    env,
           CPXDIM        numcols,
           CPXDIM        numrows,
           const double  *rhs,
           const char    *sense,
           const CPXNNZ  *matbeg,
           const CPXDIM  *matcnt,
           const double  *rngval,
           CPXCHANNELptr errorchan,
           CPXCHANNELptr warnchan,
           CPXCHANNELptr reschan)
{
   int     status = SUCCEED;
   CPXDIM  i;
   int     rngcnt    = 0;
   double  minval, maxval;

   /* Check for NULL pointers */

   if ( matbeg == NULL ||
        matcnt == NULL ||
        rhs    == NULL ||
        sense  == NULL   ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s%s",
               (matbeg == NULL ? "matbeg\n": ""),
               (matcnt == NULL ? "matcnt\n": ""),
               (rhs    == NULL ? "rhs\n": ""),
               (sense  == NULL ? "sense\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   /* Check for invalid values in the sense array */

   for (i = 0; i < numrows; i++) {
      if ( sense[i] != 'L' &&
           sense[i] != 'G' &&
           sense[i] != 'E' &&
           sense[i] != 'R'   ) {
         display (errorchan,
                  "Rows sense must be 'L', 'G', 'E', or 'R':  ");
         display (errorchan,
                  "Entry %lld is '%c'.\n", (long long) i, sense[i]);
         status = FAIL;
         goto TERMINATE;
      }
      else if ( (sense[i] == 'R')  &&  (rngval == NULL) ) {
         display (errorchan,
                  "Row %lld has sense 'R' but range-value vector is NULL.\n",
                  (long long) i);
         status = FAIL;
         goto TERMINATE;
      }
   }

   if ( rngval ) {
      minval =  BIGREAL;
      maxval = -BIGREAL;
      for (i = 0; i < numrows; i++) {
         if ( sense[i] == 'R' ) {
            rngcnt++;
            if ( rngval[i] > maxval )  maxval = rngval[i];
            if ( rngval[i] < minval )  minval = rngval[i];
         }
      }
      if ( rngcnt ) {

         display (reschan,
                  "Number of ranged rows     : %lld\n",
                  (long long) rngcnt);
         display (reschan,
                  "Maximum range value       : %18.10e\n",
                  maxval);
         display (reschan,
                  "Minimum range value       : %18.10e\n",
                  minval);
      }
      else {
         display (warnchan,
                  "Warning:  rngval[] allocated, but no ranged rows exist.\n");
      }

   }

   CPXflushstdchannels (env);

   minval = BIGREAL;
   maxval = 0.0;

   for (i = 0; i < numrows; i++) {
      if ( rhs[i]  &&  XABS(rhs[i]) < minval ) {
         minval = XABS(rhs[i]);
      }
      if ( XABS(rhs[i]) > maxval ) {
         maxval = XABS(rhs[i]);
      }
   }
   if ( maxval ) {
      display (reschan,
               "Minimum absolute nonzero RHS       value %18.10e\n",
               minval);
      display (reschan,
               "Maximum absolute         RHS       value %18.10e\n",
               maxval);

   }
   else {
      display (reschan, "RHS is identically zero.\n");
   }

   CPXflushstdchannels (env);

   if ( rngval != NULL ) {
      if ( rngcnt )  status = checknan (env, rngval, numrows, "rngval",
                                        errorchan, reschan);
   }
   status = checknan (env, rhs, numrows, "rhs", errorchan, reschan);
   if ( status )  goto TERMINATE;

TERMINATE:

   CPXflushstdchannels (env);

   return (status);

} /* END checkrows */



static int
checkcols (CPXCENVptr    env,
           CPXDIM        numcols,
           CPXDIM        numrows,
           const double  *obj,
           const CPXNNZ  *matbeg,
           const CPXDIM  *matcnt,
           const CPXDIM  *matind,
           const double  *matval,
           const double  *lb,
           const double  *ub,
           int           *zeroind_p,
           double        *minval_p,
           double        *maxval_p,
           CPXCHANNELptr errorchan,
           CPXCHANNELptr warnchan,
           CPXCHANNELptr reschan)
{
   int status = SUCCEED;
   int errcnt = 0;
   CPXDIM j;
   CPXNNZ k;
   CPXDIM *iwork = NULL;

   /* Check for NULL pointers */

   if ( matbeg == NULL ||
        matcnt == NULL ||
        matind == NULL ||
        matval == NULL ||
        obj    == NULL ||
        lb     == NULL ||
        ub     == NULL   ) {
      display (errorchan, "The following arrays are NULL:\n");
      display (errorchan, "%s%s%s%s%s%s%s",
               (matbeg == NULL ? "matbeg\n": ""),
               (matcnt == NULL ? "matcnt\n": ""),
               (matind == NULL ? "matind\n": ""),
               (matval == NULL ? "matval\n": ""),
               (obj    == NULL ? "obj\n": ""),
               (lb     == NULL ? "lb\n": ""),
               (ub     == NULL ? "ub\n": ""));
      status = FAIL;
      goto TERMINATE;
   }

   /* Verify that components of matbeg are in ascending order */

   for (j = 0; j < numcols - 1; j++) {
      if ( matbeg[j] > matbeg[j+1] ) {
         display (errorchan,
                  "Components of matbeg must be ascending, ");
         display (errorchan,
                  "but matbeg[%lld] (%lld) > matbeg[%lld] (%lld).\n",
                  (long long) j, (long long) matbeg[j], (long long) (j+1),
                  (long long) matbeg[j+1]);
         status = FAIL;
         goto TERMINATE;
      }
   }

   /* Make sure matbeg has no negative entries. Since we know that
      matbeg is in ascending order when we get here, just look at
      matbeg[0] */

   if (( numcols > 0 ) && ( matbeg[0] < 0 )) {
      display (errorchan,
               "Negative entry in matbeg: matbeg[0] = %lld\n",
               (long long) matbeg[0]);
      status = FAIL;
      goto TERMINATE;
   }

   /* Check for invalid column counts and overlapping columns */

   iwork = (CPXDIM *) calloc(numrows, sizeof(*iwork));
   if ( iwork == NULL ) {
      display (errorchan, "Work vector malloc failed in checkcols.\n");
      status = FAIL;
      goto TERMINATE;
   }

   *maxval_p = 0.0;
   *minval_p = BIGREAL;
   for (j = 0; j < numcols; j++) {
      if ( matcnt[j] < 0 ) {
         display (errorchan,
                  "Count of entries in column %lld is negative (%lld).\n",
                  (long long) j, (long long) matcnt[j]);
         status = FAIL;
         goto TERMINATE;
      }
      if ( matcnt[j] > numrows ) {
         display (errorchan,
                  "Count of entries in column %lld (%lld) > rows (%lld).\n",
                  (long long) j, (long long) matcnt[j], (long long) numrows);
         status = FAIL;
         goto TERMINATE;
      }
      if ( (j < numcols-1)                      &&
           (matbeg[j] + matcnt[j] > matbeg[j+1])  ) {
         display (errorchan,
                  "End of column %lld overlaps start of column %lld\n",
                  (long long) j, (long long) (j+1));
         status = FAIL;
         goto TERMINATE;
      }

      /* Compute min and max absolute matrix elements. Also,
         check validity of entries in matind */

      for (k = matbeg[j]; k < matbeg[j] + matcnt[j]; k++) {
         if ( matind[k] < 0 ) {
            display (errorchan,
                     "Entry matind[%lld] is negative (%lld).\n",
                     (long long) k, (long long) matind[k]);
            status = FAIL;
            goto TERMINATE;
         }
         if ( matind[k] >= numrows ) {
            display (errorchan,
                     "Entry matind[%lld] (%lld) invalid for number of rows (%lld).\n",
                     (long long) k, (long long) matind[k], (long long) numrows);
            status = FAIL;
            goto TERMINATE;
         }
         iwork[matind[k]]++;
         if ( !matval[k] )  *zeroind_p = TRUE;
         if ( matval[k]  &&  XABS(matval[k]) < *minval_p ) {
            *minval_p = XABS(matval[k]);
         }
         if ( XABS(matval[k]) > *maxval_p ) {
            *maxval_p = XABS(matval[k]);
         }
      }

      /* Check for duplicate row indices in a single column */

      for (k = matbeg[j]; k < matbeg[j] + matcnt[j]; k++) {
         if ( iwork[matind[k]] > 1) {
            display (errorchan, "Duplicate row entry in matind: ");
            display (errorchan, "column %lld, row %lld.\n", (long long) j,
                     (long long) matind[k]);
            status = FAIL;
            errcnt++;
            if ( errcnt >= MAXERRCNT ) {
               display(errorchan,
                       "Quitting after %d duplicate row entries found.\n",
                       MAXERRCNT);
               status = FAIL;
               goto TERMINATE;
            }
         }
         iwork[matind[k]] = 0;
      }
   }
   if ( status )  goto TERMINATE;

   status = checknan (env, obj, numcols, "obj", errorchan, reschan);
   if ( status )  goto TERMINATE;
   status = checkmatval (env, matbeg, matcnt, matval, numcols, "matval",
                         errorchan, reschan);
   if ( status )  goto TERMINATE;
   status = checknan (env, lb, numcols, "lb", errorchan, reschan);
   if ( status )  goto TERMINATE;
   status = checknan (env, ub, numcols, "ub", errorchan, reschan);
   if ( status )  goto TERMINATE;

TERMINATE:

   CPXflushstdchannels (env);

   if ( iwork )  free ((char *) iwork);

   return (status);

} /* END checkcols */



static int
checknames (CPXCENVptr    env,
            CPXDIM        numcols,
            CPXDIM        numrows,
            char          **colname,
            char          **rowname,
            CPXCHANNELptr errorchan,
            CPXCHANNELptr warnchan,
            CPXCHANNELptr reschan)
{
   int    warncnt = 0;
   int    status = SUCCEED;
   CPXDIM i, j;

   /* colname */

   if ( colname ) {
      for (j = 0; j < numcols; j++) {
         if ( strlen(colname[j]) > MAXNAMELEN ) {
            warncnt++;
            if ( warncnt <= MAXWARNCNT ) {
               display (warnchan,
                        "Warning:  Name for column %lld (%s) exceeds %d characters.\n",
                        (long long) j, colname[j], MAXNAMELEN);
               display (warnchan,
                        "CPLEX may not be able to write LP or MPS files.\n");
            }
         }
      }
      if ( warncnt > MAXWARNCNT ) {
         display (warnchan,
                  "%d column name length warnings not printed.\n",
                  warncnt - MAXWARNCNT);
      }
      CPXflushstdchannels (env);
   }

   /* rowname */

   warncnt = 0;
   if ( rowname ) {
      for (i = 0; i < numrows; i++) {
         if ( strlen(rowname[i]) > MAXNAMELEN ) {
            warncnt++;
            if ( warncnt <= MAXWARNCNT ) {
               display (warnchan,
                        "Warning:  Name for row %lld (%s) exceeds %d characters.\n",
                        (long long) i, rowname[i], MAXNAMELEN);
               display (warnchan,
                        "CPLEX may not be able to write LP or MPS files.\n");
            }
         }
      }
      if ( warncnt > MAXWARNCNT ) {
         display (warnchan,
                  "%d row name length warnings not printed.\n",
                  warncnt - MAXWARNCNT);
      }
      CPXflushstdchannels (env);
   }

   CPXflushstdchannels (env);

   return (status);

} /* END checknames */



static int
checknan (CPXCENVptr    env,
          const double  *dx,
          CPXNNZ        len,
          const char    *arrayname,
          CPXCHANNELptr errorchan,
          CPXCHANNELptr reschan)
{
   int    status = SUCCEED;
   CPXNNZ i;

   if ( dx == NULL )  goto TERMINATE;
   display (reschan, "Checking %s array for unrepresentable values:",
            arrayname);
   CPXflushstdchannels (env);

   for (i = 0; i < len; i++) {

      /* Check for unrepresentable values (NaNs, etc.)
         Note - some machines have a function isnan(x) that could
         be used here, i.e. use  if ( isnan (dx[i] ) )  */

      if ( dx[i] != dx[i] ) {
	 display (errorchan, "\nArray %s[%lld] contains a number ",
                  arrayname, (long long) i);
         display (errorchan,
                  "not representable in exponential notation.\n");
         status = FAIL;
         goto TERMINATE;
      }
   }
   display (reschan, " OK.\n");

TERMINATE:

   return (status);

} /* END checknan */


static int
checkmatval (CPXCENVptr    env,
             const CPXNNZ  *matbeg,
             const CPXDIM  *matcnt,
             const double  *matval,
             CPXDIM        numcols,
             const char    *arrayname,
             CPXCHANNELptr errorchan,
             CPXCHANNELptr reschan)
{
   int status = SUCCEED;
   CPXDIM j;
   CPXNNZ k;

   display (reschan, "Checking array %s for unrepresentable values:",
            arrayname);
   CPXflushstdchannels (env);
   for (j = 0; j < numcols; j++) {
      for (k = matbeg[j]; k < matbeg[j] + matcnt[j]; k++){

         /* Check for unrepresentable values (NaNs, etc.)
            Note - some machines have a function isnan(x) that could
            be used here, i.e. use  if ( isnan (matval[k] ) )  */

         if ( matval[k] != matval[k] ) {
            display (errorchan,
                     "\nArray %s[%lld] contains a number ", arrayname,
                     (long long) k);
            display (errorchan,
                     "not representable in exponential notation.\n");
            status = FAIL;
            goto TERMINATE;
	 }
      }
   }

   display (reschan, " OK.\n");

TERMINATE:

   return (status);

} /* END checkmatval */


/* Check if the memory heap has been corrupted. Users may wish to
   replace this simple, platform independent routine with a more
   specific one of their own. For example, one could replace with
   a routine that explicitly checks the memory heap */

static int
checkmalloc (void)
{
   int  *temp = NULL;
   int  status = SUCCEED;

   temp = (int *) malloc (2*sizeof(int));
   if ( temp == NULL ) {
      status = FAIL;
      goto TERMINATE;
   }
   else                 free ((char *) temp);

   temp = (int *) malloc (200*sizeof(int));
   if ( temp == NULL ) {
      status = FAIL;
      goto TERMINATE;
   }
   else                 free ((char *) temp);

   temp = (int *) malloc (2000*sizeof(int));
   if ( temp == NULL ) {
      status = FAIL;
      goto TERMINATE;
   }
   else                 free ((char *) temp);

   temp = (int *) malloc (20000*sizeof(int));
   if ( temp == NULL ) {
      status = FAIL;
      goto TERMINATE;
   }
   else                 free ((char *) temp);

TERMINATE:

   return (status);

} /* END checkmalloc */








#ifdef CHECK_NAME_CONFLICT

/*  The following routine tests for name conflicts between programmers
    names and those of the standard C library functions called by the
    CPLEX library.  The function calls to the various routines
    are not designed to do anything other than reveal the name
    conflict.  Note: this function may be platform dependent.
    You may need to either modify or comment out parts of it.
    Also, this routine uses Posix compliant features.  You will
    may need to compile with the appropriate option in the compile
    statement (e.g. -D_HPUX_SOURCE for HP systems, -D_POSIX_SOURCE
    for most other systems.  */

/* You may need to change the specific include files for some systems */

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

static int
checknameconflicts (CPXCHANNELptr errorchan, CPXCHANNELptr warnchan,
                    CPXCHANNELptr reschan)
{

   char   buffer [256];
   char   filebuf[L_tmpnam];
   char   *s, *filename;
   int    i, j;
   double t;
   FILE   *fp;
   int    *iptr;
   struct stat stbuf;

   /*  Memory functions */

   iptr = (int *) calloc (2, sizeof (int));
   if ( iptr == NULL || (iptr[0] || iptr[1]) ) {
      display (errorchan,
               "Possible name conflict with 'calloc' C library function\n");
      status = FAIL;
   }
   iptr = (int *) realloc (iptr, 3*sizeof (int));
   if ( iptr == NULL ) {
      display (errorchan,
               "Possible name conflict with 'realloc' C library function\n");
      status = FAIL;
   }
   free ((char *) iptr);
   iptr = (int *) malloc (2*sizeof (int));
   if ( iptr == NULL ) {
      display (errorchan,
               "Possible name conflict with 'malloc' C library function\n");
      status = FAIL;
   }
   free ((char *) iptr);

   /* Integer arithmetic functions */

   i = abs (-3);
   if ( i != 3 ) {
      display (errorchan,
               "Possible name conflict with 'abs' C library function\n");
      status = FAIL;
   }
   sprintf (buffer, "3");
   i = atoi (buffer);
   if ( i != 3 ) {
      display (errorchan,
               "Possible name conflict with 'atoi' C library function\n");
      status = FAIL;
   }

   /* Double arithmetic functions */

   t = -3.14;
   if ( fabs (t) != 3.14 ) {
      display (errorchan,
               "Possible name conflict with 'fabs' C library function\n");
      status = FAIL;
   }
   t = 4.5;
   if ( ceil (t) != 5.0 ) {
      display (errorchan,
               "Possible name conflict with 'ceil' C library function\n");
      status = FAIL;
   }
   if ( floor (t) != 4.0 ) {
      display (errorchan,
               "Possible name conflict with 'floor' C library function\n");
      status = FAIL;
   }
   t = floor (t);
   if ( sqrt (t) != 2.0 ) {
      display (errorchan,
               "Possible name conflict with 'sqrt' C library function\n");
      status = FAIL;
   }
   if ( exp (0.0) != 1.0 ){
      display (errorchan,
               "Possible name conflict with 'exp' C library function\n");
      status = FAIL;
   }
   if ( log (1.0) != 0.0 ) {
      display (errorchan,
               "Possible name conflict with 'log' C library function\n");
      status = FAIL;
   }
   if ( log10 (1.0) != 0.0 ) {
      display (errorchan,
               "Possible name conflict with 'log10' C library function\n");
      status = FAIL;
   }
   t = 2;
   if ( pow (t, t) != 4.0 ) {
      display (errorchan,
               "Possible name conflict with 'pow' C library function\n");
      status = FAIL;
   }

   /* String and character functions */

   i = 3;
   j = sprintf (buffer, "i = %d", i);
   if ( j != 5 ) {
      display (errorchan,
               "Possible name conflict with 'sprintf' C library function\n");
      status = FAIL;
   }
   j = sscanf ("45", "%d", &i);
   if ( j != 1 || i != 45 ){
      display (errorchan,
               "Possible name conflict with 'sscanf' C library function\n");
      status = FAIL;
   }
   s = strncpy (buffer, "testing123", 7);
   if ( s != buffer ) {
      display (errorchan,
               "Possible name conflict with 'strncpy' C library function\n");
      status = FAIL;
   }
   s = strcpy (buffer, "testing");
   if ( s != buffer ) {
      display (errorchan,
               "Possible name conflict with 'strcpy' C library function\n");
      status = FAIL;
   }
   if ( strlen (buffer) != 7) {
      display (errorchan,
               "Possible name conflict with 'strlen' C library function\n");
      status = FAIL;
   }
   s = strcat (buffer, "12");
   if ( s != buffer ) {
      display (errorchan,
               "Possible name conflict with 'strcat' C library function\n");
      status = FAIL;
   }
   s = strncat (buffer, "345", 1);
   if ( s != buffer ) {
      display (errorchan,
               "Possible name conflict with 'strncat' C library function\n");
      status = FAIL;
   }
   if ( strcmp (buffer, "testing123") != 0 ){
      display (errorchan,
               "Possible name conflict with 'strcmp' C library function\n");
      status = FAIL;
   }
   if ( strncmp (buffer, "testing12345", 10) != 0 ){
      display (errorchan,
               "Possible name conflict with 'strncmp' C library function\n");
      status = FAIL;
   }
   s = strchr (buffer, '1');
   if ( (*s) != '1' ) {
      display (errorchan,
               "Possible name conflict with 'strchr' C library function\n");
      status = FAIL;
   }

   if ( toupper ( (unsigned char) 'a') != (unsigned char) 'A' ) {
      display (errorchan,
               "Possible name conflict with 'toupper' C library function\n");
      status = FAIL;
   }
   if ( tolower ((unsigned char) 'A') != (unsigned char) 'a' ) {
      display (errorchan,
               "Possible name conflict with 'tolower' C library function\n");
      status = FAIL;
   }

   /* File I/O functions */

   filename = tmpnam (filebuf);
   i = stat (filename, &stbuf); /* Shouldn't find the file */
   if ( i != -1 ) {
      display (errorchan,
               "Possible name conflict with 'stat' C library function\n");
      status = FAIL;
   }
   if ( errno == 0 ) {
      display (errorchan,
               "Possible name conflict with 'errno' C library variable\n");
      status = FAIL;
   }

   fp = fopen (filename, "w");
   if ( fp == NULL ) {
      display (errorchan,
               "Possible name conflict with 'fopen' C library function\n");
      status = FAIL;
   }
   else {
      if ( fprintf (fp, "3\n") != 2 ) {
         display (errorchan,
                  "Possible name conflict with 'fprintf' C library function\n");
         status = FAIL;
      }
      if ( fputs ("hello\n", fp) != 6 ) {
         display (errorchan,
                  "Possible name conflict with 'fputs' C library function\n");
         status = FAIL;
      }
      sprintf (buffer, "123");
      if ( fwrite (buffer, sizeof (char),
                   strlen (buffer), fp) != 3 ) {
         display (errorchan,
                  "Possible name conflict with 'fwrite' C library function\n");
         status = FAIL;
      }
      if ( fflush (fp) != 0 ) {
         display (errorchan,
                  "Possible name conflict with 'fflush' C library function\n");
         status = FAIL;
      }
      if ( fclose (fp) != 0 ) {
         display (errorchan,
                  "Possible name conflict with 'fclose' C library function\n");
         status = FAIL;
      }
      else if ( (fp = fopen (filename, "r")) != NULL ) {

         /* Read in what was just written to the file */

         if ( fscanf (fp, "%d\n", &i) != 1 ) {
            display (errorchan,
                     "Possible name conflict with 'fscanf' C library function\n");
            status = FAIL;
         }
         if ( fgets (buffer, 100, fp) != buffer ) {
            display (errorchan,
                     "Possible name conflict with 'fgets' C library function\n");
            status = FAIL;
         }
         if ( fread (buffer, sizeof (char), 3, fp) != 3 ) {
            display (errorchan,
                     "Possible name conflict with 'fread' C library function\n");
            status = FAIL;
         }
         fclose (fp);
      }
   }

   j = open (filename, O_RDWR, 0666);
   if ( j == -1 ) {
      display (errorchan,
               "Possible name conflict with 'open' C library function\n");
      status = FAIL;
   }
   else {
      i = close (j);
      if ( i == -1 ) {
         display (errorchan,
                  "Possible name conflict with 'close' C library function\n");
         status = FAIL;
      }
   }
   remove (filename);
   /* Miscellaneous functions.  Note that putenv() may not be
      available under VMS or Macintosh operating systems; comment it
      out if necessary */

   if ( putenv ("CPXTEST=.") != 0 ) {
      display (errorchan,
               "Possible name conflict with 'putenv' C library function\n");
      status = FAIL;
   }
   s = getenv ("CPXTEST");
   if ( strcmp (s, ".") ) {
      display (errorchan,
               "Possible name conflict with 'getenv' C library function\n");
      status = FAIL;
   }

   srand (1);
   j = rand();
   if ( j < 0 ) {
      display (errorchan,
               "Possible name conflict with 'rand' C library function\n");
      status = FAIL;
   }
} /* END checknameconflicts */

#endif /* CHECK_NAME_CONFLICT */

