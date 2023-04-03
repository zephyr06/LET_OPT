// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilomodel.h
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

#ifndef __CONCERT_ilomodelH
#define __CONCERT_ilomodelH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloalg.h>
#include <ilconcert/iloiterator.h>
#include <ilconcert/iloexpression.h>
#include <ilconcert/ilolinear.h>
#include <ilconcert/iloset.h>
#include <ilconcert/iloanyset.h>
#include <ilconcert/ilobox.h>
#include <ilconcert/ilorandom.h>

class IloGenerator;
class IloGeneratorArray;
class IloIntSetVarArray;
class IloIntSet;
class IloIntExprMap;

class ILO_EXPORTED IloModel : public IloExtractable {
	ILOEXTRHANDLE(IloModel,IloExtractable)
public:
	
	IloModel(const IloEnv env, const char* name = 0);

	
	class Iterator {
		IloLinkedList<IloExtractableI>::Element* _which;
	public:
		
		Iterator(const IloModel model);
		
		void             operator++()       { _which = _which->_next; }
		
		IloExtractable   operator*()        { return (_which->_obj); }
		
		IloBool          ok() const         { return (_which != 0); }
	};

	
	
	IloExtractable add(const IloExtractable x) const;
	
	const IloExtractableArray& add(const IloExtractableArray& x) const;
	
	void remove(const IloExtractable x) const;
	
	void remove(const IloExtractableArray x) const;

	
	IloInt expand(IloExtractable e, const char* propertyName=0, const char* propertyValue=0) const;
};

template <class X> inline X IloAdd(IloModel& mdl, X x) {
	mdl.add(x);
	return (x);
}

template <class X> inline X IloAdd(IloModel& mdl, X x, const char* fileName, int lineNumber) {
        mdl.add(x);
        x.setLocation(fileName, lineNumber);
        return (x);
}

#ifndef ILONOLOCATIONS

#  define ILOADD(model, extractable) \
      IloAdd(model, extractable, __FILE__, __LINE__)
#else
#  define ILOADD(model, extractable) \
      IloAdd(model, extractable)
#endif

class ILO_EXPORTED IloDiff : public IloConstraint {
	ILOEXTRHANDLE(IloDiff,IloConstraint)
public:
	
	IloDiff(const IloEnv  env,
		const IloNumExprArg expr1,
		const IloNumExprArg expr2,
		const char*   name=0);
	
	IloDiff(const IloEnv  env,
		const IloNumExprArg expr1,
		IloNum        val,
		const char*   name=0);
};

class ILO_EXPORTED IloAllDiff : public IloConstraint {
	ILOEXTRHANDLE(IloAllDiff,IloConstraint)
public:
	IloAllDiff(const IloEnv env, const char* name = 0);

	
	IloAllDiff(const IloEnv env,
		const IloIntExprArray vars,
		const char* name = 0);
	
	IloAllDiff(const IloEnv env,
		const IloIntVarArray vars,
		const char* name = 0);
	IloAddVarToAllDiff operator() ();
};

class IloAddVarToAllDiff : public IloAddNumVar {
	ILOCOLUMNDECL

		IloAllDiffI* _diff;

	const IloExtractableI* what() const ILO_OVERRIDE;
	void destroy();

public:
	virtual ~IloAddVarToAllDiff();
	IloAddVarToAllDiff (IloAllDiffI* diff) : _diff(diff) {}

	void apply() const ILO_OVERRIDE;

	IloEnvI*   getEnv() const ILO_OVERRIDE;
	IloAllDiffI* getAllDiffI() const  { return (_diff); }
};

class IloAllMinDistanceI;
class IloAddVarToAllMinDistance;

class ILO_EXPORTED IloAllMinDistance : public IloConstraint {
	ILOEXTRHANDLE(IloAllMinDistance,IloConstraint)
public:
	IloAllMinDistance(const IloEnv env, IloInt k, const char* name = 0);
	
	IloAllMinDistance(const IloEnv env,
		const IloIntExprArray vars,
		IloInt k,
		const char* name = 0);
	
	IloAllMinDistance(const IloEnv env,
		const IloIntVarArray vars,
		IloInt k,
		const char* name = 0);

	IloAddVarToAllMinDistance operator() ();
};

inline IloDiff operator!= (IloNumExprArg arg1, IloNumExprArg arg2) {
	return IloDiff(arg1.getEnv(), arg1.getImpl(), arg2.getImpl());
}

inline IloDiff operator!= (IloNumExprArg arg, IloNum val) {
	return IloDiff(arg.getEnv(), arg, val);
}

inline IloDiff operator!= (IloNum val, IloNumExprArg arg) {
	return IloDiff(arg.getEnv(), arg, val);
}

class ILO_EXPORTED IloPack : public IloConstraint {
	ILOEXTRHANDLE(IloPack, IloConstraint)
private:
	void init(const IloEnv env,
		const IloIntExprArray load,
		const IloIntExprArray where,
		const IloIntArray weight,
		const IloIntExpr used,
		const IloIntSetVar usedSet,
		const IloArray<IloBoolArray> incompat,
		const char * name);
public:
	class IloDifferentSizeException : public IloException {
	public: IloDifferentSizeException()
				: IloException("IloPack: Where and weight arrays have different size") {}  };
			
			IloPack(const IloEnv env,
				const IloIntExprArray load,
				const IloIntExprArray where,
				const IloIntArray weight,
				const char* name = 0);

			
			IloPack(const IloEnv env,
				const IloIntExprArray load,
				const IloIntExprArray where,
				const IloIntArray weight,
				const IloIntExpr used,
				const char* name = 0);

			
			IloPack(const IloEnv env,
				const IloIntExprArray load,
				const IloIntExprArray where,
				const IloIntArray weight,
				const IloIntSetVar used,
				const char* name = 0);
			
			IloPack(const IloEnv env,
				const IloIntExprArray load,
				const IloIntExprArray where,
				const IloInt weight,
				const char* name = 0);
			
			IloPack(const IloEnv env,
				const IloIntExprArray load,
				const IloIntExprArray where,
				const char* name = 0);

			
			IloPack(const IloEnv env,
				const IloIntExprArray load,
				const IloIntExprArray where,
				const IloIntArray weight,
				const IloIntExpr used,
				const IloArray<IloBoolArray> incompat,
				const char* name = 0);
};

typedef IloNum (*IloPathTransitFunction) (IloInt i, IloInt j);

class ILO_EXPORTED IloPathTransitI {
public:
	
	IloPathTransitI() {};
	virtual ~IloPathTransitI();
	
	virtual IloNum transit(IloInt i, IloInt j) = 0;
	
	virtual IloPathTransitI* makeClone(IloEnvI*) const = 0;
};

class ILO_EXPORTED IloPathLength : public IloConstraint {
	ILOEXTRHANDLE(IloPathLength,IloConstraint)
public:
	
	IloPathLength(const IloEnv           env,
		const IloIntVarArray   next,
		const IloNumVarArray   cumul,
		IloPathTransitFunction transit,
		IloInt                 nbPaths = 1,
		const char*            name = 0);
	
	IloPathLength(const IloEnv           env,
		const IloIntVarArray   next,
		const IloIntVarArray   cumul,
		IloPathTransitFunction transit,
		IloInt                 nbPaths = 1,
		const char*            name = 0);
	
	IloPathLength(const IloEnv         env,
		const IloIntVarArray next,
		const IloNumVarArray cumul,
		IloPathTransitI*     pathTransit,
		IloInt               nbPaths = 1,
		const char*          name = 0);
	
	IloPathLength(const IloEnv         env,
		const IloIntVarArray next,
		const IloIntVarArray cumul,
		IloPathTransitI*     pathTransit,
		IloInt               nbPaths = 1,
		const char*          name = 0);

	class InvalidArraysException : public IloException {
	public:
		InvalidArraysException(IloPathLengthI*)
			: IloException("IloPathLength: invalid array sizes") {}
	};
};

class ILO_EXPORTED IloDistribute : public IloConstraint {
	ILOEXTRHANDLE(IloDistribute,IloConstraint)
public:

	
	IloDistribute(const IloEnv            env,
		const IloIntExprArray cards,
		const IloIntArray    values,
		const IloIntExprArray vars,
		const char*    name = 0);
	
	IloDistribute(const IloEnv            env,
		const IloIntExprArray cards,
		const IloIntExprArray vars,
		const char*    name = 0);
	class InvalidArraysException : public IloException {
	public:
		InvalidArraysException(IloDistributeI*)
			: IloException("IloDistribute: invalid array sizes") {}
	};
};

IloConstraint IloAbstraction(const IloEnv env,
							 const IloIntVarArray y,
							 const IloIntVarArray x,
							 const IloIntArray    values,
							 IloInt    abstractValue);

IloConstraint IloBoolAbstraction(const IloEnv env,
								 const IloBoolVarArray y,
								 const IloIntVarArray x,
								 const IloIntArray    values);

class ILO_EXPORTED IloSequence : public IloConstraint {
	ILOEXTRHANDLE(IloSequence,IloConstraint)
public:
	
	IloSequence(const IloEnv    env,
		IloInt          nbMin,
		IloInt          nbMax,
		IloInt          seqWidth,
		const IloIntVarArray vars,
		const IloIntArray     values,
		const IloIntVarArray cards,
		const char*     name = 0);
	class InvalidArraysException : public IloException {
	public:
		InvalidArraysException(IloSequenceI*)
			: IloException("IloSequence: invalid array sizes") {}
	};
};

class ILO_EXPORTED IloInverse : public IloConstraint {
	ILOEXTRHANDLE(IloInverse,IloConstraint)
public:
	
	IloInverse(const IloEnv         env,
		const IloIntExprArray f,
		const IloIntExprArray invf,
		const char*    name = 0);
};

class ILO_EXPORTED IloSetInverse : public IloConstraint {
	ILOEXTRHANDLE(IloSetInverse,IloConstraint)
public:
	
	IloSetInverse(const IloEnv         env,
		const IloIntSetVarArray f,
		const IloIntVarArray invf,
		const char*    name = 0);
};

IloConstraint IloStrong(const IloEnv env, const IloIntVarArray vars, const char * name = 0);

IloConstraint IloLexicographic(IloEnv env, IloIntExprArray x,
                               IloIntExprArray y, const char* =0);

IloConstraint IloStrictLexicographic(IloEnv env, IloIntExprArray x,
                                     IloIntExprArray y, const char* =0);

IloConstraint IloSpread(IloEnv env, IloIntExprArray a,
						IloNumExprArg m, IloNumExprArg sd,
						const char* name =0);

IloConstraint IloMember(const IloEnv,
						const IloNumExprArg expr,
						const IloNumArray elements);

IloConstraint IloMember(const IloEnv,
						const IloIntExprArg expr,
						const IloIntArray elements);

IloConstraint IloNotMember(const IloEnv,
						   const IloNumExprArg expr,
						   const IloNumArray elements);

IloConstraint IloNotMember(const IloEnv,
						   const IloIntExprArg expr,
						   const IloIntArray elements);

class IloAndI : public IloConstraintI {
	ILOEXTRDECL

		IloExtractableList _list;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloAndI(IloEnvI* m, const char* name);
	virtual ~IloAndI();

	const IloExtractableList& getList() const   { return (_list); }

	class Iterator : public IloLinkedList<IloExtractableI>::Iterator {
	public:
		Iterator(const IloAndI* g)
			: IloExtractableList::Iterator(g->getList())            {}
		IloConstraintI* operator*()  {
			return ((IloConstraintI*) getObject());
		}
		IloConstraintI* operator->() { return (operator*()); }
	};

	void add(IloConstraintI* obj);
	void add(const IloConstraintArray objs);
	void remove(IloConstraintI* obj);
	void remove(const IloConstraintArray objs);
	void flatDisplay(IloBool& first, ILOSTD(ostream)& out) const;
	IloConstraintI* findConstraint(const char* propertyLabel, const char* propertyValue) const;
	IloConstraintI* focusOnConstraint(const char* propertyLabel, const char* propertyValue) const;

	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;

	ILOEXTROTHERDECL
};

class ILO_EXPORTED IloAnd : public IloConstraint {
	ILOEXTRHANDLE(IloAnd,IloConstraint)
public:
	
	IloAnd(const IloEnv env, const char* name=0);

	class Iterator : public IloLinkedList<IloExtractableI>::Iterator {
	public:
		Iterator(const IloAnd yand)
			: IloExtractableList::Iterator(yand.getImpl()->getList()) {
				IloAssert(yand.getImpl(), "Using empty IloAnd handle.");
		}
		IloConstraint operator*()  { return ((IloConstraintI*)(void*)getObject()); }
	};

	
	
	void add(const IloConstraint constraint) const {
		IloAssert(getImpl(), "Using empty IloAnd handle.");
		IloAssert(constraint.getImpl(), "Using empty IloConstraint handle.");
		IloAssert(constraint.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be added to IloAnd of the same env");
		getImpl()->add(constraint.getImpl());
	}
	
	void add(const IloConstraintArray array) const {
		IloAssert(getImpl(), "Using empty IloAnd handle.");
		IloAssert(array.getImpl(), "Using empty IloConstraintArray handle.");
		IloAssert(array.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be added to IloAnd of the same env");
		getImpl()->add(array);
	}
	
	void remove(const IloConstraint constraint) const {
		IloAssert(getImpl(), "Using empty IloAnd handle.");
		IloAssert(constraint.getImpl(), "Using empty IloConstraint handle.");
		IloAssert(constraint.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be removed to IloAnd of the same env");
		getImpl()->remove(constraint.getImpl());
	}

	
	void remove(const IloConstraintArray array) const {
		IloAssert(getImpl(), "Using empty IloAnd handle.");
		IloAssert(array.getImpl(), "Using empty IloConstraintArray handle.");
		IloAssert(array.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be removed to IloAnd of the same env");
		getImpl()->remove(array);
	}
};

class IloAddToAnd : public IloChange {
	ILOCHANGEDECL

		IloAndI*    _and;
	IloConstraintArray _add;
	IloBool _endArray;

public:
	virtual ~IloAddToAnd();
	IloAddToAnd(IloAndI* andg, const IloConstraint obj)
		: _and(andg), _add(andg->getEnv(), 1, obj), _endArray(IloTrue) {}
	IloAddToAnd(IloAndI* andg, const IloConstraintArray obj)
		: _and(andg), _add(obj), _endArray(IloFalse) {}
	const IloAndI* getAnd() const { return _and;}
	const IloConstraintArray& getAdded() const {return _add;}
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getAnd();
	}
};

class IloRemoveFromAnd : public IloChange {
	ILOCHANGEDECL

		IloAndI* _and;
	IloConstraintArray _remove;
	IloBool _endArray;

public:
	virtual ~IloRemoveFromAnd();
	IloRemoveFromAnd(IloAndI* andg, const IloConstraint obj)
		: _and(andg), _remove(IloConstraintArray(andg->getEnv(), 1, obj)), _endArray(IloTrue) {}
	IloRemoveFromAnd(IloAndI* andg, const IloConstraintArray obj)
		: _and(andg), _remove(obj), _endArray(IloFalse) {}
	const IloAndI* getAnd() const { return _and;}
	const IloConstraintArray& getRemoved() const {return _remove;}
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getAnd();
	}
};

template <class X> inline X IloAdd(IloAnd g, X x) {
	g.add(x);
	return (x);
}

ILO_EXPORTEDFUNCTION(IloAnd) operator&& (const IloConstraint constraint1,
										 const IloConstraint constraint2);

class IloOrI : public IloConstraintI {
	ILOEXTRDECL

		IloExtractableList _list;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloOrI(IloEnvI* m, const char* name);
	virtual ~IloOrI();

	const IloExtractableList& getList() const   { return (_list); }

	class Iterator : public IloLinkedList<IloExtractableI>::Iterator {
	public:
		Iterator(const IloOrI* g)
			: IloExtractableList::Iterator(g->getList())            {}
		IloConstraintI* operator*()  {
			return ((IloConstraintI*) getObject());
		}
		IloConstraintI* operator->() { return (operator*()); }
	};

	void add(IloConstraintI* obj);
	void add(const IloConstraintArray objs);
	void remove(IloConstraintI* obj);
	void remove(const IloConstraintArray objs);

	void flatDisplay(IloBool& first, ILOSTD(ostream)& out) const;

	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;

	ILOEXTROTHERDECL
};

class ILO_EXPORTED IloOr : public IloConstraint {
	ILOEXTRHANDLE(IloOr,IloConstraint)
public:
	
	IloOr(const IloEnv env, const char* name=0);

	class Iterator : public IloLinkedList<IloExtractableI>::Iterator {
	public:
		Iterator(const IloOr yor)
			: IloExtractableList::Iterator(yor.getImpl()->getList()) {
				IloAssert(yor.getImpl(), "Using empty IloOr handle.");
		}
		IloConstraint operator*()  { return ((IloConstraintI*)(void*)getObject()); }
	};

	
	
	void add(const IloConstraint con) const {
		IloAssert(getImpl(), "Using empty IloOr handle.");
		IloAssert(con.getImpl(), "Using empty IloConstraint handle.");
		IloAssert(con.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be added to IloOr of the same env");
		getImpl()->add(con.getImpl());
	}
	
	void add(const IloConstraintArray cons) const {
		IloAssert(getImpl(), "Using empty IloOr handle.");
		IloAssert(cons.getImpl(), "Using empty IloConstraintArray handle.");
		IloAssert(cons.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be added to IloOr of the same env");
		getImpl()->add(cons);
	}
	
	void remove(const IloConstraint con) const {
		IloAssert(getImpl(), "Using empty IloOr handle.");
		IloAssert(con.getImpl(), "Using empty IloConstraint handle.");
		IloAssert(con.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be removed to IloOr of the same env");
		getImpl()->remove(con.getImpl());
	}
	
	void remove(const IloConstraintArray cons) const {
		IloAssert(getImpl(), "Using empty IloOr handle.");
		IloAssert(cons.getImpl(), "Using empty IloConstraintArray handle.");
		IloAssert(cons.getImpl()->getEnv() == getImpl()->getEnv(),
			"constraints can only be removed to IloOr of the same env");
		getImpl()->remove(cons);
	}
};

class IloAddToOr : public IloChange {
	ILOCHANGEDECL

		IloOrI* _or;
	IloConstraintArray _add;
	IloBool _endArray;

public:
	virtual ~IloAddToOr();
	IloAddToOr(IloOrI* org, const IloConstraint obj)
		: _or(org), _add(org->getEnv(), 1, obj), _endArray(IloTrue) {}
	IloAddToOr(IloOrI* org, const IloConstraintArray obj)
		: _or(org), _add(obj), _endArray(IloFalse) {}
	const IloOrI* getOr() const { return _or;}
	const IloConstraintArray& getAdded() const {return _add;}
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getOr();
	}
};

class IloRemoveFromOr : public IloChange {
	ILOCHANGEDECL

		IloOrI* _or;
	IloConstraintArray _remove;
	IloBool _endArray;

public:
	virtual ~IloRemoveFromOr();
	IloRemoveFromOr(IloOrI* org, const IloConstraint obj)
		: _or(org), _remove(IloConstraintArray(org->getEnv(), 1, obj)), _endArray(IloTrue) {}
	IloRemoveFromOr(IloOrI* org, const IloConstraintArray obj)
		: _or(org), _remove(obj), _endArray(IloFalse) {}
	const IloOrI* getOr() const { return _or;}
	const IloConstraintArray& getRemoved() const {return _remove;}
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getOr();
	}
};

template <class X> inline X IloAdd(IloOr g, X x) {
	g.add(x);
	return (x);
}

ILO_EXPORTEDFUNCTION(IloOr) operator|| (const IloConstraint constraint1,
										const IloConstraint constraint2);

class IloNotI : public IloConstraintI {
	ILOEXTRMEMBERS1DECL(IloNotI, IloConstraintI,
		IloConstraint, constraint)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloConstraint getConstraint() const {return constraint;}
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
};

class ILO_EXPORTED IloNot : public IloConstraint {
	ILOEXTRHANDLE(IloNot,IloConstraint)
public:
	IloNot(const IloConstraint constraint,
		const char* name = 0)
		: IloConstraint(new (constraint.getEnv()) IloNotI(constraint.getEnv().getImpl(),
		constraint,
		name)) {}
};

inline IloConstraint operator!(const IloConstraint constraint) {
	IloAssert(constraint.getImpl(), "using empty handle IloConstrain");
	return IloNot(constraint);
}

class IloIfThenI : public IloConstraintI {
  ILOEXTRMEMBERS2DECL(IloIfThenI, IloConstraintI,
                      IloConstraint, left,
                      IloConstraint, right)

  IloConstraint getLeft() const {return left;}
  IloConstraint getRight() const {return right;}
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
  IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
};

class ILO_EXPORTED IloIfThen : public IloConstraint {
	ILOEXTRHANDLE(IloIfThen,IloConstraint)
public:
	
	IloIfThen(const IloEnv env,
		const IloConstraint left,
		const IloConstraint right,
		const char* name=0);
};

inline IloConstraint IloTrueConstraint(IloEnv env, const char* name=0){
	return IloConstraint(new (env) IloConstConstraintI(env.getImpl(), IloTrue, name));
}
inline IloConstraint IloFalseConstraint(IloEnv env, const char* name=0){
	return IloConstraint(new (env) IloConstConstraintI(env.getImpl(), IloFalse, name));
}

#include <ilconcert/ilosmodel.h>
#include <ilconcert/ilosolution.h>

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
