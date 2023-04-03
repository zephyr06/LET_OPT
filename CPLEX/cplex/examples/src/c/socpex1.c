/* --------------------------------------------------------------------------
 * File: socpex1.c
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2003, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

/* socpex1.c -- Solve a second order cone program to optimality, fetch
 *              the dual values and test that the primal and dual solution
 *              vectors returned by CPLEX satisfy the KKT conditions.
 *              The problems that this code can handle are second order
 *              cone programs in standard form. A second order cone
 *              program in standard form is a problem of the following
 *              type (c' is the transpose of c):
 *                min c1'x1 + ... + cr'xr
 *                  A1 x1 + ... + Ar xr = b
 *                  xi in second order cone (SOC)
 *              where xi is a vector of length ni. Note that the
 *              different xi are orthogonal. The constraint "xi in SOC"
 *              for xi=(xi[1], ..., xi[ni]) is
 *                  xi[1] >= |(xi[2],...,xi[ni])|
 *              where |.| denotes the Euclidean norm. In CPLEX such a
 *              constraint is formulated as
 *                  -xi[1]^2 + xi[2]^2 + ... + xi[ni]^2 <= 0
 *                   xi[1]                              >= 0
 *                             xi[2], ..., xi[ni] free
 *              Note that if ni = 1 then the second order cone constraint
 *              reduces to xi[1] >= 0.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ilcplex/cplex.h>

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


/* Shortcut for infinity. */
#define INF CPX_INFBOUND

/* Tolerance for testing KKT conditions. */
#define TESTTOL 1e-9
/* Tolerance for barrier convergence. */
#define CONVTOL 1e-9

/* Marks columns that are in cone constraint but not the cone head. */
#define NOT_CONE_HEAD -1
/* Marks variables that are not in any cone constraint. */
#define NOT_IN_CONE   -2

/* Structure that holds information about a single quadratic constraint.
 * Instances of this structure are used to read quadratic constraints one
 * by one using function getqconstr().
 */
typedef struct {
   int    *lind;  /* Indices of linear coefficients. */
   double *lval;  /* Values of linear coefficients. */
   int    *qrow;  /* Row indices for quadratic coefficients. */
   int    *qcol;  /* Column indices for quadratic coefficients. */
   double *qval;  /* Values for quadratic coefficients. */
   double rhs;    /* Right-hand side of quadratic constraint. */
   int    lnz;    /* Number of valid entries in lind and lval. */
   int    qnz;    /* Number of valid entries in qrow, qcol and qval. */
   int    qcap;   /* Length (max number of elements) of lind and lval. */
   int    lcap;   /* Length (max number of elements) of qrow, qcol and qval. */
   char   sense;  /* Sense of quadratic constraint. */
} qbuf_type;

/* Initialize an instance of qbuf_type. */
static void
qbuf_init (qbuf_type *qbuf)
{
   memset (qbuf, 0, sizeof (*qbuf));
}

/* Release an instance of qbuf_type that was previously initialized by
 * qbuf_init().
 */
static void
qbuf_clear (qbuf_type *qbuf)
{
   if ( qbuf->lcap > 0 ) {
      free (qbuf->lind);
      free (qbuf->lval);
   }
   if ( qbuf->qcap > 0 ) {
      free (qbuf->qrow);
      free (qbuf->qcol);
      free (qbuf->qval);
   }
   memset (qbuf, 0, sizeof (*qbuf));
}

/* Read quadratic constraint number Q from LP and store it in qbuf.
 * The buffers in qbuf are expanded if they do not provide enough room
 * to store the constraint.
 * The function returns a true value on success and false if it fails.
 */
static int
getqconstr (CPXCENVptr env, CPXCLPptr lp, int q, qbuf_type *qbuf)
{
   int ok = 0;
   int status;
   int linnz, linsurplus;
   int qnz, qsurplus;
   CPXCHANNELptr errc;

   /* Get the channel for printing error messages. */
   if ( (status = CPXgetchannels (env, NULL, NULL, &errc, NULL)) != 0 )
      goto TERMINATE;

   /* Call CPXgetqconstr() a first time with zero-length buffers to figure
    * how long the buffers must be.
    */
   status = CPXgetqconstr (env, lp, &linnz, &qnz, NULL, NULL,
                           NULL, NULL, 0, &linsurplus,
                           NULL, NULL, NULL, 0, &qsurplus, q);
   if ( status != CPXERR_NEGATIVE_SURPLUS )
      goto TERMINATE;

   /* Adjust buffers for linear coefficients if necessary.
    */
   if ( linsurplus < 0 && -linsurplus >= qbuf->lcap ) {
      int newcap = -linsurplus;

      if ( (qbuf->lind = realloc (qbuf->lind, newcap * sizeof (*qbuf->lind))) == NULL ||
           (qbuf->lval = realloc (qbuf->lval, newcap * sizeof (*qbuf->lval))) == NULL )
      {
         display (errc, "Out of memory!\n");
         goto TERMINATE;
      }
      qbuf->lcap = newcap;
   }

   /* Adjust buffers for quadratic coefficients if necessary.
    */
   if ( qsurplus < 0 && -qsurplus >= qbuf->qcap ) {
      int newcap = -qsurplus;

      if ( (qbuf->qcol = realloc (qbuf->qcol, newcap * sizeof (*qbuf->qcol))) == NULL ||
           (qbuf->qrow = realloc (qbuf->qrow, newcap * sizeof (*qbuf->qrow))) == NULL ||
           (qbuf->qval = realloc (qbuf->qval, newcap * sizeof (*qbuf->qval))) == NULL )
      {
         display (errc, "Out of memory!\n");
         goto TERMINATE;
      }
      qbuf->qcap = newcap;
   }

   /* Now all buffers in QBUF are long enough to store the constraint we
    * want to query. Copy the constraint into QBUF.
    */
   status = CPXgetqconstr (env, lp, &qbuf->lnz, &qbuf->qnz, &qbuf->rhs,
                           &qbuf->sense,
                           qbuf->lind, qbuf->lval, qbuf->lcap, &linsurplus,
                           qbuf->qrow, qbuf->qcol, qbuf->qval, qbuf->qcap,
                           &qsurplus, q);
   if ( status != 0 )
      goto TERMINATE;

   ok = 1;
TERMINATE:
   return ok;
}

/* CPLEX does not provide a function to directly get the dual multipliers
 * for second order cone constraint.
 * Example qcpdual.c illustrates how the dual multipliers for a
 * quadratic constraint can be computed from that constraint's slack.
 * However, for SOCP we can do something simpler: we can read those
 * multipliers directly from the dual slacks for the
 * cone head variables. For a second order cone constraint
 *    x[1] >= |(x[2], ..., x[n])|
 * the dual multiplier is the dual slack value for x[1].
 */
static int
getsocpconstrmultipliers (CPXCENVptr env, CPXCLPptr lp,
                          double *dslack, double *socppi)
{
   int status = 0;
   int const cols = CPXgetnumcols (env, lp);
   int const qs = CPXgetnumqconstrs (env, lp);
   double *dense = NULL, *val = NULL;
   int *ind = NULL;
   int j, q;
   qbuf_type qbuf;

   qbuf_init (&qbuf);

   if ( (dense = malloc (sizeof (*dense) * cols)) == NULL ) {
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   /* First of all get the dual slack vector. This is the sum of
    * dual multipliers for bound constraints (as returned by CPXgetdj())
    * and the per-constraint dual slack vectors (as returned by
    * CPXgetqconstrdslack()).
    */

   /* Get dual multipliers for bound constraints. */
   if ( (status = CPXgetdj (env, lp, dense, 0, cols - 1)) != 0 )
      goto TERMINATE;

   /* Create a dense vector that holds the sum of dual slacks for all
    * quadratic constraints.
    */
   if ( (val = malloc (cols * sizeof (val))) == NULL ||
        (ind = malloc (cols * sizeof (ind))) == NULL )
   {
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }
   for (q = 0; q < qs; ++q) {
      int len = 0, surp;

      if ( (status = CPXgetqconstrdslack (env, lp, q, &len, ind, val,
                                          cols, &surp)) != 0 )
         goto TERMINATE;
      for (j = 0; j < len; ++j) {
         dense[ind[j]] += val[j];
      }
   }

   /* Now go through the socp constraints. For each constraint find the
    * cone head variable (the one variable that has a negative coefficient)
    * and pick up its dual slack. This is the dual multiplier for the
    * constraint.
    */
   for (q = 0; q < qs; ++q) {
      if ( !getqconstr (env, lp, q, &qbuf) ) {
         status = CPXERR_BAD_ARGUMENT;
         goto TERMINATE;
      }
      for (j = 0; j < qbuf.qnz; ++j) {
         if ( qbuf.qval[j] < 0.0 ) {
            /* This is the cone head variable. */
            socppi[q] = dense[qbuf.qcol[j]];
            break;
         }
      }
   }

   /* If the caller wants to have the dual slack vector then copy it
    * to the output array.
    */
   if ( dslack != NULL )
      memcpy (dslack, dense, sizeof (*dslack) * cols);

TERMINATE:
   free (ind);
   free (val);
   free (dense);

   qbuf_clear (&qbuf);

   return status;
}

/*
 * The function returns a true value if the tested KKT conditions are
 * satisfied and false otherwise.
 */
static int
checkkkt (CPXCENVptr env, CPXLPptr lp, int const *cone, double tol)
{
   int cols = CPXgetnumcols (env, lp);
   int rows = CPXgetnumrows (env, lp);
   int qcons = CPXgetnumqconstrs (env, lp);
   double *dslack = NULL, *pi = NULL, *socppi = NULL;
   double *val = NULL, *rhs = NULL;
   int *ind = NULL;
   char *sense = NULL;
   double *x = NULL, *slack = NULL, *qslack = NULL;
   double *sum = NULL;
   qbuf_type qbuf;
   CPXCHANNELptr resc, warnc, errc, logc;
   int ok = 0, skip = 0;
   int status;
   int i, j, q;

   qbuf_init (&qbuf);

   /* Get the channels on which we may report. */
   if ( (status = CPXgetchannels (env, &resc, &warnc, &errc, &logc)) != 0 )
      goto TERMINATE;

   /* Fetch results and problem data that we need to check the KKT
    * conditions.
    */
   display (logc, "Fetching results ... ");
   if ( (cols  > 0 && (dslack = malloc (cols *  sizeof (*dslack))) == NULL) ||
        (rows  > 0 && (pi =     malloc (rows *  sizeof (*pi)))     == NULL) ||
        (qcons > 0 && (socppi = malloc (qcons * sizeof (*socppi))) == NULL) ||
        (cols  > 0 && (x =      malloc (cols *  sizeof (*x)))      == NULL) ||
        (rows  > 0 && (sense =  malloc (rows *  sizeof (*sense)))  == NULL ) ||
        (rows  > 0 && (slack =  malloc (rows *  sizeof (*slack)))  == NULL ) ||
        (qcons > 0 && (qslack = malloc (qcons * sizeof (*qslack))) == NULL) ||
        (cols  > 0 && (sum =    malloc (cols *  sizeof (*sum)))    == NULL) ||
        (cols  > 0 && (val =    malloc (cols *  sizeof (*val)))    == NULL) ||
        (cols  > 0 && (ind =    malloc (cols *  sizeof (*ind)))    == NULL) ||
        (rows  > 0 && (rhs =    malloc (rows *  sizeof (*rhs)))    == NULL) )
   {
      display (errc, "Out of memory!\n");
      goto TERMINATE;
   }

   /* Fetch problem data. */
   if ( (status = CPXgetsense (env, lp, sense, 0, rows - 1)) != 0 )
      goto TERMINATE;
   if ( (status = CPXgetrhs (env, lp, rhs, 0, rows - 1)) != 0 )
      goto TERMINATE;

   /* Fetch solution information. */
   if ( (status = CPXgetx (env, lp, x, 0, cols - 1)) != 0 )
      goto TERMINATE;
   if ( (status = CPXgetpi (env, lp, pi, 0, rows - 1)) != 0 )
      goto TERMINATE;
   if ( (status = getsocpconstrmultipliers (env, lp, dslack, socppi)) != 0 )
      goto TERMINATE;
   if ( (status = CPXgetslack (env, lp, slack, 0, rows - 1)) != 0 )
      goto TERMINATE;
   if ( (status = CPXgetqconstrslack (env, lp, qslack, 0, qcons - 1)) != 0 )
      goto TERMINATE;
   display (logc, "ok.\n");

   /* Print out the solution data we just fetched. */
   display (resc, "x      = [");
   for (j = 0; j < cols; ++j)
      display (resc, " %+7.3f", x[j]);
   display (resc, " ]\n");
   display (resc, "dslack = [");
   for (j = 0; j < cols; ++j)
      display (resc, " %+7.3f", dslack[j]);
   display (resc, " ]\n");
   display (resc, "pi     = [");
   for (i = 0; i < rows; ++i)
      display (resc, " %+7.3f", pi[i]);
   display (resc, " ]\n");
   display (resc, "slack  = [");
   for (i = 0; i < rows; ++i)
      display (resc, " %+7.3f", slack[i]);
   display (resc, " ]\n");
   display (resc, "socppi = [");
   for (q = 0; q < qcons; ++q)
      display (resc, " %+7.3f", socppi[q]);
   display (resc, " ]\n");
   display (resc, "qslack = [");
   for (q = 0; q < qcons; ++q)
      display (resc, " %+7.3f", qslack[q]);
   display (resc, " ]\n");

   /* Test primal feasibility. */
   display (logc, "Testing primal feasibility ... ");
   /* This example illustrates the use of dual vectors returned by CPLEX
    * to verify dual feasibility, so we do not test primal feasibility
    * here. */
   display (logc, "ok.\n");

   /* Test dual feasibility.
    * We must have
    * - for all <= constraints the respective pi value is non-negative,
    * - for all >= constraints the respective pi value is non-positive,
    * - since all quadratic constraints are <= constraints the socppi
    *   value must be non-negative for all quadratic constraints,
    * - the dslack value for all non-cone variables must be non-negative.
    * Note that we do not support ranged constraints here.
    */
   display (logc, "Testing dual feasibility ... ");
   for (i = 0; i < rows; ++i) {
      switch (sense[i]) {
      case 'L':
         if ( pi[i] < -tol ) {
            display (errc, "<= row %d has invalid dual multiplier %f.\n",
                     i, pi[i]);
            goto TERMINATE;
         }
         break;
      case 'G':
         if ( pi[i] > tol ) {
            display (errc, ">= row %d has invalid dual multiplier %f.\n",
                     i, pi[i]);
            goto TERMINATE;
         }
         break;
      case 'E':
         /* Nothing to check here. */
         break;
      }
   }
   for (q = 0; q < qcons; ++q) {
      if ( socppi[q] < -tol ) {
         display (errc, "Quadratic constraint %d has invalid dual multiplier %f.\n",
                  q, socppi[q]);
         goto TERMINATE;
      }
   }
   for (j = 0; j < cols; ++j) {
      if ( cone[j] == NOT_IN_CONE && dslack[j] < -tol ) {
         display (errc, "dslack value for column %d is invalid: %f\n", j, dslack[j]);
         goto TERMINATE;
      }
   }
   display (logc, "ok.\n");

   /* Test complementary slackness.
    * For each constraint either the constraint must have zero slack or
    * the dual multiplier for the constraint must be 0. Again, we must
    * consider the special case in which a variable is not explicitly
    * contained in a second order cone constraint (conestat[j] == 0).
    */
   display (logc, "Testing complementary slackness ... ");
   for (i = 0; i < rows; ++i) {
      if ( fabs (slack[i]) > tol && fabs (pi[i]) > tol ) {
         display (errc, "Complementary slackness not satisfied for row %d (%f, %f)\n",
                  i, slack[i], pi[i]);
         goto TERMINATE;
      }
   }
   for (q = 0; q < qcons; ++q) {
      if ( fabs (qslack[q]) > tol && fabs (socppi[q]) > tol ) {
         display (errc, "Complementary slackness not satisfied for cone %d (%f, %f).\n",
                  q, qslack[q], socppi[q]);
         goto TERMINATE;
      }
   }
   for (j = 0; j < cols; ++j) {
      if ( cone[j] == NOT_IN_CONE ) {
         if ( fabs (x[j]) > tol && fabs (dslack[j]) > tol ) {
            display (errc, "Complementary slackness not satisfied for non-cone variable %f (%f, %f).\n",
                     j, x[j], dslack[j]);
            goto TERMINATE;
         }
      }
   }
   display (logc, "ok.\n");

   /* Test stationarity.
    * We must have
    *  c - g[i]'(X)*pi[i] = 0
    * where c is the objective function, g[i] is the i-th constraint of the
    * problem, g[i]'(x) is the derivate of g[i] with respect to x and X is the
    * optimal solution.
    * We need to distinguish the following cases:
    * - linear constraints g(x) = ax - b. The derivative of such a
    *   constraint is g'(x) = a.
    * - second order constraints g(x[1],...,x[n]) = -x[1] + |(x[2],...,x[n])|
    *   the derivative of such a constraint is
    *     g'(x) = (-1, x[2]/|(x[2],...,x[n])|, ..., x[n]/|(x[2],...,x[n])|
    *   (here |.| denotes the Euclidean norm).
    * - bound constraints g(x) = -x for variables that are not explicitly
    *   contained in any second order cone constraint. The derivative for
    *   such a constraint is g'(x) = -1.
    * Note that it may happen that the derivative of a second order cone
    * constraint is not defined at the optimal solution X (this happens if
    * X=0). In this case we just skip the stationarity test.
    */
   display (logc, "Testing stationarity ... ");
   /* Initialize sum = c. */
   if ( (status = CPXgetobj (env, lp, sum, 0, cols - 1)) != 0 )
      goto TERMINATE;

   /* Handle linear constraints. */
   for (i = 0; i < rows; ++i) {
      int nz, surplus, beg;
      int n;

      status = CPXgetrows (env, lp, &nz, &beg, ind, val, cols, &surplus,
                           i, i);
      if ( status != 0 )
         goto TERMINATE;
      for (n = 0; n < nz; ++n) {
         sum[ind[n]] -= pi[i] * val[n];
      }
   }
   /* Handle second order cone constraints. */
   for (q = 0; q < qcons; ++q) {
      double norm = 0.0;
      int n;

      if ( !getqconstr (env, lp, q, &qbuf) )
         goto TERMINATE;

      for (n = 0; n < qbuf.qnz; ++n) {
         if ( qbuf.qval[n] > 0 )
            norm += x[qbuf.qcol[n]] * x[qbuf.qcol[n]];
      }
      norm = sqrt (norm);
      if ( fabs (norm) <= tol ) {
         display (warnc, "WARNING: Cannot test stationarity at non-differentiable point.\n");
         skip = 1;
         break;
      }

      for (n = 0; n < qbuf.qnz; ++n) {
         if ( qbuf.qval[n] < 0 )
            sum[qbuf.qcol[n]] -= socppi[q];
         else
            sum[qbuf.qcol[n]] += socppi[q] * x[qbuf.qcol[n]] / norm;
      }
   }
   /* Handle variables that do not appear in any second order cone constraint.
    */
   for (j = 0; !skip && j < cols; ++j) {
      if ( cone[j] == NOT_IN_CONE ) {
         sum[j] -= dslack[j];
      }
   }

   /* Now test that all the entries in sum[] are 0.
    */
   for (j = 0; !skip && j < cols; ++j) {
      if ( fabs (sum[j]) > tol ) {
         display (errc, "Stationarity not satisfied at index %d: %f\n",
                  j, sum[j]);
         goto TERMINATE;
      }
   }
   display (logc, "ok.\n");

   display (logc, "KKT conditions are satisfied.\n");

   ok = 1;
TERMINATE:
   if ( !ok )
      display (logc, "failed.\n");
   qbuf_clear (&qbuf);
   free (rhs);
   free (ind);
   free (val);
   free (sum);
   free (qslack);
   free (slack);
   free (sense);
   free (x);
   free (socppi);
   free (pi);
   free (dslack);

   return ok;
}

/* This function creates the following model:
 *   Minimize
 *    obj: x1 + x2 + x3 + x4 + x5 + x6
 *   Subject To
 *    c1: x1 + x2      + x5      = 8
 *    c2:           x3 + x5 + x6 = 10
 *    q1: [ -x1^2 + x2^2 + x3^2 ] <= 0
 *    q2: [ -x4^2 + x5^2 ] <= 0
 *   Bounds
 *    x2 Free
 *    x3 Free
 *    x5 Free
 *   End
 * which is a second order cone program in standard form.
 * The function returns a true value on success and false on error.
 * The function also sets up *cone_p as follows:
 * (*cone_p)[j] >= 0              Column j is contained in a cone constraint
 *                                and is the cone head variable of that
 *                                constraint. The index of the respective
 *                                quadratic constraint is given by (*cone_p)[j].
 * (*cone_p)[j] = NOT_CONE_HEAD   Column j is contained in a cone constraint
 *                                but is not the cone head variable of that
 *                                constraint.
 * (*cone_p)[j] = NOT_IN_CONE     Column j is not contained in any cone
 *                                constraint.
 */
static int
createmodel (CPXENVptr env, CPXLPptr lp, int **cone_p)
{
   /* Column data. */
   static double const obj[]        = {  1.0,  1.0,  1.0,  1.0,  1.0,  1.0 };
   static double const lb[]         = {  0.0, -INF, -INF,  0.0, -INF,  0.0 };
   static double const ub[]         = {  INF,  INF,  INF,  INF,  INF,  INF };
   static char const *const cname[] = { "x1", "x2", "x3", "x4", "x5", "x6" };

   /* Row data. */
   static double const rval[]       = { 1.0, 1.0, 1.0,    1.0, 1.0, 1.0 };
   static int const rind[]          = {   0,   1,   4,      2,   4,   5 };
   static int const rbeg[]          = {   0,                3           };
   static double const rhs[]        = { 8.0,             10.0           };
   static char const sense[]        = { 'E',              'E'           };
   static char const *const rname[] = { "c1",             "c2"          };

   /* Data for second order cone constraints. */
   static double const qval[]  = { -1.0, 1.0, 1.0 }; /* Same for all Q cons. */
   static double const qrhs    = 0.0;                /* Same for all Q cons. */
   static char const qsense    = 'L';                /* Same for all Q cons. */
   static int const qind1[] = { 0, 1, 2 };
   static int const qind2[] = { 3, 4 };

   int status;
   int ok = 0;
   int *cone = NULL;

   CPXCHANNELptr errc;

   /* Get the channel for printing error messages. */
   if ( (status = CPXgetchannels (env, NULL, NULL, &errc, NULL)) != 0 )
      goto TERMINATE;

   cone = malloc ((sizeof (obj) / sizeof (obj[0])) * sizeof (*cone));
   if ( cone == NULL ) {
      display (errc, "Out of memory!\n");
      goto TERMINATE;
   }

   status = CPXchgobjsen (env, lp, CPX_MIN);
   if ( status != 0 )
      goto TERMINATE;

   status = CPXnewcols (env, lp, sizeof (obj) / sizeof (obj[0]),
                        obj, lb, ub, NULL, (char **)cname);
   if ( status != 0 )
      goto TERMINATE;

   status = CPXaddrows (env, lp, 0, sizeof (rhs) / sizeof (rhs[0]),
                        sizeof (rval) / sizeof (rval[0]), rhs, sense,
                        rbeg, rind, rval, NULL, (char **)rname);
   if ( status != 0 )
      goto TERMINATE;

   status = CPXaddqconstr (env, lp, 0, sizeof (qind1) / sizeof (qind1[0]),
                           qrhs, qsense, NULL, NULL,
                           qind1, qind1, qval, "q1");
   if ( status != 0 )
      goto TERMINATE;
   cone[0] = 0;
   cone[1] = NOT_CONE_HEAD;
   cone[2] = NOT_CONE_HEAD;
   status = CPXaddqconstr (env, lp, 0, sizeof (qind2) / sizeof (qind2[0]),
                           qrhs, qsense, NULL, NULL,
                           qind2, qind2, qval, "q2");
   if ( status != 0 )
      goto TERMINATE;
   cone[3] = 1;
   cone[4] = NOT_CONE_HEAD;

   cone[5] = NOT_IN_CONE;

   ok = 1;
TERMINATE:
   if ( !ok )
      free (cone);

   *cone_p = cone;

   return ok;
}

int
main (void)
{
   CPXENVptr env;
   CPXLPptr lp = NULL;
   int *cone = NULL;
   int status;
   CPXCHANNELptr resc, warnc, errc, logc;
   int retval = -1;

   /* Initialize CPLEX and get a reference to the output channels.
    * If any of this fails immediately terminate the program.
    */
   env = CPXopenCPLEX (&status);
   if ( env == NULL || status != 0 )
      abort ();

   status = CPXgetchannels (env, &resc, &warnc, &errc, &logc);
   if ( status != 0 )
      abort ();

   /* CPLEX is fully setup. Enable output.
    */
   status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status != 0 )
      goto TERMINATE;

   /* Create model. */
   lp = CPXcreateprob (env, &status, "xsocpex1");
   if ( lp == NULL || status != 0 )
      goto TERMINATE;
   if ( !createmodel (env, lp, &cone) )
      goto TERMINATE;

   /* Solve the problem to optimality. */
   display (logc, "Optimizing ...\n");
   status = CPXsetdblparam (env, CPXPARAM_Barrier_QCPConvergeTol, CONVTOL);
   if ( status != 0 )
      goto TERMINATE;
   if ( (status = CPXhybbaropt (env, lp, CPX_ALG_NONE)) != 0 )
      goto TERMINATE;

   if ( CPXgetstat (env, lp) != CPX_STAT_OPTIMAL ) {
      display (errc, "Cannot test KKT conditions on non-optimal solution.\n");
      goto TERMINATE;
   }

   /* Now test KKT conditions on the result. */
   if ( !checkkkt (env, lp, cone, TESTTOL) ) {
      display (logc, "Testing of KKT conditions failed.\n");
      display (errc, "Testing of KKT conditions failed.\n");
      goto TERMINATE;
   }

   display (resc, "KKT conditions are satisfied.\n");
   retval = 0;
TERMINATE:
   free (cone);
   if ( lp != NULL )
      CPXfreeprob (env, &lp);
   CPXcloseCPLEX (&env);

   return retval;
}
