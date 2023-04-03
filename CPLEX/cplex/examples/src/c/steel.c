/* --------------------------------------------------------------------------
 * File: steel.c
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

/* This example is an implementation of the model called "steelT.mod"
 * in the AMPL book by Fourer, Gay and Kernighan.  In the AMPL
 * example, a multiperiod production model is given, with data
 * for 4 weeks.  
 *
 * The parameters for the model are:
 *   PROD (indexed by p<NUMPROD)  a set of NUMPROD products
 *   TWEEKS = 4 (indexed by t<4)  the number of weeks
 *   rate[p]                      rate of production, product p
 *   inv0[p]                      initial inventory, product p
 *   avail[t]                     hours available in week t
 *   market[p][t]                 limit on demand for product p, week t
 *   prodcost[p]                  production cost per unit of product p 
 *   invcost[p]                   inventory cost per unit of product p
 *   revenue[p][t]                revenue per unit, product p, week t
 *
 * The decision variables of the model are:
 *   Make[p][t]                   amount produced, product p, week t
 *   Inv[p][t]                    amount inventoried, product p, week t
 *   Sell[p][t]                   amount sold, product p, week t
 * 
 * The objective function is to
 * 
 * maximize  sum(over p,t) (
 *                 revenue[p][t] * Sell[p][t]
 *               - prodcost[p]   * Make[p][t]
 *               - invcost[p]    * Inv[p][t]   )
 *
 * The constraints are
 * 
 *  For each t:   (time constraint)
 *      sum(over p)  ( ( 1/rate[p] ) * Make[p][t] ) <= avail[t]
 * 
 *  For each pair (p,t) (t>0): (balance constraint)
 *      Make[p][t] + Inv[p][t-1] - Sell[p][t] - Inv[p][t] = 0
 *
 *  For each p, (t=0): (balance constraint)
 *      Make[p][0] - Sell[p][0] - Inv[p][0] = -inv0[p]
 *
 *  The bounds on the variables are:
 *    All variables are nonnegative ( >= 0 )
 *    For each pair (p,t),
 *       Sell[p][t] <= market[p][t]
 *    All other variables have infinite upper bounds.
 *
 *
 *  To generate the CPLEX data structure for this model, we need to
 *    make an association between each row and an index number,
 *    and each variable and an index number.  
 *  For the rows:
 *    Constraint  time(t)      will be row i = t
 *    Constraint  balance(p,t) will be row 
 *                    i = TWEEKS + p*TWEEKS + t
 *
 *  For the columns:
 *    Make[p][t]  will be column 
 *            j = p*TWEEKS + t
 *    Inv[p][t]   will be column 
 *            j = (NUMPROD+p)*TWEEKS + t
 *    Sell[p][t]  will be column
 *            j = (2*NUMPROD+p)*TWEEKS + t
 *
 *  Note:  In the AMPL book, time was counted from 1 to T, with
 *     0 corresponding to the initial inventory levels.  In our
 *     implementation, we have chosen to have time run from period 0
 *     to period (TWEEKS-1) and place the initial inventory
 *     levels on the right-hand side of the balance constraints for
 *     time period 0.
 */

/* Define problem size constants */

#define  TWEEKS   4
#define  NUMPROD  2


/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include. */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ilcplex/cplex.h>



/* static function declarations */

static int 
   steelt_opt_and_soln (void (CPXPUBLIC *errmsgfunc)(void *, const char *), 
                        void *handle, int numprod, int tweeks, double *rate,
                        double *inv0, double *avail, double *flatmarket,
                        double *prodcost, double *invcost, double *flatrevenue,
                        int *lpstat_p, double *objval_p, double *flatmake, 
                        double *flatinv, double *flatsell, int buildbycolumn),
   colsteel (int numprod, int tweeks, double *rate,
             double *inv0, double *avail, double *flatmarket,
             double *prodcost, double *invcost, double *flatrevenue,
             CPXENVptr env, CPXLPptr lp),
   rowsteel (int numprod, int tweeks, double *rate,
             double *inv0, double *avail, double *flatmarket,
             double *prodcost, double *invcost, double *flatrevenue,
             CPXENVptr env, CPXLPptr lp);

static void CPXPUBLIC
   errormessage (void *handle, const char *string);

static void
   usage (char *progname);

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


/* This is the driver function to solve the Steel production
 * model.  The data is defined at compile time, and the routine
 * steelt_opt_and_soln is called to generate and optimize the problem,
 * returning the part of the solution we're interested in.
 */
int
main(int argc, char **argv)
{
   int    status = 0;

   double rate[NUMPROD] = { 200.0, 140.0 };
   double inv0[NUMPROD] = { 10.0, 0.0 };
   double avail[TWEEKS] = { 40.0, 40.0, 32.0, 40.0 };
   double market[NUMPROD][TWEEKS] =
                          { { 6000.0, 6000.0, 4000.0, 6500.0 },
                            { 4000.0, 2500.0, 3500.0, 4200.0 } };
   double prodcost[NUMPROD] = { 10.0, 11.0 };
   double invcost[NUMPROD]  = {  2.5,  3.0 };
   double revenue[NUMPROD][TWEEKS] =
                          { { 25.0, 26.0, 27.0, 27.0 },
                            { 30.0, 35.0, 37.0, 39.0 } };
   char   *prodnames[NUMPROD] = { "bands", "coils" };

   double  make[NUMPROD][TWEEKS];
   double  inv[NUMPROD][TWEEKS];
   double  sell[NUMPROD][TWEEKS];
   int     lpstat = -1;
   double  objval;
   int     buildbycolumn = 1;

   int    t,p;      /* Various loop counters */

   /* Check and evaluate the command line arguments */

   if ( argc >= 2 ) {
      if (( argc != 2 )                                        ||
          ( argv[1][0] != '-' )                                ||
          ( strchr ("rc", argv[1][1]) == NULL )  ) {
         usage (argv[0]);
         goto TERMINATE;
      }
      if      ( argv[1][1] == 'r' )  buildbycolumn = 0;
      else if ( argv[1][1] == 'c' )  buildbycolumn = 1;
      else {
         usage (argv[0]);
         goto TERMINATE;
      }
   }

   status = steelt_opt_and_soln (errormessage, stderr,
                                 NUMPROD, TWEEKS, rate, inv0,
                                 avail, &market[0][0], prodcost,
                                 invcost, &revenue[0][0], &lpstat,
                                 &objval, &make[0][0], &inv[0][0],
                                 &sell[0][0], buildbycolumn);
   if ( status )  goto TERMINATE;

   if ( lpstat != CPX_STAT_OPTIMAL ) {
      printf ("Solution not optimal!!\n");
      goto TERMINATE;
   }

   printf ("Solution status:  %d\n", lpstat);
   printf ("Objective value:  %g\n", objval);

   printf ("\nMake:\n%5s ","Week");
   for (p = 0; p < NUMPROD; p++) {
      printf ("%10s ",prodnames[p]);
   }
   printf ("\n");
   for (t = 0; t < TWEEKS; t++) {
      printf ("%5d ",t+1);
      for (p = 0; p < NUMPROD; p++) {
         printf ("%10g ",make[p][t]);
      }
      printf ("\n");
   }

   printf ("\nInv:\n%5s ","Week");
   for (p = 0; p < NUMPROD; p++) {
      printf ("%10s ",prodnames[p]);
   }
   printf ("\n");
   printf ("%5d ",0);
   for (p = 0; p < NUMPROD; p++) {
      printf ("%10g ",inv0[p]);
   }
   printf ("\n");
   for (t = 0; t < TWEEKS; t++) {
      printf ("%5d ",t+1);
      for (p = 0; p < NUMPROD; p++) {
         printf ("%10g ",inv[p][t]);
      }
      printf ("\n");
   }

   printf ("\nSell:\n%5s ","Week");
   for (p = 0; p < NUMPROD; p++) {
      printf ("%10s ",prodnames[p]);
   }
   printf ("\n");
   for (t = 0; t < TWEEKS; t++) {
      printf ("%5d ",t+1);
      for (p = 0; p < NUMPROD; p++) {
         printf ("%10g ",sell[p][t]);
      }
      printf ("\n");
   }


TERMINATE:

   return (status);
} /* END main */

/* This function takes as input the following variables:
 *  numprod        number of products
 *  tweeks         number of weeks
 *  rate           array of numprod doubles containing rate of production
 *  inv0           array of numprod doubles containing initial inventories
 *  avail          array of tweeks doubles containing available hours
 *  flatmarket     array of tweeks*numprod doubles containing limit on demand
 *                 for product p, time t in position flatmarket[p*tweeks+t]
 *  prodcost       array of numprod doubles containing production costs
 *  invcost        array of numprod doubles containing inventory costs
 *  flatrevenue    array of tweeks*numprod doubles containing revenue for
 *                 product p, time t in position flatrevenue[p*tweeks+t]
 *  buildbycolumn  1: to build model by column, 0: to build by row
 *
 * As output, the function produces the following outputs:
 *  *lpstat_p     receives the status from the LP optimization
 *  *objval_p     receives the optimal objective value
 *  flatmake      should be an array of tweeks*numprod doubles, and will be
 *                filled with Make[p][t] in position flatmake[p*tweeks+t]
 *  flatinv       should be an array of tweeks*numprod doubles, and will be
 *                filled with Inv[p][t] in position flatinv[p*tweeks+t]
 *  flatsell      should be an array of tweeks*numprod doubles, and will be
 *                filled with Sell[p][t] in position flatsell[p*tweeks+t]
 */

static int
steelt_opt_and_soln (void (CPXPUBLIC *errmsgfunc)(void *, const char *),
                     void *handle, int numprod, int tweeks, double *rate,
                     double *inv0, double *avail, double *flatmarket,
                     double *prodcost, double *invcost, double *flatrevenue,
                     int *lpstat_p, double *objval_p, double *flatmake,
                     double *flatinv, double *flatsell, int buildbycolumn)
{
   char   *probname = "steelT";

   CPXENVptr     env = NULL;
   CPXLPptr      lp  = NULL;
   CPXCHANNELptr cpxerror   = NULL;
   CPXCHANNELptr cpxwarning = NULL;
   int    status = 0;
   int    lpstat;
   double objval;


   env = CPXopenCPLEX (&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  We'll call the errmsgfunc with the error, because
      we can't call CPXmsgstr if CPXopenCPLEX failed.  */

   if ( env == NULL ) {
      char  errmsg[CPXMESSAGEBUFSIZE];
      if ( errmsgfunc != NULL ) {
         (*errmsgfunc) (handle, "Could not open CPLEX environment.\n");
         CPXgeterrorstring (env, status, errmsg);
         (*errmsgfunc) (handle, errmsg);
      }
      goto TERMINATE;
   }

   status = CPXgetchannels (env, NULL, &cpxwarning, &cpxerror, NULL);
   if ( status ) {
      if ( errmsgfunc != NULL ) {
         (*errmsgfunc) (handle, "Error in CPXgetchannels.\n");
      }
      goto TERMINATE;
   }

   if ( errmsgfunc != NULL ) {
      status = CPXaddfuncdest (env, cpxerror, handle, errmsgfunc);
      if ( !status ) {
	 status = CPXaddfuncdest (env, cpxwarning, handle, errmsgfunc);
      }

      if ( status ) {
	 (*errmsgfunc) (handle, "Error in CPXaddfuncdest.\n");
	 goto TERMINATE;
      }
   }

   /* Now that the channels have the error message function, we can
      use CPXmsgstr (via display) for errors. */

   /* Create the problem */

   lp = CPXcreateprob (env, &status, probname);

   if ( lp == NULL ) {
      status = -2;
      display (cpxerror, "Failed to create LP.\n");
      goto TERMINATE;
   }

   if ( buildbycolumn )
      status = colsteel (numprod, tweeks, rate, inv0, avail, flatmarket,
                         prodcost, invcost, flatrevenue, env, lp);
   else
      status = rowsteel (numprod, tweeks, rate, inv0, avail, flatmarket,
                         prodcost, invcost, flatrevenue, env, lp);

   if ( status )  goto TERMINATE;


   status = CPXlpopt (env, lp);
   if ( status ) {
      display (cpxerror,"Optimization failed. Status %d.\n", status);
      goto TERMINATE;
   }

   status = CPXsolution (env, lp, &lpstat, &objval, NULL, NULL, NULL, NULL);
   if ( status ) {
      display (cpxerror, "Solution failed. Status %d.\n", status);
      goto TERMINATE;
   }

   *objval_p = objval;
   *lpstat_p = lpstat;

   /* Now get the solution.  Use the fact that the variables are
    * added in the order Make, Inv, Sell, and that the slices of
    * the x vector can be used to copy over the solution into
    * the flat... arrays. */

   /* Get the Make variables */
   status = CPXgetx (env, lp, flatmake, 0, numprod*tweeks-1);
   if ( status ) {
      display (cpxerror, "CPXgetx failed to get Make solution.  Status %d.\n",
               status);
      goto TERMINATE;
   }

   /* Get the Inv variables */
   status = CPXgetx (env, lp, flatinv, numprod*tweeks, 2*numprod*tweeks-1);
   if ( status ) {
      display (cpxerror, "CPXgetx failed to get Inv solution.  Status %d.\n",
               status);
      goto TERMINATE;
   }

   /* Get the Sell variables */
   status = CPXgetx (env, lp, flatsell, 2*numprod*tweeks,
                     3*numprod*tweeks-1);
   if ( status ) {
      display (cpxerror, "CPXgetx failed to get Sell solution.  Status %d.\n",
               status);
      goto TERMINATE;
   }

TERMINATE:
   if ( lp != NULL )   CPXfreeprob (env, &lp);

   if ( errmsgfunc != NULL && env != NULL ) {
      int  dfstat;
      dfstat = CPXdelfuncdest (env, cpxerror, handle, errmsgfunc);
      if ( !dfstat ) {
         dfstat = CPXdelfuncdest (env, cpxwarning, handle, errmsgfunc);
      }
      if ( dfstat ) {
         (*errmsgfunc) (handle, "CPXdelfuncdest failed.\n");
      }
      if ( dfstat && !status )  status = dfstat;
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      int  closestat;
      closestat = CPXcloseCPLEX (&env);

      if ( closestat && errmsgfunc != NULL) {
      char  errmsg[CPXMESSAGEBUFSIZE];
         (*errmsgfunc) (handle, "Could not close CPLEX environment.\n");
         CPXgeterrorstring (env, closestat, errmsg);
         (*errmsgfunc) (handle, errmsg);
      }
      if ( closestat && !status ) status = closestat;
   }

   return (status);

} /* END steelt_opt_and_soln */


/* The following functions generate an instance of the steel example.
 * They receive the data defining the model, and place the model
 * characteristics into the lp object already created by the caller.
 * The input parameters are defined as
 *
 *  numprod     number of products
 *  tweeks      number of weeks
 *  rate        array of numprod doubles containing rate of production
 *  inv0        array of numprod doubles containing initial inventories
 *  avail       array of tweeks doubles containing available hours
 *  flatmarket  array of tweeks*numprod doubles containing limit on demand
 *                 for product p, time t in position flatmarket[p*tweeks+t]
 *  prodcost    array of numprod doubles containing production costs
 *  invcost     array of numprod doubles containing inventory costs
 *  flatrevenue array of tweeks*numprod doubles containing revenue for
 *                 product p, time t in position flatrevenue[p*tweeks+t]
 *
 *  The result of the routine is that the problem object lp is modified
 *  to hold the entire model.
 */

/* This function generates the steel production model by column.  */

static int
colsteel (int numprod, int tweeks, double *rate,
          double *inv0, double *avail, double *flatmarket,
          double *prodcost, double *invcost, double *flatrevenue,
          CPXENVptr env, CPXLPptr lp)
{
   double *obj      = NULL;
   char   *sense    = NULL;
   int    *cmatbeg  = NULL;
   int    *cmatind  = NULL;
   double *cmatval  = NULL;
   double *lb       = NULL;
   double *ub       = NULL;

   CPXCHANNELptr  cpxerror = NULL;

   int  t,p,k;      /* Various loop counters */

   int  status = 0;

   printf ("Building model by column.\n");

   status = CPXgetchannels (env, NULL, NULL, &cpxerror, NULL);
   if ( status )  goto TERMINATE;

   /* Set the objective sense to be maximize */

   status = CPXchgobjsen (env, lp, CPX_MAX);
   if ( status ) {
      display (cpxerror, "Could not change objective sense. Error %d\n",
               status);
      goto TERMINATE;
   }

   /* Set up the constraints for the problem */

   /* First do the time constraints.  Allocate space for a sense array. */

   sense = (char *) malloc (tweeks*sizeof(char));
   if ( sense == NULL ) {
      status = -2;
      goto TERMINATE;
   }

   for (t = 0; t < tweeks; t++) {
      sense[t] = 'L';
   }

   status = CPXnewrows (env, lp, tweeks, avail, sense, NULL, NULL);
   if ( status ) {
      display (cpxerror,
               "CPXnewrows failed to add time constraints.  Error %d\n",
               status);
      goto TERMINATE;
   }

   /* Free up the temporary sense array */
   free (sense);  sense = NULL;

   /* Now do the balance constraints.
      Can do this without temporary arrays, because the only nonzero
      is the negative of the inventory levels.  */

   for (p = 0; p < numprod; p++) {
      double  temprhs = -inv0[p];

      /* Fill in the initial inventory level */
      status   = CPXnewrows (env, lp, 1, &temprhs, NULL, NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d.  Error %d.\n",
                  "CPXnewrows failed to add initial inventory constraint\n",
                  p, status);
         goto TERMINATE;
      }

      /* The remaining balance constraints have 0.0 as the rhs value,
         and are equality constraints.  No need to specify the arrays */

      status   = CPXnewrows (env, lp, tweeks-1, NULL, NULL, NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d.  Error %d.\n",
                  "CPXnewrows failed to add other inventory constraints\n",
                  p, status);
         goto TERMINATE;
      }

   }

   /* Now, add the variables.  For each set of variables, we allocate
    * arrays to hold the data for the CPXaddcols() calls, and then free
    * them up, so that each set of variables is maintained in a "local"
    * piece of code.
    */

   /* First, do the Make variables.  Each Make[p][t] variable has
    * objective coefficient -prodcost[p], and bounds of (0, +infinity).
    * Each Make[p][t] variable appears in the constraints
    * time(t) and balance(p,t).
    * Create temporary arrays to hold the objective coefficients,
    * lower and upper bounds, and matrix coefficients for one product.
    */

   obj     = (double *) malloc (tweeks*sizeof(double));
   cmatbeg = (int    *) malloc (tweeks*sizeof(int));
   cmatind = (int    *) malloc (2*tweeks*sizeof(int));
   cmatval = (double *) malloc (2*tweeks*sizeof(double));

   if ( obj     == NULL ||
        cmatbeg == NULL ||
        cmatind == NULL ||
        cmatval == NULL   ) {
      status = -2;
      goto TERMINATE;
   }

   for (p = 0; p < numprod; p++) {
      k = 0;     /* Reset the nonzero count for each product */
      for (t = 0; t < tweeks; t++) {
         cmatbeg[t] = k;
         obj[t]     = -prodcost[p];
         cmatind[k] = t;
         cmatval[k] = 1.0/rate[p];
         k++;
         cmatind[k] = (p + 1)*tweeks + t;
         cmatval[k] = 1.0;
         k++;
      }
      status = CPXaddcols (env, lp, tweeks, k, obj, cmatbeg, cmatind,
                           cmatval, NULL, NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d.  Error %d.\n",
                  "CPXaddcols failed to add Make variables\n",
                  p, status);
         goto TERMINATE;
      }
   }

   /* Free up allocated memory used to add Make variables */

   free (obj);     obj = NULL;
   free (cmatbeg); cmatbeg = NULL;
   free (cmatind); cmatind = NULL;
   free (cmatval); cmatval = NULL;

   /* Now do the Inv variables in (p,t) order.  Each Inv[p][t] variable
    * has objective coefficient -invcost[p], and bounds of (0, +infinity).
    * If t is not tweeks-1, then Inv[p][t] appears in constraints
    *   balance(p,t) and balance(p,t+1).
    * If t is tweeks-1, then Inv[p][t] appears only in
    *   constraint balance(p,t).
    * Create temporary arrays to hold the objective coefficients,
    * lower and upper bounds, and matrix coefficients for one product.
    */

   obj     = (double *) malloc (tweeks*sizeof(double));
   cmatbeg = (int    *) malloc (tweeks*sizeof(int));
   cmatind = (int    *) malloc (2*tweeks*sizeof(int));
   cmatval = (double *) malloc (2*tweeks*sizeof(double));

   if ( obj     == NULL ||
        cmatbeg == NULL ||
        cmatind == NULL ||
        cmatval == NULL   ) {
      status = -2;
      goto TERMINATE;
   }

   for (p = 0; p < numprod; p++) {
      k = 0;   /* Reset the nonzero count for each product */
      for (t = 0; t < tweeks; t++) {
         obj[t]     = -invcost[p];
         cmatbeg[t] = k;
         cmatind[k] = (p+1)*tweeks + t;
         cmatval[k] = -1.0;
         k++;
         cmatind[k] = (p+1)*tweeks + t+1;
         cmatval[k] = 1.0;
         k++;
      }
      /* Now repair the coefficient for t=tweeks-1 by passing k-1 as
       * the number of nonzeros, so that the last coefficient is
       * ignored.
       */
      status = CPXaddcols (env, lp, tweeks, k-1, obj, cmatbeg, cmatind,
                           cmatval, NULL, NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d.  Error %d.\n",
                  "CPXaddcols failed to add Inv variables\n",
                  p, status);
         goto TERMINATE;
      }
   }

   /* Free up allocated memory used to add Inv variables */

   free (obj);     obj = NULL;
   free (cmatbeg); cmatbeg = NULL;
   free (cmatind); cmatind = NULL;
   free (cmatval); cmatval = NULL;


   /* Now do the Sell[p][t] variables.  The objective coefficients of
    * Sell[p][t] is flatrevenue[p*tweeks+t], which means that
    * we can pull the objective coefficients from the slice of that
    * array.  The lower bounds are 0.0, and the upper bounds are
    * flatmarket[p*tweeks+t], which means that we can pull the
    * upper bounds from the slice of that array.  Each Sell variable
    * appears only in the balance(p,t) constraint.
    * Create temporary arrays to hold the bounds, and matrix
    * coefficients for one product.
    */

   cmatbeg = (int    *) malloc (tweeks*sizeof(int));
   cmatind = (int    *) malloc (tweeks*sizeof(int));
   cmatval = (double *) malloc (tweeks*sizeof(double));

   if ( cmatbeg == NULL ||
        cmatind == NULL ||
        cmatval == NULL   ) {
      status = -2;
      goto TERMINATE;
   }


   for (p = 0; p < numprod; p++) {
      k = 0;     /* Reset the nonzero count for each product */
      for (t = 0; t < tweeks; t++) {
         cmatbeg[t] = k;
         cmatind[k] = (p+1)*tweeks + t;
         cmatval[k] = -1.0;
         k++;
      }
      status = CPXaddcols (env, lp, tweeks, k, &flatrevenue[p*tweeks],
                           cmatbeg, cmatind, cmatval, NULL,
                           &flatmarket[p*tweeks], NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d.  Error %d.\n",
                  "CPXaddcols failed to add Sell variables\n",
                  p, status);
         goto TERMINATE;
      }

   }

   /* Free up allocated memory used to add Sell variables */

   free (lb);      lb  = NULL;
   free (cmatbeg); cmatbeg = NULL;
   free (cmatind); cmatind = NULL;
   free (cmatval); cmatval = NULL;

TERMINATE:

   if ( obj     != NULL )   free ( obj );
   if ( sense   != NULL )   free ( sense );
   if ( cmatbeg != NULL )   free ( cmatbeg );
   if ( cmatind != NULL )   free ( cmatind );
   if ( cmatval != NULL )   free ( cmatval );
   if ( lb      != NULL )   free ( lb );
   if ( ub      != NULL )   free ( ub );

   return (status);

} /* END colsteel */



/* This function generates the steel production model by row.  */

static int
rowsteel (int numprod, int tweeks, double *rate,
          double *inv0, double *avail, double *flatmarket,
          double *prodcost, double *invcost, double *flatrevenue,
          CPXENVptr env, CPXLPptr lp)
{
   double *obj      = NULL;
   double *rhs      = NULL;
   char   *sense    = NULL;

   /* Row representation of the model */
   int    *rmatbeg  = NULL;
   int    *rmatind  = NULL;
   double *rmatval  = NULL;

   CPXCHANNELptr  cpxerror = NULL;

   int  t,p,j,k;      /* Various loop counters */

   int  status = 0;

   printf ("Building model by row.\n");

   status = CPXgetchannels (env, NULL, NULL, &cpxerror, NULL);
   if ( status )  goto TERMINATE;

   /* Set the objective sense to be maximize */

   status = CPXchgobjsen (env, lp, CPX_MAX);
   if ( status ) {
      display (cpxerror, "Could not change objective sense. Error %d\n",
               status);
      goto TERMINATE;
   }

   /* Set up the variables for the problem.  */

   /* Now fill in the bounds and the objective.
    * For each variable type, we allocate a vector to hold the
    * objective coefficients for one product, and multiple time
    * periods.  Note that we allocate and free the vector for
    * each decision variable type to maintain the locality of the code.
    */

   /* First, do the Make variables in (p,t) order.  The bounds are
    * (0, infinity), while the objective for Make[p][t] is -prodcost[p]
    */

   obj = (double *) malloc (tweeks*sizeof(double));
   if ( obj == NULL ) {
      status = -2;
      goto TERMINATE;
   }

   for (p = 0; p < numprod; p++) {
      for (t = 0; t < tweeks; t++) {
         obj[t]    = -prodcost[p];
      }
      status = CPXnewcols (env, lp, tweeks, obj, NULL, NULL, NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d. Error %d.\n",
                  "CPXnewcols failed to add Make variables\n",
                  p, status);
         goto TERMINATE;
      }
   }

   /* Free up the obj array. */

   free (obj);  obj = NULL;

   /* Now do the Inv variables in (p,t) order. The bounds are
    * (0, infinity), while the objective for Inv[p][t] is -invcost[p]
    */

   obj = (double *) malloc (tweeks*sizeof(double));
   if ( obj == NULL ) {
      status = -2;
      goto TERMINATE;
   }

   for (p = 0; p < numprod; p++) {
      for (t = 0; t < tweeks; t++) {
         obj[t]    = -invcost[p];
      }
      status = CPXnewcols (env, lp, tweeks, obj, NULL, NULL, NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d. Error %d.\n",
                  "CPXnewcols failed to add Inv variables\n",
                  p, status);
         goto TERMINATE;
      }
   }

   /* Free up the obj array. */

   free (obj);  obj = NULL;

   /* Now do the Sell[p][t] variables in (p,t) order.  The bounds on
    * Sell[p][t] are (0, flatmarket[p*tweeks+t]), and the objective
    * coefficient is flatrevenue[p*tweeks+t].  Because of this
    * structure, we do not need to allocate a temporary objective
    * coefficient array.
    */

   for (p = 0; p < numprod; p++) {
      status = CPXnewcols (env, lp, tweeks, &flatrevenue[p*tweeks],
                           NULL, &flatmarket[p*tweeks], NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d. Error %d.\n",
                  "CPXnewcols failed to add Make variables\n",
                  p, status);
         goto TERMINATE;
      }
   }

   /* Now generate the constraints.  */

   /* There are tweeks time constraints, each with numprod coefficients.
    * We allocate space for the sense values, and space to hold the matrix.
    * The rhs values are in avail, so there is no need to allocate rhs.
    */

   sense   = (char *)   malloc (tweeks*sizeof(char));
   rmatbeg = (int *)    malloc (tweeks*sizeof(int));
   rmatind = (int *)    malloc (tweeks*numprod*sizeof(int));
   rmatval = (double *) malloc (tweeks*numprod*sizeof(double));

   if ( sense   == NULL ||
        rmatbeg == NULL ||
        rmatind == NULL ||
        rmatval == NULL   ) {
      status = -2;
      goto TERMINATE;
   }

   /* Now generate the time constraints.  The senses are all 'L'.
    * There are numprod  nonzeros in each of these constraints,
    * with nonzero value (1/rate[p]) for variable Make[p][t]. */

   k = 0;   /* The count of the nonzeros */

   for (t = 0; t < tweeks; t++) {
      sense[t] = 'L';
      rmatbeg[t] = k;
      for (p = 0; p < numprod; p++) {
         rmatind[k] = p*tweeks + t;  /* Formula for Make variable */
         rmatval[k] = 1.0/rate[p];
         k++;
      }
   }

   status = CPXaddrows (env, lp, 0, tweeks, k, avail, sense,
                        rmatbeg, rmatind, rmatval, NULL, NULL);
   if ( status ) {
      display (cpxerror,
               "CPXaddrows failed to add time constraints. Error %d.\n",
               status);
      goto TERMINATE;
   }

   free (sense);   sense   = NULL;
   free (rmatbeg); rmatbeg = NULL;
   free (rmatind); rmatind = NULL;
   free (rmatval); rmatval = NULL;

   /* Allocate space for each product's balance constraints.  rhs
    * and sense are needed for each time period, and each constraint
    * balance[p,t] has 4 nonzeros.
    */

   rhs     = (double *) malloc (tweeks*sizeof(double));
   rmatbeg = (int *)    malloc (tweeks*sizeof(int));
   rmatind = (int *)    malloc (4*tweeks*sizeof(int));
   rmatval = (double *) malloc (4*tweeks*sizeof(double));

   if ( rhs     == NULL ||
        rmatbeg == NULL ||
        rmatind == NULL ||
        rmatval == NULL   ) {
      status = -2;
      goto TERMINATE;
   }

   /* Now generate the balance constraints.  Add rows by product.
    * The rhs values are -inv0[p] for the first time period,
    * and 0.0 otherwise.  The senses are all 'E'.
    * Handle t=0 specially.
    * Use order of variables Inv[t-1], Make, Inv[t], Sell so that
    * rmatind is in column order.
    */

   for (p = 0; p < numprod; p++) {

      k = 0;  /* Initialize the nonzero count for each product */

      for (t = 0; t < tweeks; t++) {
         rmatbeg[t] = k;
         if ( t > 0 ) {
            rhs[t]   = 0.0;
            j = (numprod+p)*tweeks + t-1;  /* Inv[p][t-1] index */
            rmatind[k] = j;
            rmatval[k] = 1.0;
            k++;
         }
         else {
            rhs[t] = -inv0[p];
         }
         rmatind[k] = p*tweeks + t;   /* Make[p][t] index */
         rmatval[k] = 1.0;
         k++;
         rmatind[k] = (numprod+p)*tweeks + t;  /* Inv[p][t] index */
         rmatval[k] = -1.0;
         k++;
         rmatind[k] = (2*numprod+p)*tweeks + t;  /* Sell[p][t] index */
         rmatval[k] = -1.0;
         k++;
      }
      /* Now add the rows to the problem */

      status = CPXaddrows (env, lp, 0, tweeks, k, rhs, NULL,
                           rmatbeg, rmatind, rmatval, NULL, NULL);
      if ( status ) {
         display (cpxerror, "%sfor product %d. Error %d.\n",
                  "CPXaddrows failed to add balance constraint\n",
                  p, status);
         goto TERMINATE;
      }

   }

   /* Free up arrays used to build balance constraints */

   free (rhs);     rhs   = NULL;
   free (rmatbeg); rmatbeg = NULL;
   free (rmatind); rmatind = NULL;
   free (rmatval); rmatval = NULL;

TERMINATE:

   if ( rmatbeg != NULL )   free ( rmatbeg );
   if ( rmatind != NULL )   free ( rmatind );
   if ( rmatval != NULL )   free ( rmatval );
   if ( rhs     != NULL )   free ( rhs );
   if ( sense   != NULL )   free ( sense );
   if ( obj     != NULL )   free ( obj );

   return (status);

} /* END rowsteel */


/* This function prints a help text on how to
 * run the example from the command line.
 */
static void
usage (char *progname)
{
   fprintf (stderr,"Usage: %s [-X]\n", progname);
   fprintf (stderr,"   where X is one of the following options: \n");
   fprintf (stderr,"      r          generate problem by row\n");
   fprintf (stderr,"      c          generate problem by column\n");
   fprintf (stderr," Exiting...\n");
} /* END usage */

/* This is a very simple message handler to just print the message
 * to the file pointed to by handle.
 */

static void CPXPUBLIC
errormessage (void *handle, const char *string)
{
   FILE *fp;

   fp = (FILE *) handle;  /* Convert handle to a FILE * type */
   fprintf (fp, "%s", string);
} /* END errormessage */

