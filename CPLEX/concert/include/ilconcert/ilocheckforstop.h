// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilocheckforstop.h
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

#ifndef __CONCERT_ilocheckforstopH
#define __CONCERT_ilocheckforstopH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/ilobasic.h>

class IloCheckForStopI
{
public:

	IloCheckForStopI(){}
	virtual IloBool stop() const ILO_OVERRIDE {return IloFalse;}
};

class IloCheckForStop
{
	ILOHANDLEMINI(IloCheckForStop,IloCheckForStopI);
public:
	IloBool stop() const {return _impl->stop();}
};

#define ILOCHECKFORSTOP0(_this) \
class name2(_this,ConcertI) : public IloCheckForStopI { \
public:\
	name2(_this,ConcertI)();\
	~name2(_this,ConcertI)();\
	virtual IloBool stop() const ILO_OVERRIDE;\
};\
	name2(_this,ConcertI)::name2(_this,ConcertI)() : IloCheckForStopI() {}\
	name2(_this,ConcertI)::~name2(_this,ConcertI)() {}\
	IloCheckForStop _this(IloEnv env) {\
	return new (env) name2(_this,ConcertI)();\
}\
	IloBool name2(_this,ConcertI)::stop() const

#define ILOCHECKFORSTOP1(_this, t1, a1) \
class name2(_this,ConcertI) : public IloCheckForStopI { \
	t1 a1;\
public:\
	name2(_this,ConcertI)(t1);\
	~name2(_this,ConcertI)();\
	virtual IloBool stop() const ILO_OVERRIDE;\
};\
	name2(_this,ConcertI)::name2(_this,ConcertI)(t1 name2(a1,a1)) : IloCheckForStopI(), \
	a1(name2(a1, a1)) {}\
	name2(_this,ConcertI)::~name2(_this,ConcertI)() {}\
	IloCheckForStop _this(IloEnv env, t1 name2(a1, a1)) {\
	return new (env) name2(_this,ConcertI)(name2(a1, a1));\
}\
	IloBool name2(_this,ConcertI)::stop() const

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
