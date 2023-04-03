/* --------------------------------------------------------------------------
 * File: xlpex3.c
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

/* xlpex3.c, example of using CPXXaddrows to solve a problem */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include. */

#include <ilcplex/cplexx.h>

/* Bring in the declarations for the string functions */

#include <stdio.h>
#include <stdlib.h>

/* 
 *   minimize  c*x
 *   subject to  Hx = d
 *               Ax = b
 *               l <= x <= u
 *   where
 *
 *   H = (  0  0  0  0  0  0  0 -1 -1 -1  0  0 )  d = ( -1 )
 *       (  1  0  0  0  0  1  0  1  0  0  0  0 )      (  4 )
 *       (  0  1  0  1  0  0  1  0  1  0  0  0 )      (  1 )
 *       (  0  0  1  0  1  0  0  0  0  1  0  0 )      (  1 )
 *       (  0  0  0  0  0 -1 -1  0  0  0 -1  1 )      ( -2 )
 *       (  0  0  0 -1 -1  0  1  0  0  0  1  0 )      ( -2 )
 *       ( -1 -1 -1  0  0  0  0  0  0  0  0 -1 )      ( -1 )
 *
 *   A = (  0  0  0  0  0  0  0  0  0  0  2  5 )  b = (  2 )
 *       (  1  0  1  0  0  1  0  0  0  0  0  0 )      (  3 )
 *
 *   c = (  1  1  1  1  1  1  1  0  0  0  2  2 ) 
 *   l = (  0  0  0  0  0  0  0  0  0  0  0  0 )
 *   u = ( 50 50 50 50 50 50 50 50 50 50 50 50 )
 *
 *
 *
 *  Treat the constraints with A as the complicating constraints, and
 *  the constraints with H as the "simple" problem.
 *  
 *  The idea is to solve the simple problem first, and then add the
 *  constraints for the complicating constraints, and solve with dual.
 *
 */


#define  COLSORIG  12 
#define  ROWSSUB   7
#define  NZSUB     (2*COLSORIG)
#define  ROWSCOMP  2
#define  NZCOMP    (ROWSCOMP*COLSORIG)
#define  ROWSTOT   (ROWSSUB+ROWSCOMP)
#define  NZTOT     (NZCOMP+NZSUB)


int
main()
{
   /* Data for original problem.  Arrays have to be big enough to hold
      problem plus additional constraints.  */

   double  Hrhs[ROWSTOT]     = { -1, 4, 1, 1, -2, -2,  -1};
   double  Hlb[COLSORIG]     = {  0,  0,  0,  0,  0,  0,  0,  0,
                                  0,  0,  0,  0};
   double  Hub[COLSORIG]     = { 50, 50, 50, 50, 50, 50, 50, 50,
                                 50, 50, 50, 50};
   double  Hcost[COLSORIG]   = {  1,  1,  1,  1,  1,  1,  1,  0,
                                  0,  0,  2,  2};
   char    Hsense[ROWSTOT]   = { 'E', 'E', 'E', 'E', 'E', 'E', 'E' };
   CPXNNZ  Hmatbeg[COLSORIG] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22};
   CPXDIM  Hmatcnt[COLSORIG] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
   CPXDIM  Hmatind[NZTOT]    = { 1, 6, 2, 6, 3, 6,
                                 2, 5, 3, 5, 1, 4,
                                 2, 4, 0, 1, 0, 2,
                                 0, 3, 4, 5, 4, 6};
   double  Hmatval[NZTOT]    = { 1.0, -1.0,  1.0, -1.0,  1.0, -1.0,
                                 1.0, -1.0,  1.0, -1.0,  1.0, -1.0,
                                 1.0, -1.0, -1.0,  1.0, -1.0,  1.0,
                                -1.0,  1.0, -1.0,  1.0,  1.0, -1.0 };

   /* Data for CPXXaddrows call */

   double  Arhs[ROWSCOMP]    = { 2.0, 3.0};
   char    Asense[ROWSCOMP]  = { 'E', 'E' };
   /* Note - use a trick for rmatbeg by putting the total nonzero count in 
             the last element.  This is not required by the CPXXaddrows call. */
   CPXNNZ  Armatbeg[ROWSCOMP+1] = { 0, 2, 5};
   CPXDIM  Armatind[NZCOMP]   = { 10, 11,
                                   0,  2,  5};
   double  Armatval[NZCOMP]   = {  2.0,  5.0,
                                   1.0,  1.0,  1.0};

   double        x[COLSORIG];

   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;

   int           j;
   int           status, lpstat;
   double        objval;

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

   status = CPXXsetintparam (env, CPXPARAM_Simplex_Display, 2);
   if ( status ) {
      fprintf (stderr,"Failed to turn up simplex display level.\n");
      goto TERMINATE;
   }

   /* Create the problem */

   lp = CPXXcreateprob (env, &status, "chvatal");
   
   if ( lp == NULL ) {
      fprintf (stderr,"Failed to create subproblem\n");
      status = 1;
      goto TERMINATE;
   }

   /* Copy network part of problem.  */

   status = CPXXcopylp (env, lp, COLSORIG, ROWSSUB, CPX_MIN, Hcost, Hrhs, 
                       Hsense, Hmatbeg, Hmatcnt, Hmatind, Hmatval, 
                       Hlb, Hub, NULL);

   if ( status ) {
      fprintf (stderr, "CPXXcopylp failed.\n");
      goto TERMINATE;
   }


   status = CPXXsetintparam (env, CPXPARAM_LPMethod, CPX_ALG_NET);
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

   status = CPXXgetobjval (env, lp, &objval);
   if ( status ) {
      fprintf (stderr,"CPXXgetobjval failed\n");
      goto TERMINATE;
   }

   printf ("After network optimization, objective is %.10g\n", objval);

   /* Now add the extra rows to the problem.  */

   status = CPXXaddrows (env, lp, 0, ROWSCOMP, Armatbeg[ROWSCOMP], 
                        Arhs, Asense, Armatbeg, Armatind, Armatval, 
                        NULL, NULL);
   if ( status ) {
      fprintf (stderr,"CPXXaddrows failed.\n");
      goto TERMINATE;
   }

   /* Because the problem is dual feasible with the rows added, using
      the dual simplex method is indicated.  */

   status = CPXXsetintparam (env, CPXPARAM_LPMethod, CPX_ALG_DUAL);
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

   status = CPXXsolution (env, lp, &lpstat, &objval, x, NULL, NULL, NULL);
   if ( status ) {
      fprintf (stderr,"CPXXsolution failed.\n");
      goto TERMINATE;
   }

   printf ("Solution status %d\n",lpstat);
   printf ("Objective value %g\n",objval);
   printf ("Solution is:\n");
   for (j = 0; j < COLSORIG; j++) {
      printf ("x[%d] = %g\n",j,x[j]);
   }

   /* Put the problem and basis into a SAV file to use it in the
    * Interactive Optimizer and see if problem is correct */

   status = CPXXwriteprob (env, lp, "lpex3.sav", NULL);
   if ( status ) {
      fprintf (stderr, "CPXXwriteprob failed.\n");
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
     
   return (status);

} /* END main */
