/* --------------------------------------------------------------------------
 * File: conflictex1.c
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

/* Reading a problem and using the conflict refiner.
 *
 * See the usage message for more details.
 */

#include <stdio.h>
#include <ilcplex/cplex.h>


#define CONFLICT_FILE "conflictex1.c.lp"

/* Print a usage message to stderr and abort. */
static void
usage(const char *progname)
{
   fprintf(stderr, "Usage: %s filename\n", progname);
   fprintf(stderr,
           "  filename   Name of a file, with .mps, .lp, or .sav\n"
           "             extension, and a possible, additional .gz\n"
           "             extension\n");
   exit(2);
}

/* Invoke the conflict refiner on a model and display the results. */
int
main(int argc, char **argv)
{
   /* CPLEX Environment, problem, error status, error buffer.*/
   CPXENVptr env = NULL;
   CPXLPptr lp = NULL;
   int status = 0;
   char errbuf[CPXMESSAGEBUFSIZE];

   /* Variables for constraint groups. */
   int grpcnt = 0;
   double *grppref = NULL;
   int *grpbeg = NULL;
   int *grpind = NULL;
   char *grptype = NULL;
   int *grpstat = NULL;

   /* Variables for constraint counts. */
   int ncols;
   int nrows;
   int nqconstrs;
   int nsos;
   int nindconstrs;
   int npwl;
   int idx;
   int i;

   /* Check comand line arguments. */

   if ( argc != 2 )
      usage (argv[0]);

   /* Create CPLEX environment and model. */

   env = CPXopenCPLEX(&status);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to open CPLEX: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Enable screen output. */

   status = CPXsetintparam(env, CPXPARAM_ScreenOutput, CPX_ON);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to enable screen output: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Create a problem object. */

   lp = CPXcreateprob(env, &status, "conflictex1");
   if ( status != 0 ) {
      fprintf(stderr, "Failed to create problem: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Now read the file, and copy the data into the created problem. */

   status = CPXreadcopyprob (env, lp, argv[1], NULL);
   if ( status ) {
      fprintf (stderr, "Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }

   /* Define constraint groups. */

   ncols = CPXgetnumcols (env, lp);
   nrows = CPXgetnumrows (env, lp);
   nqconstrs = CPXgetnumqconstrs (env, lp);
   nsos = CPXgetnumsos (env, lp);
   nindconstrs = CPXgetnumindconstrs (env, lp);
   npwl = CPXgetnumpwl (env, lp);

   grpcnt += ncols * 2;  /* For upper and lower bounds. */
   grpcnt += nrows;
   grpcnt += nqconstrs;
   grpcnt += nsos;
   grpcnt += nindconstrs;
   grpcnt += npwl;

   grppref = malloc (grpcnt * sizeof(*grppref));
   grpbeg = malloc ((grpcnt + 1) * sizeof(*grpbeg));
   grpind = malloc (grpcnt * sizeof(*grpind));
   grptype = malloc (grpcnt * sizeof(*grptype));

   if ( grppref == NULL ||
        grpbeg == NULL  ||
        grpind == NULL  ||
        grptype == NULL   ) {
      fprintf (stderr, "No memory for constraint groups.\n");
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   /* Variable lower bound groups. */

   idx = 0;
   grpbeg[0] = 0;
   for (i = 0; i < ncols; ++i, ++idx) {
      grppref[idx] = 1.0;
      grpbeg[idx + 1] = grpbeg[idx] + 1;
      grpind[idx] = i;
      grptype[idx] = CPX_CON_LOWER_BOUND;
   }

   /* Variable upper bound groups. */

   for (i = 0; i < ncols; ++i, ++idx) {
      grppref[idx] = 1.0;
      grpbeg[idx + 1] = grpbeg[idx] + 1;
      grpind[idx] = i;
      grptype[idx] = CPX_CON_UPPER_BOUND;
   }

   /* Linear constraints groups. */

   for (i = 0; i < nrows; ++i, ++idx) {
      grppref[idx] = 1.0;
      grpbeg[idx + 1] = grpbeg[idx] + 1;
      grpind[idx] = i;
      grptype[idx] = CPX_CON_LINEAR;
   }

   /* Quadratic constraint groups. */

   for (i = 0; i < nqconstrs; ++i, ++idx) {
      grppref[idx] = 1.0;
      grpbeg[idx + 1] = grpbeg[idx] + 1;
      grpind[idx] = i;
      grptype[idx] = CPX_CON_QUADRATIC;
   }

   /* Special ordered set groups. */

   for (i = 0; i < nsos; ++i, ++idx) {
      grppref[idx] = 1.0;
      grpbeg[idx + 1] = grpbeg[idx] + 1;
      grpind[idx] = i;
      grptype[idx] = CPX_CON_SOS;
   }

   /* Indicator constraint groups. */

   for (i = 0; i < nindconstrs; ++i, ++idx) {
      grppref[idx] = 1.0;
      grpbeg[idx + 1] = grpbeg[idx] + 1;
      grpind[idx] = i;
      grptype[idx] = CPX_CON_INDICATOR;
   }

   /* Piecewise linear constraint groups. */

   for (i = 0; i < npwl; ++i, ++idx) {
      grppref[idx] = 1.0;
      grpbeg[idx + 1] = grpbeg[idx] + 1;
      grpind[idx] = i;
      grptype[idx] = CPX_CON_PWL;
   }

   /* Run the conflict refiner. As opposed to letting the conflict
      refiner run to completion (as is done here), the user can set
      a resource limit (e.g., a time limit, an iteration limit, or
      node limit) and still potentially get a "possible" conflict. */

   status = CPXrefineconflictext (env, lp, grpcnt, grpcnt, grppref,
                                  grpbeg, grpind, grptype);
   if ( status != 0 ) {
      fprintf(stderr, "Failed to refine conflict: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Display the solution status. */

   {
      int solstat = CPXgetstat (env, lp);
      fprintf (stdout, "Solution status: %s (%d)\n",
               CPXgetstatstring (env, solstat, errbuf),
               solstat);
   }

   /* Get the conflict status for the groups that were specified above. */

   grpstat = malloc (grpcnt * sizeof(*grpstat));

   if ( grpstat == NULL ) {
      fprintf (stderr, "No memory for constraint group status.\n");
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   status = CPXgetconflictext (env, lp, grpstat, 0, grpcnt - 1);

   if ( status != 0 ) {
      if ( status == CPXERR_NO_CONFLICT ) {
         fprintf (stdout, "A conflict was not identified.\n");
         fprintf (stdout, "Exiting....\n");
      }
      else {
         fprintf(stderr, "Failed to get conflict status: %s\n",
                 CPXgeterrorstring(env, status, errbuf));
      }
      goto TERMINATE;
   }

   /* Count the number of conflicts found for each constraint group and
      print the results. */

   {
      int numBoundConf = 0;
      int numConConf = 0;
      int numQConConf = 0;
      int numSosConf = 0;
      int numIndConConf = 0;
      int numPwlConf = 0;

      for (i = 0; i < grpcnt; ++i) {
         if (grpstat[i] == CPX_CONFLICT_MEMBER          ||
             grpstat[i] == CPX_CONFLICT_POSSIBLE_MEMBER   ) {
            switch (grptype[i]) {
            case CPX_CON_LOWER_BOUND: /* fallthrough */
            case CPX_CON_UPPER_BOUND:
               numBoundConf++;
               break;
            case CPX_CON_LINEAR:
               numConConf++;
               break;
            case CPX_CON_QUADRATIC:
               numQConConf++;
               break;
            case CPX_CON_SOS:
               numSosConf++;
               break;
            case CPX_CON_INDICATOR:
               numIndConConf++;
               break;
            case CPX_CON_PWL:
               numPwlConf++;
               break;
            default:
               fprintf(stderr, "unhandled constraint group type: %c",
                       grptype[i]);
               abort ();
            }
         }
      }

      printf ("Conflict Summary:\n");
      printf ("  Variable Bound conflicts       = %d\n", numBoundConf);
      printf ("  Constraint conflicts           = %d\n", numConConf);
      printf ("  Quadratic constraint conflicts = %d\n", numQConConf);
      printf ("  SOS conflicts                  = %d\n", numSosConf);
      printf ("  Indicator constraint conflicts = %d\n", numIndConConf);
      printf ("  PWL constraint conflicts       = %d\n", numPwlConf);
   }

   /*  Write the identified conflict in the LP format. */

   printf ("Writing conflict file to '%s'....\n", CONFLICT_FILE);
   status = CPXclpwrite (env, lp, CONFLICT_FILE);

   if ( status != 0 ) {
      fprintf(stderr, "Failed to write conflict file: %s\n",
              CPXgeterrorstring(env, status, errbuf));
      goto TERMINATE;
   }

   /* Display the entire conflict subproblem. */

   {
      char buf[1024];
      size_t nread;
      FILE *file = fopen (CONFLICT_FILE, "r");
      if ( file ) {
         while ((nread = fread (buf, 1, sizeof(buf), file)) > 0)
            fwrite (buf, 1, nread, stdout);
         fclose (file);
      }
   }

 TERMINATE:

   /* Free dynamic memory. */

   free (grppref);
   free (grpbeg);
   free (grpind);
   free (grptype);
   free (grpstat);

   /* Free the problem as allocated by CPXcreateprob and
      CPXreadcopyprob, if necessary */

   if ( lp != NULL ) {
      int xstatus = CPXfreeprob (env, &lp);

      if ( !status ) status = xstatus;
   }

   /* Free the CPLEX environment, if necessary */

   if ( env != NULL ) {
      int xstatus = CPXcloseCPLEX (&env);

      if ( !status ) status = xstatus;
   }

   return (status);
}
