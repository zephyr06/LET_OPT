// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilobasic.h
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
//
// 5725-A06 5725-A29 5724-Y47 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corp. 2000, 2022
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// ---------------------------------------------------------------------------

#ifndef __CONCERT_IloBasicH
#define __CONCERT_IloBasicH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloalg.h>

#define IloNumToNumStepFunctionI    IloSegmentedFunctionI
#define IloNumToNumSegmentFunctionI IloSegmentedFunctionI

#if defined(ILO_HP_aCC)
#define ILOCOPYCTOR(Hname) Hname(const Hname& h):_impl(h._impl){}
#else
#define ILOCOPYCTOR(Hname)
#endif

#define ILOHANDLEMINI(Hname, Iname)\
	typedef Iname ImplClass; \
public:\
	Hname():_impl(0){}\
	Hname(Iname* impl):_impl(impl){}\
	ILOCOPYCTOR(Hname)\
	Iname* getImpl() const {return _impl;}\
	void operator=(const Hname& h){_impl = h._impl;}\
protected:\
	Iname* _impl

#define ILOHANDLEMININOIMPL(Hname, Iname, RHname)\
	typedef Iname ImplClass; \
public:\
	Hname():RHname(){}\
	Hname(Iname* impl):RHname(impl){}\
	Iname* getImpl() const {return (Iname*)RHname::getImpl();}

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
