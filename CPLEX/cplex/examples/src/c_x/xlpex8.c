/* --------------------------------------------------------------------------
 * File: xlpex8.c
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

/* xlpex8.c - Entering and optimizing a problem */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the include of cplexx.h. */

#include <ilcplex/cplexx.h>
#include <stdlib.h>

/* Bring in the declarations for the string functions */

#include <string.h>

/* Include declaration for function at end of program */

static int
   setproblemdata (char **probname_p, CPXDIM *numcols_p, CPXDIM *numrows_p, 
                   int *objsen_p, double **obj_p, double **rhs_p, 
                   char **sense_p, CPXNNZ **matbeg_p, CPXDIM **matcnt_p, 
                   CPXDIM **matind_p, double **matval_p, 
                   double **lb_p, double **ub_p);

static void
   free_and_null (char **ptr);


/* The problem we are optimizing will have 2 rows, 3 columns 
   and 6 nonzeros.  */

#define NUMROWS    2
#define NUMCOLS    3
#define NUMNZ      6


int
main (void)
{
   /* Declare pointers for the variables and arrays that will contain
      the data which define the LP problem.  The setproblemdata() routine
      allocates space for the problem data.  */

   char     *probname = NULL;  
   CPXDIM   numcols;
   CPXDIM   numrows;
   int      objsen;
   double   *obj = NULL;
   double   *rhs = NULL;
   char     *sense = NULL;
   CPXNNZ   *matbeg = NULL;
   CPXDIM   *matcnt = NULL;
   CPXDIM   *matind = NULL;
   double   *matval = NULL;
   double   *lb = NULL;
   double   *ub = NULL;

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


   CPXENVptr  env = NULL;
   CPXLPptr   lp = NULL;
   int        status;
   CPXDIM     i, j;
   CPXDIM     cur_numrows, cur_numcols;

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

   /* Allocate memory and fill in the data for the problem.  */

   status = setproblemdata (&probname, &numcols, &numrows, &objsen, 
                            &obj, &rhs, &sense, &matbeg, &matcnt, 
                            &matind, &matval, &lb, &ub);
   if ( status ) {
      fprintf (stderr, "Failed to build problem data arrays.\n");
      goto TERMINATE;
   }

   /* Create the problem. */

   lp = CPXXcreateprob (env, &status, probname);

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

   /* Now copy the problem data into the lp */

   status = CPXXcopylp (env, lp, numcols, numrows, objsen, obj, rhs, 
                       sense, matbeg, matcnt, matind, matval,
                       lb, ub, NULL);

   if ( status ) {
      fprintf (stderr, "Failed to copy problem data.\n");
      goto TERMINATE;
   }


   /* Optimize the problem and obtain solution. */

   status = CPXXlpopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize LP.\n");
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
      the actual size is, rather than using what was passed to CPXXcopylp.
      cur_numrows and cur_numcols store the current number of rows and
      columns, respectively.  */

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

   status = CPXXwriteprob (env, lp, "lpex1.lp", NULL);
   if ( status ) {
      fprintf (stderr, "Failed to write LP to disk.\n");
      goto TERMINATE;
   }
   
   
TERMINATE:

   /* Free up the problem as allocated by CPXXcreateprob, if necessary */

   if ( lp != NULL ) {
      status = CPXXfreeprob (env, &lp);
      if ( status ) {
         fprintf (stderr, "CPXXfreeprob failed, error code %d.\n", status);
      }
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      status = CPXXcloseCPLEX (&env);

      /* Note that CPXXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */

      if ( status ) {
         char  errmsg[CPXMESSAGEBUFSIZE];
         fprintf (stderr, "Could not close CPLEX environment.\n");
         CPXXgeterrorstring (env, status, errmsg);
         fprintf (stderr, "%s", errmsg);
      }
   }

   /* Free up the problem data arrays, if necessary. */

   free_and_null ((char **) &probname);
   free_and_null ((char **) &obj);
   free_and_null ((char **) &rhs);
   free_and_null ((char **) &sense);
   free_and_null ((char **) &matbeg);
   free_and_null ((char **) &matcnt);
   free_and_null ((char **) &matind);
   free_and_null ((char **) &matval);
   free_and_null ((char **) &lb);
   free_and_null ((char **) &ub);
        
   return (status);

}  /* END main */


/* This function fills in the data structures for the linear program:

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
setproblemdata (char **probname_p, CPXDIM *numcols_p, CPXDIM *numrows_p, 
                int *objsen_p, double **obj_p, double **rhs_p, 
                char **sense_p, CPXNNZ **matbeg_p, CPXDIM **matcnt_p, 
                CPXDIM **matind_p, double **matval_p, 
                double **lb_p, double **ub_p)
{
   char     *zprobname = NULL;     /* Problem name <= 16 characters */        
   double   *zobj = NULL;
   double   *zrhs = NULL;
   char     *zsense = NULL;
   CPXNNZ   *zmatbeg = NULL;
   CPXDIM   *zmatcnt = NULL;
   CPXDIM   *zmatind = NULL;
   double   *zmatval = NULL;
   double   *zlb = NULL;
   double   *zub = NULL;
   int      status = 0;

   zprobname = malloc (16 * sizeof(*zprobname)); 
   zobj      = malloc (NUMCOLS * sizeof(*zobj));
   zrhs      = malloc (NUMROWS * sizeof(*zrhs));
   zsense    = malloc (NUMROWS * sizeof(*zsense)); 
   zmatbeg   = malloc (NUMCOLS * sizeof(*zmatbeg));   
   zmatcnt   = malloc (NUMCOLS * sizeof(*zmatcnt));   
   zmatind   = malloc (NUMNZ * sizeof(*zmatind));   
   zmatval   = malloc (NUMNZ * sizeof(*zmatval));
   zlb       = malloc (NUMCOLS * sizeof(*zlb));
   zub       = malloc (NUMCOLS * sizeof(*zub));

   if ( zprobname == NULL || zobj    == NULL ||
        zrhs      == NULL || zsense  == NULL ||
        zmatbeg   == NULL || zmatcnt == NULL ||
        zmatind   == NULL || zmatval == NULL ||
        zlb       == NULL || zub     == NULL   )  {
      status = 1;
      goto TERMINATE;
   }

   strcpy (zprobname, "example");

   /* The code is formatted to make a visual correspondence 
      between the mathematical linear program and the specific data
      items.   */

     zobj[0]  = 1.0;  zobj[1]    = 2.0;  zobj[2]    = 3.0;

   zmatbeg[0] = 0;    zmatbeg[1] = 2;    zmatbeg[2] = 4;
   zmatcnt[0] = 2;    zmatcnt[1] = 2;    zmatcnt[2] = 2;
      
   zmatind[0] = 0;    zmatind[2] = 0;    zmatind[4] = 0;    zsense[0] = 'L';
   zmatval[0] = -1.0; zmatval[2] = 1.0;  zmatval[4] = 1.0;  zrhs[0]   = 20.0;

   zmatind[1] = 1;    zmatind[3] = 1;    zmatind[5] = 1;    zsense[1] = 'L';
   zmatval[1] = 1.0;  zmatval[3] = -3.0; zmatval[5] = 1.0;  zrhs[1]   = 30.0;

       zlb[0] = 0.0;      zlb[1] = 0.0;          zlb[2] = 0.0;
       zub[0] = 40.0;     zub[1] = CPX_INFBOUND; zub[2] = CPX_INFBOUND;

TERMINATE:
   
   if ( status ) {
      free_and_null ((char **) &zprobname);
      free_and_null ((char **) &zobj);
      free_and_null ((char **) &zrhs);
      free_and_null ((char **) &zsense);
      free_and_null ((char **) &zmatbeg);
      free_and_null ((char **) &zmatcnt);
      free_and_null ((char **) &zmatind);
      free_and_null ((char **) &zmatval);
      free_and_null ((char **) &zlb);
      free_and_null ((char **) &zub);
   }
   else {
      *numcols_p   = NUMCOLS;
      *numrows_p   = NUMROWS;
      *objsen_p    = CPX_MAX;   /* The problem is maximization */
   
      *probname_p  = zprobname;
      *obj_p       = zobj;
      *rhs_p       = zrhs;
      *sense_p     = zsense;
      *matbeg_p    = zmatbeg;
      *matcnt_p    = zmatcnt;
      *matind_p    = zmatind;
      *matval_p    = zmatval;
      *lb_p        = zlb;
      *ub_p        = zub;
   }
   return (status);

}  /* END setproblemdata */




/* This simple routine frees up the pointer *ptr, and sets *ptr to NULL */

static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */  
