/* --------------------------------------------------------------------------
 * File: xlpex4.c
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

/* xlpex4.c - Illustrating the CPLEX callback functionality.
 
   This is a modification of lpex1.c, where we use a callback
   function to print the iteration info, rather than have CPLEX
   do it; also uses cplexx.h. */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include. */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string functions */

#include <string.h>

/* Include declaration for function at end of program */

static int
   populatebycolumn  (CPXENVptr env, CPXLPptr lp);

static int CPXPUBLIC
   mycallback (CPXCENVptr env, void *cbdata, int wherefrom, 
               void *cbhandle);



/* The problem we are optimizing will have 2 rows, 3 columns 
   and 6 nonzeros.  */

#define NUMROWS    2
#define NUMCOLS    3
#define NUMNZ      6

int
main (void)
{
   char     probname[16];  /* Problem name is max 16 characters */

   /* Declare and allocate space for the variables and arrays where we
      will store the optimization results including the status, objective
      value, variable values, dual values, row slacks and variable
      reduced costs. */

   int      solstat;
   double   objval;
   double   x[NUMCOLS];
   double   pi[NUMROWS];
   double   slack[NUMROWS];
   double   dj[NUMCOLS];


   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status;
   CPXDIM        i, j;
   CPXDIM        cur_numrows, cur_numcols;

   /* Initialize the CPLEX environment */

   env = CPXXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  The error message will be printed at the end of the
      program. */

   if ( env == NULL ) {
      fprintf (stderr, "Could not open CPLEX environment.\n");
      goto TERMINATE;
   }

   /* Turn *off* output to the screen since we'll be producing it
      via the callback function.  This also means we won't see any
      CPLEX generated errors, but we'll handle that at the end of
      the program. */

   status = CPXXsetintparam (env, CPXPARAM_ScreenOutput, CPX_OFF);
   if ( status ) {
      fprintf (stderr, 
               "Failure to turn off screen indicator, error %d.\n", status);
      goto TERMINATE;
   }

   /* Create the problem. */

   strcpy (probname, "example");
   lp = CPXXcreateprob (env, &status, probname);

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of 
      failure, an error message will have been written to the error 
      channel from inside CPLEX.  In this example, we wouldn't see
      an error message from CPXXcreateprob since we turned off the 
      CPXPARAM_ScreenOutput parameter above.  The only way to see this message
      would be to use the CPLEX message handler, but that clutters up
      the simplicity of this example, which has a point of illustrating
      the CPLEX callback functionality.   */

   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }

   /* Now populate the problem with the data. */

   status = populatebycolumn (env, lp);

   if ( status ) {
      fprintf (stderr, "Failed to populate problem data.\n");
      goto TERMINATE;
   }

   status = CPXXsetlpcallbackfunc (env, mycallback, NULL);
   if ( status ) {
      fprintf (stderr, "Failed to set callback function.\n");
      goto TERMINATE;
   }

   /* Optimize the problem and obtain solution. */

   status = CPXXsetintparam (env, CPXPARAM_LPMethod, CPX_ALG_PRIMAL);
   if ( status ) {
      fprintf (stderr, 
               "Failed to set the optimization method, error %d.\n", status);
      goto TERMINATE;
   }


   status = CPXXlpopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize LP.\n");
      goto TERMINATE;
   }

   /* Turn off the callback function.  This isn't strictly necessary,
      but is good practice.  Note that the cast in front of NULL
      is only necessary for some compilers.   */

   status = CPXXsetlpcallbackfunc (env,
              (int (CPXPUBLIC *)(CPXCENVptr, void *, int, void *)) NULL, NULL);

   if ( status ) {
      fprintf (stderr, "Failed to turn off callback function.\n");
      goto TERMINATE;
   }

   status = CPXXsolution (env, lp, &solstat, &objval, x, pi, slack, dj);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution.\n");
      goto TERMINATE;
   }


   /* Write the output to the screen. */

   printf ("\nSolution status = %d\n", solstat);
   printf ("Solution value  = %f\n\n", objval);

   /* The size of the problem should be obtained by asking CPLEX what
      the actual size is, rather than using sizes from when the problem
      was built.  cur_numrows and cur_numcols store the current number 
      of rows and columns, respectively.  */

   cur_numrows = CPXXgetnumrows (env, lp);
   cur_numcols = CPXXgetnumcols (env, lp);
   for (i = 0; i < cur_numrows; i++) {
      printf ("Row %d:  Slack = %10f  Pi = %10f\n", i, slack[i], pi[i]);
   }

   for (j = 0; j < cur_numcols; j++) {
      printf ("Column %d:  Value = %10f  Reduced cost = %10f\n",
              j, x[j], dj[j]);
   }

   /* Finally, write a copy of the problem to a file. */

   status = CPXXwriteprob (env, lp, "lpex4.lp", NULL);
   if ( status ) {
      fprintf (stderr, "Failed to write LP to disk.\n");
      goto TERMINATE;
   }
   
   
TERMINATE:

   /* Free up the problem as allocated by CPXXcreateprob, if necessary */

   if ( lp != NULL ) {
      int  frstatus;
      frstatus = CPXXfreeprob (env, &lp);
      if ( frstatus ) {
         fprintf (stderr, "CPXXfreeprob failed, error code %d.\n", frstatus);
         if (( !status ) && frstatus )  status = frstatus;
      }
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      int  clstatus;
      clstatus = CPXXcloseCPLEX (&env);

      if ( clstatus ) {
         fprintf (stderr, "CPXXcloseCPLEX failed, error code %d.\n", clstatus);
         if (( !status ) && clstatus )  status = clstatus;
      }
   }

   if ( status ) {
      char  errmsg[CPXMESSAGEBUFSIZE];

      /* Note that since we have turned off the CPLEX screen indicator,
         we'll need to print the error message ourselves. */

      CPXXgeterrorstring (env, status, errmsg);
      fprintf (stderr, "%s", errmsg);
   }
     
   return (status);

}  /* END main */


/* This function builds by column the linear program:

      Maximize
       obj: x1 + 2 x2 + 3 x3
      Subject To
       c1: - x1 + x2 + x3 <= 20
       c2: x1 - 3 x2 + x3 <= 30
      Bounds
       0 <= x1 <= 40
      End
 */

static int
populatebycolumn (CPXENVptr env, CPXLPptr lp)
{
   int        status    = 0;
   double     obj[NUMCOLS];
   double     lb[NUMCOLS];
   double     ub[NUMCOLS];
   char const *colname[NUMCOLS];
   CPXNNZ     matbeg[NUMCOLS];
   CPXDIM     matind[NUMNZ];
   double     matval[NUMNZ];
   double     rhs[NUMROWS];
   char       sense[NUMROWS];
   char const *rowname[NUMROWS];

   /* To build the problem by column, create the rows, and then
      add the columns. */

   status = CPXXchgobjsen (env, lp, CPX_MAX);  /* Problem is maximization */
   if ( status ) {
      fprintf (stderr, "Could not change objective sense.\n");
      goto TERMINATE;
   }

   /* Now create the new rows.  First, populate the arrays. */

   rowname[0] = "c1";
   sense[0]   = 'L';
   rhs[0]     = 20.0;

   rowname[1] = "c2";
   sense[1]   = 'L';
   rhs[1]     = 30.0;

   status = CPXXnewrows (env, lp, NUMROWS, rhs, sense, NULL, rowname);
   if ( status )   goto TERMINATE;

   /* Now add the new columns.  First, populate the arrays. */

       obj[0] = 1.0;      obj[1] = 2.0;           obj[2] = 3.0;

    matbeg[0] = 0;     matbeg[1] = 2;          matbeg[2] = 4;

    matind[0] = 0;     matind[2] = 0;          matind[4] = 0;
    matval[0] = -1.0;  matval[2] = 1.0;        matval[4] = 1.0;

    matind[1] = 1;     matind[3] = 1;          matind[5] = 1;
    matval[1] = 1.0;   matval[3] = -3.0;       matval[5] = 1.0;

        lb[0] = 0.0;       lb[1] = 0.0;           lb[2]  = 0.0;
        ub[0] = 40.0;      ub[1] = CPX_INFBOUND;  ub[2]  = CPX_INFBOUND;

   colname[0] = "x1"; colname[1] = "x2";      colname[2] = "x3";

   status = CPXXaddcols (env, lp, NUMCOLS, NUMNZ, obj, matbeg, matind,
                        matval, lb, ub, colname);
   if ( status )  goto TERMINATE;

TERMINATE:

   return (status);

}  /* END populatebycolumn */


/* The callback function will print out the Phase of the simplex method,
   the sum of infeasibilities if in Phase 1, or the objective if in Phase 2.
   If any of our requests fails, we'll return an indication to abort.
 */

static int CPXPUBLIC
mycallback (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle)
{
   int    status = 0;
   int    phase  = -1;
   double suminf_or_objective;
   CPXCNT itcnt = -1;

   /* Silence compiler warning(s) about unused parameter(s). */

   (void)cbhandle;

   if ( wherefrom == CPX_CALLBACK_PRIMAL ) {
      status = CPXXgetcallbackinfo (env, cbdata, wherefrom, 
                                   CPX_CALLBACK_INFO_ITCOUNT_LONG, &itcnt);
      if ( status )  goto TERMINATE;

      status = CPXXgetcallbackinfo (env, cbdata, wherefrom, 
                                   CPX_CALLBACK_INFO_PRIMAL_FEAS, &phase);
      if ( status )  goto TERMINATE;

      if ( phase == 0 ) {
         status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                      CPX_CALLBACK_INFO_PRIMAL_INFMEAS,
                                      &suminf_or_objective);
         if ( status )  goto TERMINATE;

         printf ("Iteration %lld: Infeasibility measure = %f\n", 
                  itcnt, suminf_or_objective);
      }
      else {
         status = CPXXgetcallbackinfo (env, cbdata, wherefrom,
                                      CPX_CALLBACK_INFO_PRIMAL_OBJ,
                                      &suminf_or_objective);
         if ( status )  goto TERMINATE;

         printf ("Iteration %lld: Objective = %f\n", 
                  itcnt, suminf_or_objective);
      }

   }

TERMINATE:

   return (status);

} /* END mycallback */


