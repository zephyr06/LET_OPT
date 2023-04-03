/* --------------------------------------------------------------------------
 * File: xmiqpex1.c
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2002, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

/* xmiqpex1.c - Entering and optimizing a MIQP problem */

/* Bring in the CPLEX function declarations and the C library
   header file stdio.h with the include of cplexx.h. */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string functions */

#include <string.h>
#include <stdlib.h>

/* Include declaration for function at end of program */

static int
   setproblemdata (char **probname_p, CPXDIM *numcols_p, CPXDIM *numrows_p,
                   int *objsen_p, double **obj_p, double **rhs_p,
                   char **sense_p, CPXNNZ **matbeg_p, CPXDIM **matcnt_p,
                   CPXDIM **matind_p, double **matval_p,
                   double **lb_p, double **ub_p, char **ctype_p,
                   CPXNNZ **qmatbeg_p, CPXDIM **qmatcnt_p,
                   CPXDIM **qmatind_p, double **qmatval_p);

static void
   free_and_null (char **ptr);


/* The problem we are optimizing will have 3 rows, 4 columns and
   9 nonzeros, and 7 nonzeros in the quadratic coefficient matrix.  */

#define NUMROWS    3
#define NUMCOLS    4
#define NUMNZ      9
#define NUMQNZ     7


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
   char     *ctype = NULL;
   CPXNNZ   *qmatbeg = NULL;
   CPXDIM   *qmatcnt = NULL;
   CPXDIM   *qmatind = NULL;
   double   *qmatval = NULL;

   /* Declare and allocate space for the variables and arrays where we will
      store the optimization results including the status, objective value,
      variable values, and row slacks. */

   int      solstat;
   double   objval;
   double   x[NUMCOLS];
   double   slack[NUMROWS];


   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status;
   CPXDIM        i, j;
   CPXDIM        cur_numrows, cur_numcols;

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
                            &rhs, &sense, &matbeg, &matcnt, &matind, &matval,
                            &lb, &ub, &ctype, &qmatbeg, &qmatcnt, &qmatind,
                            &qmatval);
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

   /* Now copy the ctype array */

   status = CPXXcopyctype (env, lp, ctype);
   if ( status ) {
      fprintf (stderr, "Failed to copy ctype\n");
      goto TERMINATE;
   }

   status = CPXXcopyquad (env, lp, qmatbeg, qmatcnt, qmatind, qmatval);
   if ( status ) {
      fprintf (stderr, "Failed to copy quadratic matrix.\n");
      goto TERMINATE;
   }


   /* Optimize the problem and obtain solution. */

   status = CPXXmipopt (env, lp);
   if ( status ) {
      fprintf (stderr, "Failed to optimize MIQP.\n");
      goto TERMINATE;
   }

   solstat = CPXXgetstat (env, lp);

   /* Write the output to the screen. */

   printf ("\nSolution status = %d\n", solstat);

   status = CPXXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr,"No MIQP objective value available.  Exiting...\n");
      goto TERMINATE;
   }

   printf ("Solution value  = %f\n\n", objval);

   /* The size of the problem should be obtained by asking CPLEX what
      the actual size is, rather than using what was passed to CPXXcopylp.
      cur_numrows and cur_numcols store the current number of rows and
      columns, respectively.  */

   cur_numrows = CPXXgetnumrows (env, lp);
   cur_numcols = CPXXgetnumcols (env, lp);

   status = CPXXgetx (env, lp, x, 0, cur_numcols-1);
   if ( status ) {
      fprintf (stderr, "Failed to get optimal integer x.\n");
      goto TERMINATE;
   }

   status = CPXXgetslack (env, lp, slack, 0, cur_numrows-1);
   if ( status ) {
      fprintf (stderr, "Failed to get optimal slack values.\n");
      goto TERMINATE;
   }

   for (i = 0; i < cur_numrows; i++) {
      printf ("Row %d:  Slack = %10f\n", i, slack[i]);
   }

   for (j = 0; j < cur_numcols; j++) {
      printf ("Column %d:  Value = %10f\n", j, x[j]);
   }

   /* Finally, write a copy of the problem to a file. */

   status = CPXXwriteprob (env, lp, "miqpex1.lp", NULL);
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
   free_and_null ((char **) &ctype);
   free_and_null ((char **) &qmatbeg);
   free_and_null ((char **) &qmatcnt);
   free_and_null ((char **) &qmatind);
   free_and_null ((char **) &qmatval);

   return (status);

}  /* END main */


/* This function fills in the data structures for the mixed integer
   quadratic program:

      Maximize
       obj: x1 + 2 x2 + 3 x3 + x4
            - 0.5 ( 33x1*x1 + 22*x2*x2 + 11*x3*x3
                   -  12*x1*x2 - 23*x2*x3 )
      Subject To
       c1: - x1 + x2 + x3 + 10x4  <= 20
       c2: x1 - 3 x2 + x3         <= 30
       c3:       x2       - 3.5x4  = 0
      Bounds
       0 <= x1 <= 40
       0 <= x4 <= 3
      Integers
        x4
      End
 */


static int
setproblemdata (char **probname_p, CPXDIM *numcols_p, CPXDIM *numrows_p,
                int *objsen_p, double **obj_p, double **rhs_p,
                char **sense_p, CPXNNZ **matbeg_p, CPXDIM **matcnt_p,
                CPXDIM **matind_p, double **matval_p,
                double **lb_p, double **ub_p, char **ctype_p,
                CPXNNZ **qmatbeg_p, CPXDIM **qmatcnt_p,
                CPXDIM **qmatind_p, double **qmatval_p)
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
   char     *zctype = NULL;
   CPXNNZ   *zqmatbeg = NULL;
   CPXDIM   *zqmatcnt = NULL;
   CPXDIM   *zqmatind = NULL;
   double   *zqmatval = NULL;
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
   zctype    = malloc (NUMCOLS * sizeof(*zctype));
   zqmatbeg  = malloc (NUMCOLS * sizeof(*zqmatbeg));
   zqmatcnt  = malloc (NUMCOLS * sizeof(*zqmatcnt));
   zqmatind  = malloc (NUMQNZ * sizeof(*zqmatind));
   zqmatval  = malloc (NUMQNZ * sizeof(*zqmatval));

   if ( zprobname == NULL || zobj    == NULL ||
        zrhs      == NULL || zsense  == NULL ||
        zmatbeg   == NULL || zmatcnt == NULL ||
        zmatind   == NULL || zmatval == NULL ||
        zlb       == NULL || zub     == NULL ||
        zctype    == NULL ||
        zqmatbeg  == NULL || zqmatcnt == NULL ||
        zqmatind  == NULL || zqmatval == NULL   )  {
      status = 1;
      goto TERMINATE;
   }

   strcpy (zprobname, "example");

   /* The code is formatted to make a visual correspondence
      between the mathematical linear program and the specific data
      items.   */

     zobj[0]  = 1.0;   zobj[1]   = 2.0;  zobj[2]    = 3.0;    zobj[3] = 1.0;

   zmatbeg[0] = 0;    zmatbeg[1] = 2;    zmatbeg[2] = 5;   zmatbeg[3] = 7;
   zmatcnt[0] = 2;    zmatcnt[1] = 3;    zmatcnt[2] = 2;   zmatcnt[3] = 2;

   zmatind[0] = 0;    zmatind[2] = 0;    zmatind[5] = 0;   zmatind[7] = 0;
   zmatval[0] = -1.0; zmatval[2] = 1.0;  zmatval[5] = 1.0; zmatval[7] = 10.0;

   zmatind[1] = 1;    zmatind[3] = 1;    zmatind[6] = 1;
   zmatval[1] = 1.0;  zmatval[3] = -3.0; zmatval[6] = 1.0;

                      zmatind[4] = 2;                      zmatind[8] = 2;
                      zmatval[4] = 1.0;                    zmatval[8] = -3.5;

   zlb[0] = 0.0;   zlb[1] = 0.0;          zlb[2] = 0.0;          zlb[3] = 0.0;
   zub[0] = 40.0;  zub[1] = CPX_INFBOUND; zub[2] = CPX_INFBOUND; zub[3] = 3.0;

   zctype[0] = 'C';   zctype[1] = 'C';   zctype[2] = 'C';   zctype[3] = 'I';

  /* The right-hand-side values don't fit nicely on a line above.  So put
     them here.  */

   zsense[0] = 'L';
   zrhs[0]   = 20.0;

   zsense[1] = 'L';
   zrhs[1]   = 30.0;

   zsense[2] = 'E';
   zrhs[2]   = 0.0;

   /* Now set up the Q matrix.  Note that we set the values knowing that
    * we're doing a maximization problem, so negative values go on
    * the diagonal.  Also, the off diagonal terms are each repeated,
    * by taking the algebraic term and dividing by 2 */

   zqmatbeg[0] = 0;     zqmatbeg[1] = 2;     zqmatbeg[2] = 5;
   zqmatbeg[3] = 7;

   zqmatcnt[0] = 2;     zqmatcnt[1] = 3;     zqmatcnt[2] = 2;
   zqmatcnt[3] = 0;

   zqmatind[0] = 0;     zqmatind[2] = 0;
   zqmatval[0] = -33.0; zqmatval[2] = 6.0;

   zqmatind[1] = 1;     zqmatind[3] = 1;     zqmatind[5] = 1;
   zqmatval[1] = 6.0;   zqmatval[3] = -22.0; zqmatval[5] = 11.5;

                        zqmatind[4] = 2;     zqmatind[6] = 2;
                        zqmatval[4] = 11.5;  zqmatval[6] = -11.0;


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
      free_and_null ((char **) &zctype);
      free_and_null ((char **) &zqmatbeg);
      free_and_null ((char **) &zqmatcnt);
      free_and_null ((char **) &zqmatind);
      free_and_null ((char **) &zqmatval);
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
      *ctype_p     = zctype;
      *qmatbeg_p   = zqmatbeg;
      *qmatcnt_p   = zqmatcnt;
      *qmatind_p   = zqmatind;
      *qmatval_p   = zqmatval;
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
