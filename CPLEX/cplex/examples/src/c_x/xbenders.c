/* --------------------------------------------------------------------------
 * File: examples/src/cplex/xbenders.c
 * Version 22.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 2013, 2022. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 * --------------------------------------------------------------------------
 */

/* Example on how to use CPXXbendersopt.
 * Read in a MIP from a file and solve it using Benders decomposition.
 * If an annotation file is provided, use that annotation file.
 * Otherwise auto-decompose the problem and dump the annotation
 * to the file 'benders.ann'.
 */

/* Bring in the CPLEX function declarations and the C library
   header file stdio.h with the following single include. */

#include <ilcplex/cplexx.h>
#include <string.h>
#include <math.h>
#include "assert.h"

static void
usage (const char *progname)
{
   fprintf (stderr,"Usage: %s filename [annofile]\n", progname);
   fprintf (stderr,"      where filename is a file with extension \n");
   fprintf (stderr,"      MPS, SAV, or LP (lower case is allowed)\n");
   fprintf (stderr,"      annofile: optional ann file with model annotations. \n");
   fprintf (stderr,"                If \"create\" is used, the annotation is computed.\n");
   fprintf (stderr,"      This program uses the CPLEX MIP optimizer.\n");
   fprintf (stderr,"Exiting...\n");
} /* END usage */


static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */



/* Setup and install a default benders partition whereby
 * all continuous variables go into a single worker and
 * all other variables go into the master partition.
 */
static int
create_annotation (CPXENVptr env, CPXLPptr lp)
{
   int status = 0;

   CPXDIM j, num, cur_numcols, anno_idx;

   CPXLONG *partition = NULL;
   CPXDIM *colidx = NULL;
   char   *ctype = NULL;

   cur_numcols = CPXXgetnumcols (env, lp);

   ctype = malloc (cur_numcols * sizeof(char));
   if ( ctype == NULL ) {
      status = CPXERR_NO_MEMORY;
      fprintf (stderr, "Could not allocate memory for ctype.\n");
      goto TERMINATE;
   }

   colidx = malloc (cur_numcols * sizeof(CPXDIM));
   if ( colidx == NULL ) {
      status = CPXERR_NO_MEMORY;
      fprintf (stderr, "Could not allocate memory for colidx.\n");
      goto TERMINATE;
   }

   partition = malloc (cur_numcols * sizeof(CPXLONG));
   if ( colidx == NULL ) {
      status = CPXERR_NO_MEMORY;
      fprintf (stderr, "Could not allocate memory for partition.\n");
      goto TERMINATE;
   }

   /* Create benders annotation */
   status = CPXXnewlongannotation (env, lp, CPX_BENDERS_ANNOTATION,
                                            CPX_BENDERS_MASTERVALUE);
   if ( status ) {
      fprintf (stderr, "Could not create benders annotation.\n");
      goto TERMINATE;
   }

   /* Query benders annotation index */
   status = CPXXgetlongannotationindex (env, lp, CPX_BENDERS_ANNOTATION,
                                                 &anno_idx);
   if ( status ) {
      fprintf (stderr, "Could not retrieve benders annotation index.\n");
      goto TERMINATE;
   }

   /* query variable types */
   status = CPXXgetctype (env, lp, ctype, 0, cur_numcols-1);
   if ( status ) {
      fprintf (stderr, "Could not query ctype.\n");
      goto TERMINATE;
   }

   /* copy to partition */
   for (j = num = 0; j < cur_numcols; ++j) {
      if ( ctype[j] == CPX_CONTINUOUS ) {
         partition[j] = CPX_BENDERS_MASTERVALUE+1;
         colidx[num++]  = j;
      }
   }

   /* set annotation values */
   status = CPXXsetlongannotations (env, lp, anno_idx, CPX_ANNOTATIONOBJ_COL,
                                    num, colidx, partition);
   if ( status ) {
      fprintf (stderr, "Could not set benders annotation.\n");
      goto TERMINATE;
   }


TERMINATE:

   free_and_null (&ctype);
   free_and_null ((char **)&colidx);
   free_and_null ((char **)&partition);

   return (status);

} /* END create_annotation */


int
main (int argc, char** argv)
{
   int status = 0;
   int solstat;

   CPXENVptr env = NULL;
   CPXLPptr lp  = NULL;

   char   *annofile   = NULL;
   double primalbound = CPX_INFBOUND;
   double dualbound   = -CPX_INFBOUND;

   /* Check arguments */
   if ( argc == 3 ) {
      annofile = argv[2];
   }
   else if ( argc != 2 ) {
      usage (argv[0]);
      goto TERMINATE;
   }

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
      fprintf (stderr, "Failure to turn on screen indicator, error %d.\n", status);
      goto TERMINATE;
   }

   /* Create empty problem */
   lp = CPXXcreateprob (env, &status, argv[1]);

   if ( lp == NULL ) {
      printf ("Failed to create LP.\n");
      goto TERMINATE;
   }

   /* Now read the file, and copy the data into the created lp */
   status = CPXXreadcopyprob (env, lp, argv[1], NULL);
   if ( status ) {
      printf ("Failed to read and copy the problem data.\n");
      goto TERMINATE;
   }

   if ( annofile != NULL ) {
      /* Generate default annotations if annofile is "create". */
      if ( strcmp (annofile, "create") == 0 ) {
         status = create_annotation (env, lp);
         if ( status )
            goto TERMINATE;
      }
      else {
         /* Otherwise, read the annotation file. */
         status = CPXXreadcopyannotations (env, lp, annofile);
         if ( status ) {
            printf ("Failed to read and copy the annotation data.\n");
            goto TERMINATE;
         }
      }
   }
   else {
      /* Set benders strategy to auto-generate a decomposition */
      status = CPXXsetintparam (env, CPXPARAM_Benders_Strategy, CPX_BENDERSSTRATEGY_FULL);
      if ( status ) {
         fprintf (stderr, "Failure set the benders strategy, error %d.\n", status);
         goto TERMINATE;
      }

      /* Write out the auto-generated annotation */
      status = CPXXwritebendersannotation (env, lp, "benders.ann");
      if ( status ) {
         printf ("Failed to write the annotation file.\n");
         goto TERMINATE;
      }
   }

   /* Call Benders */
   status = CPXXbendersopt (env, lp);
   if ( status ) {
      printf ("Failure in optimization.\n");
      goto TERMINATE;
   }

   /* Get best bounds */
   status = CPXXgetbestobjval (env, lp, &dualbound);
   if ( status ) {
      printf ("Failure in getting best bound.\n");
      goto TERMINATE;
   }

   /* Get objective function value */
   status = CPXXgetobjval (env, lp, &primalbound);
   if ( status ) {
      printf ("Failure in getting optimal value.\n");
      goto TERMINATE;
   }

   solstat = CPXXgetstat (env, lp);
   printf ("Solution status: %d\n", solstat);
   printf ("Best bound:      %g\n", dualbound);
   printf ("Best integer:    %g\n", primalbound);


TERMINATE:


   /* Free up the problem, if necessary */
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

}  /* END main */
