// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloshareddomaini.h
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

#ifndef __CONCERT_iloshareddomainiH
#define __CONCERT_iloshareddomainiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilodomaini.h>
#include <ilconcert/ilohash.h>

class IloNumSharedVarDomainI : public IloNumVarDomainI {
public:
	IloNumSharedVarDomainI(const IloNumRangeI* coll) : IloNumVarDomainI(coll){}
	IloNumSharedVarDomainI(IloNum lb, IloNum ub) : IloNumVarDomainI(lb, ub){}
	virtual IloBool isShared() const ILO_OVERRIDE { return IloTrue; }
	virtual ~IloNumSharedVarDomainI(){}
	virtual void suicide(IloEnvI*, void* ptr) ILO_OVERRIDE;
};

class IloIntSharedVarDomainI : public IloIntVarDomainI {
public:
	IloIntSharedVarDomainI(const IloIntRangeI* coll) : IloIntVarDomainI(coll){}
	IloIntSharedVarDomainI(IloInt lb, IloInt ub, IloBool any = IloFalse) : IloIntVarDomainI(lb, ub, any){}
	virtual IloBool isShared() const ILO_OVERRIDE { return IloTrue; }
	virtual ~IloIntSharedVarDomainI(){}
	virtual void suicide(IloEnvI*, void* ptr) ILO_OVERRIDE;
};

class IloBoolSharedVarDomainI : public IloBoolVarDomainI {
public:
	IloBoolSharedVarDomainI(IloInt lb=0, IloInt ub=1) : IloBoolVarDomainI(lb, ub){}
	virtual IloBool isShared() const ILO_OVERRIDE { return IloTrue; }
	virtual ~IloBoolSharedVarDomainI(){}
	virtual void suicide(IloEnvI*, void* ptr) ILO_OVERRIDE;
};

template <class KeyType>
class IloSharedDomainHash : public IloEnvHashTable<KeyType, IloVarDomainI*>{
public:
	typedef IloInt (*IloHashFunction) (KeyType, IloInt);
	typedef IloBool (*IloCompFunction) (KeyType, KeyType);

	IloSharedDomainHash(IloEnv env,
		IloHashFunction hashFct,
		IloCompFunction compFct,
		IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<KeyType, IloVarDomainI*>(env, hashFct,compFct, size) {}
		~IloSharedDomainHash(){
		}
};

extern IloInt  IloIntDomainHashFunction(IloIntRangeI*, IloInt size);
extern IloBool IloIntDomainCompFunction(IloIntRangeI* key1, IloIntRangeI* key2);

class IloIntSharedDomainHash : public IloSharedDomainHash<IloIntRangeI*>{
public:
	IloIntSharedDomainHash(IloEnv env, IloInt size=IloDefaultHashSize)
		:IloSharedDomainHash<IloIntRangeI*>(env,
		IloIntDomainHashFunction,
		IloIntDomainCompFunction,
		size) {}
		~IloIntSharedDomainHash(); 
};

extern IloInt  IloNumDomainHashFunction(IloNumRangeI*, IloInt size);
extern IloBool IloNumDomainCompFunction(IloNumRangeI* key1, IloNumRangeI* key2);

class IloNumSharedDomainHash : public IloSharedDomainHash<IloNumRangeI*>{
public:
	IloNumSharedDomainHash(IloEnv env, IloInt size=IloDefaultHashSize)
		:IloSharedDomainHash<IloNumRangeI*>(env,
		IloNumDomainHashFunction,
		IloNumDomainCompFunction,
		size) {}
		~IloNumSharedDomainHash(); 
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
