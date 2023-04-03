/* --------------------------------------------------------------------------
 * File: lpex1.c
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

/* lpex1.c - Entering and optimizing a problem.  Demonstrates different
    methods for creating a problem.  The user has to choose the method
    on the command line:

      lpex1  -r     generates the problem by adding rows
      lpex1  -c     generates the problem by adding columns
      lpex1  -n     generates the problem by adding a list of coefficients
 */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include. */

#include <ilcplex/cplex.h>
#include <stdlib.h>

/* Bring in the declarations for the string functions */

#include <string.h>

/* Include declaration for functions at end of program */

static int
   populatebyrow     (CPXENVptr env, CPXLPptr lp),
   populatebycolumn  (CPXENVptr env, CPXLPptr lp),
   populatebynonzero (CPXENVptr env, CPXLPptr lp);

static void
   free_and_null     (char **ptr),
   usage             (char *progname);



int
main (int argc, char **argv)
{
   /* Declare and allocate space for the variables and arrays where we
      will store the optimization results including the status, objective
      value, variable values, dual values, row slacks and variable
      reduced costs. */

   int      solstat;
   double   objval;
   double   *x = NULL;
   double   *pi = NULL;
   double   *slack = NULL;
   double   *dj = NULL;


   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status = 0;
   int           i, j;
   int           cur_numrows, cur_numcols;

   /* Check the command line arguments */

   if (( argc != 2 )                                         ||
       ( argv[1][0] != '-' )                                 ||
       ( strchr ("rcn", argv[1][1]) == NULL )  ) {
      usage (argv[0]);
      goto TERMINATE;
   }

   /* Initialize the CPLEX environment */

   env = CPXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXgeterrorstring will produce the text of
      the error message.  Note that CPXopenCPLEX produces no output,
      so the only way to see the cause of the error is to use
      CPXgeterrorstring.  For other CPLEX routines, the errors will
      be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON.  */

   if ( env == NULL ) {
      char  errmsg[CPXMESSAGEBUFSIZE];
      fprintf (stderr, "Could not open CPLEX environment.\n");
      CPXgeterrorstring (env, status, errmsg);
      fprintf (stderr, "%s", errmsg);
      goto TERMINATE;
   }

   /* Turn on output to the screen */

   status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status ) {
      fprintf (stderr, 
               "Failure to turn on screen indicator, error %d.\n", status);
      goto TERMINATE;
   }

   /* Turn on data checking */

   status = CPXsetintparam (env, CPXPARAM_Read_DataCheck,
			    CPX_DATACHECK_WARN);
   if ( status ) {
      fprintf (stderr, 
               "Failure to turn on data checking, error %d.\n", status);
      goto TERMINATE;
   }


   /* Create the problem. */

   lp = CPXcreateprob (env, &status, "lpex1");

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of 
      failure, an error message will have been written to the error 
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPXPARAM_ScreenOutput causes the error message to
      appear on stdout.  */

   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }

   /* Now populate the problem with the data.  For building large
      problems, consider setting the row, column and nonzero growth
      parameters before performing this task. */

   switch (argv[1][1]) {
      case 'r':
         status = populatebyrow (env, lp);
         break;
      case 'c':
         status = populatebycolumn (env, lp);
         break;
      case 'n':
         status = populatebynonzero (env, lp);
         break;
   }

   if ( status ) {
      fprintf (stderr, "Failed to populate problem.\n");
      goto TERMINATE;
   }

   /* Optimize the problem and obtain solution. */

   status = CPXlpopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize LP.\n");
      goto TERMINATE;
   }

   /* The size of the problem should be obtained by asking CPLEX what
      the actual size is, rather than using sizes from when the problem
      was built.  cur_numrows and cur_numcols store the current number 
      of rows and columns, respectively.  */

   cur_numrows = CPXgetnumrows (env, lp);
   cur_numcols = CPXgetnumcols (env, lp);

   x = (double *) malloc (cur_numcols * sizeof(double));
   slack = (double *) malloc (cur_numrows * sizeof(double));
   dj = (double *) malloc (cur_numcols * sizeof(double));
   pi = (double *) malloc (cur_numrows * sizeof(double));

   if ( x     == NULL ||
        slack == NULL ||
        dj    == NULL ||
        pi    == NULL   ) {
      status = CPXERR_NO_MEMORY;
      fprintf (stderr, "Could not allocate memory for solution.\n");
      goto TERMINATE;
   }

   status = CPXsolution (env, lp, &solstat, &objval, x, pi, slack, dj);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution.\n");
      goto TERMINATE;
   }

   /* Write the output to the screen. */

   printf ("\nSolution status = %d\n", solstat);
   printf ("Solution value  = %f\n\n", objval);

   for (i = 0; i < cur_numrows; i++) {
      printf ("Row %d:  Slack = %10f  Pi = %10f\n", i, slack[i], pi[i]);
   }

   for (j = 0; j < cur_numcols; j++) {
      printf ("Column %d:  Value = %10f  Reduced cost = %10f\n",
              j, x[j], dj[j]);
   }

   /* Finally, write a copy of the problem to a file. */

   status = CPXwriteprob (env, lp, "lpex1.lp", NULL);
   if ( status ) {
      fprintf (stderr, "Failed to write LP to disk.\n");
      goto TERMINATE;
   }

   
TERMINATE:

   /* Free up the solution */

   free_and_null ((char **) &x);
   free_and_null ((char **) &slack);
   free_and_null ((char **) &dj);
   free_and_null ((char **) &pi);

   /* Free up the problem as allocated by CPXcreateprob, if necessary */

   if ( lp != NULL ) {
      status = CPXfreeprob (env, &lp);
      if ( status ) {
         fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
      }
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      status = CPXcloseCPLEX (&env);

      /* Note that CPXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */

      if ( status ) {
         char  errmsg[CPXMESSAGEBUFSIZE];
         fprintf (stderr, "Could not close CPLEX environment.\n");
         CPXgeterrorstring (env, status, errmsg);
         fprintf (stderr, "%s", errmsg);
      }
   }
     
   return (status);

}  /* END main */


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
   fprintf (stderr,"Usage: %s -X\n", progname);
   fprintf (stderr,"   where X is one of the following options: \n");
   fprintf (stderr,"      r          generate problem by row\n");
   fprintf (stderr,"      c          generate problem by column\n");
   fprintf (stderr,"      n          generate problem by nonzero\n");
   fprintf (stderr," Exiting...\n");
} /* END usage */


/* These functions all populate the problem with data for the following
   linear program:

      Maximize
       obj: x1 + 2 x2 + 3 x3
      Subject To
       c1: - x1 + x2 + x3 <= 20
       c2: x1 - 3 x2 + x3 <= 30
      Bounds
       0 <= x1 <= 40
      End
 */

#define NUMROWS    2
#define NUMCOLS    3
#define NUMNZ      6


/* To populate by row, we first create the columns, and then add the
   rows.  */

static int
populatebyrow (CPXENVptr env, CPXLPptr lp)
{
   int      status    = 0;
   double   obj[NUMCOLS];
   double   lb[NUMCOLS];
   double   ub[NUMCOLS];
   char     *colname[NUMCOLS];
   int      rmatbeg[NUMROWS];
   int      rmatind[NUMNZ];
   double   rmatval[NUMNZ];
   double   rhs[NUMROWS];
   char     sense[NUMROWS];
   char     *rowname[NUMROWS];

   status = CPXchgobjsen (env, lp, CPX_MAX);  /* Problem is maximization */
   if ( status )  goto TERMINATE;

   /* Now create the new columns.  First, populate the arrays. */

       obj[0] = 1.0;      obj[1] = 2.0;          obj[2] = 3.0;

        lb[0] = 0.0;       lb[1] = 0.0;           lb[2] = 0.0;
        ub[0] = 40.0;      ub[1] = CPX_INFBOUND;  ub[2] = CPX_INFBOUND;

   colname[0] = "x1"; colname[1] = "x2";     colname[2] = "x3";

   status = CPXnewcols (env, lp, NUMCOLS, obj, lb, ub, NULL, colname);
   if ( status )  goto TERMINATE;

   /* Now add the constraints.  */

   rmatbeg[0] = 0;     rowname[0] = "c1";

   rmatind[0] = 0;     rmatind[1] = 1;    rmatind[2] = 2;    sense[0] = 'L';
   rmatval[0] = -1.0;  rmatval[1] = 1.0;  rmatval[2] = 1.0;  rhs[0]   = 20.0;

   rmatbeg[1] = 3;     rowname[1] = "c2";
   rmatind[3] = 0;     rmatind[4] = 1;    rmatind[5] = 2;    sense[1] = 'L';
   rmatval[3] = 1.0;   rmatval[4] = -3.0; rmatval[5] = 1.0;  rhs[1]   = 30.0;

   status = CPXaddrows (env, lp, 0, NUMROWS, NUMNZ, rhs, sense, rmatbeg,
                        rmatind, rmatval, NULL, rowname);
   if ( status )  goto TERMINATE;

TERMINATE:

   return (status);

}  /* END populatebyrow */



/* To populate by column, we first create the rows, and then add the
   columns.  */

static int
populatebycolumn (CPXENVptr env, CPXLPptr lp)
{
   int      status    = 0;
   double   obj[NUMCOLS];
   double   lb[NUMCOLS];
   double   ub[NUMCOLS];
   char     *colname[NUMCOLS];
   int      matbeg[NUMCOLS];
   int      matind[NUMNZ];
   double   matval[NUMNZ];
   double   rhs[NUMROWS];
   char     sense[NUMROWS];
   char     *rowname[NUMROWS];

   status = CPXchgobjsen (env, lp, CPX_MAX);  /* Problem is maximization */
   if ( status )  goto TERMINATE;

   /* Now create the new rows.  First, populate the arrays. */

   rowname[0] = "c1";
   sense[0]   = 'L';
   rhs[0]     = 20.0;

   rowname[1] = "c2";
   sense[1]   = 'L';
   rhs[1]     = 30.0;

   status = CPXnewrows (env, lp, NUMROWS, rhs, sense, NULL, rowname);
   if ( status )   goto TERMINATE;

   /* Now add the new columns.  First, populate the arrays. */

       obj[0] = 1.0;      obj[1] = 2.0;           obj[2] = 3.0;

    matbeg[0] = 0;     matbeg[1] = 2;          matbeg[2] = 4;
      
    matind[0] = 0;     matind[2] = 0;          matind[4] = 0;
    matval[0] = -1.0;  matval[2] = 1.0;        matval[4] = 1.0;
 
    matind[1] = 1;     matind[3] = 1;          matind[5] = 1;
    matval[1] = 1.0;   matval[3] = -3.0;       matval[5] = 1.0;

        lb[0] = 0.0;       lb[1] = 0.0;            lb[2] = 0.0;
        ub[0] = 40.0;      ub[1] = CPX_INFBOUND;   ub[2] = CPX_INFBOUND;

   colname[0] = "x1"; colname[1] = "x2";      colname[2] = "x3";

   status = CPXaddcols (env, lp, NUMCOLS, NUMNZ, obj, matbeg, matind,
                        matval, lb, ub, colname);
   if ( status )  goto TERMINATE;

TERMINATE:

   return (status);

}  /* END populatebycolumn */


/* To populate by nonzero, we first create the rows, then create the
   columns, and then change the nonzeros of the matrix 1 at a time.  */

static int
populatebynonzero (CPXENVptr env, CPXLPptr lp)
{
   int      status    = 0;
   double   obj[NUMCOLS];
   double   lb[NUMCOLS];
   double   ub[NUMCOLS];
   char     *colname[NUMCOLS];
   double   rhs[NUMROWS];
   char     sense[NUMROWS];
   char     *rowname[NUMROWS];
   int      rowlist[NUMNZ];
   int      collist[NUMNZ];
   double   vallist[NUMNZ];

   status = CPXchgobjsen (env, lp, CPX_MAX);  /* Problem is maximization */
   if ( status )  goto TERMINATE;

   /* Now create the new rows.  First, populate the arrays. */

   rowname[0] = "c1";
   sense[0]   = 'L';
   rhs[0]     = 20.0;

   rowname[1] = "c2";
   sense[1]   = 'L';
   rhs[1]     = 30.0;

   status = CPXnewrows (env, lp, NUMROWS, rhs, sense, NULL, rowname);
   if ( status )   goto TERMINATE;

   /* Now add the new columns.  First, populate the arrays. */

       obj[0] = 1.0;      obj[1] = 2.0;           obj[2] = 3.0;

        lb[0] = 0.0;       lb[1] = 0.0;           lb[2]  = 0.0;
        ub[0] = 40.0;      ub[1] = CPX_INFBOUND;  ub[2]  = CPX_INFBOUND;

   colname[0] = "x1"; colname[1] = "x2";      colname[2] = "x3";

   status = CPXnewcols (env, lp, NUMCOLS, obj, lb, ub, NULL, colname);
   if ( status )  goto TERMINATE;

   /* Now create the list of coefficients */

   rowlist[0] = 0;   collist[0] = 0;   vallist[0] = -1.0;
   rowlist[1] = 0;   collist[1] = 1;   vallist[1] = 1.0;
   rowlist[2] = 0;   collist[2] = 2;   vallist[2] = 1.0;
   rowlist[3] = 1;   collist[3] = 0;   vallist[3] = 1.0;
   rowlist[4] = 1;   collist[4] = 1;   vallist[4] = -3.0;
   rowlist[5] = 1;   collist[5] = 2;   vallist[5] = 1.0;

   status = CPXchgcoeflist (env, lp, 6, rowlist, collist, vallist);
   
   if ( status )  goto TERMINATE;

TERMINATE:

   return (status);

}  /* END populatebynonzero */

