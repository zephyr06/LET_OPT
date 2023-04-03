// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloany.h
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

#ifndef __CONCERT_iloanyH
#define __CONCERT_iloanyH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>
#include <ilconcert/ilolinear.h>

class IloDiscreteDataCollection;
class IloAnycollection;

typedef IloArray<IloAny> IloAnyArrayBase;

class IloAnyArray : public IloAnyArrayBase {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloAnyArray(IloDefaultArrayI* i=0) : IloAnyArrayBase(i) {}
	
	IloAnyArray(const IloAnyArray& copy) : IloAnyArrayBase(copy) {}
	
	IloAnyArray(const IloEnv env, IloInt n = 0) : IloAnyArrayBase(env, n) {}
	IloAnyArray(const IloMemoryManager env, IloInt n = 0) : IloAnyArrayBase(env, n) {}
	IloAnyArray(const IloEnv env, IloInt n, const IloAny v0);

	
	IloAnyArray(const IloEnv env, IloInt n, const IloAny p0, const IloAny p1,...);

#ifdef CPPREF_GENERATION
	
	void add(const IloAny p);
#endif

	IloIntArray toIntArray() const;
        IloAnyArray& operator=(const IloAnyArray& other) {
          setImpl(other.getImpl());
          return *this;
        }

	
	IloBool contains(IloAny e) const;
};

typedef IloDefaultArrayI IloAnyArrayI;

typedef IloArray<IloAnyArray> IloAnyArray2Base;

class IloAnyArray2 : public IloAnyArray2Base {
public:
	typedef IloDefaultArrayI ImplClass;
	IloAnyArray2(IloDefaultArrayI* i=0) : IloAnyArray2Base(i) {}
	IloAnyArray2(const IloAnyArray2& copy) : IloAnyArray2Base(copy) {}
	IloAnyArray2(const IloEnv env, IloInt n = 0) : IloAnyArray2Base(env, n) {}
	IloAnyArray2(const IloMemoryManager env, IloInt n = 0) : IloAnyArray2Base(env, n) {}
	IloAnyArray2(const IloEnv env, IloInt n, const IloAnyArray v0);
	using IloArray<IloAnyArray>::add;
	static IloBool equal(IloAnyArray array1, IloAnyArray array2);
	IloBool contains(const IloAnyArray array) const;
};

#define IloAnyVarI IloNumVarI

class IloAnyVar : public IloExtractable {
	ILOEXTRHANDLE(IloAnyVar,IloExtractable)
public:
	
	IloAnyVar(const IloEnv      env,
		const IloAnyArray array,
		const char*       name = 0);

	
	void getPossibleValues(IloAnyArray values) const;
	
	void setPossibleValues(const IloAnyArray values);
};

class IloAnyVarArray : public IloExtractableArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloAnyVarArray(IloDefaultArrayI* i=0) : IloExtractableArray(i) {}
	
	IloAnyVarArray(const IloEnv env, IloInt n = 0)
		: IloExtractableArray(env, n) {}
	IloAnyVarArray(const IloMemoryManager env, IloInt n = 0)
		: IloExtractableArray(env, n) {}

	IloAnyVarArray(const IloEnv env, IloInt n, const IloAnyVar v0);
#ifdef ILOALLOWVARARGS
	
	IloAnyVarArray(const IloEnv env, IloInt n, const IloAnyVar v0, const IloAnyVar v1 ...);
#else 
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2,
		const IloAnyVar v3);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2,
		const IloAnyVar v3, const IloAnyVar v4);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2,
		const IloAnyVar v3, const IloAnyVar v4, const IloAnyVar v5);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2,
		const IloAnyVar v3, const IloAnyVar v4, const IloAnyVar v5,
		const IloAnyVar v6);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2,
		const IloAnyVar v3, const IloAnyVar v4, const IloAnyVar v5,
		const IloAnyVar v6, const IloAnyVar v7);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2,
		const IloAnyVar v3, const IloAnyVar v4, const IloAnyVar v5,
		const IloAnyVar v6, const IloAnyVar v7, const IloAnyVar v8);
	IloAnyVarArray(const IloEnv env, IloInt n,
		const IloAnyVar v0, const IloAnyVar v1, const IloAnyVar v2,
		const IloAnyVar v3, const IloAnyVar v4, const IloAnyVar v5,
		const IloAnyVar v6, const IloAnyVar v7, const IloAnyVar v8,
		const IloAnyVar v9);
#endif

	
	IloAnyVar& operator[] (IloInt i) {
		return (*(IloAnyVar*)&(IloExtractableArray::operator[](i)));
	}

	
	IloAnyVar  operator[] (IloInt i) const {
		return ((IloAnyVarI*)(void*)((IloExtractableArray::operator[](i)).getImpl()));
	}

	
	void add(const IloAnyVarArray array) { IloExtractableArray::add (array); }
	
	void add(const IloAnyVar x) { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloAnyVar x) {
		IloExtractableArray::add (more, x);
	}
	
	IloAnyVarArray getClone(IloEnvI*) const;
};

typedef IloDefaultArrayI IloAnyVarArrayI;

IloConstraint operator!=(const IloAnyVar var1, const IloAnyVar var2);

IloConstraint operator!=(const IloAnyVar var1, IloAny val);

IloConstraint operator!=(IloAny val, const IloAnyVar var1);

IloConstraint operator==(const IloAnyVar var1, const IloAnyVar var2);

IloConstraint operator==(const IloAnyVar var1, IloAny val);

IloConstraint operator==(IloAny val, const IloAnyVar var1);

IloConstraint IloAbstraction(const IloEnv env,
							 const IloAnyVarArray y,
							 const IloAnyVarArray x,
							 const IloAnyArray    values,
							 IloAny    abstractValue);

IloConstraint IloBoolAbstraction(const IloEnv env,
								 const IloBoolVarArray avars,
								 const IloAnyVarArray vars,
								 const IloAnyArray    values);

#define IloROAnyArray IloAnyArray
#define IloROAnyVarArray IloAnyVarArray

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
