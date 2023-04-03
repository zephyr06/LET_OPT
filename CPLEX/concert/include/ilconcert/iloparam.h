// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloparam.h
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

#ifndef __CONCERT_IloParamH
#define __CONCERT_IloParamH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloparami.h>

class IloParameter {
	ILOHANDLEMINI(IloParameter,IloParameterI);
public:
	inline void display(ILOSTD(ostream)& out) const {
		getImpl()->display(out);
	}
	inline void end() const {
		delete getImpl();
		((IloParameter*)this)->_impl = 0;
	}
	ILOEXTENSIONH(IloParameterI)
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
