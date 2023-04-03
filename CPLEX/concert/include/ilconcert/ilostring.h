// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilostring.h
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

#ifndef __CONCERT_ilostringH
#define __CONCERT_ilostringH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloany.h>
#include <ilconcert/iloexpression.h>
#include <ilconcert/ilolinear.h>

typedef IloArray<const char*> IloStringArrayBase;

class IloStringArray : public IloStringArrayBase {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloStringArray(IloDefaultArrayI* i=0) : IloStringArrayBase(i) {}

	
	IloStringArray(const IloStringArray& copy) : IloStringArrayBase(copy) {}

	
	IloStringArray(const IloEnv env, IloInt n = 0) : IloStringArrayBase(env, n) {}
	IloStringArray(const IloMemoryManager env, IloInt n = 0) : IloStringArrayBase(env, n) {}

#ifdef CPPREF_GENERATION
	
	void add(const char* p);
#endif
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
