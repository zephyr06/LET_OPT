// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloanyset.h
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

#ifndef __CONCERT_iloanysetH
#define __CONCERT_iloanysetH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloany.h>
#include <ilconcert/iloset.h>
#include <ilconcert/ilocollection.h>

#define IloROAnySet IloAnySet
#define IloROAnySetVarArray IloAnySetVarArray

class IloAnySet : public IloAnyCollection {
public:
	
	IloAnySet(const IloEnv env, const IloAnyArray array, IloBool withIndex=IloFalse);
	
	IloAnySet(const IloEnv env, IloBool withIndex = IloFalse);

	IloAnySet(IloAnySetI* impl=0);

	IloAnySet(const IloEnv env, const IloIntArray array, IloBool withIndex = IloFalse);

	IloIntSet toIntSet() const;

	IloAnySetI* getImpl() const;

	
	IloAnySet copy() const;

	
	IloInt getSize() const;

	
	void add(IloAny elt);
	
	void addWithoutCheck(IloAny elt);

	
	void add(IloAnySet set);
	
	void remove(IloAny elt);
	
	void remove(IloAnySet set);
	
	void setIntersection(IloAny elt);
	
	void setIntersection(IloAnySet set);
	
	void empty();
	
	IloBool contains(IloAny elt) const;
	
	IloBool contains(IloAnySet set) const;
	
	IloBool intersects(IloAnySet set) const;

	IloAny getFirst() const;
	IloAny getLast() const;

	
	IloAny getNext(IloAny value, IloInt n=1) const;

	
	IloAny getPrevious(IloAny value, IloInt n=1) const;

	
	IloAny getNextC(IloAny value, IloInt n=1) const;

	
	IloAny getPreviousC(IloAny value, IloInt n=1) const;

	
	class Iterator : public IloAnyDefaultDataIterator {
	public:
		Iterator(): IloAnyDefaultDataIterator(){}
		
		Iterator(const IloAnySetI* coll);
		
		Iterator(const IloAnySet coll);

		
		Iterator(IloGenAlloc* heap, const IloAnySetI* coll);
		
		virtual ~Iterator();
#ifdef CPPREF_GENERATION
		
		IloBool ok() const {
			return _ok;
		}

		
		void operator++() {
			_ok = next();
		}

		
		IloAny operator*();
#endif
	};
};

typedef IloAnySet::Iterator IloAnySetIterator;

ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloAnySet& set);

#define IloAnySetVarI IloIntSetVarI

class IloAnySetVar : public IloExtractable {
	ILOEXTRHANDLE(IloAnySetVar,IloExtractable)
public:
	
	IloAnySetVar(const IloEnv env,
		const IloAnyArray possible,
		const char* name=0);
	
	IloAnySetVar(const IloEnv env,
		const IloAnyArray possible,
		const IloAnyArray required,
		const char* name=0);

	
	IloAnySetVar(const IloAnyCollection possible, const char* name=0);

	
	IloAnySetVar(const IloAnyCollection possible, const IloAnyCollection required, const char* name=0);

	
	IloAnySet getPossibleSet() const;
	
	IloAnySet getRequiredSet() const;
	
	void getPossibleSet(IloAnySet set) const;
	
	void getRequiredSet(IloAnySet set) const;
	
	IloAnySet::Iterator getPossibleSetIterator() const;
	
	IloAnySet::Iterator getRequiredSetIterator() const;
	
	void addRequired(IloAny elt) const;
	
	void removeRequired(IloAny elt) const;
	
	void addPossible(IloAny elt) const;
	
	void removePossible(IloAny elt) const;
};

IloIntVar IloCard(IloAnySetVar vars);

class IloAnySetVarArray : public IloExtractableArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloAnySetVarArray(IloDefaultArrayI* i=0) : IloExtractableArray(i) {}
	
	IloAnySetVarArray(const IloEnv env, IloInt n = 0) : IloExtractableArray(env, n) {}
	IloAnySetVarArray(const IloMemoryManager env, IloInt n = 0) : IloExtractableArray(env, n) {}
	IloAnySetVarArray(const IloEnv env, IloInt n, const IloAnySetVar v0);
#ifdef ILOALLOWVARARGS
	
	IloAnySetVarArray(const IloEnv env, IloInt n, const IloAnySetVar v0, const IloAnySetVar v1 ...);
#else 
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2,
		const IloAnySetVar v3);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2,
		const IloAnySetVar v3, const IloAnySetVar v4);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2,
		const IloAnySetVar v3, const IloAnySetVar v4, const IloAnySetVar v5);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2,
		const IloAnySetVar v3, const IloAnySetVar v4, const IloAnySetVar v5,
		const IloAnySetVar v6);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2,
		const IloAnySetVar v3, const IloAnySetVar v4, const IloAnySetVar v5,
		const IloAnySetVar v6, const IloAnySetVar v7);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2,
		const IloAnySetVar v3, const IloAnySetVar v4, const IloAnySetVar v5,
		const IloAnySetVar v6, const IloAnySetVar v7, const IloAnySetVar v8);
	IloAnySetVarArray(const IloEnv env, IloInt n,
		const IloAnySetVar v0, const IloAnySetVar v1, const IloAnySetVar v2,
		const IloAnySetVar v3, const IloAnySetVar v4, const IloAnySetVar v5,
		const IloAnySetVar v6, const IloAnySetVar v7, const IloAnySetVar v8,
		const IloAnySetVar v9);
#endif

	
	IloAnySetVar& operator[] (IloInt i) {
		return (*(IloAnySetVar*)&(IloExtractableArray::operator[](i)));
	}

	
	IloAnySetVar  operator[] (IloInt i) const {
		return ((IloAnySetVarI*)(void*)((IloExtractableArray::operator[](i)).getImpl()));
	}

	
	void add(const IloAnySetVarArray array)         { IloExtractableArray::add (array); }
	
	void add(const IloAnySetVar x)              { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloAnySetVar x) { IloExtractableArray::add (more, x); }
	IloAnySetVarArray getClone(IloEnvI*) const;
};

typedef IloDefaultArrayI IloAnySetVarArrayI;

IloConstraint operator!=(const IloAnySetVar var1,
						 const IloAnySetVar var2);

IloConstraint operator!=(const IloAnySetVar var1,
						 const IloAnySet    set);

IloConstraint operator!=(const IloAnySet    set,
						 const IloAnySetVar var1);

IloConstraint operator==(const IloAnySetVar var1, const IloAnySetVar var2);

IloConstraint operator==(const IloAnySetVar var1, const IloAnySet set);

IloConstraint operator==(const IloAnySet set, const IloAnySetVar var1);

IloConstraint IloEqIntersection(const IloEnv env,
								const IloAnySetVar var1,
								const IloAnySetVar var2,
								const IloAnySetVar var3);

IloConstraint IloEqPartition(const IloEnv env,
							 const IloAnySetVar var,
							 const IloAnySetVarArray vars);

IloConstraint IloPartition(const IloEnv env,
						   const IloAnySetVarArray vars);

IloConstraint IloPartition(const IloEnv env,
						   const IloAnySetVarArray vars,
						   const IloAnyArray vals);

IloConstraint IloEqUnion(const IloEnv env,
						 const IloAnySetVar var1,
						 const IloAnySetVar var2,
						 const IloAnySetVar var3);

ILO_DECLARE_FUNCTION(Num,Any)
ILO_DECLARE_FUNCTION(Num,AnyVar)
ILO_DECLARE_FUNCTION(Num,AnySetVar)
ILO_DECLARE_FUNCTION(Any,Num)
ILO_DECLARE_FUNCTION(Any,NumVar)
ILO_DECLARE_FUNCTION(Any,NumSetVar)

ILO_DECLARE_FUNCTION(Int,Any)
ILO_DECLARE_FUNCTION(Int,AnyVar)
ILO_DECLARE_FUNCTION(Int,AnySetVar)
ILO_DECLARE_FUNCTION(Any,Int)
ILO_DECLARE_FUNCTION(Any,IntVar)
ILO_DECLARE_FUNCTION(Any,IntSetVar)
ILO_DECLARE_FUNCTION(Any,Any)
ILO_DECLARE_FUNCTION(Any,AnyVar)
ILO_DECLARE_FUNCTION(Any,AnySetVar)

#define ILO_DECLARE_SETSET_FUN_CST_CAST(X,Z,Y,N)\
	IloConstraint name2(IloEq,N)(const IloEnv env, \
	const name2(Ilo,name2(X,SetVar)) var1, \
	const name2(Ilo,name2(Z,SetVar)) var2,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function)) f);

ILO_DECLARE_SETSET_FUN_CST_CAST(Int,Any,Any,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Int,Any,AnyVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Int,Any,AnySetVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Int,Int,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Int,IntVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Int,IntSetVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Any,Any,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Any,AnyVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Any,AnySetVar,Union)

ILO_DECLARE_SETSET_FUN_CST_CAST(Num,Any,Any,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Num,Any,AnyVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Num,Any,AnySetVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Num,Num,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Num,NumVar,Union)
ILO_DECLARE_SETSET_FUN_CST_CAST(Any,Num,NumSetVar,Union)

#define ILO_DECLARE_SETEXP_FUN_CST_CAST(X,Z,Y,N) \
	IloConstraint name2(IloEq,N)(const IloEnv, const name2(Ilo,name2(X,SetVar)) var1,			\
	const name2(Ilo,name2(Z,Var)) var2,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function)) f);

ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Int,Int,Sum)
ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Int,IntVar,Sum)

ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Int,Int,Min)
ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Int,IntVar,Min)

ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Int,Int,Max)
ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Int,IntVar,Max)

ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Num,Num,Sum)
ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Num,NumVar,Sum)

ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Num,Num,Min)
ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Num,NumVar,Min)

ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Num,Num,Max)
ILO_DECLARE_SETEXP_FUN_CST_CAST(Any,Num,NumVar,Max)

IloConstraint IloEqUnion(const IloEnv env,
						 const IloAnySetVar var,
						 const IloAnySetVarArray vars);

IloConstraint IloSubset(const IloEnv env,
						const IloAnySetVar var1,
						const IloAnySetVar var2);

IloConstraint IloSubset(const IloEnv env,
						const IloAnySet var1,
						const IloAnySetVar var2);

IloConstraint IloSubset(const IloEnv env,
						const IloAnySetVar var1,
						const IloAnySet var2);

IloConstraint IloSubsetEq(const IloEnv env,
						  const IloAnySetVar var1,
						  const IloAnySetVar var2);

IloConstraint IloSubsetEq(const IloEnv env,
						  const IloAnySet var1,
						  const IloAnySetVar var2);

IloConstraint IloSubsetEq(const IloEnv env,
						  const IloAnySetVar var1,
						  const IloAnySet var2);

IloConstraint IloNullIntersect(const IloEnv env,
							   const IloAnySetVar var1,
							   const IloAnySetVar var2);

IloConstraint IloNullIntersect(const IloEnv env,
							   const IloAnySet var1,
							   const IloAnySetVar var2);

IloConstraint IloNullIntersect(const IloEnv env,
							   const IloAnySetVar var1,
							   const IloAnySet var2);

IloConstraint IloMember(const IloEnv env,
						const IloAnyVar element,
						const IloAnySetVar setVar);

IloConstraint IloMember(const IloEnv env,
						IloAny element,
						const IloAnySetVar setVar);

IloConstraint IloNotMember(const IloEnv env,
						   const IloAnyVar var1,
						   const IloAnySetVar var2);

IloConstraint IloNotMember(const IloEnv env,
						   IloAny val,
						   const IloAnySetVar var2);

IloConstraint IloAllNullIntersect(const IloEnv env,
								  const IloAnySetVarArray vars);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

