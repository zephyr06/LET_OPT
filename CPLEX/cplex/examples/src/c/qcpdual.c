/* --------------------------------------------------------------------------
 * File: qpcdual.c
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

/* qcpdual.c - Illustrates how to query and analyze dual values of a QCP.
 */

#include <math.h>
#include <stdio.h>
#include <ilcplex/cplex.h>

/* The tolerance we apply when testing whether values are zero or not.
 */
#define ZEROTOL 1e-6

/* Data for the following model:
Minimize
 obj: 3x1 - x2 + 3x3 + 2x4 + x5 + 2x6 + 4x7
Subject To
 c1: x1 + x2 = 4
 c2: x1 + x3 >= 3
 c3: x6 + x7 <= 5
 c4: -x1 + x7 >= -2
 q1: [ -x1^2 + x2^2 ] <= 0
 q2: [ 4.25x3^2 -2x3*x4 + 4.25x4^2 - 2x4*x5 + 4x5^2  ] + 2 x1 <= 9.0
 q3: [ x6^2 - x7^2 ] >= 4
Bounds
 0 <= x1 <= 3
 x2 Free
 0 <= x3 <= 0.5
 x4 Free
 x5 Free
 x7 Free
End
 */
static double const obj[] = { 3.0, -1.0, 3, 2, 1, 2, 4 };
static double const lb[]  = { 0.0, -CPX_INFBOUND, 0, -CPX_INFBOUND,
                              -CPX_INFBOUND, 0.0, -CPX_INFBOUND };
static double const ub[]  = { 3.0, CPX_INFBOUND, 0.5, CPX_INFBOUND,
                              CPX_INFBOUND, CPX_INFBOUND, CPX_INFBOUND };
static char *cname[]      = { "x1", "x2", "x3", "x4", "x5", "x6", "x7" };
#define NUMCOLS ((int)(sizeof (obj) / sizeof (obj[0])))

static double const rhs[]     = { 4.0, 3.0, 5.0, -2.0 };
static char const sense[]     = { 'E', 'G', 'L', 'G' };
static int const rmatbeg[]    = { 0,         2,         4,         6 };
static int const rmatind[]    = { 0, 1,      0, 2,      5, 6,      0, 6 };
static double const rmatval[] = { 1.0, 1.0,  1.0, 1.0,  1.0, 1.0,  -1.0, 1.0 };
static char *rname[]          = { "c1", "c2", "c3", "c4" };
#define NUMROWS ((int)(sizeof (rhs) / sizeof (rhs[0])))
#define NUMNZS  ((int)(sizeof (rmatind) / sizeof (rmatind[0])))

static int const linnzcnt[]   = { 0, 1, 0 };
static int const quadnzcnt[]  = { 2, 5, 2 };
static double const qrhs[]    = { 0, 9.0, 4.0 };
static char const qsense[]    = { 'L', 'L', 'G' };
static int const linbeg[]     = { 0, 0, 1 };
static int const linind[]     = { 0 };
static double const linval[]  = { 2.0 };
static int const quadbeg[]    = { 0,       2,               7 };
static int const quadrow[]    = { 0, 1,    2, 2, 3, 3, 4,   5, 6 };
static int const quadcol[]    = { 0, 1,    2, 3, 3, 4, 4,   5, 6 };
static double const quadval[] = { -1.0, 1.0,
                                  4.25, -2.0, 4.25, -2.0, 4.0,
                                  1.0, -1.0 };
static char *qname[]          = { "q1", "q2", "q3" };
#define NUMQS ((int)(sizeof (qrhs) / sizeof (qrhs[0])))
#define NUMLINNZ ((int)(sizeof (linval) / sizeof (linval[0])))
#define NUMQUADNZ ((int)(sizeof (quadval) / sizeof (quadval[0])))

/* ********************************************************************** *
 *                                                                        *
 *    C A L C U L A T E   D U A L S   F O R   Q U A D   C O N S T R S     *
 *                                                                        *
 *    CPLEX does not give us the dual multipliers for quadratic           *
 *    constraints directly. This is because they may not be properly      *
 *    defined at the cone top and deciding whether we are at the cone     *
 *    top or not involves (problem specific) tolerance issues. CPLEX      *
 *    instead gives us all the values we need in order to compute the     *
 *    dual multipliers if we are not at the cone top.                     *
 *    Function getqconstrmultipliers() takes the following arguments:     *
 *    env       CPLEX environment that was used to create LP.             *
 *    lp        CPLEX problem object for which the dual multipliers are   *
 *              to be calculated.                                         *
 *    x         The optimal solution.                                     *
 *    qpi       Array that stores the dual multipliers upon success.      *
 *    zerotol   The tolerance that is used to decide whether a value      *
 *              if zero or not (used to decide about "cone top" or not).  *
 *                                                                        *
 * ********************************************************************** */
static int
getqconstrmultipliers (CPXCENVptr   env,
                       CPXCLPptr    lp,
                       double const *x,
                       double       *qpi,
                       double       zerotol)
{
   int const cols = CPXgetnumcols (env, lp);
   int const numqs = CPXgetnumqconstrs (env, lp);
   double *dense = NULL;
   double *grad = NULL;
   int *slackind = NULL;
   double *slackval = NULL;
   int status = 0;
   int i;

   if ( (dense    = malloc (sizeof (*dense) * cols)) == NULL ||
        (grad     = malloc (sizeof (*grad) * cols)) == NULL ||
        (slackind = malloc (sizeof (*slackind) * cols)) == NULL ||
        (slackval = malloc (sizeof (*slackval) * cols)) == NULL )
   {
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   /* Calculate dual multipliers for quadratic constraints from dual
    * slack vectors and optimal solutions.
    * The dual multiplier is essentially the dual slack divided
    * by the derivative evaluated at the optimal solution. If the optimal
    * solution is 0 then the derivative at this point is not defined (we are
    * at the cone top) and we cannot compute a dual multiplier.
    */
   for (i = 0; i < numqs; ++i) {
      int j, k;
      int surplus, len;
      int conetop;

      /* Clear out dense slack and gradient vector. */
      for (j = 0; j < cols; ++j) {
         dense[j] = 0.0;
         grad[j] = 0;
      }

      /* Get dual slack vector and expand it to a dense vector. */
      status = CPXgetqconstrdslack (env, lp, i, &len, slackind, slackval,
                                    cols, &surplus);
      if ( status != 0 )
         goto TERMINATE;
      for (k = 0; k < len; ++k)
         dense[slackind[k]] = slackval[k];

      /* Compute value of derivative at optimal solution. */
      /* The derivative of a quadratic constraint x^TQx + a^Tx + b <= 0
       * is Q^Tx + Qx + a.
       */
      conetop = 1;
      for (k = quadbeg[i]; k < quadbeg[i] + quadnzcnt[i]; ++k) {
         if ( fabs (x[quadrow[k]]) > zerotol ||
              fabs (x[quadcol[k]]) > zerotol )
            conetop = 0;
         grad[quadcol[k]] += quadval[k] * x[quadrow[k]];
         grad[quadrow[k]] += quadval[k] * x[quadcol[k]];
      }
      for (j = linbeg[i]; j < linbeg[i] + linnzcnt[i]; ++j) {
         grad[linind[j]] += linval[j];
         if ( fabs (x[linind[j]]) > zerotol )
            conetop = 0;
      }

      if ( conetop ) {
         fprintf (stderr,
                  "#### WARNING: Cannot compute dual multipler at cone top!\n");
         status = CPXERR_BAD_ARGUMENT;
         goto TERMINATE;
      }
      else {
         int ok = 0;
         double maxabs = -1.0;

         /* Compute qpi[i] as slack/gradient.
          * We may have several indices to choose from and use the one
          * with largest absolute value in the denominator.
          */
         for (j = 0; j < cols; ++j) {
            if ( fabs (grad[j]) > zerotol ) {
               if ( fabs (grad[j]) > maxabs ) {
                  qpi[i] = dense[j] / grad[j];
                  maxabs = fabs (grad[j]);
               }
               ok = 1;
            }
         }
            
         if ( !ok ) {
            /* Dual slack is all 0. qpi[i] can be anything, just set
             * to 0. */
            qpi[i] = 0.0;
         }
      }
   }

 TERMINATE:
   free (slackval);
   free (slackind);
   free (grad);
   free (dense);

   return status;
}

int
main (void)
{
   int status, solstat;
   CPXENVptr env;
   CPXLPptr lp;
   int i;
   double x[NUMCOLS];
   double cpi[NUMCOLS];
   double rpi[NUMROWS];
   double qpi[NUMQS];
   double slack[NUMROWS], qslack[NUMQS];
   double kktsum[NUMCOLS];

   /* ********************************************************************** *
    *                                                                        *
    *    S E T U P   P R O B L E M                                           *
    *                                                                        *
    * ********************************************************************** */

   /* Create CPLEX environment and enable screen output.
    */
   env = CPXopenCPLEX (&status);
   if ( status != 0 )
      goto TERMINATE;
   status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status != 0 )
      goto TERMINATE;

   /* Create the problem object and populate it.
    */
   lp = CPXcreateprob (env, &status, "qcpdual");
   if ( status != 0 )
      goto TERMINATE;
   status = CPXnewcols (env, lp, NUMCOLS, obj, lb, ub, NULL, cname);
   if ( status != 0 )
      goto TERMINATE;
   status = CPXaddrows (env, lp, 0, NUMROWS, NUMNZS, rhs, sense,
                        rmatbeg, rmatind, rmatval, NULL, rname);
   if ( status != 0 )
      goto TERMINATE;
   for (i = 0; i < NUMQS; ++i) {
      int const linend = (i == NUMQS - 1) ? NUMLINNZ : linbeg[i + 1];
      int const quadend = (i == NUMQS - 1) ? NUMQUADNZ : quadbeg[i + 1];

      status = CPXaddqconstr (env, lp, linend - linbeg[i],
                              quadend - quadbeg[i], qrhs[i], qsense[i],
                              &linind[linbeg[i]], &linval[linbeg[i]],
                              &quadrow[quadbeg[i]], &quadcol[quadbeg[i]],
                              &quadval[quadbeg[i]], qname[i]);
      if ( status != 0 )
         goto TERMINATE;
   }

   /* ********************************************************************** *
    *                                                                        *
    *    O P T I M I Z E   P R O B L E M                                     *
    *                                                                        *
    * ********************************************************************** */
   status = CPXsetdblparam (env, CPXPARAM_Barrier_QCPConvergeTol, 1e-10);
   if ( status != 0 )
      goto TERMINATE;

   /* Solve the problem.
    */
   status = CPXbaropt (env, lp);
   if ( status != 0 )
      goto TERMINATE;

   solstat = CPXgetstat (env, lp);

   if ( solstat != CPX_STAT_OPTIMAL ) {
      fprintf (stderr, "No optimal solution found!\n");
      goto TERMINATE;
   }

   /* ********************************************************************** *
    *                                                                        *
    *    Q U E R Y   S O L U T I O N                                         *
    *                                                                        *
    * ********************************************************************** */

   /* Optimal solution and slacks for linear and quadratic constraints. */
   status = CPXgetx (env, lp, x, 0, NUMCOLS - 1);
   if ( status != 0 )
      goto TERMINATE;
   status = CPXgetslack (env, lp, slack, 0, NUMROWS - 1);
   if ( status != 0 )
      goto TERMINATE;
   status = CPXgetqconstrslack (env, lp, qslack, 0, NUMQS - 1);
   if ( status != 0 )
      goto TERMINATE;
   /* Dual multipliers for linear constraints and bound constraints. */
   status = CPXgetdj (env, lp, cpi, 0, NUMCOLS - 1);
   if ( status != 0 )
      goto TERMINATE;
   status = CPXgetpi (env, lp, rpi, 0, NUMROWS - 1);
   if ( status != 0 )
      goto TERMINATE;
   status = getqconstrmultipliers (env, lp, x, qpi, ZEROTOL);
   if ( status != 0 )
      goto TERMINATE;

   /* ********************************************************************** *
    *                                                                        *
    *    C H E C K   K K T   C O N D I T I O N S                             *
    *                                                                        *
    *    Here we verify that the optimal solution computed by CPLEX (and     *
    *    the qpi[] values computed above) satisfy the KKT conditions.        *
    *                                                                        *
    * ********************************************************************** */

   /* Primal feasibility: This example is about duals so we skip this test. */

   /* Dual feasibility: We must verify
    * - for <= constraints (linear or quadratic) the dual
    *   multiplier is non-positive.
    * - for >= constraints (linear or quadratic) the dual
    *   multiplier is non-negative.
    */
   for (i = 0; i < NUMROWS; ++i) {
      switch (sense[i]) {
      case 'E': /* nothing */ break;
      case 'R': /* nothing */ break;
      case 'L':
         if ( rpi[i] > ZEROTOL ) {
            fprintf (stderr,
                     "Dual feasibility test failed for <= row %d: %f\n",
                     i, rpi[i]);
            status = -1;
            goto TERMINATE;
         }
         break;
      case 'G':
         if ( rpi[i] < -ZEROTOL ) {
            fprintf (stderr,
                     "Dual feasibility test failed for >= row %d: %f\n",
                     i, rpi[i]);
            status = -1;
            goto TERMINATE;
         }
         break;
      }
   }
   for (i = 0; i < NUMQS; ++i) {
      switch (qsense[i]) {
      case 'E': /* nothing */ break;
      case 'L':
         if ( qpi[i] > ZEROTOL ) {
            fprintf (stderr,
                     "Dual feasibility test failed for <= quad %d: %f\n",
                     i, qpi[i]);
            status = -1;
            goto TERMINATE;
         }
         break;
      case 'G':
         if ( qpi[i] < -ZEROTOL ) {
            fprintf (stderr,
                     "Dual feasibility test failed for >= quad %d: %f\n",
                     i, qpi[i]);
            status = -1;
            goto TERMINATE;
         }
         break;
      }
   }

   /* Complementary slackness.
    * For any constraint the product of primal slack and dual multiplier
    * must be 0.
    */
   for (i = 0; i < NUMROWS; ++i) {
      if ( sense[i] != 'E' && fabs (slack[i] * rpi[i]) > ZEROTOL ) {
         fprintf (stderr,
                  "Complementary slackness test failed for row %d: %f\n",
                  i, fabs (slack[i] * rpi[i]));
         status = -1;
         goto TERMINATE;
      }
   }
   for (i = 0; i < NUMQS; ++i) {
      if ( qsense[i] != 'E' && fabs (qslack[i] * qpi[i]) > ZEROTOL ) {
         fprintf (stderr,
                  "Complementary slackness test failed for quad %d: %f\n",
                  i, fabs (qslack[i] * qpi[i]));
         status = -1;
         goto TERMINATE;
      }
   }
   for (i = 0; i < NUMCOLS; ++i) {
      if ( ub[i] < CPX_INFBOUND ) {
         double const slk = ub[i] - x[i];
         double const dual = cpi[i] < -ZEROTOL ? cpi[i] : 0.0;
         if ( fabs (slk * dual) > ZEROTOL ) {
            fprintf (stderr,
                     "Complementary slackness test failed for ub %d: %f\n",
                     i, fabs (slk * dual));
            status = -1;
            goto TERMINATE;
         }
      }
      if ( lb[i] > -CPX_INFBOUND ) {
         double const slk = x[i] - lb[i];
         double const dual = cpi[i] > ZEROTOL ? cpi[i] : 0.0;
         if ( fabs (slk * dual) > ZEROTOL ) {
            printf ("lb=%f, x=%f, cpi=%f\n", lb[i], x[i], cpi[i]);
            fprintf (stderr,
                     "Complementary slackness test failed for lb %d: %f\n",
                     i, fabs (slk * dual));
            status = -1;
            goto TERMINATE;
         }
      }
   }

   /* Stationarity.
    * The difference between objective function and gradient at optimal
    * solution multiplied by dual multipliers must be 0, i.e., for the
    * optimal solution x
    * 0 == c
    *      - sum(r in rows)  r'(x)*rpi[r]
    *      - sum(q in quads) q'(x)*qpi[q]
    *      - sum(c in cols)  b'(x)*cpi[c]
    * where r' and q' are the derivatives of a row or quadratic constraint,
    * x is the optimal solution and rpi[r] and qpi[q] are the dual
    * multipliers for row r and quadratic constraint q.
    * b' is the derivative of a bound constraint and cpi[c] the dual bound
    * multiplier for column c.
    */

   /* Objective function. */
   for (i = 0; i < NUMCOLS; ++i)
      kktsum[i] = obj[i];

   /* Linear constraints.
    * The derivative of a linear constraint ax - b (<)= 0 is just a.
    */
   for (i = 0; i < NUMROWS; ++i) {
      int const end = (i == NUMROWS - 1) ? NUMNZS : rmatbeg[i + 1];
      int k;

      for (k = rmatbeg[i]; k < end; ++k)
         kktsum[rmatind[k]] -= rpi[i] * rmatval[k];
   }

   /* Quadratic constraints.
    * The derivative of a constraint xQx + ax - b <= 0 is
    * Qx + Q'x + a.
    */
   for (i = 0; i < NUMQS; ++i) {
      int j;
      int k;

      for (j = linbeg[i]; j < linbeg[i] + linnzcnt[i]; ++j)
         kktsum[linind[j]] -= qpi[i] * linval[j];
      for (k = quadbeg[i]; k < quadbeg[i] + quadnzcnt[i]; ++k) {
         kktsum[quadrow[k]] -= qpi[i] * x[quadcol[k]] * quadval[k];
         kktsum[quadcol[k]] -= qpi[i] * x[quadrow[k]] * quadval[k];
      }
   }

   /* Bounds.
    * The derivative for lower bounds is -1 and that for upper bounds
    * is 1.
    * CPLEX already returns dj with the appropriate sign so there is
    * no need to distinguish between different bound types here.
    */
   for (i = 0; i < NUMCOLS; ++i) {
      kktsum[i] -= cpi[i];
   }

   for (i = 0; i < NUMCOLS; ++i) {
      if ( fabs (kktsum[i]) > ZEROTOL ) {
         fprintf (stderr, "Stationarity test failed at index %d: %f\n",
                  i, kktsum[i]);
         status = -1;
         goto TERMINATE;
      }
   }

   /* KKT conditions satisfied. Dump out the optimal solutions and
    * the dual values.
    */

   printf ("Optimal solution satisfies KKT conditions.\n");
   printf ("  x[] =");
   for (i = 0; i < NUMCOLS; ++i)
      printf (" %7.3f", x[i]);
   printf ("\n");
   printf ("cpi[] =");
   for (i = 0; i < NUMCOLS; ++i)
      printf (" %7.3f", cpi[i]);
   printf ("\n");
   printf ("rpi[] =");
   for (i = 0; i < NUMROWS; ++i)
      printf (" %7.3f", rpi[i]);
   printf ("\n");
   printf ("qpi[] =");
   for (i = 0; i < NUMQS; ++i)
      printf (" %7.3f", qpi[i]);
   printf ("\n");
   
 TERMINATE:
   /* ********************************************************************** *
    *                                                                        *
    *    C L E A N U P                                                       *
    *                                                                        *
    * ********************************************************************** */

   status = CPXfreeprob (env, &lp);
   if ( status != 0 ) {
      fprintf (stderr, "WARNING: Failed to free problem: %d\n", status);
   }
   status = CPXcloseCPLEX (&env);
   if ( status != 0 ) {
      fprintf (stderr, "WARNING: Failed to close CPLEX: %d\n", status);
   }

   return status;
}
