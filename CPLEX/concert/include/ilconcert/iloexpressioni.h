// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloexpressioni.h
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
#ifndef __CONCERT_iloexpressioniH
#define __CONCERT_iloexpressioniH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>

class IloDataIterator;

class IloGeneratorI : public IloExtractableI {
	ILOEXTRDECL
private:
	IloInt _exprUsed;
	IloExtractableList _filters;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	virtual IloBool hasNongroundType() const ILO_OVERRIDE;
public:
	IloGeneratorI(IloEnvI* env);
	virtual ~IloGeneratorI();
	virtual IloBool isOpen() const;
	virtual IloInt getIndexKey() const=0;
	const IloExtractableList& getFilters() const { return _filters; }
	IloGeneratorI* suchThat(IloConstraintI* cond);
	IloGeneratorI* suchThat(IloConstraint cond){ return suchThat(cond.getImpl()); }
	IloGeneratorI* suchThat(IloBool cond);
	void displayFilters(ILOSTD(ostream)& out) const;
	virtual IloBool generatesDuplicates() const=0;
	virtual IloDiscreteDataCollectionI* getDiscreteDataCollection() const = 0;
	virtual IloBool hasDiscreteDataCollection() const { return IloFalse; }
public:
	IloGeneratorI* lockExpr() {
		_exprUsed++;
		return this;
	}
	IloGeneratorI* unlockExpr() {
		_exprUsed--;
		return this;
	}
	virtual void end() ILO_OVERRIDE;
	void release();
	virtual IloInt getExprUsed() const ILO_OVERRIDE {
		return _exprUsed;
	}
	virtual IloBool isGenerator() ILO_OVERRIDE {
		return IloTrue;
	}
};

class ILO_EXPORTED IloGenerator {
private:
	IloGeneratorI* _impl;
public:
	IloGenerator():_impl(0) {}
	IloGenerator(IloGeneratorI* impl):_impl(impl) {}
	~IloGenerator() {}
	IloGeneratorI* getImpl() const { return _impl; }
	IloGenerator suchThat(IloConstraint ct) const {
		IloAssert(getImpl(), "Using empty IloGenerator handle.");
		IloAssert(ct.getImpl(), "Using empty IloConstraint handle.");
		return (getImpl()->suchThat(ct.getImpl()));
	}
	IloGenerator suchThat(IloBool ct) const {
		IloAssert(getImpl(), "Using empty IloGenerator handle.");
		return (getImpl()->suchThat(ct));
	}

	class Exception : public IloException {
	protected:
		const IloExtractableI* _context;
	public:
		Exception(const IloExtractableI* ctx);
		virtual ~Exception();
		IloExtractable getContext() const;
		void setContext(const IloExtractableI* e);
	};

	class MultipleDefinition : public Exception {
	private:
		MultipleDefinition &operator=(MultipleDefinition const &); 
		const IloInt _key;
	public:
		MultipleDefinition(const IloExtractableI* ctx,
			const IloInt key);
		virtual const char* getMessage() const ILO_OVERRIDE;
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;

	};

	class OutOfScope : public Exception {
	private:
		OutOfScope &operator=(OutOfScope const &); 
		const IloInt _key;
	public:
		OutOfScope(const IloExtractableI* ctx,
			const IloInt key);
		virtual const char* getMessage() const ILO_OVERRIDE;
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	};

	class InvalidFilter : public Exception {
	private:
		const IloExtractableI* _filterExp;
	public:
		InvalidFilter(const IloExtractableI* exp);
		virtual ~InvalidFilter();
		virtual const char* getMessage() const ILO_OVERRIDE;
		virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	};
};

class ILO_EXPORTED IloGeneratorArray : public IloArray<IloGenerator> {
public:
	typedef IloDefaultArrayI ImplClass;

	IloGeneratorArray(ImplClass* i=0) : IloArray<IloGenerator>(i) {}
	IloGeneratorArray(const IloGeneratorArray& copy) : IloArray<IloGenerator>(copy) {}
	IloGeneratorArray(const IloEnv env, IloInt n = 0) : IloArray<IloGenerator>(env, n) {}
	void unlockGenerators();
	void lockGenerators();
};

class IloComprehensionI : public IloExtractableI {
	ILOEXTRDECL
private:
	IloInt _exprUsed;
	IloInt _nGenerators;
	IloGeneratorI** _generators;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	virtual IloBool hasNongroundType() const ILO_OVERRIDE;
public:
	IloComprehensionI(IloEnvI* env, IloGeneratorArray generators);
	IloComprehensionI(IloEnvI* env, IloInt n, IloGeneratorI** generators);

	virtual ~IloComprehensionI();
	IloInt getNbGenerators() const { return _nGenerators; }
	IloGeneratorI** getGenerators() const { return _generators; }
	ILOEXTROTHERDECL
public:
	IloComprehensionI* lockExpr() { _exprUsed++; return this; }
	IloComprehensionI* unlockExpr() {_exprUsed--; return this; }
	virtual void end() ILO_OVERRIDE;
	void release();
	virtual IloInt getExprUsed() const ILO_OVERRIDE { return _exprUsed; }
	virtual IloBool isComprehension() ILO_OVERRIDE {
		return IloTrue;
	}
};

class IloForAllI : public IloConstraintI {
  ILOEXTRDECL
  friend class IloEvalForallI;
 private:
  IloComprehensionI* _comp;
  IloConstraintI*    _ct;
  IloConstraintI* _innerct;
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
  virtual IloBool hasNongroundType() const ILO_OVERRIDE;
#ifndef ILCFORALL
  virtual void whenAddedToCP(const IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE;
#endif 
 public:
  IloForAllI(IloEnvI* env,
             IloComprehensionI* comp,
             IloConstraintI* ct,
             const char* name=0);
  virtual ~IloForAllI();
  virtual IloInt expandTo(IloModelI* m, const char* propertyName=0, const char* propertyValue=0) const ILO_OVERRIDE;
  IloComprehensionI* getComprehension() const { return _comp; }
  IloInt getNbGenerators() const { return _comp->getNbGenerators(); }
  IloGeneratorI** getGenerators() const { return _comp->getGenerators(); }
  IloConstraintI* getConstraint() const { return _ct; }
  virtual IloConstraintI* getInnerConstraint() const ILO_OVERRIDE { return _innerct; }
  IloConstraintI* findConstraint(const char* propertyLabel, const char* propertyValue) const;
  IloConstraintI* focusOnConstraint(const char* propertyLabel, const char* propertyValue) const;
  ILOEXTROTHERDECL
};

class IloConditionalConstraintI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloConditionalConstraintI, IloConstraintI,
		IloIntExprI*, _conditionExpr,
		IloConstraintI*, _thenConstraint,
		IloConstraintI*, _elseConstraint)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	virtual ~IloConditionalConstraintI();
	IloIntExprArg getConditionExpr() const {return _conditionExpr;}
	IloConstraint getThenConstraint() const {return _thenConstraint;}
	IloConstraint getElseConstraint() const {return _elseConstraint;}
	IloConstraintI* findConstraint(const char* propertyLabel, const char* propertyValue) const;
	IloConstraintI* focusOnConstraint(const char* propertyLabel, const char* propertyValue) const;
};

IloConstraint IloConditionalConstraint(IloEnv env, IloIntExprArg expr, IloConstraint ct1 ,IloConstraint ct2, const char* name = 0);

class IloSetNumVarBounds : public IloChange {
	ILOCHANGEDECL

		IloNumVarI* _var;
	IloNum    _lb, _ub;

	IloSetNumVarBounds();
public:
	IloSetNumVarBounds(IloNumVarI* var, IloNum lb, IloNum ub)
		: _var(var), _lb(lb), _ub(ub)   {}

	IloNumVarI* getVar() const    { return (_var); }
	IloNum    getLb()  const    { return (_lb); }
	IloNum    getUb()  const    { return (_ub); }
};

class IloSetNumVarArrayBounds : public IloChange {
	ILOCHANGEDECL
	
	IloSetNumVarArrayBounds &operator=(IloSetNumVarArrayBounds const &); 

		const IloNumVarArray _var;
	const IloNumArray  _lb;
	const IloNumArray  _ub;

	IloSetNumVarArrayBounds (const IloNumVarArray var,
		const IloNumArray    lb,
		const IloNumArray    ub)
		: _var(var), _lb(lb), _ub(ub) {}

	friend class IloNumVarArray;

public:
	const IloNumVarArray& getVars() const { return (_var); }
	const IloNumArray&  getLbs()  const { return (_lb); }
	const IloNumArray&  getUbs()  const { return (_ub); }
};

class IloSemiContVarI : public IloNumVarI {
	ILOEXTRMEMBERS1DECL(IloSemiContVarI, IloNumVarI,
		IloNum,          _threshold)
public:
	IloSemiContVarI(IloEnvI*         env,
		IloNum           sclb,
		IloNum           ub,
		IloNumVar::Type  type = ILOFLOAT,
		const char*      name = 0)
		: IloNumVarI(env, type, 0.0, ub, name), _threshold(sclb) {}

	IloNum getBd() const                  { return (_threshold); }
	void   setBd(IloNum threshold);
};

class IloSetSemiContBound : public IloChange {
	ILOCHANGEDECL
		friend class IloSemiContVarI;

	IloSemiContVarI* _var;
	IloNum           _oldBd;
	IloNum           _newBd;

	IloSetSemiContBound(IloSemiContVarI* var, IloNum oldBd, IloNum newBd)
		: _var(var), _oldBd(oldBd), _newBd(newBd) {}

public:

	IloSemiContVarI* getVar() const   { return (_var); }
	IloNum           getOldBd() const { return (_oldBd); }
	IloNum           getNewBd() const { return (_newBd); }
};

class IloConstraintIndVarI : public IloNumVarI {
	ILOEXTRDECL
		IloConstraintI* _constraint;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloConstraintIndVarI(IloEnvI* env,
		IloConstraintI* constraint,
		const char* name)
		: IloNumVarI(env, ILOINT, 0, 1, name), _constraint(constraint) {}

	ILOEXTROTHERDECL

		IloConstraintI* getConstraint() const {return _constraint;}
};

class IloNumQuadExprTermI {
	IloNumVarI*          _var1;
	IloNumVarI*          _var2;
	IloNumQuadExprTermI* _next;
	IloNum               _coef;
public:
	IloNumQuadExprTermI(IloNumVarI* var1, IloNumVarI* var2, IloNum coef);
	~IloNumQuadExprTermI();
    
    
    IloInt getSize() const;
    IloBool hasSameVars(const IloNumQuadExprTermI* other) const;
	IloNum getCoef() const       { 
		return _coef; 
	}
	void   setCoef(IloNum coef)  { 
		_coef = coef; 
	}
	void   scaleBy(IloNum scale) { 
		_coef *= scale;
	}
	IloNumVarI* getVar1() const  { 
		return _var1; 
	}
	IloNumVarI* getVar2() const  { 
		return _var2; 
	}
	IloNumQuadExprTermI* getNext() const    { 
		return _next; 
	}
	void setNext(IloNumQuadExprTermI* next) { _next = next; }

	IloEnvI* getEnv() const      { return _var1->getEnv(); }
	IloBool isInteger() const;
	friend class IloQuadExprI;
	friend class IloObjectiveI;
	friend class IloEnvI;
};

class IloQuadExprI {
	IloNumQuadExprTermI *_first;
	IloNumQuadExprTermI *_last;
public:
	IloQuadExprI()                          : _first(0),    _last(0)    {}
	IloQuadExprI(IloNumQuadExprTermI* quad) : _first(quad), _last(quad) {}
	~IloQuadExprI();
	IloNumQuadExprTermI* getFirst() const  { 
		return _first; 
	}
	void append (IloNumQuadExprTermI* term);

	void scaleBy(IloNum scale);
	void normalize(IloEnvI *env);
	void removeMarked(IloEnvI *env);
	IloNum setQuadCoef(IloEnvI* env, IloNumVarI *var1, IloNumVarI *var2, IloNum coef);

	IloNum eval(const IloAlgorithm alg) const;
	void visitSubExtractables(IloNumExprI *quad, IloExtractableVisitor* v);
	void display(ILOSTD(ostream)&) const;

	friend class IloNumQuadExprI;
	friend class IloIntQuadExprI;
	friend class IloNumLinTermI;
	friend class IloIntLinTermI;
	IloQuadExprI makeClone(IloEnvI*) const;
	IloQuadExprI copy() const;
	void clear(){ 
		_first = 0;
		_last = 0;
	}
	void remove(IloNumVarI* var);
	void atRemove(IloNumVarI* var, IloNumExprI* quadExpr);
	IloBool isInteger() const;
};

class IloNumQuadExprI : public IloNumExprI {
   ILOEXTRDECL
   IloQuadExprI _quadX;
public:
   IloNumQuadExprI(IloEnvI* env);
   IloNumQuadExprI(IloEnvI* env, IloQuadExprI quad);
   IloNumQuadExprI(IloNumQuadExprTermI* quad);
   virtual ~IloNumQuadExprI();
   IloNumQuadExprTermI* getFirst() const   { return _quadX.getFirst(); }
   void append (IloNumQuadExprTermI* term) { _quadX.append(term); }
   void normalize()                        { _quadX.normalize(getEnv()); }
   void removeMarked()                     { _quadX.removeMarked(getEnv()); }
   void scaleBy(IloNum scale);
   IloNum setQuadCoef(IloNumVarI *var1, IloNumVarI *var2, IloNum coef) {
	   return _quadX.setQuadCoef (getEnv(), var1, var2, coef);
   }

   virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
   virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
   virtual void display(ILOSTD(ostream)&) const ILO_OVERRIDE;
   virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
   virtual IloNumExprI* copy() ILO_OVERRIDE;

   void clear();
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	void remove(IloNumVarI* var){
		_quadX.remove(var);
	   if (!getFirst()) getEnv()->release(this);
	}
};

class IloIntQuadExprI : public IloIntExprI {
   ILOEXTRDECL
   IloQuadExprI _quadX;
public:
   IloIntQuadExprI(IloEnvI* env);
   IloIntQuadExprI(IloEnvI* env, IloQuadExprI quad);
   IloIntQuadExprI(IloNumQuadExprTermI* quad);
   virtual ~IloIntQuadExprI();

   IloNumQuadExprTermI* getFirst() const   { return _quadX.getFirst(); }
   void append (IloNumQuadExprTermI* term) { _quadX.append(term); }
   void normalize()                        { _quadX.normalize(getEnv()); }
   void removeMarked()                     { _quadX.removeMarked(getEnv()); }
   void scaleBy(IloInt scale)              { _quadX.scaleBy ((IloNum)scale); }
   IloNum setQuadCoef(IloNumVarI *var1, IloNumVarI *var2, IloNum coef) {
      return _quadX.setQuadCoef (getEnv(), var1, var2, coef);
   }

   virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
   virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
   virtual void display(ILOSTD(ostream)&) const ILO_OVERRIDE;
   IloNumQuadExprI* toNumQuadExprI();
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	void clear();
	virtual IloNumExprI* copy() ILO_OVERRIDE;

protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	void remove(IloNumVarI* var){
		_quadX.remove(var);
	   if (!getFirst()) getEnv()->release(this);
	}
};

class IloNumQuadExprAddTerm : public IloChange {
   ILOCHANGEDECL

   IloNumExprI*  _quad;
   const IloNumVarI* _var1;
   const IloNumVarI* _var2;
   IloNum            _coef;

public:
   IloNumQuadExprAddTerm(IloNumExprI* quad, IloNumVarI* var1, IloNumVarI* var2, IloNum coef) : _quad(quad), _var1(var1), _var2(var2), _coef(coef)    {}

   IloBool isInt() const;
   const IloNumExprI* getExpr() const          { return _quad;    }
   const IloNumVarI*     getVar1() const          { return (_var1); }
   const IloNumVarI*     getVar2() const          { return (_var2); }
   IloNum                getCoef() const          { return (_coef); }
};

class IloNumQuadExprRemoveTerm : public IloChange {
   ILOCHANGEDECL

   IloNumExprI*   _quad;
   const IloNumVarI* _var1;
   const IloNumVarI* _var2;
   IloNum            _coef;

public:
   IloNumQuadExprRemoveTerm(IloNumExprI* quad, IloNumVarI* var1, IloNumVarI* var2, IloNum coef) : _quad(quad), _var1(var1), _var2(var2), _coef(coef)    {}

   IloBool isInt() const;
   const IloNumExprI* getExpr() const          { return (_quad); }
   const IloNumVarI*     getVar1() const          { return (_var1); }
   const IloNumVarI*     getVar2() const          { return (_var2); }
   IloNum                getCoef() const          { return (_coef); }
};

class IloNumQuadExprChangeTerm : public IloChange {
   ILOCHANGEDECL

   IloNumExprI*   _quad;
   const IloNumVarI* _var1;
   const IloNumVarI* _var2;
   IloNum            _oldcoef;
   IloNum            _newcoef;

public:
   IloNumQuadExprChangeTerm(IloNumExprI* quad, IloNumVarI* var1, IloNumVarI* var2, IloNum oldcoef, IloNum newcoef) : _quad(quad), _var1(var1), _var2(var2), _oldcoef(oldcoef), _newcoef(newcoef)    {}

   IloBool isInt() const;
   const IloNumExprI* getExpr() const          { return (_quad); }
   const IloNumVarI*     getVar1() const          { return (_var1); }
   const IloNumVarI*     getVar2() const          { return (_var2); }
   IloNum                getOldCoef() const       { return (_oldcoef); }
   IloNum                getNewCoef() const       { return (_newcoef); }
};

class IloSetNumVarPossibleValues : public IloChange {
	ILOCHANGEDECL

		IloNumVarI* _var;
	IloNumArray _floatValues;
	IloIntArray _intValues;

	IloSetNumVarPossibleValues();
public:
	IloSetNumVarPossibleValues(IloNumVarI* var,
		const IloNumArray values)
		: _var(var), _floatValues(values), _intValues(0)   {}
	IloSetNumVarPossibleValues(IloNumVarI* var,
		const IloIntArray values)
		: _var(var), _floatValues(0), _intValues(values)   {}

	IloNumVarI* getVar() const     { return (_var); }
	IloNumArray  getFloatValues()  const { return (_floatValues); }
	IloIntArray  getIntValues()  const { return (_intValues); }
};

class IloNumLinExprTermI {
	IloNumVarI* _var;
	IloNumLinExprTermI* _next;
	IloNum _coef;
public:
	IloNumLinExprTermI(IloNumVarI* var, IloNum coef);
	IloNum getCoef() const { return _coef; }
	void setCoef(IloNum coef)  { _coef = coef; }
	IloNumVarI* getVar() const { return _var; }
	IloNumLinExprTermI* getNext() const { return _next; }
	void setNext(IloNumLinExprTermI* next) { _next = next; }
	IloEnvI* getEnv() const { return _var->getEnv(); }
	friend class IloNumLinTermI;
	friend class IloIntLinTermI;
	friend class IloRangeI;
	friend class IloEnvI;
};

class IloAnySetI;
class IloNumLinTermI : public IloNumExprI {
public:
	class ExprList {
		IloNumExprI* _expr;
		ExprList* _next;
	public:
		ExprList(IloNumExprI* expr, ExprList* next) : _expr(expr), _next(next) {}
		IloNumExprI* getExpr() const { return _expr; }
		void setExpr(IloNumExprI* e) { _expr = e; }
		void setNext(ExprList* next) { _next = next; }
		ExprList* getNext() const { return _next; }
		friend class IloIntLinTermI;
		friend class IloNumLinTermI;
	};
private:

	ILOEXTRDECL
		IloIntArray _quads;
        IloNumLinExprTermI* _firstLinear;
	IloNumLinExprTermI* _lastLinear;
	IloNum _cst;
	IloNumQuadExprI* _quad; 
	ExprList* _firstGeneral;
	ExprList* _lastGeneral;
	IloBool   _isNormalized;
	virtual IloNumExprI* copy() ILO_OVERRIDE;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	void removeQExpr();
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloNumLinTermI(const IloNumLinExprTerm term);
	IloNumLinTermI(const IloIntLinExprTerm term);
	IloNumLinTermI(IloEnvI* env, IloNum cst);
	virtual ~IloNumLinTermI();
	IloNumQuadExprTermI* getFirstQuad() const { 
		return _quad ?  _quad->getFirst() : 0; 
	}
	IloNumQuadExprI* getQuad() const { 
		return _quad; 
	}

	IloNumLinExprTermI* getFirstLinear() const { return _firstLinear; }
	IloNumLinExprTermI* getLastLinear() const { return _lastLinear; }
	ExprList* getFirstGeneral() const { return _firstGeneral; }
	ExprList* getLastGeneral() const { return _lastGeneral; }
	void append(IloNumLinExprTermI* ap) {
		if (_lastLinear) {
			_lastLinear->_next = ap;
			_lastLinear = ap;
		}
		else {
			_firstLinear = ap;
			_lastLinear = ap;
		}
	}
	void append(IloNumQuadExprTermI*);
	void prepend(IloNumLinExprTermI*);
	void append(IloNumExprI*);
	void prepend(IloNumExprI*);
	void append(IloNumLinTermI*);
	
	void setNumConstant(IloNum constant) { _cst = constant; }
	IloNum getNumConstant() const ILO_OVERRIDE { return _cst; }
	ExprList* getGeneral() const { return _firstGeneral; }
	virtual IloBool displayTerm(ILOSTD(ostream)&, IloNumLinExprTermI*) const;
	virtual void display(ILOSTD(ostream)&) const ILO_OVERRIDE;
	IloNumLinTermI* multiply(IloNum constant);
	IloNumLinTermI* divide(IloNum constant);
	friend class IloIntLinTermI;
	friend class IloExpr;
	friend class IloDefaultDeleterExtensionI;
	IloNum setCoef(const IloNumVar var, const IloNum val);
	void setCoef(const IloNumVarArray vars, const IloNumArray vals,IloNumArray status = 0);
	IloNum setQuadCoef(const IloNumVar var1, const IloNumVar var2, const IloNum val);
	void remove(IloNumVarI* var);
	void remove(const IloExtractableArray vars);
	void removeMarked();
	IloInt normalize();
	virtual IloNumLinTermI* toNumLinTermI() ILO_OVERRIDE;
	virtual IloBool isInteger() const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	void clear();

	
	
	
	
	IloBool containsAggregate() const;
	void oplNormalization();
	void setNormalized(IloBool flag);
	void replaceQExpr(IloNumExprI* oldq, IloNumQuadExprI* newq);
	virtual IloBool isDecisionExpr() const ILO_OVERRIDE;
	virtual IloBool isGround() const ILO_OVERRIDE;
};

class IloNumLinTermRemoveVar : public IloChange {
	ILOCHANGEDECL

		IloNumLinTermI*        _rng;
	const IloNumVarI* _var;

public:
	IloNumLinTermRemoveVar(IloNumLinTermI* rng, IloNumVarI* var)
		: _rng(rng), _var(var)    {}

	const IloNumLinTermI*  getNumLinTerm() const   { return (_rng); }
	const IloNumVarI* getVar   () const   { return (_var); }
};

class IloNumLinTermAddVar : public IloChange {
	ILOCHANGEDECL

		IloNumLinTermI*        _rng;
	const IloNumVarI* _var;

public:
	IloNumLinTermAddVar(IloNumLinTermI* rng, IloNumVarI* var)
		: _rng(rng), _var(var)    {}

	const IloNumLinTermI*  getNumLinTerm() const   { return (_rng); }
	const IloNumVarI* getVar   () const   { return (_var); }
};

class IloIntLinTermI : public IloIntExprI {
public:
	class ExprList {
		IloIntExprI* _expr;
		ExprList* _next;
	public:
		ExprList(IloIntExprI* expr, ExprList* next) : _expr(expr), _next(next) {}
		void setExpr(IloIntExprI* e) { _expr = e; }
		IloIntExprI* getExpr() const { return _expr; }
		void setNext(ExprList* next) { _next = next; }
		ExprList* getNext() const { return _next; }
		friend class IloIntLinTermI;
	};
private:
	ILOEXTRDECL
		IloIntArray _quads;
		IloNumLinExprTermI* _firstLinear;
	IloNumLinExprTermI* _lastLinear;
	IloInt _cst;
	IloIntQuadExprI* _quad; 
	ExprList* _firstGeneral;
	ExprList* _lastGeneral;
	virtual IloIntExprI* intCopy() ILO_OVERRIDE;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	void removeQExpr();
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloIntLinTermI(const IloIntLinExprTerm term);
	IloIntLinTermI(IloEnvI* env, IloInt cst);
	virtual ~IloIntLinTermI();
	IloNumQuadExprTermI* getFirstQuad() const { 
		return _quad ?  _quad->getFirst() : 0; 
	}
	IloIntQuadExprI* getQuad() const { 
		return _quad; 
	}
	IloNumLinExprTermI* getFirstLinear() const { return _firstLinear; }
	IloNumLinExprTermI* getLastLinear() const { return _lastLinear; }
	ExprList* getFirstGeneral() const { return _firstGeneral; }
	ExprList* getLastGeneral() const { return _lastGeneral; }
	void append(IloNumLinExprTermI*);
	void prepend(IloNumLinExprTermI*);
	void append(IloNumQuadExprTermI*);
	void append(IloIntExprI*);
	void prepend(IloIntExprI*);
	void append(IloIntLinTermI*);
	
	virtual void display(ILOSTD(ostream)&) const ILO_OVERRIDE;
	virtual IloBool displayTerm(ILOSTD(ostream)&, IloNumLinExprTermI*) const;
	IloNum getNumConstant() const ILO_OVERRIDE;
	void setIntConstant(IloInt constant) { _cst = constant; }
	IloInt getIntConstant() const { return _cst; }
	virtual IloNumLinTermI* toNumLinTermI() ILO_OVERRIDE;
	virtual IloIntLinTermI* toIntLinTermI() ILO_OVERRIDE;
	IloIntLinTermI* multiply(IloInt constant);
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	void remove(const IloExtractableArray vars);
	void remove(IloIntVarI* var);
	void removeMarked();
	IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	friend class IloDefaultDeleterExtensionI;
	IloNum setQuadCoef(const IloNumVar var1, const IloNumVar var2, const IloNum val);
	void normalize(IloEnvI *env);
	void clear();
	virtual IloBool isDecisionExpr() const ILO_OVERRIDE;
	virtual IloBool isGround() const ILO_OVERRIDE;
};

class IloIntLinTermRemoveVar : public IloChange {
	ILOCHANGEDECL

		IloIntLinTermI*        _rng;
	const IloIntVarI* _var;

public:
	IloIntLinTermRemoveVar(IloIntLinTermI* rng, IloIntVarI* var)
		: _rng(rng), _var(var)    {}

	const IloIntLinTermI*  getIntLinTerm() const   { return (_rng); }
	const IloIntVarI* getVar   () const   { return (_var); }
};

class IloIntModuloI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _left;
	IloIntExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntModuloI(IloEnvI* env, IloIntExprI* left, IloIntExprI* right) :
	  IloIntExprI(env), _left(left->intLockExpr()),
		  _right(right->intLockExpr()) {}
	  virtual ~IloIntModuloI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getLeftExpr() const { return _left; }
	  IloIntExprI* getRightExpr() const { return _right; }
};

class IloIntModuloCstI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	IloInt _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntModuloCstI(IloEnvI* env, IloIntExprI* arg, IloInt cst) :
	  IloIntExprI(env), _arg(arg->intLockExpr()), _cst(cst) {}
	  virtual ~IloIntModuloCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
	  IloInt getIntCst() const { return _cst; }
};

class IloIntRecModuloCstI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	IloInt _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntRecModuloCstI(IloEnvI* env, IloInt cst, IloIntExprI* arg) :
	  IloIntExprI(env), _arg(arg->intLockExpr()), _cst(cst) {}
	  virtual ~IloIntRecModuloCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
	  IloInt getIntCst() const { return _cst; }
};

class IloNumTimesCstI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	IloNum _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumTimesCstI(IloEnvI* env, IloNumExprI* arg, IloNum cst) :
	  IloNumExprI(env), _arg(arg->lockExpr()), _cst(cst) {}
	  virtual ~IloNumTimesCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  IloNum getNumCst() const { return _cst; }
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumTimesI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _left;
	IloNumExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumTimesI(IloEnvI* env, IloNumExprI* left, IloNumExprI* right) :
	  IloNumExprI(env), _left(left->lockExpr()), _right(right->lockExpr()) {}
	  virtual ~IloNumTimesI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  IloNumExprI* getLeftExpr() const { return _left; }
	  IloNumExprI* getRightExpr() const { return _right; }
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumDivI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _left;
	IloNumExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumDivI(IloEnvI* env, IloNumExprI* left, IloNumExprI* right) :
	  IloNumExprI(env), _left(left->lockExpr()), _right(right->lockExpr()) {}
	  virtual ~IloNumDivI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
	  IloNumExprI* getLeftExpr() const { return _left; }
	  IloNumExprI* getRightExpr() const { return _right; }
};

class IloNumDivCstI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	IloNum _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumDivCstI(IloEnvI* env, IloNumExprI* arg, IloNum cst) :
	  IloNumExprI(env), _arg(arg->lockExpr()), _cst(cst) {}
	  virtual ~IloNumDivCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  IloNum getNumCst() const { return _cst; }
};

class IloNumRecDivCstI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	IloNum _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumRecDivCstI(IloEnvI* env, IloNumExprI* arg, IloNum cst) :
	  IloNumExprI(env), _arg(arg->lockExpr()), _cst(cst) {}
	  virtual ~IloNumRecDivCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  IloNum getNumCst() const { return _cst; }
};

class IloIntTimesCstI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	IloInt _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntTimesCstI(IloEnvI* env, IloIntExprI* arg, IloInt cst) :
	  IloIntExprI(env), _arg(arg->intLockExpr()), _cst(cst) {}
	  virtual ~IloIntTimesCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloInt getIntCst() const { return _cst; }
};

class IloIntTimesI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _left;
	IloIntExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntTimesI(IloEnvI* env, IloIntExprI* left, IloIntExprI* right) :
	  IloIntExprI(env), _left(left->intLockExpr()),
		  _right(right->intLockExpr()) {}
	  virtual ~IloIntTimesI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  IloIntExprI* getLeftExpr() const { return _left; }
	  IloIntExprI* getRightExpr() const { return _right; }
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloIntDivI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _left;
	IloIntExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntDivI(IloEnvI* env, IloIntExprI* left, IloIntExprI* right) :
	  IloIntExprI(env), _left(left->intLockExpr()),
		  _right(right->intLockExpr()) {}
	  virtual ~IloIntDivI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getLeftExpr() const { return _left; }
	  IloIntExprI* getRightExpr() const { return _right; }
};

class IloIntDivCstI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	IloInt _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntDivCstI(IloEnvI* env, IloIntExprI* arg, IloInt cst) :
	  IloIntExprI(env), _arg(arg->intLockExpr()), _cst(cst) {}
	  virtual ~IloIntDivCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
	  IloInt getIntCst() const { return _cst; }
};

class IloIntRecDivCstI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	IloInt _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntRecDivCstI(IloEnvI* env, IloIntExprI* arg, IloInt cst) :
	  IloIntExprI(env), _arg(arg->intLockExpr()), _cst(cst) {}
	  virtual ~IloIntRecDivCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
	  IloInt getIntCst() const { return _cst; }
};

class IloNumExprArrayElementI : public IloNumExprI {
	ILOEXTRDECL
private:
	IloIntExprI*    _index;
	IloNumExprArray _array;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloNumExprArrayElementI(IloEnvI* env,
		IloIntExprI* idx, IloNumExprArray array);
	virtual ~IloNumExprArrayElementI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	IloNumExprArray getExprArray() const { return _array; }
	IloIntExprI* getIndex() const { return _index; }
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloIntExprArrayElementI : public IloIntExprI {
	ILOEXTRDECL
private:
	IloIntExprI*    _index;
	IloIntExprArray _array;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloIntExprArrayElementI(IloEnvI* env, IloIntExprI* idx,
		IloIntExprArray array);
	virtual ~IloIntExprArrayElementI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	IloIntExprArray getExprArray() const { return _array; }
	IloIntExprI* getIndex() const { return _index; }
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloIntArrayElementI : public IloIntExprI {
	ILOEXTRDECL
private:
	IloIntExprI* _index;
	IloIntArray  _array;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntArrayElementI(IloEnvI* env, IloIntExprI* index, IloIntArray array):
	  IloIntExprI(env), _index(index->intLockExpr()), _array(array) {}
	  virtual ~IloIntArrayElementI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  IloIntArray getArray() const { return _array; }
	  IloIntExprI* getIndex() const { return _index; }
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloNumArrayElementI : public IloNumExprI {
	ILOEXTRDECL
private:
	IloIntExprI* _index;
	IloNumArray  _array;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumArrayElementI(IloEnvI* env, IloIntExprI* index, IloNumArray array):
	  IloNumExprI(env), _index(index->intLockExpr()), _array(array) {}
	  virtual ~IloNumArrayElementI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  IloNumArray getArray() const { return _array; }
	  IloIntExprI* getIndex() const { return _index; }
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloSinI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloSinI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloSinI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};

class IloCosI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloCosI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloCosI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};

class IloTanI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloTanI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloTanI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};

class IloArcSinI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloArcSinI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloArcSinI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};

class IloArcCosI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloArcCosI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloArcCosI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};
class IloArcTanI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloArcTanI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloArcTanI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};
class IloExponentI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloExponentI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloExponentI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};
class IloLogI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloLogI(IloEnvI* env, IloNumExprI* arg) : IloNumExprI(env),
		_arg(arg->lockExpr()) {}
	virtual ~IloLogI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	IloNumExprI* getExpr() const { return _arg; }
};

class IloIntSquareI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntSquareI(IloEnvI* env, IloIntExprI* arg) :
	  IloIntExprI(env), _arg(arg->intLockExpr()) {}
	  virtual ~IloIntSquareI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
};

class IloNumSquareI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumSquareI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumSquareI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloIntAbsI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntAbsI(IloEnvI* env, IloIntExprI* arg) :
	  IloIntExprI(env), _arg(arg->intLockExpr()) {}
	  virtual ~IloIntAbsI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
};

class IloNumAbsI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumAbsI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumAbsI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloSgnI : public IloIntExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloSgnI(IloEnvI* env, IloNumExprI* arg) :
	  IloIntExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloSgnI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
};

class IloNumMinCstI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	IloNum _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumMinCstI(IloEnvI* env, IloNumExprI* arg, IloNum cst) :
	  IloNumExprI(env), _arg(arg->lockExpr()), _cst(cst) {}
	  virtual ~IloNumMinCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  IloNum getNumCst() const { return _cst; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumMinI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _left;
	IloNumExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumMinI(IloEnvI* env, IloNumExprI* left, IloNumExprI* right) :
	  IloNumExprI(env), _left(left->lockExpr()), _right(right->lockExpr()) {}
	  virtual ~IloNumMinI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getLeftExpr() const { return _left; }
	  IloNumExprI* getRightExpr() const { return _right; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloIntMinCstI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	IloInt _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntMinCstI(IloEnvI* env, IloIntExprI* arg, IloInt cst) :
	  IloIntExprI(env), _arg(arg->intLockExpr()), _cst(cst) {}
	  virtual ~IloIntMinCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
	  IloInt getIntCst() const { return _cst; }
};

class IloIntMinI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _left;
	IloIntExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntMinI(IloEnvI* env, IloIntExprI* left, IloIntExprI* right) :
	  IloIntExprI(env), _left(left->intLockExpr()),
		  _right(right->intLockExpr()) {}
	  virtual ~IloIntMinI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getLeftExpr() const { return _left; }
	  IloIntExprI* getRightExpr() const { return _right; }
};

class IloNumMinArrayI  : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprArray _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloNumMinArrayI(IloEnvI* env, IloNumExprArray arg);
	virtual ~IloNumMinArrayI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	IloNumExprArray getExprArray() const { return _arg; }
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloIntMinArrayI  : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprArray _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloIntMinArrayI(IloEnvI* env, IloIntExprArray arg);
	virtual ~IloIntMinArrayI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	IloIntExprArray getExprArray() const { return _arg; }
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloNumMaxCstI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	IloNum _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumMaxCstI(IloEnvI* env, IloNumExprI* arg, IloNum cst) :
	  IloNumExprI(env), _arg(arg->lockExpr()), _cst(cst) {}
	  virtual ~IloNumMaxCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  IloNum getNumCst() const { return _cst; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumMaxI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _left;
	IloNumExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumMaxI(IloEnvI* env, IloNumExprI* left, IloNumExprI* right) :
	  IloNumExprI(env), _left(left->lockExpr()), _right(right->lockExpr()) {}
	  virtual ~IloNumMaxI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getLeftExpr() const { return _left; }
	  IloNumExprI* getRightExpr() const { return _right; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloIntMaxCstI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _arg;
	IloInt _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntMaxCstI(IloEnvI* env, IloIntExprI* arg, IloInt cst) :
	  IloIntExprI(env), _arg(arg->intLockExpr()), _cst(cst) {}
	  virtual ~IloIntMaxCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getExpr() const { return _arg; }
	  IloInt getIntCst() const { return _cst; }
};

class IloIntMaxI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprI* _left;
	IloIntExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntMaxI(IloEnvI* env, IloIntExprI* left, IloIntExprI* right) :
	  IloIntExprI(env), _left(left->intLockExpr()),
		  _right(right->intLockExpr()) {}
	  virtual ~IloIntMaxI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloIntExprI* getLeftExpr() const { return _left; }
	  IloIntExprI* getRightExpr() const { return _right; }
};

class IloNumMaxArrayI  : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprArray _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloNumMaxArrayI(IloEnvI* env, IloNumExprArray arg);
	virtual ~IloNumMaxArrayI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	IloNumExprArray getExprArray() const { return _arg; }
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloIntMaxArrayI  : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprArray _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloIntMaxArrayI(IloEnvI* env, IloIntExprArray arg);
	virtual ~IloIntMaxArrayI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	IloIntExprArray getExprArray() const { return _arg; }
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloNumPowerI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _left;
	IloNumExprI* _right;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumPowerI(IloEnvI* env, IloNumExprI* left, IloNumExprI* right) :
	  IloNumExprI(env), _left(left->lockExpr()), _right(right->lockExpr()) {}
	  virtual ~IloNumPowerI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
	  IloNumExprI* getLeftExpr() const { return _left; }
	  IloNumExprI* getRightExpr() const { return _right; }
};

class IloNumPowerCstI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	IloNum _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumPowerCstI(IloEnvI* env, IloNumExprI* arg, IloNum cst) :
	  IloNumExprI(env), _arg(arg->lockExpr()), _cst(cst) {}
	  virtual ~IloNumPowerCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  IloNum getNumCst() const { return _cst; }
};

class IloNumRecPowerCstI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	IloNum _cst;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumRecPowerCstI(IloEnvI* env, IloNumExprI* arg, IloNum cst) :
	  IloNumExprI(env), _arg(arg->lockExpr()), _cst(cst) {}
	  virtual ~IloNumRecPowerCstI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isInteger() const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  IloNum getNumCst() const { return _cst; }
};

class IloExprPiecewiseLinearI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _term;
	IloBool      _useSlope;
	IloNumArray  _point;
	IloNumArray  _slope;
	IloNum       _a;
	IloNum       _b;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloExprPiecewiseLinearI(IloEnvI* env,
		IloBool useSlope,
		IloNumExprI* x,
		const IloNumArray point,
		const IloNumArray slope,
		IloNum a,
		IloNum b);
	IloExprPiecewiseLinearI(IloEnvI* env,
		IloNumExprI* x,
		const IloNumArray point,
		const IloNumArray slope,
		IloNum a,
		IloNum fa);
	IloExprPiecewiseLinearI(IloEnvI* env,
		IloNumExprI* x,
		IloNum firstSlope,
		const IloNumArray point,
		const IloNumArray value,
		IloNum lastSlope);

	virtual ~IloExprPiecewiseLinearI();
	

	
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

	IloNum getPiecewiseLinearValue(IloNum) const;

	IloBool useSlope() const { return _useSlope; }
	IloNumArray getPoint() const { return _point; }
	IloNumArray getSlope() const { return _slope; }
	IloNum getA() const { return _a; }
	IloNum getB() const { return _b; }
	IloNumExprI* getTerm() const { return _term; }
};

class IloMonotonicNumExprI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _term;
	IloNumFunction _f;
	IloNumFunction _invf;
	IloBool _isDecreasing; 
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloMonotonicNumExprI(IloEnvI* env,
		IloNumExprI* term,
		IloNumFunction f,
		IloNumFunction invf,
		IloBool isDecreasing)
		:IloNumExprI(env),
		_term(term->lockExpr()),
		_f(f),
		_invf(invf),
		_isDecreasing(isDecreasing){}
	virtual ~IloMonotonicNumExprI();

	IloBool isDecreasing() const {
		return _isDecreasing;
	}
	IloNumFunction getF() const {
		return _f;
	}
	IloNumFunction getInvF() const {
		return _invf;
	}
	IloNumExprI* getTerm() const {
		return _term;
	}
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloNumCeilI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumCeilI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumCeilI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumFloorI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumFloorI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumFloorI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumTruncI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumTruncI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumTruncI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloDistToIntI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloDistToIntI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloDistToIntI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumToIntI : public IloIntExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumToIntI(IloEnvI* env, IloNumExprI* arg) :
	  IloIntExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumToIntI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
};

class IloNumFractI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumFractI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumFractI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloNumRoundI : public IloNumExprI {
	ILOEXTRDECL
		IloNumExprI* _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloNumRoundI(IloEnvI* env, IloNumExprI* arg) :
	  IloNumExprI(env), _arg(arg->lockExpr()) {}
	  virtual ~IloNumRoundI();
	  void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  IloNumExprI* getExpr() const { return _arg; }
	  virtual IloBool isInteger() const ILO_OVERRIDE;
};

class IloCountI : public IloIntExprI {
	ILOEXTRDECL
		IloIntExprArray _arg;
	IloInt          _value;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloCountI(IloEnvI* env, IloIntExprArray exprs, IloInt value);

	virtual ~IloCountI();

	IloIntExprArray getExprArray() const { return _arg; }
	IloInt          getValue() const     { return _value; }

	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloCountDifferentI : public IloIntExprI {
	ILOEXTRDECL
        IloIntExprArray _arg;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloCountDifferentI(IloEnvI* env, IloIntExprArray exprs);

	virtual ~IloCountDifferentI();

	IloIntExprArray getExprArray() const { return _arg; }

	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloStandardDeviationI : public IloNumExprI {
	ILOEXTRDECL
		IloIntExprArray _arg;
	IloNum          _meanLB;
	IloNum          _meanUB;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloStandardDeviationI(IloEnvI* env,
		IloIntExprArray exprs,
		IloNum meanLB,
		IloNum meanUB);
	virtual ~IloStandardDeviationI();
	IloIntExprArray getExprArray() const { return _arg; }
	IloNum getMeanLB() const { return _meanLB; }
	IloNum getMeanUB() const { return _meanUB; }
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	static IloNum Eval(const IloAlgorithm alg, const IloIntExprArray arg);
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

#include <ilconcert/ilsched/ilosbasic.h>
#include <ilconcert/ilonumfunci.h>

class IloAdvPiecewiseFunctionExprI: public IloExtractableI {
	ILOS_CPEXTR_DECL(IloAdvPiecewiseFunctionExprI, IloExtractableI)
protected:
	IloAdvPiecewiseFunctionExprI(IloEnvI* env, const char* name = 0);
	virtual ~IloAdvPiecewiseFunctionExprI();
public:
	IloNumExprI* apply(IloNumExprI* x);
	virtual IloBool isStepwise() const { return IloFalse; }
};

class IloIntervalVarI;

class IloIntervalSequenceExprI: public IloExtractableI {
	ILOS_CPEXTR_DECL(IloIntervalSequenceExprI, IloExtractableI)
protected:
	IloIntervalSequenceExprI(IloEnvI* env, const char* name = 0);
	virtual ~IloIntervalSequenceExprI();
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE = 0;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE = 0;
};
#define IloIntervalSequenceVarI_SUPER IloIntervalSequenceExprI

class IloIntervalSequenceVarI: public IloIntervalSequenceVarI_SUPER {
	ILOS_CPEXTR_DECL(IloIntervalSequenceVarI, IloIntervalSequenceVarI_SUPER)
        void init(IloExtractableArray, IloIntArray);
public:
	IloIntervalSequenceVarI(IloEnvI* env, IloExtractableArray a, IloIntArray types, const char* name);
	IloIntervalSequenceVarI(IloEnvI* env, IloExtractableArray a, IloIntArray types, IloBool isOPL, const char* name);
	virtual ~IloIntervalSequenceVarI();
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	void remove(IloIntervalVarI* interval);
	IloInt getSize() const { return _intervals.getSize(); }
	IloIntervalVarI* getInterval(IloInt i) const;
	IloInt getPosition(IloIntervalVarI* ti) const;
	IloBool hasTypes() const { return _types.getImpl()!=0; }
	IloInt getType(IloIntervalVarI*) const;
	IloInt getType(IloInt i) const { return _types[i]; }
	IloIntArray getTypes() const { return _types; }
  IloExtractableArray getIntervals() const { return _intervals; }
  void setOPL()    { _status |= ILOS_BITSET_MASK(0); }
  void setHidden() { _status |= ILOS_BITSET_MASK(1);	}
	IloBool isOPL()     const { return IloBitTest(_status, 0); }
	IloBool isHidden()  const { return IloBitTest(_status, 1); }
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;

private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

private:
	IloExtractableArray _intervals;
	IloIntArray         _types;
  IloUInt             _status;
};

class IloCumulAtomI;
class IloAdvCumulAtomI;
class IloAggregateCumulExprI;
class IloCumulFunctionExprSubMapExprI;
class IloConditionalCumulFunctionExprI;
class IloCumulFunctionExprI : public IloExtractableI { 
	ILOS_CPEXTR_DECL(IloCumulFunctionExprI, IloExtractableI)

public:
	IloCumulFunctionExprI(IloEnvI* env, const char* name = 0);
	virtual ~IloCumulFunctionExprI();
	virtual IloBool isAtomic() const = 0;
	virtual IloBool isEmptyAtom() const;
public: 
	struct AtomVisitorContext {
		IloInt _sign;
		AtomVisitorContext() { _sign = +1; }
	};
public:
	class AtomVisitor {
	public:
		AtomVisitor();
		virtual ~AtomVisitor();
		virtual void visit(IloCumulAtomI * atom, IloInt sgn) = 0;
		virtual void visit(IloAdvCumulAtomI* atom, IloInt sgn) = 0;
		virtual void visit(IloAggregateCumulExprI* atom, IloInt sgn) = 0;
		virtual void visit(IloCumulFunctionExprSubMapExprI* atom, IloInt sgn) = 0;
		virtual void visit(IloConditionalCumulFunctionExprI* atom, IloInt sgn)= 0;
	};
	virtual void visitAtoms(AtomVisitor * visitor, AtomVisitorContext * ctx) = 0;
	void visit(class IloCumulFunctionExprI::AtomVisitor * visitor) {
		AtomVisitorContext ctx;
		visitAtoms(visitor, &ctx);
	}
};

class IloStateFunctionExprI: public IloExtractableI {
	ILOS_CPEXTR_DECL(IloStateFunctionExprI, IloExtractableI)
protected:
	IloStateFunctionExprI(IloEnvI* env, const char* name = 0);
	virtual ~IloStateFunctionExprI();
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE = 0;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE = 0;
};

class IloIntervalExprI: public IloExtractableI {
	ILOS_CPEXTR_DECL(IloIntervalExprI, IloExtractableI)
protected:
	IloIntervalExprI(IloEnvI* env, const char* name=0);
	virtual ~IloIntervalExprI();
public:
	virtual IloBool isVar() const;
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE = 0;
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE = 0;
};

class IloIntervalVarExprI: public IloIntExprI {
	ILOS_CPEXTR_DECL(IloIntervalVarExprI, IloIntExprI)
public:
	enum Type {
		Start = 0,
		End,
		Length,
		Size,
		NumberOfTypes  
	};

private:
	union DVal {
		IloInt       _num;
		IloIntExprI* _expr;
		DVal(IloInt num): _num(num) {};
		DVal(IloIntExprI* expr): _expr(expr) {}
	};
	IloIntervalExprI* _interval;
	IloUInt          _type; 
	DVal             _nonExecVal;

	friend class IloIntervalExprI;
	friend class IloIntervalVarI;

public:
	IloIntervalVarExprI(IloEnvI*         env,
		IloIntervalExprI* interval,
		Type             type,
		IloInt           nonExecVal);
	IloIntervalVarExprI(IloEnvI*         env,
		IloIntervalExprI* interval,
		Type             type,
		IloIntExprArg    nonExecVal);

	virtual ~IloIntervalVarExprI();

	virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
	static IloEvalCP _evalCP;

	IloIntervalExprI* getInterval() const { return _interval; }
	Type getType() const { return (Type)(_type & 31L); }
	IloBool hasNonExecValueConst() const { return !hasNonExecValueExpr(); }
	IloBool hasNonExecValueExpr() const { return IloBitTest(_type, 6); }
	IloInt getNonExecValue() const {
		IloAssert(hasNonExecValueConst(), "getNonExecValue: nonExecValue is an expression.");
		return _nonExecVal._num;
	}
	IloIntExprI* getNonExecValueExpr() const {
		IloAssert(hasNonExecValueExpr(), "getNonExecValueExpr: nonExecValue is a number.");
		return _nonExecVal._expr;
	}

public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloIntervalVarI: public IloIntervalExprI {
	ILOS_CPEXTR_DECL(IloIntervalVarI, IloIntervalExprI)

public:
	enum Bound1Index {
		IndexStartMin = 0,
		IndexEndMax,
		IndexSizeMin,
		IndexSizeMax,
		NumBound1Indexes 
	};
	enum Bound2Index {
		IndexStartMax =0,
		IndexEndMin,
		IndexLengthMin,
		IndexLengthMax,
		NumBound2Indexes 
	};

	class IntensityExtensionI {
	public:
		IntensityExtensionI()
			:_intensityParamLink(0),
			_granularity(100),
			_oplPWIntensity(0) {}
		~IntensityExtensionI(){}
		IloParameterExtractableLinkI* _intensityParamLink;
		IloUInt                       _granularity;
		const IloAdvPiecewiseFunctionExprI* _oplPWIntensity;
	};
private:
	friend class IloSavedIntervalI;
	enum StatusBits {
		BitPresent = 0,
		BitAbsent  = 1
	};
	IloUInt              _status;
	IloIntArray          _bounds1;
	IloIntArray          _bounds2;
	IloExtractableArray  _expressions;
	IntensityExtensionI* _intensityExt;

public:
	IloIntervalVarI(IloEnvI* env);
	virtual ~IloIntervalVarI();
	virtual IloBool isVar() const ILO_OVERRIDE;

	
	void setPresent() {
		notify(IloSchedChange(!isAbsent() ?
			IloSchedChange::IloMonotonicChange :
		IloSchedChange::IloDestructiveChange));
		_status = ILOS_BITSET_MASK(BitPresent);
	}
	void setAbsent() {
		notify(IloSchedChange(!isPresent() ?
			IloSchedChange::IloMonotonicChange :
		IloSchedChange::IloDestructiveChange));
		_status = ILOS_BITSET_MASK(BitAbsent);
	}
	void setOptional() {
		notify(IloSchedChange());
		_status = 0;
	}
	IloBool isPresent() const { return IloBitTest(_status, BitPresent); }
	IloBool isAbsent()  const { return IloBitTest(_status, BitAbsent); }

	
	void setStartMin (IloInt startMin)  { setBoundMin(IndexStartMin, startMin); }
	void setStartMax (IloInt startMax)  { setBoundMax(IndexStartMax, startMax); }
	void setEndMin   (IloInt endMin)    { setBoundMin(IndexEndMin, endMin); }
	void setEndMax   (IloInt endMax)    { setBoundMax(IndexEndMax, endMax); }
	void setLengthMin(IloInt lengthMin) { setBoundMin(IndexLengthMin, lengthMin); }
	void setLengthMax(IloInt lengthMax) { setBoundMax(IndexLengthMax, lengthMax); }
	void setSizeMin  (IloInt sizeMin)   { setBoundMin(IndexSizeMin, sizeMin); }
	void setSizeMax  (IloInt sizeMax)   { setBoundMax(IndexSizeMax, sizeMax); }
	IloInt getStartMin()  const { return getBoundMin(IndexStartMin); }
	IloInt getStartMax()  const { return getBoundMax(IndexStartMax); }
	IloInt getEndMin()    const { return getBoundMin(IndexEndMin); }
	IloInt getEndMax()    const { return getBoundMax(IndexEndMax); }
	IloInt getLengthMin() const { return getLengthMin(IndexLengthMin); }
	IloInt getLengthMax() const { return getLengthMax(IndexLengthMax); }
	IloInt getSizeMin()   const { return getLengthMin(IndexSizeMin); }
	IloInt getSizeMax()   const { return getLengthMax(IndexSizeMax); }

	
	IntensityExtensionI* getOrMakeIntensityExtension();
	void setIntensity(IloSegmentedFunctionI* intensity, IloUInt granularity);
	IloSegmentedFunctionI* getIntensity() const;
	IloUInt getGranularity() const;
	void setIntensity(const IloAdvPiecewiseFunctionExprI* intensity, IloUInt granularity);
	IloBool hasOplPWIntensity() const;
	const IloAdvPiecewiseFunctionExprI* getOplPWIntensity() const;

	
	IloIntExprArgI* makeStartOf(IloInt dval)  { return (IloIntExprArgI*)getExpr(IloIntervalVarExprI::Start, dval); }
	IloIntExprArgI* makeEndOf(IloInt dval)    { return (IloIntExprArgI*)getExpr(IloIntervalVarExprI::End, dval); }
	IloIntExprArgI* makeLengthOf(IloInt dval) { return (IloIntExprArgI*)getExpr(IloIntervalVarExprI::Length, dval); }
	IloIntExprArgI* makeSizeOf(IloInt dval)   { return (IloIntExprArgI*)getExpr(IloIntervalVarExprI::Size, dval); }
	IloConstraintI* getPresenceConstraint()   { return (IloConstraintI*)getExpr(IloIntervalVarExprI::NumberOfTypes); }
	void cleanExpr(IloIntervalVarExprI::Type type);
	
	friend class IloModelEvaluatorI;
	IloIntervalVarExprI* getUnsafeStartOf() const {
		return getUnsafeExpr(IloIntervalVarExprI::Start); }
	IloIntervalVarExprI* getUnsafeEndOf() const {
		return getUnsafeExpr(IloIntervalVarExprI::End); }
	IloIntervalVarExprI* getUnsafeLengthExpr() const {
		return getUnsafeExpr(IloIntervalVarExprI::Length); }
	IloIntervalVarExprI* getUnsafeSizeExpr() const {
		return getUnsafeExpr(IloIntervalVarExprI::Size); }

private:
	void makeBounds1();
	void makeBounds2();
	void setBoundMin(Bound1Index index, IloInt value);
	void setBoundMin(Bound2Index index, IloInt value);
	void setBoundMax(Bound1Index index, IloInt value);
	void setBoundMax(Bound2Index index, IloInt value);
	IloInt getBoundMin (Bound1Index index) const;
	IloInt getBoundMin (Bound2Index index) const;
	IloInt getBoundMax (Bound1Index index) const;
	IloInt getBoundMax (Bound2Index index) const;
	IloInt getLengthMin(Bound1Index index) const;
	IloInt getLengthMin(Bound2Index index) const;
	IloInt getLengthMax(Bound1Index index) const;
	IloInt getLengthMax(Bound2Index index) const;

	friend class IloIntervalVarExprI;
	IloExtractableI* getExpr(IloIntervalVarExprI::Type type, IloInt dval =0);
	IloIntervalVarExprI* getUnsafeExpr(IloIntervalVarExprI::Type type) const;

public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual void changeParameter(const IloParameterExtractableLinkI* param) ILO_OVERRIDE;
	virtual void endParameter(const IloParameterExtractableLinkI* link) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

	
public:
	static IloIntervalVarI* Make(IloEnvI* env,
		IloInt ndmin,
		IloInt ndmax,
		const char* name,
		const IloAdvPiecewiseFunctionExprI* eff,
		IloInt granularity,
		IloBool opt);
	
	
	IloBool isObsoleteInconsistent()  const;
	IloBool isObsoletePresent()       const;
	IloInt getObsoleteStart()         const;
	IloInt getObsoleteEnd()           const;
	IloInt getObsoleteLength()        const;
	IloInt getObsoleteSize()          const;
	
	IloInt getObsoleteEndInternal()   const;
};

class IloAdvPiecewiseFunctionI: public IloAdvPiecewiseFunctionExprI {
	ILOS_CPEXTR_DECL(IloAdvPiecewiseFunctionI,IloAdvPiecewiseFunctionExprI )
private:
	IloSegmentedFunctionI* _segFunction;
	IloBool _isStepwise;
	
	IloAdvPiecewiseFunctionI(IloEnvI* env, IloBool isStepwise,
		IloSegmentedFunctionI* segFunction);
public:
	IloAdvPiecewiseFunctionI(IloEnvI* env,
		const IloNumArray x,
		const IloNumArray y,
		const char* name = 0);
	IloAdvPiecewiseFunctionI(IloEnvI* env,
		const IloNumArray x,
		const IloNumArray s,
		IloNum x0,
		IloNum y0,
		const char* name = 0);
	virtual ~IloAdvPiecewiseFunctionI();
	IloSegmentedFunctionI* getSegmentedFunction() const {
		return _segFunction;
	}
	virtual IloBool isStepwise() const ILO_OVERRIDE { return _isStepwise; }
	void		clearSegmentedFunction() {
		_segFunction = 0;
	}
	void getSlopesAndBreaks( IloNumArray& slopes, IloNumArray& breaks ) const;
public:
	ILOEXTROTHERDECL
};

class IloAdvExprPiecewiseLinearI : public IloNumExprI {
  ILOEXTRDECL
  IloNumExprI*                  _term;
  IloSegmentedFunctionI*        _func;
  IloAdvPiecewiseFunctionExprI* _pwf;
public:
  IloAdvExprPiecewiseLinearI(IloEnvI* env, IloNumExprI* term, IloSegmentedFunctionI* f);
  IloAdvExprPiecewiseLinearI(IloEnvI* env, IloNumExprI* term, IloAdvPiecewiseFunctionExprI* pwf);
  virtual ~IloAdvExprPiecewiseLinearI();
  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
  IloNumExprI* getTerm() const { return _term; }
  IloSegmentedFunctionI* getFunction() const { return _func; }
  IloBool hasOplPWFunction() const { return (0 != _pwf); }
  IloAdvPiecewiseFunctionExprI* getOplPWFunction() const { return _pwf; };
  static IloEvalCP _evalCP;

  
 private:
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
 protected:
  virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
 public:
  virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
