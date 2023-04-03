// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilolinear.h
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

#ifndef __CONCERT_ilolinearH
#define __CONCERT_ilolinearH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>

class IloIntSet;
class IloNumSet;
class IloIntSetI;
class IloNumSetI;

class IloNumLinTermI;
class IloIntLinTermI;

class IloMultiCriterionExpr;

class IloRangeI : public IloConstraintI {
	ILOEXTRDECL
		friend class IloRangeArray;
	friend class IloRange;
	IloNumLinTermI* _expr;
	IloNum  _lhs, _rhs;

	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloRangeI(IloEnvI*    env,
		IloNum      lhs,
		IloNum      rhs,
		const char* name=0);
	IloRangeI(IloEnvI* env,
		IloNum lhs,
		IloNumExprI*  expr,
		IloNum rhs=IloInfinity,
		const char* name=0);
	virtual ~IloRangeI ();

	ILOEXTROTHERDECL

	IloNumLinTermI* getExpr() const { return _expr; }

	IloNum getLb () const                  { return (_lhs); }
	IloNum getUb () const                  { return (_rhs); }

	void addVar(IloNumVarI* var, IloNum val);

	IloInt normalize();

	void setCoef(const IloNumVar, IloNum);
	inline void setBounds(IloNum, IloNum);
	inline void setUb(IloNum);
	inline void setLb(IloNum);
	void setExpr(const IloNumExprArg);
	void setCoef(const IloNumVarArray, const IloNumArray);

	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;

	class LinearIterator {
		IloNumLinExprTermI* _iter;
	public:
		LinearIterator (const IloNumLinTermI* expr);
		LinearIterator (const IloIntLinTermI* expr=0);
		IloBool           ok () const;
		void              operator++ ();
		const IloNumVarI* getVar     () const;
		IloNum            getVal     () const;
	};

  virtual IloBool hasEditableBounds() const ILO_OVERRIDE { return IloTrue;}
};

class IloAddValueToRange : public IloAddNumVar {
	ILOCOLUMNDECL

		IloRangeI* _rng;
	IloNum     _value;

	friend class IloNumColumnI;

	const IloExtractableI* what() const  ILO_OVERRIDE{ return _rng; }

public:
	
	IloAddValueToRange (IloRangeI* rng, IloNum val)
		: _rng(rng), _value(val) {}

	const IloAddValueToRange* getNext() const {
		return ((const IloAddValueToRange*)IloAddNumVar::getNext());
	}
	IloNum       getValue() const  { return (_value); }
	IloRangeI*   getRange() const  { return (_rng); }

	friend inline const IloAddValueToRange& operator+ (const IloAddValueToRange& one,
		const IloAddValueToRange& two);
	void apply() const ILO_OVERRIDE;
};

inline const IloAddValueToRange& operator+ (const IloAddValueToRange& one,
											const IloAddValueToRange& two) {
												return ((const IloAddValueToRange&)(((const IloAddNumVar&)one) +
													((const IloAddNumVar&)two)  ));
}

class IloRangeBase;

class ILO_EXPORTED IloRange : public IloConstraint {
	ILOEXTRHANDLE(IloRange,IloConstraint)
public:
	
	IloRange(const IloEnv env,
		IloNum lb,
		IloNum ub,
		const char* name=0)
		: IloConstraint (new (env.getImpl()) IloRangeI(env.getImpl(),lb,ub,name)) {}

	

	IloRange(const IloEnv env,
		IloNum lhs,
		const IloNumExprArg expr,
		IloNum rhs = IloInfinity,
		const char* name = 0)
		: IloConstraint (new (env.getImpl()) IloRangeI(env.getImpl(), lhs, expr.getImpl(), rhs, name)) {
			IloAssert (expr.getImpl(), "Using empty IloNumExpr handle.");
	}

	

	IloRange(const IloEnv env,
		const IloNumExprArg expr,
		IloNum rhs = IloInfinity,
		const char* name = 0)
		: IloConstraint (new (env.getImpl()) IloRangeI(env.getImpl(), -IloInfinity, expr.getImpl(), rhs, name)) {
			IloAssert (expr.getImpl(), "Using empty IloNumExpr handle.");
	}

#if defined(ILOBADRANGE)
	IloRange(const IloRangeBase b);
#endif

	IloNum getLb() const {
		IloAssert (getImpl(), "Using empty IloRange handle.");
		return (getImpl()->getLb());
	}
	
	IloNum getLB() const { return getLb(); }
	IloNum getUb() const {
		IloAssert (getImpl(), "Using empty IloRange handle.");
		return (getImpl()->getUb());
	}
	
	IloNum getUB() const { return getUb(); }
	
	IloNumExprArg getExpr() const {
		IloAssert(getImpl(), "IloRange: using empty handle");
		return (IloNumExprI*)(void*)(getImpl()->getExpr());
	}
	IloAddValueToRange operator() () const {
		IloAssert(getImpl(), "IloRange: using empty handle");
		return IloAddValueToRange(getImpl(), 1.0);
	}
	
	IloAddValueToRange operator() (IloNum value) const {
		IloAssert(getImpl(), "IloRange: using empty handle");
		return IloAddValueToRange(getImpl(), value);
	}

	IloInt normalize() const {
		IloAssert(getImpl(), "Using empty IloRange handle");
		return getImpl()->normalize();
	}

	
	void setLinearCoef(const IloNumVar var, IloNum value) {
		if (!getImpl()) throw IloEmptyHandleException("Using Empty handle");
		getImpl()->setCoef(var, value);
	}
	
	void setLinearCoefs(const IloNumVarArray vars, const IloNumArray values) {
		IloAssert(getImpl(), "Trying changing IloRange through 0 handle");
		getImpl()->setCoef(vars, values);
	}
	
	void setExpr(const IloNumExprArg expr) {
		IloAssert(getImpl(), "Trying changing IloRange through 0 handle");
		getImpl()->setExpr(expr);
	}
	
	void setBounds(IloNum lb, IloNum ub) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		getImpl()->setBounds(lb, ub);
	}
	void setUb(IloNum ub) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		getImpl()->setUb(ub);
	}
	
	void setUB(IloNum ub) { setUb(ub); }
	void setLb(IloNum lb) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		getImpl()->setLb(lb);
	}
	
	void setLB(IloNum lb) { setLb(lb); }

	
	IloExpr::LinearIterator getLinearIterator() const;
	
	IloExpr::QuadIterator getQuadIterator() const;
};

class ILO_EXPORTED IloRangeArray : public IloConstraintArray {
  public:
  typedef IloDefaultArrayI ImplClass;
  
  IloRangeArray(IloDefaultArrayI* i=0) : IloConstraintArray(i) {}
  
  IloRangeArray(const IloEnv env, IloInt n = 0)
    : IloConstraintArray(env, n)     {}
  IloRangeArray(const IloMemoryManager env, IloInt n = 0)
    : IloConstraintArray(env, n)     {}

  IloRangeArray(const IloEnv env, IloInt n, const IloRange v0);
#ifdef ILOALLOWVARARGS
  IloRangeArray(const IloEnv env, IloInt n, const IloRange v0, const IloRange v1 ...);
#else 
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2,
		const IloRange v3);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2,
		const IloRange v3, const IloRange v4);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2,
		const IloRange v3, const IloRange v4, const IloRange v5);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2,
		const IloRange v3, const IloRange v4, const IloRange v5,
		const IloRange v6);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2,
		const IloRange v3, const IloRange v4, const IloRange v5,
		const IloRange v6, const IloRange v7);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2,
		const IloRange v3, const IloRange v4, const IloRange v5,
		const IloRange v6, const IloRange v7, const IloRange v8);
  IloRangeArray(const IloEnv env, IloInt n,
		const IloRange v0, const IloRange v1, const IloRange v2,
		const IloRange v3, const IloRange v4, const IloRange v5,
		const IloRange v6, const IloRange v7, const IloRange v8,
		const IloRange v9);
#endif

  
  IloRangeArray(const IloEnv env, IloInt n, IloNum lb, IloNum ub);

  
  IloRangeArray(const IloEnv       env,
		const IloNumArray  lbs,
		const IloNumExprArray rows,
		const IloNumArray  ubs);
  
  IloRangeArray(const IloEnv       env,
		IloNum             lb,
		const IloNumExprArray rows,
		const IloNumArray  ubs);
  
  IloRangeArray(const IloEnv       env,
		const IloNumArray  lbs,
		const IloNumExprArray rows,
		IloNum             ub);
  
  IloRangeArray(const IloEnv        env,
		IloNum              lb,
		const IloNumExprArray  rows,
		IloNum              ub);

  
  IloRangeArray(const IloEnv       env,
		const IloIntArray  lbs,
		const IloNumExprArray rows,
		const IloIntArray  ubs);
  
  IloRangeArray(const IloEnv       env,
		IloNum             lb,
		const IloNumExprArray rows,
		const IloIntArray  ubs);
  
  IloRangeArray(const IloEnv       env,
		const IloIntArray  lbs,
		const IloNumExprArray rows,
		IloNum             ub);

  
  IloRangeArray(const IloEnv      env,
		const IloNumArray lbs,
		const IloNumArray ubs);
  

  IloRangeArray(const IloEnv      env,
		const IloIntArray lbs,
		const IloIntArray ubs);

  
  IloRangeArray(const IloEnv      env,
		IloNum            lb,
		const IloNumArray ubs);
  
  IloRangeArray(const IloEnv      env,
		const IloNumArray lbs,
		IloNum            ub);
  
  IloRangeArray(const IloEnv      env,
		IloNum            lb,
		const IloIntArray ubs);
  
  IloRangeArray(const IloEnv      env,
		const IloIntArray lbs,
		IloNum            ub);

  
  void add(const IloRangeArray array)         { IloExtractableArray::add (array); }
  
  void add(const IloRange range)              { IloExtractableArray::add (range); }
  
  void add(IloInt more, const IloRange range) { IloExtractableArray::add (more, range); }

  
  IloRange& operator[] (IloInt i) {
    return (*(IloRange*)&(IloExtractableArray::operator[](i)));
  }
  
  IloRange  operator[] (IloInt i) const {
    return ((IloRangeI*)((IloExtractableArray::operator[](i)).getImpl()));
  }

  
  IloNumColumn operator() (const IloIntArray vals);
  
  IloNumColumn operator() (const IloNumArray vals);

  
  
  void setBounds(const IloNumArray lbs, const IloNumArray ubs);
  
  void setBounds(const IloIntArray lbs, const IloIntArray ubs);

  void setLbs(const IloNumArray lbs);
  void setUbs(const IloNumArray ubs);

};

typedef IloDefaultArrayI IloRangeArrayI;

class IloSetRangeBounds : public IloChange {
	ILOCHANGEDECL

		IloRangeI* _rng;
	IloNum   _lb, _ub;
	IloNum   _oldlb, _oldub;

	IloSetRangeBounds ();
	IloSetRangeBounds (IloRangeI* rng, IloNum lb, IloNum ub)
		: _rng(rng), _lb(lb), _ub(ub), _oldlb(rng->getLb()), _oldub(rng->getUb())   {}

	friend class IloRangeI;

public:

	IloRangeI* getRange() const          { return (_rng); }
	IloNum     getLb() const             { return (_lb); }
	IloNum     getUb() const             { return (_ub); }
	IloNum     getOldLb() const          { return (_oldlb); }
	IloNum     getOldUb() const          { return (_oldub); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getRange();
	}
};

void IloRangeI::setBounds(IloNum lb, IloNum ub) {
	if (_lhs != lb || _rhs != ub) {
		IloSetRangeBounds newbds(this, lb, ub);
		_lhs = lb;
		_rhs = ub;
		notify(newbds);
	}
}

void IloRangeI::setUb(IloNum val) {
	if (_rhs != val) {
		IloSetRangeBounds newbds(this, _lhs, val);
		_rhs = val;
		notify(newbds);
	}
}

void IloRangeI::setLb(IloNum val) {
	if (_lhs != val) {
		IloSetRangeBounds newbds(this, val, _rhs);
		_lhs = val;
		notify(newbds);
	}
}

class IloSetRangeArrayBounds : public IloChange {
  ILOCHANGEDECL
  
  IloSetRangeArrayBounds &operator=(IloSetRangeArrayBounds const &); 

  IloRangeArray     _rng;
  const IloNumArray _lb;
  const IloNumArray _ub;
  const IloNumArray _oldlb;
  const IloNumArray _oldub;

public:
  IloSetRangeArrayBounds (const IloRangeArray rng,
                          const IloNumArray lb,
                          const IloNumArray ub,
                          const IloNumArray oldlb,
                          const IloNumArray oldub)
    : _rng(rng), _lb(lb), _ub(ub), _oldlb(oldlb), _oldub(oldub)        {}

  const IloRangeArray& getRanges () const  { return (_rng); }
  const IloNumArray& getLbs () const     { return (_lb); }
  const IloNumArray& getUbs () const     { return (_ub); }
  const IloNumArray& getOldLbs () const     { return (_oldlb); }
  const IloNumArray& getOldUbs () const     { return (_oldub); }
  virtual IloBool isApplyingOnArray() const ILO_OVERRIDE { return IloTrue; }
  virtual IloExtractableArray getChangedExtractableArray() const ILO_OVERRIDE {
    return getRanges();
  }
};

class IloSetRangeArrayLbs : public IloChange {
  ILOCHANGEDECL
  
  IloSetRangeArrayLbs &operator=(IloSetRangeArrayLbs const &); 

  IloRangeArray     _rng;
  const IloNumArray _lbs;
  const IloNumArray _oldlbs;

public:
  IloSetRangeArrayLbs (const IloRangeArray rng,
                       const IloNumArray lbs,
                       const IloNumArray oldlbs)
    : _rng(rng), _lbs(lbs), _oldlbs(oldlbs) {}

  const IloRangeArray& getRanges () const  { return (_rng); }
  const IloNumArray& getLbs () const     { return (_lbs); }
  const IloNumArray& getOldLbs () const     { return (_oldlbs); }
  virtual IloBool isApplyingOnArray() const ILO_OVERRIDE { return IloTrue; }
  virtual IloExtractableArray getChangedExtractableArray() const ILO_OVERRIDE {
    return getRanges();
  }
};

class IloSetRangeArrayUbs : public IloChange {
  ILOCHANGEDECL
  
  IloSetRangeArrayUbs &operator=(IloSetRangeArrayUbs const &); 

  IloRangeArray     _rng;
  const IloNumArray _ubs;
  const IloNumArray _oldubs;

public:
  IloSetRangeArrayUbs (const IloRangeArray rng,
                       const IloNumArray ubs,
                       const IloNumArray oldubs)
    : _rng(rng), _ubs(ubs), _oldubs(oldubs)        {}

  const IloRangeArray& getRanges () const  { return (_rng); }
  const IloNumArray& getUbs () const     { return (_ubs); }
  const IloNumArray& getOldUbs () const     { return (_oldubs); }
  virtual IloBool isApplyingOnArray() const ILO_OVERRIDE { return IloTrue; }
  virtual IloExtractableArray getChangedExtractableArray() const ILO_OVERRIDE {
    return getRanges();
  }
};

class IloSetRangeCoef : public IloChange {
	ILOCHANGEDECL

		IloRangeI*        _rng;
	const IloNumVarI* _var;
	IloNum            _val;
	IloNum            _old;

public:
	IloSetRangeCoef (IloRangeI* rng, IloNumVarI* var, IloNum val, IloNum old)
		: _rng(rng), _var(var), _val(val), _old(old)    {}

	const IloRangeI*  getRange () const   { return (_rng); }
	const IloNumVarI* getVar   () const   { return (_var); }
	IloNum            getValue () const   { return (_val); }
	IloNum            getOldVal() const   { return (_old); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getRange();
	}
};

class IloSetRangeCoefs : public IloChange {
	ILOCHANGEDECL
	
	IloSetRangeCoefs &operator=(IloSetRangeCoefs const &); 

		IloRangeI*           _rng;
	const IloNumVarArray _vars;
	const IloNumArray    _vals;
	const IloNumArray    _old;

public:
	IloSetRangeCoefs(const IloRange rng,
		const IloNumVarArray vars,
		const IloNumArray vals,
		const IloNumArray old)
		: _rng(rng.getImpl()), _vars(vars), _vals(vals), _old(old)     {}

	const IloRangeI*      getRange() const    { return (_rng); }
	const IloNumVarArray& getVars () const    { return (_vars); }
	const IloNumArray&    getVals () const    { return (_vals); }
	const IloNumArray&    getOldVals() const  { return (_old); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getRange();
	}
};

class IloSetRangeExpr : public IloChange {
	ILOCHANGEDECL

		IloRangeI*      _rng;
	IloNumLinTermI* _oldPosExpr;
	IloNumLinTermI* _newPosExpr;

public:
	IloSetRangeExpr(IloRangeI*      rng,
		IloNumLinTermI* oldPosExpr,
		IloNumLinTermI*    newPosExpr)
		: _rng(rng), _oldPosExpr(oldPosExpr), _newPosExpr(newPosExpr) {}

	IloRangeI*       getRange() const      { return (_rng); }
	IloNumLinTermI*  getOldPosExpr() const { return (_oldPosExpr); }
	IloNumLinTermI*  getNewPosExpr() const { return (_newPosExpr); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getRange();
	}
};

class ILO_EXPORTED IloInvalidInequalityException : public IloException {
public:
	IloInvalidInequalityException()
		:  IloException("Strict Inequality is defined only on Integer Expressions")
	{}
};

class IloRangeBase : public IloExtractable {
	IloNum   _cnst;
public:
	IloRangeBase (IloRangeI* rng, IloNum cst=0.0)
		: IloExtractable(rng), _cnst (cst)  {}
	IloRangeI* getImpl() const {
		return ((IloRangeI*)IloExtractable::getImpl());
	}
	IloNum getConst() const { return _cnst; }
#if defined(ILOBADRANGE)
	operator IloConstraint() { return (getImpl()); }
#else
	operator IloRange() { return (getImpl()); }
#endif
};

class IloBinaryNumConstraintI : public IloConstraintI {
	ILOEXTRDECL
protected:
	IloNumExprI*_expr1;
	IloNumExprI*_expr2;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	IloInt _state;
public:
	IloBinaryNumConstraintI (IloEnvI*  e,
		IloNumExprI* expr1,
		IloNumExprI* expr2,
		const char* name);
	virtual ~IloBinaryNumConstraintI();

	IloNumExprI* getExpr1() const {return _expr1;}
	IloNumExprI* getExpr2() const {return _expr2;}
	IloBool isVar1() const { return (_state & 1) == 0; }
	IloBool isVar2() const { return (_state & 2) == 0; }
	ILOEXTROTHERDECL
		virtual IloNum getLb (IloModelEvaluatorI* e) = 0;
	virtual IloNum getUb (IloModelEvaluatorI* e) = 0;
};

class IloNumEqI : public IloBinaryNumConstraintI {
	ILOEXTRDECL
public:
	IloNumEqI (IloEnvI*  e, IloNumExprI* expr1, IloNumExprI* expr2, const char* name=0);
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	ILOEXTROTHERDECL
		virtual IloNum getLb (IloModelEvaluatorI* e) ILO_OVERRIDE;
	virtual IloNum getUb (IloModelEvaluatorI* e) ILO_OVERRIDE;
};

class IloNumGeI : public IloBinaryNumConstraintI {
	ILOEXTRDECL
public:
	IloNumGeI (IloEnvI*  e, IloNumExprI* expr1, IloNumExprI* expr2, const char* name=0);
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	ILOEXTROTHERDECL
		virtual IloNum getLb (IloModelEvaluatorI* e) ILO_OVERRIDE;
	virtual IloNum getUb (IloModelEvaluatorI* e) ILO_OVERRIDE;
};

class IloNumLeI : public IloBinaryNumConstraintI {
	ILOEXTRDECL
public:
	IloNumLeI (IloEnvI*  e, IloNumExprI* expr1, IloNumExprI* expr2, const char* name=0);
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	ILOEXTROTHERDECL
		virtual IloNum getLb (IloModelEvaluatorI* e) ILO_OVERRIDE;
	virtual IloNum getUb (IloModelEvaluatorI* e) ILO_OVERRIDE;
};

class IloNumGTI : public IloBinaryNumConstraintI {
	ILOEXTRDECL
public:
	IloNumGTI (IloEnvI*  e, IloNumExprI* expr1, IloNumExprI* expr2, const char* name=0);
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	ILOEXTROTHERDECL
		virtual IloNum getLb (IloModelEvaluatorI* e) ILO_OVERRIDE;
	virtual IloNum getUb (IloModelEvaluatorI* e) ILO_OVERRIDE;
};

class IloNumLTI : public IloBinaryNumConstraintI {
	ILOEXTRDECL
public:
	IloNumLTI (IloEnvI*  e, IloNumExprI* expr1, IloNumExprI* expr2, const char* name=0);
	IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	ILOEXTROTHERDECL
		virtual IloNum getLb (IloModelEvaluatorI* e) ILO_OVERRIDE;
	virtual IloNum getUb (IloModelEvaluatorI* e) ILO_OVERRIDE;
};

ILO_EXPORTEDFUNCTION(IloConstraint) operator== (IloNumExprArg base, IloNumExprArg expr);

ILO_EXPORTEDFUNCTION(IloRange) operator== (IloNumExprArg base, IloNum val);

ILO_EXPORTEDFUNCTION(IloRange) operator== (IloNum val, IloNumExprArg eb);

ILO_EXPORTEDFUNCTION(IloConstraint) operator>= (IloNumExprArg base, IloNumExprArg base2);

ILO_EXPORTEDFUNCTION(IloRange) operator>= (IloNumExprArg expr, IloNum val);
inline IloRange operator>= (IloNumExprArg expr, IloInt val) {
	return IloNumExprArg(expr) >= IloNum(val);
}
inline IloRange operator>= (IloNumExprArg expr, int val) {
	return IloNumExprArg(expr) >= IloNum(val);
}
inline IloRange operator>= (IloIntExprArg expr, IloNum val) {
	return IloNumExprArg(expr) >= IloNum(val);
}
inline IloRange operator>= (IloIntExprArg expr, IloInt val) {
	return IloNumExprArg(expr) >= IloNum(val);
}
inline IloRange operator>= (IloIntExprArg expr, int val) {
	return IloNumExprArg(expr) >= IloNum(val);
}

ILO_EXPORTEDFUNCTION(IloRange) operator>= (IloNum val, IloNumExprArg eb);
inline IloRange operator>= (IloInt val, IloNumExprArg expr) {
	return IloNum(val) >= IloNumExprArg(expr);
}
inline IloRange operator>= (int val, IloNumExprArg expr) {
	return IloNum(val) >= IloNumExprArg(expr);
}
inline IloRange operator>= (IloNum val, IloIntExprArg expr) {
	return IloNum(val) >= IloNumExprArg(expr);
}
inline IloRange operator>= (IloInt val, IloIntExprArg expr) {
	return IloNum(val) >= IloNumExprArg(expr);
}
inline IloRange operator>= (int val, IloIntExprArg expr) {
	return IloNum(val) >= IloNumExprArg(expr);
}

ILO_EXPORTEDFUNCTION(IloConstraint) operator> (IloNumExprArg base, IloNumExprArg base2);

ILO_EXPORTEDFUNCTION(IloConstraint) operator> (IloNumExprArg base, IloNum val);

ILO_EXPORTEDFUNCTION(IloConstraint) operator> (IloNum val, IloNumExprArg eb);

ILO_EXPORTEDFUNCTION(IloConstraint) operator> (IloIntExprArg base, IloIntExprArg base2);

ILO_EXPORTEDFUNCTION(IloConstraint) operator> (IloIntExprArg base, IloInt val);

ILO_EXPORTEDFUNCTION(IloConstraint) operator> (IloInt val, IloIntExprArg eb);

ILO_EXPORTEDFUNCTION(IloConstraint) operator<= (IloNumExprArg base, IloNumExprArg base2);

ILO_EXPORTEDFUNCTION(IloRange) operator<= (IloNumExprArg base, IloNum val);
inline IloRange operator<= (IloNumExprArg expr, IloInt val) {
	return IloNumExprArg(expr) <= IloNum(val);
}
inline IloRange operator<= (IloNumExprArg expr, int val) {
	return IloNumExprArg(expr) <= IloNum(val);
}
inline IloRange operator<= (IloIntExprArg expr, IloNum val) {
	return IloNumExprArg(expr) <= IloNum(val);
}
inline IloRange operator<= (IloIntExprArg expr, IloInt val) {
	return IloNumExprArg(expr) <= IloNum(val);
}
inline IloRange operator<= (IloIntExprArg expr, int val) {
	return IloNumExprArg(expr) <= IloNum(val);
}

ILO_EXPORTEDFUNCTION(IloRangeBase) operator<= (IloNum val, const IloNumExprArg expr);
inline IloRangeBase operator<= (IloInt val, IloNumExprArg x) {
	return IloNum(val) <= IloNumExprArg(x);
}
inline IloRangeBase operator<= (int val, IloNumExprArg x) {
	return IloNum(val) <= IloNumExprArg(x);
}
inline IloRangeBase operator<= (IloNum val, IloIntExprArg x) {
	return IloNum(val) <= IloNumExprArg(x);
}
inline IloRangeBase operator<= (IloInt val, IloIntExprArg x) {
	return IloNum(val) <= IloNumExprArg(x);
}
inline IloRangeBase operator<= (int val, IloIntExprArg x) {
	return IloNum(val) <= IloNumExprArg(x);
}

ILO_EXPORTEDFUNCTION(IloRange) operator<=(const IloRangeBase base, IloNum val);
inline IloRange operator<= (const IloRangeBase base, IloInt val) {
	return base <= IloNum(val);
}
inline IloRange operator<= (const IloRangeBase base, int val) {
	return base <= IloNum(val);
}

ILO_EXPORTEDFUNCTION(IloConstraint) operator< (IloNumExprArg base, IloNumExprArg base2);

ILO_EXPORTEDFUNCTION(IloConstraint) operator< (IloNumExprArg base, IloNum val);

ILO_EXPORTEDFUNCTION(IloConstraint) operator< (IloNum val, const IloNumExprArg expr);

ILO_EXPORTEDFUNCTION(IloConstraint) operator< (IloIntExprArg base, IloIntExprArg base2);

ILO_EXPORTEDFUNCTION(IloConstraint) operator< (IloIntExprArg base, IloInt val);

ILO_EXPORTEDFUNCTION(IloConstraint) operator< (IloInt val, IloIntExprArg base);

class IloSOS1I: public IloConstraintI {
	ILOEXTRDECL

		IloNumVarArray _varArray;
	IloNumArray    _valArray;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	virtual ~IloSOS1I();
	IloSOS1I(IloEnvI* m, const char* name=0);
	IloSOS1I(IloEnvI* m, const IloNumVarArray vars, const char* name=0);
	IloSOS1I(IloEnvI* m, const IloNumVarArray vars, const IloNumArray vals,
		const char* name=0);

	void addVar(IloNumVar var);
	void removeVar(IloNumVar);
	void addVal(IloNum    val);
	IloNum getMaxVal() const;

	ILOEXTROTHERDECL

		IloNumVarArray getVarArray() const {return _varArray;}
	IloNumArray    getValArray() const {return _valArray;}
};

class IloSOS1RemoveVar : public IloChange {
	ILOCHANGEDECL

		IloNumVarI* _var;
	IloSOS1I* _sos;
public:
	IloSOS1RemoveVar(IloSOS1I* sos, IloNumVarI* var)
		: IloChange(), _var(var), _sos(sos) {}
	IloNumVarI* getVar() const     { return (_var); }
	IloSOS1I*  getSOS1() const { return (_sos); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getSOS1();
	}
};

class IloAddVarToSOS1 : public IloAddNumVar {
	ILOCOLUMNDECL

		IloSOS1I* _sos1;
	IloNum    _val;

	const IloExtractableI* what() const ILO_OVERRIDE {return _sos1;}
	void destroy();

public:
	virtual ~IloAddVarToSOS1();
	IloAddVarToSOS1(IloSOS1I* sos1, IloNum val) : _sos1(sos1), _val(val) {}

	void apply() const ILO_OVERRIDE;

	IloEnvI*  getEnv() const ILO_OVERRIDE { return (_sos1->getEnv()); }
	IloSOS1I* getSOS1I() const  { return (_sos1); }
	IloNum    getValue() const  { return (_val); }
	virtual IloExtractableI* getChangedExtractabl() const {
		return getSOS1I();
	}
};

class ILO_EXPORTED IloSOS1 : public IloConstraint {
	ILOEXTRHANDLE(IloSOS1,IloConstraint)
public:
	
	IloSOS1(const IloEnv env, const char* name = 0);
	
	IloSOS1(const IloEnv env, const IloNumVarArray vars, const char* name = 0)
		: IloConstraint(new (env.getImpl()) IloSOS1I(env.getImpl(), vars.copy(), name)) {
			IloAssert(vars.getImpl(), "Using empty IloNumVarArray handle.");
	}

	
	IloSOS1(const IloEnv env, const IloNumVarArray vars, const IloNumArray vals,
		const char* name = 0)
		: IloConstraint(new (env.getImpl()) IloSOS1I(env.getImpl(),
		vars.copy(),
		vals.copy(), name)) {
			IloAssert(vars.getImpl(), "Using empty IloNumVarArray handle.");
			IloAssert(vals.getImpl(), "Using empty IloNumArray handle.");
			IloAssert(vars.getSize() == vals.getSize(),
				"variable and value array sizes does not match for SOS1");
	}

	IloAddVarToSOS1 operator() () {
		return (IloAddVarToSOS1(getImpl(), -1.0));
	}

	IloAddVarToSOS1 operator() (IloNum val) {
		return (IloAddVarToSOS1(getImpl(), val));
	}

	
	void getValues(IloNumArray values) const {
		IloAssert(getImpl(), "Using empty IloSOS1 handle.");
		IloAssert(values.getImpl(), "Using empty IloNumArray handle.");
		values.add(getImpl()->getValArray());
	}
	void getVariables(IloNumVarArray variables) const {
		IloAssert(getImpl(), "Using empty IloSOS1 handle.");
		IloAssert(variables.getImpl(), "Using empty IloNumVarArray handle.");
		variables.add(getImpl()->getVarArray());
	}
	
	void getNumVars(IloNumVarArray variables) const { getVariables(variables); }
};

class ILO_EXPORTED IloSOS1Array : public IloConstraintArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloSOS1Array(IloDefaultArrayI* i=0) : IloConstraintArray(i) {}
	
	IloSOS1Array(const IloEnv env, IloInt n = 0) : IloConstraintArray(env, n) {}
	IloSOS1Array(const IloMemoryManager env, IloInt n = 0) : IloConstraintArray(env, n) {}

	IloSOS1Array(const IloEnv env, IloInt n, const IloSOS1 v0);
#ifdef ILOALLOWVARARGS
	IloSOS1Array(const IloEnv env, IloInt n, const IloSOS1 v0, const IloSOS1 v1 ...);
#else 
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2,
		const IloSOS1 v3);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2,
		const IloSOS1 v3, const IloSOS1 v4);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2,
		const IloSOS1 v3, const IloSOS1 v4, const IloSOS1 v5);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2,
		const IloSOS1 v3, const IloSOS1 v4, const IloSOS1 v5,
		const IloSOS1 v6);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2,
		const IloSOS1 v3, const IloSOS1 v4, const IloSOS1 v5,
		const IloSOS1 v6, const IloSOS1 v7);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2,
		const IloSOS1 v3, const IloSOS1 v4, const IloSOS1 v5,
		const IloSOS1 v6, const IloSOS1 v7, const IloSOS1 v8);
	IloSOS1Array(const IloEnv env, IloInt n,
		const IloSOS1 v0, const IloSOS1 v1, const IloSOS1 v2,
		const IloSOS1 v3, const IloSOS1 v4, const IloSOS1 v5,
		const IloSOS1 v6, const IloSOS1 v7, const IloSOS1 v8,
		const IloSOS1 v9);
#endif

	
	void add(const IloSOS1Array& x)         { IloExtractableArray::add (x); }
	
	void add(const IloSOS1& x)              { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloSOS1& x) { IloExtractableArray::add (more, x); }

	
	IloSOS1& operator[] (IloInt i) {
		return (*(IloSOS1*)&(IloExtractableArray::operator[](i)));
	}
	
	IloSOS1  operator[] (IloInt i) const {
		return ((IloSOS1I*)((IloExtractableArray::operator[](i)).getImpl()));
	}
};

typedef IloDefaultArrayI IloSOS1ArrayI;

class IloSOS2I: public IloConstraintI {
	ILOEXTRDECL

		IloNumVarArray _varArray;
	IloNumArray    _valArray;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	virtual ~IloSOS2I();
	IloSOS2I(IloEnvI* m, const char* name=0);
	IloSOS2I(IloEnvI* m, const IloNumVarArray vars, const char* name=0);
	IloSOS2I(IloEnvI* m, const IloNumVarArray vars, const IloNumArray vals,
		const char* name=0);

	void addVar(IloNumVar var);
	void removeVar(IloNumVar);
	void addVal(IloNum    val);
	IloNum getMaxVal() const;

	ILOEXTROTHERDECL

		IloNumVarArray getVarArray() const {return _varArray;}
	IloNumArray    getValArray() const {return _valArray;}
};

class IloSOS2RemoveVar : public IloChange {
	ILOCHANGEDECL

		IloNumVarI* _var;
	IloSOS2I* _sos;
public:
	IloSOS2RemoveVar(IloSOS2I* sos, IloNumVarI* var) : IloChange(), _var(var), _sos(sos) {}
	IloNumVarI* getVar() const     { return (_var); }
	IloSOS2I*  getSOS2() const { return (_sos); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getSOS2();
	}
};

class IloAddVarToSOS2 : public IloAddNumVar {
	ILOCOLUMNDECL

		IloSOS2I* _sos2;
	IloNum    _val;

	const IloExtractableI* what() const ILO_OVERRIDE {return _sos2;}
	void destroy();

public:
	virtual ~IloAddVarToSOS2();
	IloAddVarToSOS2(IloSOS2I* sos2, IloNum val) : _sos2(sos2), _val(val) {}

	void apply() const ILO_OVERRIDE;

	IloEnvI*  getEnv() const ILO_OVERRIDE { return (_sos2->getEnv()); }
	IloSOS2I* getSOS2I() const  { return (_sos2); }
	IloNum    getValue() const  { return (_val); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getSOS2I();
	}
};

class ILO_EXPORTED IloSOS2 : public IloConstraint {
	ILOEXTRHANDLE(IloSOS2,IloConstraint)
public:
	
	IloSOS2(const IloEnv env, const char* name = 0);
	
	IloSOS2(const IloEnv env, const IloNumVarArray vars, const char* name = 0)
		: IloConstraint(new (env.getImpl()) IloSOS2I(env.getImpl(), vars.copy(), name)) {
			IloAssert(vars.getImpl(), "Using empty IloNumVarArray handle.");
	}
	
	IloSOS2(const IloEnv env, const IloNumVarArray vars, const IloNumArray vals,
		const char* name = 0)
		: IloConstraint(new (env.getImpl()) IloSOS2I(env.getImpl(),
		vars.copy(),
		vals.copy(), name)) {
			IloAssert(vars.getImpl(), "Using empty IloNumVarArray handle.");
			IloAssert(vals.getImpl(), "Using empty IloNumArray handle.");
			IloAssert (vars.getSize() == vals.getSize(),
				"Inconsistent variable and value set for SOS2");
	}

	IloAddVarToSOS2 operator() () {
		return (IloAddVarToSOS2(getImpl(), -1.0));
	}

	IloAddVarToSOS2 operator() (IloNum val) {
		return (IloAddVarToSOS2(getImpl(), val));
	}

	
	void getValues(IloNumArray values) const {
		IloAssert(getImpl(), "Using empty IloSOS2 handle.");
		IloAssert(values.getImpl(), "Using empty IloNumArray handle.");
		values.add(getImpl()->getValArray());
	}
	void getVariables(IloNumVarArray variables) const {
		IloAssert(getImpl(), "Using empty IloSOS2 handle.");
		IloAssert(variables.getImpl(), "Using empty IloNumVarArray handle.");
		variables.add(getImpl()->getVarArray());
	}
	
	void getNumVars(IloNumVarArray variables) const { getVariables(variables); }
};

class ILO_EXPORTED IloSOS2Array : public IloConstraintArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloSOS2Array(IloDefaultArrayI* i=0) : IloConstraintArray(i) {}
	
	IloSOS2Array(const IloEnv env, IloInt num = 0) : IloConstraintArray(env, num) {}
	IloSOS2Array(const IloMemoryManager env, IloInt num = 0) : IloConstraintArray(env, num) {}

	IloSOS2Array(const IloEnv env, IloInt num, const IloSOS2 v0);
#ifdef ILOALLOWVARARGS
	
	IloSOS2Array(const IloEnv env, IloInt num, const IloSOS2 v0, const IloSOS2 v1, ...);
#else 
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2,
		const IloSOS2 v3);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2,
		const IloSOS2 v3, const IloSOS2 v4);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2,
		const IloSOS2 v3, const IloSOS2 v4, const IloSOS2 v5);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2,
		const IloSOS2 v3, const IloSOS2 v4, const IloSOS2 v5,
		const IloSOS2 v6);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2,
		const IloSOS2 v3, const IloSOS2 v4, const IloSOS2 v5,
		const IloSOS2 v6, const IloSOS2 v7);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2,
		const IloSOS2 v3, const IloSOS2 v4, const IloSOS2 v5,
		const IloSOS2 v6, const IloSOS2 v7, const IloSOS2 v8);
	IloSOS2Array(const IloEnv env, IloInt num,
		const IloSOS2 v0, const IloSOS2 v1, const IloSOS2 v2,
		const IloSOS2 v3, const IloSOS2 v4, const IloSOS2 v5,
		const IloSOS2 v6, const IloSOS2 v7, const IloSOS2 v8,
		const IloSOS2 v9);
#endif

	
	void add(const IloSOS2Array array)     { IloExtractableArray::add (array); }
	
	void add(const IloSOS2 x)              { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloSOS2 x) { IloExtractableArray::add (more, x); }

	
	IloSOS2& operator[] (IloInt i) {
		return (*(IloSOS2*)&(IloExtractableArray::operator[](i)));
	}
	
	IloSOS2  operator[] (IloInt i) const {
		return ((IloSOS2I*)((IloExtractableArray::operator[](i)).getImpl()));
	}
};

class IloAddValueToObj : public IloAddNumVar {
	ILOCOLUMNDECL

		IloNum _value;
	IloObjectiveI* _obj;

	const IloExtractableI* what() const ILO_OVERRIDE;

public:
	
	IloAddValueToObj (IloObjective obj, IloNum val);

	void apply    () const ILO_OVERRIDE;

	IloNum getValue()            const { return (_value); }
	IloObjectiveI* getObjective() const { return (_obj); }
};

typedef IloDefaultArrayI IloSOS2ArrayI;

class IloMultiCriterionExprI;

class ILO_EXPORTED IloObjective : public IloExtractable {
	ILOEXTRHANDLE(IloObjective, IloExtractable)
public:
	
	enum Sense {
		Minimize = 1,
		Maximize = -1
	};

	
	IloObjective(const IloEnv env,
		IloNum       constant = 0.0,
		Sense        sense = Minimize,
		const char*  name = 0);

	
	inline IloObjective(const IloEnv     env,
		const IloNumExprArg expr,
		Sense            sense = Minimize,
		const char*      name = 0);
	
	IloObjective(const IloEnv env,
		     IloMultiCriterionExpr moExpr,
                     IloObjective::Sense sense,
                     const char * name=0);

	
	Sense  getSense () const;
	IloNum getConst () const;
	
	IloNum getConstant() const { return getConst(); }
	operator IloNumExprArg() const;

	
	IloAddValueToObj operator() () {
		return (IloAddValueToObj (*this, 1.0));
	}

	
	IloAddValueToObj operator() (IloNum value) {
		return (IloAddValueToObj (*this, value));
	}

	
	
	void setSense(IloObjective::Sense sense);
	void setConst(IloNum);
	
	void setConstant(IloNum constant) { setConst(constant); }
	
	IloNumExprArg getExpr() const;
	
	void setExpr(const IloNumExprArg);
	
	void setLinearCoef(const IloNumVar var, IloNum value);
	
	void setLinearCoefs(const IloNumVarArray vars, const IloNumArray values);

	
	void setQuadCoef(const IloNumVar var1, const IloNumVar var2, IloNum value);

	
	IloExpr::LinearIterator getLinearIterator() const;
	
	IloExpr::QuadIterator getQuadIterator() const;
	
		
	IloBool isMultiCrit() const;
	    
	IloInt getNumCriteria() const;
	    
	IloNumExprArg getCriterion(IloInt i) const;
	
	IloNum getWeight(IloInt i) const;
};

class ILO_EXPORTED IloObjectiveI : public IloExtractableI {
	ILOEXTRDECL
        IloObjective::Sense _sense;
	IloNumLinTermI* _expr;
        IloMultiCriterionExprI * _moExpr;

	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	virtual void whenAddedToCP(const IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE;
	virtual void whenRemovedFromCP(const IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE;
	static IloAddToCP _addToCP;
	static IloRemoveFromCP _removeFromCP;
public:
	IloObjectiveI(IloEnvI*            env,
		IloNum              cnst,
		IloObjective::Sense sense,
		const char*         name=0);

	IloObjectiveI(IloEnvI*            env,
		IloNumExprI*    expr,
		IloObjective::Sense sense,
		const char*         name=0);
	IloObjectiveI(IloEnvI*            env,
		      IloMultiCriterionExprI* moExpr,
                      IloObjective::Sense sense,
                      const char*         name=0);
	virtual ~IloObjectiveI();

	IloNumLinTermI* getExpr () const;
        IloBool isMultiCrit() const { return _moExpr != 0; }
        IloMultiCriterionExprI * getMultiCritExpr() const { return _moExpr; }
        IloInt getNumCriteria() const;
        IloNumExprI * getCriterion(IloInt i) const;
	IloObjective::Sense getSense() const  { return (_sense); }

	void addVar(IloNumVarI* var, IloNum val);

	void setSense(IloObjective::Sense);
	void setConst(IloNum);
	void setCoef(const IloNumVar, IloNum);
	void setQuadCoef(const IloNumVar, const IloNumVar, IloNum);
	void setExpr(const IloNumExprArg);
	void setCoef(const IloNumVarArray, const IloNumArray);

	IloInt normalize();

	ILOEXTROTHERDECL
};

class IloSetObjSense : public IloChange {
	ILOCHANGEDECL

		IloObjectiveI*      _obj;
	IloObjective::Sense _sense;

public:
	IloSetObjSense(IloObjectiveI* obj, IloObjective::Sense sense)
		: _obj(obj), _sense(sense)        {}

	IloObjectiveI*      getObjective() const    { return (_obj); }
	IloObjective::Sense getSense() const        { return (_sense); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getObjective();
	}
};

class IloSetObjConst : public IloChange {
	ILOCHANGEDECL

		IloObjectiveI* _obj;
	IloNum _val;

public:
	IloSetObjConst(IloObjectiveI* obj, IloNum val)
		: _obj(obj), _val(val)    {}

	IloObjectiveI* getObjective() const    { return (_obj); }
	IloNum getConst() const        { return (_val); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getObjective();
	}
};

class IloSetObjCoef : public IloChange {
	ILOCHANGEDECL

		IloObjectiveI*    _obj;
	const IloNumVarI* _var;
	IloNum            _val;
	IloNum            _old;

public:
	IloSetObjCoef(IloObjectiveI* obj, const IloNumVar var, IloNum val, IloNum old)
		: _obj (obj)
		, _var (var.getImpl())
		, _val (val)
		, _old(old)    {}

	const IloObjectiveI*    getObjective() const { return (_obj); }
	const IloNumVarI* getVar() const       { return (_var); }
	IloNum            getVal() const       { return (_val); }
	IloNum            getOldVal() const   { return (_old); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getObjective();
	}
};

class IloSetObjCoefs : public IloChange {
	ILOCHANGEDECL
	
	IloSetObjCoefs &operator=(IloSetObjCoefs const &); 

		IloObjectiveI*    _obj;
	IloNumVarArray    _vars;
	const IloNumArray _vals;
	const IloNumArray _old;

public:
	IloSetObjCoefs (IloObjectiveI*    obj,
		IloNumVarArray    vars,
		const IloNumArray vals,
		const IloNumArray old)
		: _obj (obj)
		, _vars (vars)
		, _vals (vals)
		, _old(old)     {}

	const IloObjectiveI*  getObjective() const { return (_obj); }
	const IloNumVarArray& getVars() const      { return (_vars); }
	const IloNumArray&    getVals() const      { return (_vals); }
	const IloNumArray&    getOldVals() const   { return (_old); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getObjective();
	}
};

class IloSetObjQuadCoef : public IloChange {
   ILOCHANGEDECL

   IloObjectiveI*    _obj;
   const IloNumVarI* _var1;
   const IloNumVarI* _var2;
   IloNum            _val;
   IloNum            _old;

public:
   IloSetObjQuadCoef(IloObjectiveI* obj, const IloNumVar var1, const IloNumVar var2,
                                         IloNum val, IloNum old)
      : _obj (obj)
      , _var1 (var1.getImpl())
      , _var2 (var2.getImpl())
      , _val (val)
      , _old(old)    {}

   const IloObjectiveI* getObjective() const { return (_obj); }
   const IloNumVarI*    getVar1() const      { return (_var1); }
   const IloNumVarI*    getVar2() const      { return (_var2); }
   IloNum               getVal() const       { return (_val); }
   IloNum               getOldVal() const    { return (_old); }
   virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
      return getObjective();
   }
};

class IloSetObjExpr : public IloChange {
	ILOCHANGEDECL

	IloObjectiveI* _obj;
	IloNumLinTermI* _oldExpr;
	IloNumLinTermI* _newExpr;

public:
	IloSetObjExpr(IloObjectiveI* obj, IloNumLinTermI* oldExpr,
		                          IloNumLinTermI* newExpr)
		: _obj(obj), _oldExpr(oldExpr), _newExpr(newExpr) {}

	IloObjectiveI*  getObjective() const { return (_obj); }
	IloNumLinTermI* getOldExpr() const   { return (_oldExpr); }
	IloNumLinTermI* getNewExpr() const   { return (_newExpr); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getObjective();
	}
};

ILO_EXPORTED IloObjective::IloObjective(const IloEnv env,
										const IloNumExprArg expr,
										Sense sense,
										const char* name)
										: IloExtractable(new (env.getImpl()) IloObjectiveI(env.getImpl(), expr.getImpl(), sense, name)) {
											IloAssert(env.getImpl(), "Using empty IloEnv handle");
											IloAssert(expr.getImpl(), "Using empty IloNumExpr handle");
}

class ILO_EXPORTED IloMultiCriterionExprI : public IloRttiEnvObjectI {
  ILORTTIDECL
public:
  typedef enum {
    StaticLex = 0,
    DynamicLex = 1
  } Type;
protected:
  Type            _type;
  const char *    _name;
  IloNumExprArray _exprs;
  IloNumArray _weightArray;
  IloIntArray _priorityArray;
  IloNumArray _absTolArray;
  IloNumArray _relTolArray;
public:
  IloMultiCriterionExprI(
    IloEnvI* env,
    const IloNumExprArray exprArray,
    Type type,
    const IloNumArray _weightArray = 0,
    const IloIntArray _priorityArray = 0,
    const IloNumArray _absTolArray = 0,
    const IloNumArray _relTolArray = 0,
    const char* name=0
  );
  IloMultiCriterionExprI(
    IloEnvI* env,
    const IloNumExprArray exprArray,
    Type type,
    const char* name
  );
  virtual ~IloMultiCriterionExprI();
  IloMultiCriterionExprI::Type getType() const { return _type; }
  const char * getName() const { return _name; }
  void setName(const char * name);
  virtual IloInt getNumCriteria();
  virtual IloNumExprI * getCriterion(IloInt i);
  virtual IloNumExprArray getExprs() const;
  virtual const IloNumArray getWeights() const;
  virtual const IloIntArray getPriorities() const;
  virtual const IloNumArray getAbsTols() const;
  virtual const IloNumArray getRelTols() const;
  virtual void display(ILOSTD(ostream)& out);
  virtual IloMultiCriterionExprI * duplicate(IloEnvI * env) const;
  virtual IloMultiCriterionExprI * makeClone(IloEnvI * env) const;
  virtual IloNumExprArray getOrMakeExprs();
  virtual IloBool isOpl() const;
  IloNum getWeight(IloInt idx) {
	  IloTestAndRaise(idx < _exprs.getSize(), "getWeight problem of size");
	  if (!_weightArray.getImpl()) return 1.0;
	  if (!_weightArray.getSize()) return 1.0; 
	  return _weightArray[idx];
  }
};

class ILO_EXPORTED IloMultiCriterionExpr {
  IloMultiCriterionExprI * _impl;
public:
  typedef IloMultiCriterionExprI::Type Type;

  
  IloMultiCriterionExpr(const IloEnv env,
                          const IloNumExprArray exprArray,
                          Type type,
                          const char* name = 0);
  IloMultiCriterionExpr(const IloEnv env,
                          Type type,
                          const IloNumExprArray exprArray,
                          const IloNumArray weightArray=0,
                          const IloIntArray priorityArray=0,
                          const IloNumArray absTolArray=0,
                          const IloNumArray relTolArray=0,
                          const char* name = 0);
  IloMultiCriterionExpr() : _impl(0) { }
  IloMultiCriterionExpr(IloMultiCriterionExprI * impl) : _impl(impl) { }
  IloMultiCriterionExprI * getImpl() const { return _impl; }
  const IloNumArray getWeights() const {
    IloAssert(_impl != 0, "IloMultiCriterionExpr: using empty handle");
    return _impl->getWeights();
  }
  const IloIntArray getPriorities() const {
    IloAssert(_impl != 0, "IloMultiCriterionExpr: using empty handle");
    return _impl->getPriorities();
  }
  const IloNumArray getAbsTols() const {
    IloAssert(_impl != 0, "IloMultiCriterionExpr: using empty handle");
    return _impl->getAbsTols();
  }
  const IloNumArray getRelTols() const {
    IloAssert(_impl != 0, "IloMultiCriterionExpr: using empty handle");
    return _impl->getRelTols();
  }
  const char * getName() const {
    IloAssert(_impl != 0, "IloMultiCriterionExpr: using empty handle");
    return _impl->getName();
  }
  void setName(const char * name) const {
    IloAssert(_impl != 0, "IloMultiCriterionExpr: using empty handle");
    _impl->setName(name);
  }
  void end();
  
  IloNum getWeight(IloInt idx) {
	  return _impl->getWeight(idx);
  }
};

class ILO_EXPORTED IloNotAllowedWithMultiCriterionException : public IloException {
public:
  IloNotAllowedWithMultiCriterionException(const char * message);
  void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
};

ILO_EXPORTEDFUNCTION(ILOSTD(ostream)&) operator << (ILOSTD(ostream)&, const IloMultiCriterionExpr& );

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloStaticLex(IloEnv env, IloNumExprArray criteria,
                                                                     const IloNumArray weights,
                                                                     const IloIntArray priorities,
                                                                     const IloNumArray absTols,
                                                                     const IloNumArray relTols,
                                                                     const char * name = 0);

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloStaticLex(IloEnv env, IloNumExprArray criteria, const char * name);

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloStaticLex(IloEnv env, IloNumExprArray criteria);

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloStaticLex(IloEnv env, IloNumExprArg c1);

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloStaticLex(IloEnv env, IloNumExprArg c1, IloNumExprArg c2);

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloStaticLex(IloEnv env, IloNumExprArg c1, IloNumExprArg c2, IloNumExprArg c3);

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloStaticLex(IloEnv env, IloNumExprArg c1, IloNumExprArg c2, IloNumExprArg c3, IloNumExprArg c4);

ILO_EXPORTEDFUNCTION(IloMultiCriterionExpr) IloDynamicLex(IloEnv env, IloNumExprArray criteria, const char * name = 0);

inline IloObjective IloMinimize(const IloEnv env,
								IloNum constant=0.0,
								const char* name=0) {
									IloAssert (0L != env.getImpl(), "Using empty handle");
									return IloObjective(env, constant, IloObjective::Minimize, name);
}

inline IloObjective IloMinimize(const IloEnv     env,
								const IloNumExprArg expr,
								const char*      name=0) {
									IloAssert (0L != env.getImpl(), "Using empty handle");
									return IloObjective(env, expr, IloObjective::Minimize, name);
}

inline IloObjective IloMaximize(const IloEnv env,
								IloNum constant=0.0,
								const char* name=0) {
									IloAssert (0L != env.getImpl(), "Using empty handle");
									return IloObjective(env, constant, IloObjective::Maximize, name);
}

inline IloObjective IloMaximize(const IloEnv env,
								const IloNumExprArg expr,
								const char* name=0) {
									IloAssert (0L != env.getImpl(), "Using empty handle");
									return IloObjective(env, expr, IloObjective::Maximize, name);
}

ILO_EXPORTEDFUNCTION(IloObjective) IloMinimize(
  const IloEnv env,
  const IloMultiCriterionExpr expr,
  const char* name=0
);

ILO_EXPORTEDFUNCTION(IloObjective) IloMaximize(
  const IloEnv env,
  const IloMultiCriterionExpr expr,
  const char* name=0
);

class IloForAllI;

class IloForAllRangeI : public IloConstraintI {
	ILOEXTRDECL
		IloForAllI* forall;
	IloInt position;
public:
	IloForAllRangeI(IloEnvI* env, IloForAllI* all, IloInt pos, const char* name = 0);
	ILOEXTROTHERDECL
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	virtual ~IloForAllRangeI(){ }
	IloForAllI* getForAll() const   { return forall;}
	IloInt   getPosition() const         { return position;}
	IloConstraintI* getExpanded() const { return 0;} 
	inline void setLinearCoef(const IloNumVar, IloNum);
	inline void setBounds(IloNum, IloNum);
	inline void setUb(IloNum);
	inline void setLb(IloNum);
	inline void setLinearCoefs(const IloNumVarArray, const IloNumArray);
  
  virtual IloBool hasEditableBounds() const ILO_OVERRIDE { return IloTrue;}

};

class IloSetForAllRangeBounds : public IloChange {
	ILOCHANGEDECL

		IloForAllRangeI* _rng;
	IloNum   _lb, _ub;

	IloSetForAllRangeBounds ();
	IloSetForAllRangeBounds (IloForAllRangeI* rng, IloNum lb, IloNum ub)
		: _rng(rng), _lb(lb), _ub(ub)   {}

	friend class IloForAlllRangeI;

public:

	IloForAllRangeI* getForAllRange() const { return (_rng); }
	IloNum     getLb() const             { return (_lb); }
	IloNum     getUb() const             { return (_ub); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE;
};

void IloForAllRangeI::setBounds(IloNum lb, IloNum ub) {
	notify(IloSetForAllRangeBounds(this, lb, ub));
}

class IloSetForAllRangeUB : public IloChange {
	ILOCHANGEDECL

		IloForAllRangeI* _rng;
	IloNum   _ub;

	IloSetForAllRangeUB ();
	IloSetForAllRangeUB (IloForAllRangeI* rng, IloNum ub)
		: _rng(rng), _ub(ub)   {}

	friend class IloForAlllRangeI;

public:

	IloForAllRangeI* getForAllRange() const { return (_rng); }
	IloNum     getUB() const             { return (_ub); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE;
};

void IloForAllRangeI::setUb(IloNum val) {
  notify(IloSetForAllRangeUB(this, val));
}

class IloSetForAllRangeLB : public IloChange {
	ILOCHANGEDECL

		IloForAllRangeI* _rng;
	IloNum   _lb;

	IloSetForAllRangeLB ();
	IloSetForAllRangeLB (IloForAllRangeI* rng, IloNum lb)
		: _rng(rng), _lb(lb)   {}

	friend class IloForAlllRangeI;

public:

	IloForAllRangeI* getForAllRange() const { return (_rng); }
	IloNum     getLB() const             { return (_lb); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE;
};

void IloForAllRangeI::setLb(IloNum val) {
	notify(IloSetForAllRangeLB(this, val));
}

class IloSetForAllRangeLinearCoef : public IloChange {
	ILOCHANGEDECL

		IloForAllRangeI*  _rng;
	const IloNumVarI* _var;
	IloNum            _val;

public:
	IloSetForAllRangeLinearCoef (IloForAllRangeI* rng, IloNumVarI* var, IloNum val)
		: _rng(rng), _var(var), _val(val) {}

	const IloForAllRangeI*  getForAllRange () const   { return (_rng); }
	const IloNumVarI* getVar   () const   { return (_var); }
	IloNum            getValue () const   { return (_val); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE;
};

void IloForAllRangeI::setLinearCoef(const IloNumVar var, IloNum coef) {
	notify(IloSetForAllRangeLinearCoef(this, var.getImpl(), coef));
}

class IloSetForAllRangeLinearCoefs : public IloChange {
	ILOCHANGEDECL
	
	IloSetForAllRangeLinearCoefs &operator=(IloSetForAllRangeLinearCoefs const &); 

		const IloForAllRangeI* _rng;
	const IloNumVarArray   _vars;
	const IloNumArray      _vals;

public:
	IloSetForAllRangeLinearCoefs(const IloForAllRangeI* rng,
		const IloNumVarArray vars,
		const IloNumArray vals)
		: _rng(rng), _vars(vars), _vals(vals) {}

	const IloForAllRangeI* getForAllRange() const    { return (_rng); }
	const IloNumVarArray& getVars () const    { return (_vars); }
	const IloNumArray&    getVals () const    { return (_vals); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE;
};

void IloForAllRangeI::setLinearCoefs(const IloNumVarArray vars, const IloNumArray coefs) {
	notify(IloSetForAllRangeLinearCoefs(this, vars, coefs));
}

class IloForAllRange : public IloConstraint {
	ILOEXTRHANDLE(IloForAllRange,IloConstraint)
public:
	IloForAllRange(IloEnv      env,
		IloForAllI* forall,
		IloInt      position,
		char*       name = 0);

	
	IloForAllRange(const IloConstraint& ct);

	
	void setLinearCoef(const IloNumVar var, IloNum value) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		getImpl()->setLinearCoef(var, value);
	}

	
	void setLinearCoefs(const IloNumVarArray vars, const IloNumArray values) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		IloAssert(vars.getImpl(), "Using empty IloNumVarArray handle.");
		IloAssert(values.getImpl(), "Using empty IloNumArray handle.");
		getImpl()->setLinearCoefs(vars, values);
	}

	
	void setBounds(IloNum lb, IloNum ub) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		getImpl()->setBounds(lb, ub);
	}

	
	void setUB(IloNum ub) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		getImpl()->setUb(ub);
	}

	
	void setLB(IloNum lb) {
		IloAssert(getImpl(), "Using empty IloRange handle.");
		getImpl()->setLb(lb);
	}
}; 

typedef IloArray<IloForAllRange> IloForAllRangeArray;

class IloSetForAllRangeArrayChange : public IloChange {
  IloForAllRangeArray _forallRanges;

protected:
  IloSetForAllRangeArrayChange(IloForAllRangeArray forallRanges)
    : _forallRanges(forallRanges) {}

public:
  IloForAllRangeArray getForAllRanges() const { return _forallRanges; }
  virtual IloBool isApplyingOnArray() const ILO_OVERRIDE { return IloTrue; }
  virtual IloExtractableArray getChangedExtractableArray() const ILO_OVERRIDE {
    IloArrayI* impl = getForAllRanges().getImpl();
    return IloExtractableArray(impl);
  }
};

class IloSetForAllRangeArrayBounds : public IloSetForAllRangeArrayChange {
  ILOCHANGEDECL

  IloNumArray _lbs;
  IloNumArray _ubs;
public:
  IloSetForAllRangeArrayBounds(IloForAllRangeArray forallRanges, IloNumArray lbs, IloNumArray ubs)
    : IloSetForAllRangeArrayChange(forallRanges)
    , _lbs(lbs)
    , _ubs(ubs)
  {}
  const IloNumArray& getLbs () const     { return (_lbs); }
  const IloNumArray& getUbs () const     { return (_ubs); }
};

class IloSetForAllRangeArrayUBs : public IloSetForAllRangeArrayChange {
  ILOCHANGEDECL

  IloNumArray _ubs;
public:
  IloSetForAllRangeArrayUBs(IloForAllRangeArray forallRanges, IloNumArray ubs)
    : IloSetForAllRangeArrayChange(forallRanges)
    , _ubs(ubs)
  {}
  const IloNumArray& getUbs () const     { return (_ubs); }
      
};

class IloSetForAllRangeArrayLBs : public IloSetForAllRangeArrayChange {
  ILOCHANGEDECL

  IloNumArray _lbs;
public:
  IloSetForAllRangeArrayLBs(IloForAllRangeArray forallRanges, IloNumArray lbs)
    : IloSetForAllRangeArrayChange(forallRanges)
    , _lbs(lbs)
  {}
  const IloNumArray& getLbs () const     { return (_lbs); }
      
};

void IloOplSetForAllRangeArrayBounds(IloEnv, IloForAllRangeArray forallcts, IloNumArray lbs, IloNumArray ubs);
void IloOplSetForAllRangeArrayUBs   (IloEnv, IloForAllRangeArray forallcts, IloNumArray ubs);
void IloOplSetForAllRangeArrayLBs   (IloEnv, IloForAllRangeArray forallcts, IloNumArray lbs);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
