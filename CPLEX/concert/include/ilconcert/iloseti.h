// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloseti.h
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

#ifndef __CONCERT_ilosetiH 
#define __CONCERT_ilosetiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpressioni.h>
#include <ilconcert/ilocollectioni.h> 

class IloIntSetCardVarI;

class IloIntSetVarI : public IloExtractableI {
	ILOEXTRDECL
		IloIntSet _possibleSet;
	IloIntSet _requiredSet;
	IloIntSetCardVarI* _cardVar;

	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
protected:
	IloIntSetVarI(IloEnvI* env,
		IloIntSet possibleSet,
		IloIntSet requiredSet,
		const char* name=0);
public:
	IloIntSetVarI(IloEnvI* env,
		const IloIntArray possibleArray,
		const char* name=0);
	IloIntSetVarI(IloEnvI* env,
		const IloIntArray possibleArray,
		const IloIntArray requiredArray,
		const char* name=0);

	IloIntSetVarI(IloEnvI* env,
		const IloNumArray possibleArray,
		const char* name=0);
	IloIntSetVarI(IloEnvI* env,
		const IloNumArray possibleArray,
		const IloNumArray requiredArray,
		const char* name=0);

	IloIntSetVarI(IloEnvI* env,
		const IloAnyArray possibleArray,
		const char* name=0);
	IloIntSetVarI(IloEnvI* env,
		const IloAnyArray possibleArray,
		const IloAnyArray requiredArray,
		const char* name=0);

	IloIntSetVarI(const IloIntCollectionI* poss,
		const char* name=0);
	IloIntSetVarI(const IloIntCollectionI* possible,
		const IloIntCollectionI* required,
		const char* name=0);

	IloIntSetVarI(const IloNumCollectionI* possible,
		const char* name=0);
	IloIntSetVarI(const IloNumCollectionI* possible,
		const IloNumCollectionI* required,
		const char* name=0);

	IloIntSetVarI(const IloAnyCollectionI* possible,
		const char* name=0);
	IloIntSetVarI(const IloAnyCollectionI* possibleArray,
		const IloAnyCollectionI* requiredArray,
		const char* name=0);

	virtual ~IloIntSetVarI();

	ILOEXTROTHERDECL

		IloIntSet getPossibleSet() const {
			return _possibleSet;
	}
	IloIntSet getRequiredSet() const {
		return _requiredSet;
	}
	
	
	IloBool hasCardVar() const {return _cardVar!=0;}
	IloIntSetCardVarI* getCardVar();

	virtual void addRequired(IloInt elt);
	virtual void removeRequired(IloInt elt);
	virtual void addPossible(IloInt elt);
	virtual void removePossible(IloInt elt);
};

class IloIntSetCardVarI : public IloIntVarI {
	ILOEXTRDECL
		IloIntSetVarI* _set;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetCardVarI(IloEnvI* env,
		IloIntSetVar set,
		const char* name)
		: IloIntVarI(env, 0, IloIntMax, ILOINT, name), _set(set.getImpl()) {}

	ILOEXTROTHERDECL

		IloIntSetVarI* getSetVar() const {return _set;}
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
};

class IloIntSetVarAddRequired : public IloChange {
	ILOCHANGEDECL

		IloIntSetVarI* _var;
	IloInt         _elt;

	IloIntSetVarAddRequired(IloIntSetVarI* var, IloInt elt)
		: _var(var), _elt(elt)   {}

	friend class IloIntSetVarI;

public:

	IloIntSetVarI* getVar() const    { return (_var); }
	IloInt         getElement()  const    { return (_elt); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getVar();
	}
};

class IloIntSetVarRemoveRequired : public IloChange {
	ILOCHANGEDECL

		IloIntSetVarI* _var;
	IloInt         _elt;

	IloIntSetVarRemoveRequired(IloIntSetVarI* var, IloInt elt)
		: _var(var), _elt(elt)   {}

	friend class IloIntSetVarI;

public:

	IloIntSetVarI* getVar() const    { return (_var); }
	IloInt         getElement()  const    { return (_elt); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getVar();
	}
};

class IloIntSetVarAddPossible : public IloChange {
	ILOCHANGEDECL

		IloIntSetVarI* _var;
	IloInt         _elt;

	IloIntSetVarAddPossible(IloIntSetVarI* var, IloInt elt)
		: _var(var), _elt(elt)   {}

	friend class IloIntSetVarI;

public:

	IloIntSetVarI* getVar() const    { return (_var); }
	IloInt         getElement()  const    { return (_elt); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getVar();
	}
};

class IloIntSetVarRemovePossible : public IloChange {
	ILOCHANGEDECL

		IloIntSetVarI* _var;
	IloInt         _elt;

	IloIntSetVarRemovePossible(IloIntSetVarI* var, IloInt elt)
		: _var(var), _elt(elt)   {}

	friend class IloIntSetVarI;

public:
	IloIntSetVarI* getVar() const    { return (_var); }
	IloInt         getElement()  const    { return (_elt); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getVar();
	}
};

class IloIntSetDiffI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntSetDiffI, IloConstraintI,
		IloIntSetVar, var1,
		IloIntSetVar, var2,
		IloIntSet,    set)
		virtual ~IloIntSetDiffI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar1() const {return var1;}
	IloIntSetVar getVar2() const {return var2;}
	IloIntSet getSet() const {return set;}
};

class IloIntSetEqI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntSetEqI, IloConstraintI,
		IloIntSetVar, var1,
		IloIntSetVar, var2,
		IloIntSet,    set)
		virtual ~IloIntSetEqI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar1() const {return var1;}
	IloIntSetVar getVar2() const {return var2;}
	IloIntSet getSet() const {return set;}
};

class IloIntEqIntersectionI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntEqIntersectionI, IloConstraintI,
		IloIntSetVar, var1,
		IloIntSetVar, var2,
		IloIntSetVar, var3)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar1() const {return var1;}
	IloIntSetVar getVar2() const {return var2;}
	IloIntSetVar getVar3() const {return var3;}
};

class IloIntEqPartitionI : public IloConstraintI {
	ILOEXTRMEMBERS2DECL(IloIntEqPartitionI, IloConstraintI,
		IloIntSetVar,      var,
		IloIntSetVarArray, vars)
		virtual ~IloIntEqPartitionI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar() const {return var;}
	IloIntSetVarArray getVars() const {return vars;}
};

class IloIntEqPartition : public IloConstraint {
	ILOEXTRHANDLE(IloIntEqPartition,IloConstraint)
public:
	IloIntEqPartition(const IloIntSetVar var,
		const IloIntSetVarArray vars,
		const char* name=0);
};

class IloIntPartition1I : public IloConstraintI {
	ILOEXTRMEMBERS1DECL(IloIntPartition1I, IloConstraintI,
		IloIntSetVarArray, vars)
		virtual ~IloIntPartition1I();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVarArray getVars() const {return vars;}
};

class IloIntPartition1 : public IloConstraint {
	ILOEXTRHANDLE(IloIntPartition1,IloConstraint)
public:
	IloIntPartition1(const IloIntSetVarArray vars,
		const char* name=0);
};

class IloIntPartition2I : public IloConstraintI {
	ILOEXTRMEMBERS2DECL(IloIntPartition2I, IloConstraintI,
		IloIntSetVarArray, vars,
		IloIntArray,       vals)
		virtual ~IloIntPartition2I();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVarArray getVars() const {return vars;}
	IloIntArray getVals() const {return vals;}
};

class IloIntPartition2 : public IloConstraint {
	ILOEXTRHANDLE(IloIntPartition2,IloConstraint)
public:
	IloIntPartition2(const IloIntSetVarArray vars,
		const IloIntArray vals,
		const char* name=0);
};

class IloIntEqUnionI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntEqUnionI, IloConstraintI,
		IloIntSetVar, var1,
		IloIntSetVar, var2,
		IloIntSetVar, var3)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar1() const {return var1;}
	IloIntSetVar getVar2() const {return var2;}
	IloIntSetVar getVar3() const {return var3;}
};

#define ILO_DECLARE_SETSET_FUN_CST(X,Z,Y,N)\
class name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I))) : public IloConstraintI {  \
	ILOEXTRMEMBERS3DECL(name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I))), IloConstraintI,\
	name2(Ilo,name2(X,SetVar)), var1,\
	name2(Ilo,name2(Z,SetVar)), var2,\
	name2(Ilo,name2(name2(X,name2(To,Y)),Function)), f) \
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;\
public:\
	name2(Ilo,name2(X,SetVar)) getVar1() const {return var1;} \
	name2(Ilo,name2(Z,SetVar)) getVar2() const {return var2;} \
	name2(Ilo,name2(name2(X,name2(To,Y)),Function)) getFunction() const {return f;} \
};

ILO_DECLARE_SETSET_FUN_CST(Int,Int,Int,Union)
ILO_DECLARE_SETSET_FUN_CST(Int,Int,IntVar,Union)
ILO_DECLARE_SETSET_FUN_CST(Int,Int,IntSetVar,Union)

#define ILO_DECLARE_SETEXP_FUN_CST(X,Z,Y,N)\
class name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I))) : public IloConstraintI {\
	ILOEXTRMEMBERS3DECL(name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I))), IloConstraintI,\
	name2(Ilo,name2(X,SetVar)), var1,\
	name2(Ilo,name2(Z,Var)),    var2,\
	name2(Ilo,name2(name2(X,name2(To,Y)),Function)), f)\
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;\
public:\
	name2(Ilo,name2(X,SetVar)) getVar1() const {return var1;}\
	name2(Ilo,name2(Z,Var)) getVar2() const {return var2;}\
	name2(Ilo,name2(name2(X,name2(To,Y)),Function)) getFunction() const {return f;}\
};

ILO_DECLARE_SETEXP_FUN_CST(Int,Int,Int,Sum)
ILO_DECLARE_SETEXP_FUN_CST(Int,Int,IntVar,Sum)

ILO_DECLARE_SETEXP_FUN_CST(Int,Int,Int,Min)
ILO_DECLARE_SETEXP_FUN_CST(Int,Int,IntVar,Min)

ILO_DECLARE_SETEXP_FUN_CST(Int,Int,Int,Max)
ILO_DECLARE_SETEXP_FUN_CST(Int,Int,IntVar,Max)

#define ILO_DEFINE_FUNCTION(X,Y) \
	ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function))&) {\
	out << "Function: Ilo" << ILO_STRINGIZE(X) << "->Ilo" << ILO_STRINGIZE(Y);\
	return out;\
}

#define ILO_DEFINE_SETSET_FUN_CST(X,Z,Y,N)\
	ILOEXTRMEMBERS3( name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I))),IloConstraintI,\
	name2(Ilo,name2(X,SetVar)), var1,\
	name2(Ilo,name2(Z,SetVar)), var2,\
	name2(Ilo,name2(name2(X,name2(To,Y)),Function)), f)\
	IloConstraint name2(IloEq,N)(const IloEnv env, \
	const name2(Ilo,name2(X,SetVar)) var1, \
	const name2(Ilo,name2(Z,SetVar)) var2,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function)) f) {\
	IloAssert(var1.getImpl(), "Using empty Ilo" ILO_STRINGIZE(X) "SetVar handle");\
	IloAssert(var2.getImpl(), "Using empty Ilo" ILO_STRINGIZE(Z) "SetVar handle");\
	IloAssert(var1.getImpl()->getEnv() == env.getImpl(),\
	"Constraint mixing environments");\
	IloAssert(var2.getImpl()->getEnv() == env.getImpl(),\
	"Constraint mixing environments");\
	return new (env)\
	name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I)))(env.getImpl(),\
	var1, var2, f, 0);\
}\
	void name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I)))::visitSubExtractables(IloExtractableVisitor* v) {\
	v->beginVisit(this);\
	v->visitChildren(this, getVar1().getImpl());\
	v->visitChildren(this, getVar2().getImpl());\
	v->endVisit(this);\
}

#define ILO_DEFINE_SETEXP_FUN_CST(X,Z,Y,N)\
	ILOEXTRMEMBERS3( name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I))),IloConstraintI,\
	name2(Ilo,name2(X,SetVar)), var1,\
	name2(Ilo,name2(Z,Var)), var2, \
	name2(Ilo,name2(name2(X,name2(To,Y)),Function)), f)\
	IloConstraint name2(IloEq,N)(const IloEnv, const name2(Ilo,name2(X,SetVar)) var1,\
	const name2(Ilo,name2(Z,Var)) var2,\
	const name2(Ilo,name2(name2(X,name2(To,Y)),Function)) f) {\
	IloAssert(var1.getImpl(), "Using empty Ilo" ILO_STRINGIZE(X) "SetVar handle");        \
	IloAssert(var2.getImpl(), "Using empty Ilo" ILO_STRINGIZE(Z) "Var handle");           \
	IloAssert(((IloExtractableI*)var1.getImpl())->getEnv() == ((IloExtractableI*)var2.getImpl())->getEnv(),\
	"Constraint mixing environments");\
	return new (((IloExtractableI*)var1.getImpl())->getEnv())\
	name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I)))(((IloExtractableI*)var1.getImpl())->getEnv(),\
	var1, var2, f, 0);\
}\
	void name2(IloEq,name2(name2(X,name2(To,Y)),name2(N,I)))::visitSubExtractables(IloExtractableVisitor* v) {\
	v->beginVisit(this);\
	v->visitChildren(this, getVar1().getImpl());\
	v->visitChildren(this, getVar2().getImpl());\
	v->endVisit(this);\
}

class IloIntArrayEqUnionI : public IloConstraintI {
	ILOEXTRMEMBERS2DECL(IloIntArrayEqUnionI, IloConstraintI,
		IloIntSetVar,      var,
		IloIntSetVarArray, array)
		virtual ~IloIntArrayEqUnionI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar() const {return var;}
	IloIntSetVarArray getArray() const {return array;}
};

class IloIntArrayEqUnion : public IloConstraint {
	ILOEXTRHANDLE(IloIntArrayEqUnion,IloConstraint)
public:
	IloIntArrayEqUnion(const IloIntSetVar var,
		const IloIntSetVarArray array,
		const char* name=0);
};

class IloIntSubsetI : public IloConstraintI {
	ILOEXTRMEMBERS4DECL(IloIntSubsetI, IloConstraintI,
		IloIntSetVar, var1,
		IloIntSetVar, var2,
		IloIntSet,    set,
		IloBool,      sense)
		virtual ~IloIntSubsetI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar1() const {return var1;}
	IloIntSetVar getVar2() const {return var2;}
	IloIntSet getSet() const {return set;}
	IloBool getSense() const {return sense;}
};

class IloIntSubset : public IloConstraint {
	ILOEXTRHANDLE(IloIntSubset,IloConstraint)
public:
	IloIntSubset(const IloIntSetVar var1,
		const IloIntSetVar var2,
		const char* name=0);
	IloIntSubset(const IloIntSet set,
		const IloIntSetVar var,
		const char* name=0);
	IloIntSubset(const IloIntSetVar var,
		const IloIntSet set,
		const char* name=0);
	IloIntSubset(const IloNumSet set,
		const IloIntSetVar var,
		const char* name=0);
	IloIntSubset(const IloIntSetVar var,
		const IloNumSet set,
		const char* name=0);
};

class IloIntSubsetEqI : public IloConstraintI {
	ILOEXTRMEMBERS4DECL(IloIntSubsetEqI, IloConstraintI,
		IloIntSetVar, var1,
		IloIntSetVar, var2,
		IloIntSet,    set,
		IloBool,      sense)
		virtual ~IloIntSubsetEqI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar1() const {return var1;}
	IloIntSetVar getVar2() const {return var2;}
	IloIntSet getSet() const {return set;}
	IloBool getSense() const {return sense;}
};

class IloIntSubsetEq : public IloConstraint {
	ILOEXTRHANDLE(IloIntSubsetEq,IloConstraint)
public:
	IloIntSubsetEq(const IloIntSetVar var1, const IloIntSetVar var2, const char* name=0);
	IloIntSubsetEq(const IloIntSet set, const IloIntSetVar var, const char* name=0);
	IloIntSubsetEq(const IloIntSetVar var, const IloIntSet set, const char* name=0);
	IloIntSubsetEq(const IloNumSet set, const IloIntSetVar var, const char* name=0);
	IloIntSubsetEq(const IloIntSetVar var, const IloNumSet set, const char* name=0);
};

class IloIntNullIntersectI : public IloConstraintI {
	ILOEXTRMEMBERS4DECL(IloIntNullIntersectI, IloConstraintI,
		IloIntSetVar, var1,
		IloIntSetVar, var2,
		IloIntSet,    set,
		IloBool,      sense)
		virtual ~IloIntNullIntersectI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getVar1() const {return var1;}
	IloIntSetVar getVar2() const {return var2;}
	IloIntSet getSet() const {return set;}
	IloBool getSense() const {return sense;}
};

class IloIntNullIntersect : public IloConstraint {
	ILOEXTRHANDLE(IloIntNullIntersect,IloConstraint)
public:
	IloIntNullIntersect(const IloIntSetVar var1, const IloIntSetVar var2, const char* name=0);
	IloIntNullIntersect(const IloIntSet set, const IloIntSetVar var, const char* name=0);
	IloIntNullIntersect(const IloIntSetVar var, const IloIntSet set, const char* name=0);
	IloIntNullIntersect(const IloNumSet set, const IloIntSetVar var, const char* name=0);
	IloIntNullIntersect(const IloIntSetVar var, const IloNumSet set, const char* name=0);
};

class IloIntMemberI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntMemberI,IloConstraintI,
		IloIntSetVar, setvar,
		IloIntVar,    var,
		IloInt,       val)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getSetVar() const {return setvar;}
	IloIntVar getVar() const {return var;}
	IloInt getVal() const {return val;}
};

class IloIntMember : public IloConstraint {
	ILOEXTRHANDLE(IloIntMember,IloConstraint)
public:
	IloIntMember(const IloIntVar var,
		const IloIntSetVar setvar,
		const char* name=0);
	IloIntMember(IloInt val,
		const IloIntSetVar setvar,
		const char* name=0);
};

class IloIntNotMemberI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntNotMemberI,IloConstraintI,
		IloIntSetVar, setvar,
		IloIntVar,    var,
		IloInt,       val)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVar getSetVar() const {return setvar;}
	IloIntVar getVar() const {return var;}
	IloInt getVal() const {return val;}
};

class IloIntNotMember : public IloConstraint {
	ILOEXTRHANDLE(IloIntNotMember,IloConstraint)
public:
	IloIntNotMember(const IloIntVar var,
		const IloIntSetVar setvar,
		const char* name=0);
	IloIntNotMember(IloInt val,
		const IloIntSetVar setvar,
		const char* name=0);
};

class IloIntAllNullIntersectI : public IloConstraintI {
	ILOEXTRMEMBERS1DECL(IloIntAllNullIntersectI, IloConstraintI,
		IloIntSetVarArray, array)
		virtual ~IloIntAllNullIntersectI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSetVarArray getArray() const {return array;}
};

class IloIntAllNullIntersect : public IloConstraint {
	ILOEXTRHANDLE(IloIntAllNullIntersect,IloConstraint)
public:
	IloIntAllNullIntersect(const IloEnv env,
		const IloIntSetVarArray array,
		const char* name=0);
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

