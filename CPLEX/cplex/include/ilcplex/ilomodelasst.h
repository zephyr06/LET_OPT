// -------------------------------------------------------------- -*- C++ -*-
// File: ilomodelasst.h
// Version 22.1.0
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corporation 2000, 2022. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------

#ifndef _ILOMODELASST_H
#define _ILOMODELASST_H

namespace IloModelingAssistance {

   struct Callback {

      virtual ~Callback() {}

      virtual void invoke(int issueid, char const *message) = 0;
   };

   // Modeling information IDs will be inserted below by the
   // util/stubsilomodelasst.awk script.

   extern const int BIGM_COEF;
   extern const int BIGM_TO_IND;
   extern const int BIGM_VARBOUND;
   extern const int CANCEL_TOL;
   extern const int EPGAP_LARGE;
   extern const int EPGAP_OBJOFFSET;
   extern const int FEAS_TOL;
   extern const int FRACTION_SCALING;
   extern const int IND_NZ_LARGE_NUM;
   extern const int IND_NZ_SMALL_NUM;
   extern const int IND_RHS_LARGE_NUM;
   extern const int IND_RHS_SMALL_NUM;
   extern const int KAPPA_ILLPOSED;
   extern const int KAPPA_SUSPICIOUS;
   extern const int KAPPA_UNSTABLE;
   extern const int LB_LARGE_NUM;
   extern const int LB_SMALL_NUM;
   extern const int LC_NZ_LARGE_NUM;
   extern const int LC_NZ_SMALL_NUM;
   extern const int LC_RHS_LARGE_NUM;
   extern const int LC_RHS_SMALL_NUM;
   extern const int MULTIOBJ_COEFFS;
   extern const int MULTIOBJ_LARGE_NUM;
   extern const int MULTIOBJ_MIX;
   extern const int MULTIOBJ_OPT_TOL;
   extern const int MULTIOBJ_SMALL_NUM;
   extern const int NZ_LARGE_NUM;
   extern const int NZ_SMALL_NUM;
   extern const int OBJ_LARGE_NUM;
   extern const int OBJ_SMALL_NUM;
   extern const int OPT_TOL;
   extern const int PWL_SLOPE_LARGE_NUM;
   extern const int PWL_SLOPE_SMALL_NUM;
   extern const int QC_LINNZ_LARGE_NUM;
   extern const int QC_LINNZ_SMALL_NUM;
   extern const int QC_QNZ_LARGE_NUM;
   extern const int QC_QNZ_SMALL_NUM;
   extern const int QC_RHS_LARGE_NUM;
   extern const int QC_RHS_SMALL_NUM;
   extern const int QOBJ_LARGE_NUM;
   extern const int QOBJ_SMALL_NUM;
   extern const int QOPT_TOL;
   extern const int RHS_LARGE_NUM;
   extern const int RHS_SMALL_NUM;
   extern const int SAMECOEFF_COL;
   extern const int SAMECOEFF_IND;
   extern const int SAMECOEFF_LAZY;
   extern const int SAMECOEFF_MULTIOBJ;
   extern const int SAMECOEFF_OBJ;
   extern const int SAMECOEFF_QLIN;
   extern const int SAMECOEFF_QUAD;
   extern const int SAMECOEFF_RHS;
   extern const int SAMECOEFF_ROW;
   extern const int SAMECOEFF_UCUT;
   extern const int SINGLE_PRECISION;
   extern const int SYMMETRY_BREAKING_INEQ;
   extern const int UB_LARGE_NUM;
   extern const int UB_SMALL_NUM;
   extern const int UC_NZ_LARGE_NUM;
   extern const int UC_NZ_SMALL_NUM;
   extern const int UC_RHS_LARGE_NUM;
   extern const int UC_RHS_SMALL_NUM;
   extern const int WIDE_COEFF_RANGE;
}


#endif // _ILOMODELASST_H

