/* --------------------------------------------------------------------------
 * File: xindefqpex1.c
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

/* xindefqpex1.c - Entering and optimizing an indefinite quadratic
   programming problem */

/* Bring in the CPLEX function declarations and the C library
   header file stdio.h with include of cplex.h. */

#include <ilcplex/cplexx.h>
#include <stdlib.h>

/* Bring in the declarations for the string functions */

#include <string.h>

/* Include declaration for function at end of program */

static int
   setproblemdata (char **probname_p, CPXDIM *numcols_p, CPXDIM *numrows_p,
                   int *objsen_p, double **obj_p, double **rhs_p,
                   char **sense_p, CPXNNZ **matbeg_p, CPXDIM **matcnt_p,
                   CPXDIM **matind_p, double **matval_p, double **lb_p,
                   double **ub_p, CPXNNZ **qmatbeg_p, CPXDIM **qmatcnt_p,
                   CPXDIM **qmatind_p, double **qmatval_p,
                   CPXDIM *numrows_extra_p, CPXNNZ *numnnz_extra_p,
                   double **rhs_extra_p, char **sense_extra_p,
                   CPXNNZ **rmatbeg_p, CPXDIM **rmatind_p, double **rmatval_p),
   optimize_and_report (CPXENVptr env, CPXLPptr lp,
                        int *solstat_p, double *objval_p);

static void
   free_and_null (char **ptr);


/* The problem we are optimizing will at first have 2 rows, 2 columns,
   4 nonzeros, and 4 nonzeros in the quadratic coefficient matrix. */

#define NUMROWS    2
#define NUMCOLS    2
#define NUMNZ      4
#define NUMQNZ     4

/* We will later add a constraint, at which point the problem will
   have 3 rows and 5 nonzeros. */

#define TOTROWS    3
#define TOTNZ      5

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
   CPXNNZ   *qmatbeg = NULL;
   CPXDIM   *qmatcnt = NULL;
   CPXDIM   *qmatind = NULL;
   double   *qmatval = NULL;

   /* Declare pointers for the variables that will contain the data
      for the constraint that cuts off certain local optima. */

   CPXDIM   numrows_extra;
   CPXNNZ   numnnz_extra;
   double   *rhs_extra = NULL;
   char     *sense_extra = NULL;
   CPXNNZ   *rmatbeg = NULL;
   CPXDIM   *rmatind = NULL;
   double   *rmatval = NULL;
   CPXDIM   rowind[1];

   /* Declare and allocate space for the variables and arrays where we
      will store the optimization results including the status, objective
      value, variable values, dual values, row slacks and variable
      reduced costs. */

   int      solstat;
   double   objval;

   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status;

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

   /* Fill in the data for the problem.  */

   status = setproblemdata (&probname, &numcols, &numrows, &objsen, &obj,
                            &rhs, &sense, &matbeg, &matcnt, &matind,
                            &matval, &lb, &ub, &qmatbeg, &qmatcnt,
                            &qmatind, &qmatval,
                            &numrows_extra, &numnnz_extra, 
                            &rhs_extra, &sense_extra, 
                            &rmatbeg, &rmatind, &rmatval);
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
      fprintf (stderr, "Failed to create problem.\n");
      goto TERMINATE;
   }

   /* Now copy the LP part of the problem data into the lp */

   status = CPXXcopylp (env, lp, numcols, numrows, objsen, obj, rhs,
                       sense, matbeg, matcnt, matind, matval,
                       lb, ub, NULL);

   if ( status ) {
      fprintf (stderr, "Failed to copy problem data.\n");
      goto TERMINATE;
   }

   status = CPXXcopyquad (env, lp, qmatbeg, qmatcnt, qmatind, qmatval);
   if ( status ) {
      fprintf (stderr, "Failed to copy quadratic matrix.\n");
      goto TERMINATE;
   }


   /* When a non-convex objective function is present, CPLEX will
      return error CPXERR_Q_NOT_POS_DEF unless the parameter
      CPXPARAM_OptimalityTarget is set to accept first-order optimal
      solutions.  */
   status = CPXXsetintparam (env, CPXPARAM_OptimalityTarget,
                             CPX_OPTIMALITYTARGET_FIRSTORDER);
   if ( status ) goto TERMINATE;

   /* Optimize the problem and obtain solution. */

   status = optimize_and_report(env, lp, &solstat, &objval);
   if ( status ) goto TERMINATE;


   /* Add a constraint to cut off the solution at (-1, 1) */

   status = CPXXaddrows (env, lp, 0, numrows_extra, numnnz_extra,
                        rhs_extra, sense_extra,
                        rmatbeg, rmatind, rmatval,
                        NULL, NULL);
   if ( status ) goto TERMINATE;

   status = optimize_and_report(env, lp, &solstat, &objval);
   if ( status ) goto TERMINATE;


   /* Reverse the sense of the new constraint to cut off the solution at (1, 1) */

   rowind[0] = CPXXgetnumrows (env, lp) - 1;
   status    = CPXXchgsense (env, lp, 1, rowind, "L");
   if ( status ) goto TERMINATE;

   status = optimize_and_report(env, lp, &solstat, &objval);
   if ( status ) goto TERMINATE;

   /* Finally, write a copy of the problem to a file. */

   status = CPXXwriteprob (env, lp, "indefqpex1.lp", NULL);
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
   free_and_null ((char **) &qmatbeg);
   free_and_null ((char **) &qmatcnt);
   free_and_null ((char **) &qmatind);
   free_and_null ((char **) &qmatval);
   free_and_null ((char **) &rhs_extra);
   free_and_null ((char **) &sense_extra);
   free_and_null ((char **) &rmatbeg);
   free_and_null ((char **) &rmatind);
   free_and_null ((char **) &rmatval);

   return (status);

}  /* END main */


/* This function fills in the data structures for the quadratic program:

      Minimize
       obj: 0.5 ( -3*x1*x1 - 3*x2*x2
                   -  1*x1*x2 )
      Subject To
       c1:   x1 + x2 >= 0
       c2: - x1 + x2 >= 0
      Bounds
       -1 <= x1 <= 1
        0 <= x2 <= 1
      End

   It also sets up arrays for the additional constraint

       x1 >= 0

   which is used to cut off one of the local optima of the original
   problem.

 */


static int
setproblemdata (char **probname_p, CPXDIM *numcols_p, CPXDIM *numrows_p,
                int *objsen_p, double **obj_p, double **rhs_p,
                char **sense_p, CPXNNZ **matbeg_p, CPXDIM **matcnt_p,
                CPXDIM **matind_p, double **matval_p, double **lb_p,
                double **ub_p, CPXNNZ **qmatbeg_p, CPXDIM **qmatcnt_p,
                CPXDIM **qmatind_p, double **qmatval_p,
                CPXDIM *numrows_extra_p, CPXNNZ *numnnz_extra_p,
                double **rhs_extra_p, char **sense_extra_p,
                CPXNNZ **rmatbeg_p, CPXDIM **rmatind_p, double **rmatval_p)
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
   CPXNNZ   *zqmatbeg = NULL;
   CPXDIM   *zqmatcnt = NULL;
   CPXDIM   *zqmatind = NULL;
   double   *zqmatval = NULL;
   double   *xrhs = NULL;
   char     *xsense = NULL;
   CPXNNZ   *rmatbeg = NULL;
   CPXDIM   *rmatind = NULL;
   double   *rmatval = NULL;
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
   zqmatbeg  = malloc (NUMCOLS * sizeof(*zqmatbeg));
   zqmatcnt  = malloc (NUMCOLS * sizeof(*zqmatcnt));
   zqmatind  = malloc (NUMQNZ * sizeof(*zqmatind));
   zqmatval  = malloc (NUMQNZ * sizeof(*zqmatval));
   
   xrhs      = malloc ((TOTROWS - NUMROWS) * sizeof(*xrhs));
   xsense    = malloc ((TOTROWS - NUMROWS) * sizeof(*xsense));
   rmatbeg   = malloc ((TOTNZ - NUMNZ + 1) * sizeof(*rmatbeg));
   rmatind   = malloc ((TOTNZ - NUMNZ) * sizeof(*rmatind));
   rmatval   = malloc ((TOTNZ - NUMNZ) * sizeof(*rmatval));


   if ( zprobname == NULL || zobj     == NULL ||
        zrhs      == NULL || zsense   == NULL ||
        zmatbeg   == NULL || zmatcnt  == NULL ||
        zmatind   == NULL || zmatval  == NULL ||
        zlb       == NULL || zub      == NULL ||
        zqmatbeg  == NULL || zqmatcnt == NULL ||
        zqmatind  == NULL || zqmatval == NULL ||
        xrhs      == NULL || xsense   == NULL ||
        rmatbeg   == NULL || rmatind  == NULL ||
        rmatval   == NULL                      )  {
      status = 1;
      goto TERMINATE;
   }

   strcpy (zprobname, "example");

   /* The code is formatted to make a visual correspondence
      between the mathematical linear program and the specific data
      items.   */

     zobj[0]  = 0.0;   zobj[1]   = 0.0;

   zmatbeg[0] = 0;     zmatbeg[1] = 2;
   zmatcnt[0] = 2;     zmatcnt[1] = 2;

   zmatind[0] = 0;     zmatind[2] = 0;    zsense[0] = 'G';
   zmatval[0] = 1.0;   zmatval[2] = 1.0;  zrhs[0]   = 0.0;

   zmatind[1] = 1;     zmatind[3] = 1;    zsense[1] = 'G';
   zmatval[1] = -1.0;  zmatval[3] = 1.0;  zrhs[1]   = 0.0;

       zlb[0] = -1.0;      zlb[1] = 0.0;
       zub[0] =  1.0;      zub[1] = 1.0;

   /* Now set up the Q matrix.  Note that the off diagonal term is
    * repeated, by taking the algebraic term and dividing by 2 */

   zqmatbeg[0] = 0;     zqmatbeg[1] = 2;
   zqmatcnt[0] = 2;     zqmatcnt[1] = 2;

   /* Matrix is set up visually. */

   zqmatind[0] = 0;     zqmatind[2] = 0;
   zqmatval[0] = -3.0;  zqmatval[2] = -0.5;

   zqmatind[1] = 1;     zqmatind[3] = 1;
   zqmatval[1] = -0.5;  zqmatval[3] = -3.0;

   xrhs[0]    = 0.0;
   xsense[0]  = 'G';
   rmatbeg[0] = 0;
   rmatind[0] = 0;
   rmatval[0] = 1.0;

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
      free_and_null ((char **) &zqmatbeg);
      free_and_null ((char **) &zqmatcnt);
      free_and_null ((char **) &zqmatind);
      free_and_null ((char **) &zqmatval);
      free_and_null ((char **) &xrhs);
      free_and_null ((char **) &xsense);
      free_and_null ((char **) &rmatbeg);
      free_and_null ((char **) &rmatind);
      free_and_null ((char **) &rmatval);
   }
   else {
      *numcols_p   = NUMCOLS;
      *numrows_p   = NUMROWS;
      *objsen_p    = CPX_MIN;

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
      *qmatbeg_p   = zqmatbeg;
      *qmatcnt_p   = zqmatcnt;
      *qmatind_p   = zqmatind;
      *qmatval_p   = zqmatval;

      *numrows_extra_p = 1;
      *numnnz_extra_p  = 1;

      *rhs_extra_p     = xrhs;
      *sense_extra_p   = xsense;
      *rmatbeg_p       = rmatbeg;
      *rmatind_p       = rmatind;
      *rmatval_p       = rmatval;
   }
   return (status);

}  /* END setproblemdata */

static int
optimize_and_report (CPXENVptr env, CPXLPptr lp,
                     int *solstat_p, double *objval_p)
{
   int status = 0;
   
   double   x[NUMCOLS];
   double   pi[TOTROWS];
   double   slack[TOTROWS];
   double   dj[NUMCOLS];

   int      i, j;
   int      cur_numrows, cur_numcols;
   
   status = CPXXqpopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize QP.\n");
      goto TERMINATE;
   }

   status = CPXXsolution (env, lp, solstat_p, objval_p, x, pi, slack, dj);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution.\n");
      goto TERMINATE;
   }


   /* Write the output to the screen. */

   printf ("\nSolution status = %d\n", *solstat_p);
   printf ("Solution value  = %f\n\n", *objval_p);

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

 TERMINATE:

   return (status);

}  /* END optimize_and_report */

/* This simple routine frees up the pointer *ptr, and sets *ptr to NULL */

static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */

