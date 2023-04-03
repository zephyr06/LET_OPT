// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilodiffi.h
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

#ifndef __CONCERT_ilodiffiH
#define __CONCERT_ilodiffiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilomodel.h>

class IloDiffI : public IloConstraintI {
	ILOEXTRDECL

		IloNumExprI*_expr1;
	IloNumExprI*_expr2;
	IloNum  _cnst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloDiffI (IloEnvI*  m,
		IloNumExprI* expr1,
		IloNumExprI* expr2,
		const char* name);
	IloDiffI (IloEnvI*  m,
		IloNumExprI* expr1,
		IloNum      val,
		const char* name);

	virtual ~IloDiffI();

	IloNumExprI* getExpr1() const {return _expr1;}
	IloNumExprI* getExpr2() const {return _expr2;}
	IloNum getConst() const { return _cnst;}
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;

	ILOEXTROTHERDECL
};

class IloAllDiffRemoveVar : public IloChange {
	ILOCHANGEDECL

		IloIntExprI* _expr;
	IloAllDiffI* _alldiff;
public:
	IloAllDiffRemoveVar(IloAllDiffI* d, IloIntExprI* expr) 
		: IloChange(), _expr(expr), _alldiff(d) {}
	IloIntExprI* getExpr() const     { return (_expr); }
	IloAllDiffI*  getAllDiff() const { return (_alldiff); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE;
};

class IloAllDiffI : public IloConstraintI {
	friend class IloAddVarToAllDiff;
	ILOEXTRMEMBERS1DECL(IloAllDiffI, IloConstraintI,
		IloIntExprArray, _exprArray)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	void addExpr(IloIntExprI*);
	void removeExpr(IloIntExprI*);
	IloAllDiffI(IloEnvI* env, const char* name);
	virtual ~IloAllDiffI();
	IloIntExprArray getExprArray() const {return _exprArray;}
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
};

class IloAllMinDistanceI : public IloConstraintI {
	friend class IloAddVarToAllMinDistance;
	ILOEXTRMEMBERS2DECL(IloAllMinDistanceI, IloConstraintI,
		IloIntExprArray, _exprArray,
		IloInt, k)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	void addExpr(const IloIntExprI*);
	void removeExpr(const IloIntExprI*);
	IloAllMinDistanceI(IloEnvI* env, IloInt k, const char* name);
	virtual ~IloAllMinDistanceI();
	IloIntExprArray getExprArray() const {return _exprArray;}
	IloInt getK() const {return k;}
};

class IloAddVarToAllMinDistance : public IloAddNumVar {
	ILOCOLUMNDECL

		IloAllMinDistanceI* _minDistance;

	const IloExtractableI* what() const ILO_OVERRIDE {return _minDistance;}
	void destroy();

public:
	virtual ~IloAddVarToAllMinDistance();

	IloAddVarToAllMinDistance(IloAllMinDistanceI* minDistance) 
		: _minDistance(minDistance) {}

	void apply() const ILO_OVERRIDE;

	IloEnvI*   getEnv() const ILO_OVERRIDE { return (_minDistance->getEnv()); }

	IloAllMinDistanceI* getAllMinDistanceI() const  { return (_minDistance); }
};

class IloPackI : public IloConstraintI {
	ILOEXTRDECL
private:
	IloIntExprArray _load;
	IloIntExprArray _where;
	IloIntArray     _weight;
	IloIntExpr      _used;
	IloIntSetVar    _usedSet;

	friend class IloPack; 
	IloPackI(IloEnvI * env,
		IloIntExprArray load,
		IloIntExprArray where,
		IloIntArray weight,
		IloIntExpr used,
		IloIntSetVar usedSet,
		const char * name);

	IloArray<IloBoolArray> _incompat;
	IloPackI(IloEnvI * env,
		IloIntExprArray load,
		IloIntExprArray where,
		IloIntArray weight,
		IloIntExpr used,
		IloIntSetVar usedSet,
		IloArray<IloBoolArray> incompat,
		const char * name);

public:
	IloPackI(IloEnvI * env,
		IloIntExprArray load,
		IloIntExprArray where,
		IloIntArray weight,
		const char * name);
	IloPackI(IloEnvI * env,
		IloIntExprArray load,
		IloIntExprArray where,
		IloIntArray weight,
		IloIntExpr used,
		const char * name);
	IloPackI(IloEnvI * env,
		IloIntExprArray load,
		IloIntExprArray where,
		IloIntArray weight,
		IloIntExpr used,
		IloArray<IloBoolArray> incompat,
		const char * name);
	IloPackI(IloEnvI * env,
		IloIntExprArray load,
		IloIntExprArray where,
		IloIntArray weight,
		IloIntSetVar usedSet,
		const char * name);

	virtual ~IloPackI();
	IloIntExprArray getLoadArray() const { return _load; }
	IloIntArray     getWeightArray() const { return _weight; }
	IloIntExprArray getWhereArray() const { return _where; }
	IloIntExpr      getUsed() const { return _used; }
	IloIntSetVar    getUsedSet() const { return _usedSet; }
	IloArray<IloBoolArray> getIncompat() const { return _incompat; }

	IloExtractableI* makeClone(IloEnvI *) const ILO_OVERRIDE;
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
