// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloset.h
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
#ifndef __CONCERT_ilosetH
#define __CONCERT_ilosetH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>
#include <ilconcert/ilocollection.h>

class IloAnySetI;
class IloIntSetI;
class IloNumSetI;

#define IloNumSetVar IloIntSetVar
#define IloNumSetVarI IloIntSetVarI
#define IloRONumSet IloNumSet

class IloIntSet : public IloIntCollection {
public:
	
	IloIntSet(const IloEnv env, const IloIntArray array, IloBool withIndex=IloFalse);

	
	IloIntSet(const IloEnv env, const IloNumArray array, IloBool withIndex=IloFalse);

	
	IloIntSet(const IloEnv env, IloBool withIndex = IloFalse);

	
	

	IloIntSet(const IloEnv env, const IloAnyArray array, IloBool withIndex=IloFalse);

	
	IloIntSet(IloIntSetI* impl=0);

	
	IloIntSetI* getImpl() const;

	IloIntSet copy() const;

	
	IloInt getSize() const;

	
	void add(IloInt elt);
	
	void addWithoutCheck(IloInt elt);

	
	void add(IloIntSet set);

	
	void remove(IloInt elt);

	
	void remove(IloIntSet set);

	
	void setIntersection(IloInt elt);

	
	void setIntersection(IloIntSet set);

	
	void empty();

	
	IloBool contains(IloInt elt) const;

	
	IloBool contains(IloIntSet set) const;

	
	IloBool intersects(IloIntSet set) const;
	IloIntArray toArray() const;
	IloNumSet toNumSet() const;
	IloAnySet toAnySet() const;

	
	IloInt getFirst() const;
	
	IloInt getLast() const;

	
	IloInt getNext(IloInt value, IloInt offset=1) const;

	
	IloInt getPrevious(IloInt value, IloInt offset=1) const;

	
	IloInt getNextC(IloInt value, IloInt offset=1) const;

	
	IloInt getPreviousC(IloInt value, IloInt offset=1) const;

	
	class Iterator : public IloIntDefaultDataIterator {
	public:
		Iterator(): IloIntDefaultDataIterator(){}
		
		Iterator(const IloIntSetI* coll);
		
		Iterator(const IloIntSet coll);

		
		Iterator(IloGenAlloc* heap, const IloIntSetI* coll);
		
		virtual ~Iterator();
#ifdef CPPREF_GENERATION
		
		IloBool ok() const {
			return _ok;
		}

		
		void operator++() {
			_ok = next();
		}
		
		IloInt operator*();
#endif
	};

	const IloInt& operator[] (IloInt i) const;
};

typedef IloIntSet::Iterator IloIntSetIterator;

inline ILOSTD(istream)& operator>>(ILOSTD(istream)& in, IloIntSet& a) {
	IloAssert (a.getImpl(), "Using empty IloIntArray handle.");
	char ch;
	in >> ch;
	if (ch != '[') {
		in.putback(ch);
		return (in);
	}
	a.empty();
	in >> ch;
	if (ch == ']') {
		return (in);
	}
	else {
		in.putback(ch);
	}
	for(;;) {
		IloInt x;
		in >> x >> ch;
		a.add(x);
		if (ch == ']') break;
		if (ch != ',') {
			in.clear(ILOSTD(ios)::badbit);
			break;
		}
	}
	return (in);
}

class IloNumSet : public IloNumCollection {
public:
	
	IloNumSet(const IloEnv env, const IloNumArray array, IloBool withIndex=IloFalse);

	
	IloNumSet(const IloEnv env, IloBool withIndex = IloFalse);
	
	
	
	IloNumSet(IloNumSetI* impl=0);

	
	IloNumSetI* getImpl() const;

	IloNumSet copy() const;

	
	IloInt getSize() const;

	
	void add(IloNum elt);
	
	void addWithoutCheck(IloNum elt);

	
	void add(IloNumSet set);

	
	void remove(IloNum elt);

	
	void remove(IloNumSet set);

	
	void setIntersection(IloNum elt);

	
	void setIntersection(IloNumSet set);

	
	void empty();

	
	IloBool contains(IloNum elt) const;

	
	IloBool contains(IloNumSet set) const;

	
	IloBool intersects(IloNumSet set) const;
	IloIntSet toIntSet() const;
	IloNumArray toArray() const;

	
	IloNum getFirst() const;
	
	IloNum getLast() const;

	
	IloNum getNext(IloNum value, IloInt offset=1) const;

	
	IloNum getPrevious(IloNum value, IloInt offset=1) const;

	
	IloNum getNextC(IloNum value, IloInt offset=1) const;

	
	IloNum getPreviousC(IloNum value, IloInt offset=1) const;

	
	class Iterator : public IloNumDefaultDataIterator {
	public:
		Iterator(): IloNumDefaultDataIterator(){}

		
		Iterator(const IloNumSetI* coll);

		
		Iterator(const IloNumSet coll);

		
		Iterator(IloGenAlloc* heap, const IloNumSetI* coll);

		
		virtual ~Iterator();

#ifdef CPPREF_GENERATION
		
		IloBool ok() const {
			return _ok;
		}

		
		void operator++() {
			_ok = next();
		}

		
		IloInt operator*();
#endif
	};

	const IloNum& operator[] (IloInt i) const;
};

typedef IloNumSet::Iterator IloNumSetIterator;

inline ILOSTD(istream)& operator>>(ILOSTD(istream)& in, IloNumSet& a) {
	IloAssert (a.getImpl(), "Using empty IloIntArray handle.");
	char ch;
	in >> ch;
	if (ch != '[') {
		in.putback(ch);
		return (in);
	}
	a.empty();
	in >> ch;
	if (ch == ']') {
		return (in);
	}
	else {
		in.putback(ch);
	}
	for(;;) {
		IloNum x;
		in >> x >> ch;
		a.add(x);
		if (ch == ']') break;
		if (ch != ',') {
			in.clear(ILOSTD(ios)::badbit);
			break;
		}
	}
	return (in);
}

class IloIntSetVarI;

class IloIntSetVar : public IloExtractable {
	ILOEXTRHANDLE(IloIntSetVar,IloExtractable)
public:
	
	IloIntSetVar(const IloEnv env, const IloIntArray array, const char* name=0);
	
	IloIntSetVar(const IloEnv env,
		const IloIntArray possible,
		const IloIntArray required,
		const char* name=0);
	
	IloIntSetVar(const IloEnv env, const IloNumArray array, const char* name=0);
	
	IloIntSetVar(const IloEnv env,
		const IloNumArray possible,
		const IloNumArray required,
		const char* name=0);

	
	IloIntSetVar(const IloIntCollection possible, const char* name=0);

	
	IloIntSetVar(const IloIntCollection possible, const IloIntCollection required, const char* name=0);

	
	IloIntSetVar(const IloNumCollection possible, const char* name=0);

	
	IloIntSetVar(const IloNumCollection possible, const IloNumCollection required, const char* name=0);

	
	IloIntSet getPossibleSet() const;
	
	IloIntSet getRequiredSet() const;
	
	void getPossibleSet(IloIntSet set) const;
	
	void getRequiredSet(IloIntSet set) const;
	

	IloIntSet::Iterator getPossibleSetIterator() const;
	
	IloIntSet::Iterator getRequiredSetIterator() const;
	
	void addRequired(IloInt elt) const;
	
	void removeRequired(IloInt elt) const;
	
	void addPossible(IloInt elt) const;
	
	void removePossible(IloInt elt) const;
};

class IloIntSetVarI;
class IloAnySetVarI;

IloIntVar IloCard(IloIntSetVar vars);

#define IloNumSetVarArray IloIntSetVarArray

class IloIntSetVarArray : public IloExtractableArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloIntSetVarArray(IloDefaultArrayI* i=0) : IloExtractableArray(i) {}
	
	IloIntSetVarArray(const IloEnv env, IloInt n = 0)
		: IloExtractableArray(env, n) {}
	IloIntSetVarArray(const IloMemoryManager env, IloInt n = 0)
		: IloExtractableArray(env, n) {}

	IloIntSetVarArray(const IloEnv env, IloInt n, const IloIntSetVar v0);

#ifdef ILOALLOWVARARGS
	
	IloIntSetVarArray(const IloEnv env, IloInt n, const IloIntSetVar v0, const IloIntSetVar v1 ...);
#else 
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2,
		const IloIntSetVar v3);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2,
		const IloIntSetVar v3, const IloIntSetVar v4);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2,
		const IloIntSetVar v3, const IloIntSetVar v4, const IloIntSetVar v5);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2,
		const IloIntSetVar v3, const IloIntSetVar v4, const IloIntSetVar v5,
		const IloIntSetVar v6);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2,
		const IloIntSetVar v3, const IloIntSetVar v4, const IloIntSetVar v5,
		const IloIntSetVar v6, const IloIntSetVar v7);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2,
		const IloIntSetVar v3, const IloIntSetVar v4, const IloIntSetVar v5,
		const IloIntSetVar v6, const IloIntSetVar v7, const IloIntSetVar v8);
	IloIntSetVarArray(const IloEnv env, IloInt n,
		const IloIntSetVar v0, const IloIntSetVar v1, const IloIntSetVar v2,
		const IloIntSetVar v3, const IloIntSetVar v4, const IloIntSetVar v5,
		const IloIntSetVar v6, const IloIntSetVar v7, const IloIntSetVar v8, const IloIntSetVar v9);
#endif

	
	IloIntSetVar& operator[] (IloInt i) {
		return (*(IloIntSetVar*)&(IloExtractableArray::operator[](i)));
	}
	
	IloIntSetVar  operator[] (IloInt i) const {
		return ((IloIntSetVarI*)(void*)((IloExtractableArray::operator[](i)).getImpl()));
	}

	
	void add(const IloIntSetVarArray array) { IloExtractableArray::add (array); }
	
	void add(const IloIntSetVar x) { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloIntSetVar x) {
		IloExtractableArray::add (more, x);
	}
	IloIntSetVarArray getClone(IloEnvI*) const;
};

typedef IloDefaultArrayI IloIntSetVarArrayI;

IloConstraint operator!=(const IloIntSetVar var1, const IloIntSetVar var2);

IloConstraint operator!=(const IloIntSetVar var, const IloIntSet set);

IloConstraint operator!=(const IloIntSet set, const IloIntSetVar var);

IloConstraint operator==(const IloIntSetVar var1, const IloIntSetVar var2);

IloConstraint operator==(const IloIntSetVar var1, const IloIntSet set);

IloConstraint operator==(const IloIntSet set, const IloIntSetVar var);

IloConstraint IloEqIntersection(const IloEnv env,
								const IloIntSetVar var1,
								const IloIntSetVar var2,
								const IloIntSetVar var3);

IloConstraint IloEqPartition(const IloEnv,
							 const IloIntSetVar var,
							 const IloIntSetVarArray vars);

IloConstraint IloPartition(const IloEnv,
						   const IloIntSetVarArray vars);

IloConstraint IloPartition(const IloEnv,
						   const IloIntSetVarArray vars,
						   const IloIntArray vals);

IloConstraint IloEqUnion(const IloEnv env,
						 const IloIntSetVar var1,
						 const IloIntSetVar var2,
						 const IloIntSetVar var3);

template <class X, class Y>
class IloFunctionI : public IloDestroyableI {
public:
	IloFunctionI(IloEnvI* e) : IloDestroyableI(e) {}
	virtual Y getValue(X) = 0;
};

template <class X, class Y>
class IloFunction : public IloDestroyable {
public:
	IloFunction() : IloDestroyable() {}
	IloFunction(IloFunctionI<X,Y>* impl) : IloDestroyable(impl) {}
	IloFunctionI<X,Y>* getImpl() const {return (IloFunctionI<X,Y>*)IloDestroyable::getImpl();}
	Y getValue(X x) {
		IloAssert(getImpl(), "Using empty IloFunction handle.");
		return getImpl()->getValue(x);
	}
};

#define ILO_DECLARE_FUNCTION(X,Y)   \
	typedef IloFunction<name2(Ilo,X),name2(Ilo,Y)> name2(Ilo,name2(name2(X,name2(To,Y)),Function));	\
	ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function))&);

ILO_DECLARE_FUNCTION(Int,Int)
ILO_DECLARE_FUNCTION(Int,IntVar)
ILO_DECLARE_FUNCTION(Int,IntSetVar)

#define ILO_DECLARE_SETSET_FUN_CST0(X,Z,Y,N)\
	IloConstraint name2(IloEq,N)(const IloEnv env, \
	const name2(Ilo,name2(X,SetVar)) var1, \
	const name2(Ilo,name2(Z,SetVar)) var2,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function)) f);

ILO_DECLARE_SETSET_FUN_CST0(Int,Int,Int,Union)
ILO_DECLARE_SETSET_FUN_CST0(Int,Int,IntVar,Union)
ILO_DECLARE_SETSET_FUN_CST0(Int,Int,IntSetVar,Union)

#define ILO_DECLARE_SETEXP_FUN_CST0(X,Z,Y,N)\
	IloConstraint name2(IloEq,N)(const IloEnv, const name2(Ilo,name2(X,SetVar)) var1,			\
	const name2(Ilo,name2(Z,Var)) var2,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function)) f);

ILO_DECLARE_SETEXP_FUN_CST0(Int,Int,Int,Sum)
ILO_DECLARE_SETEXP_FUN_CST0(Int,Int,IntVar,Sum)

ILO_DECLARE_SETEXP_FUN_CST0(Int,Int,Int,Min)
ILO_DECLARE_SETEXP_FUN_CST0(Int,Int,IntVar,Min)

ILO_DECLARE_SETEXP_FUN_CST0(Int,Int,Int,Max)
ILO_DECLARE_SETEXP_FUN_CST0(Int,Int,IntVar,Max)

#ifdef CPPREF_GENERATION

IloConstraint IloEqUnion(const IloEnv env,
						 const IloIntSetVar var1,
						 const IloIntSetVar var2,
						 const IloIntToIntFunction f);

IloConstraint IloEqUnion(const IloEnv env,
						 const IloIntSetVar var1,
						 const IloIntSetVar var2,
						 const IloIntToIntVarFunction f);

IloConstraint IloEqUnion(const IloEnv env,
						 const IloIntSetVar var1,
						 const IloIntSetVar var2,
						 const IloIntToIntSetVarFunction f);

IloConstraint IloEqSum(const IloEnv,
					   const IloIntSetVar var1,
					   const IloIntVar var2,
					   const IloIntToIntFunction f);

IloConstraint IloEqSum(const IloEnv,
					   const IloIntSetVar var1,
					   const IloIntVar var2,
					   const IloIntToIntVarFunction f);

IloConstraint IloEqMin(const IloEnv,
					   const IloIntSetVar var1,
					   const IloIntVar var2,
					   const IloIntToIntFunction f);

IloConstraint IloEqMin(const IloEnv,
					   const IloIntSetVar var1,
					   const IloIntVar var2,
					   const IloIntToIntVarFunction f);

IloConstraint IloEqMax(const IloEnv,
					   const IloIntSetVar var1,
					   const IloIntVar var2,
					   const IloIntToIntFunction f);

IloConstraint IloEqMax(const IloEnv,
					   const IloIntSetVar var1,
					   const IloIntVar var2,
					   const IloIntToIntVarFunction f);

#endif

IloConstraint IloEqUnion(const IloEnv,
						 const IloIntSetVar var,
						 const IloIntSetVarArray vars);

IloConstraint IloSubset(const IloEnv,
						const IloIntSetVar var1,
						const IloIntSetVar var2);

IloConstraint IloSubset(const IloEnv,
						const IloIntSet var1,
						const IloIntSetVar var2);

IloConstraint IloSubset(const IloEnv,
						const IloIntSetVar var1,
						const IloIntSet var2);

IloConstraint IloSubsetEq(const IloEnv,
						  const IloIntSetVar var1,
						  const IloIntSetVar var2);

IloConstraint IloSubsetEq(const IloEnv,
						  const IloIntSet var1,
						  const IloIntSetVar var2);

IloConstraint IloSubsetEq(const IloEnv,
						  const IloIntSetVar var1,
						  const IloIntSet var2);

IloConstraint IloNullIntersect(const IloEnv,
							   const IloIntSetVar var1,
							   const IloIntSetVar var2);

IloConstraint IloNullIntersect(const IloEnv,
							   const IloIntSet var1,
							   const IloIntSetVar var2);

IloConstraint IloNullIntersect(const IloEnv,
							   const IloIntSetVar var1,
							   const IloIntSet var2);

IloConstraint IloMember(const IloEnv,
						const IloIntVar var1,
						const IloIntSetVar var2);

IloConstraint IloMember(const IloEnv,
						IloInt var1,
						const IloIntSetVar var2);

IloConstraint IloNotMember(const IloEnv,
						   const IloIntVar var1,
						   const IloIntSetVar var2);

IloConstraint IloNotMember(const IloEnv,
						   IloInt var1,
						   const IloIntSetVar var2);

IloConstraint IloAllNullIntersect(const IloEnv env,
								  const IloIntSetVarArray vars);

#define IloRONumSetVarArray IloNumSetVarArray

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

