// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilsched/ilostimei.h
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

#ifndef __CONCERT_ilostimeiH
#define __CONCERT_ilostimeiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/iloexpression.h>
#include <ilconcert/iloexpressioni.h>
#include <ilconcert/iloparami.h>
#include <ilconcert/ilonumfunci.h>
#include <ilconcert/ilsched/ilosbasic.h>

#define ILO_RTC_36064

class IloIntervalExprI;
class IloIntervalVarI;
class IloIntervalVar;

class IloSchedGlobalExtractableI: public IloExtractableI {
	ILOS_CPEXTR_DECL(IloSchedGlobalExtractableI, IloExtractableI)
public:
	IloSchedGlobalExtractableI(IloEnvI* env);
	~IloSchedGlobalExtractableI(){}
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloAdvPiecewiseFunctionExprI;

typedef IloArray<IloIntervalVarExprI*> IloIntervalVarExprIArray;

class IloIntervalVarEvalI: public IloNumExprI {
	ILOS_CPEXTR_DECL(IloIntervalVarEvalI, IloNumExprI)
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
		IloNum       _num;
		IloNumExprI* _expr;
		DVal(IloNum num): _num(num) {};
		DVal(IloNumExprI* expr): _expr(expr) {}
	};
	IloIntervalExprI*       _interval;
	IloSegmentedFunctionI* _func;
	IloUInt                _type; 
	DVal                   _nonExecVal;
	IloAdvPiecewiseFunctionExprI* _pwf;

public:
	IloIntervalVarEvalI(IloEnvI* env,
		IloIntervalExprI* interval,
		Type type,
		IloSegmentedFunctionI* f,
		IloNum nonExecVal);

	IloIntervalVarEvalI(IloEnvI* env,
		IloIntervalExprI* interval,
		Type type,
		IloAdvPiecewiseFunctionExprI* f,
		IloNumExprArg nonExecVal);
	virtual ~IloIntervalVarEvalI();

	virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
	static IloEvalCP _evalCP;

	IloIntervalExprI* getInterval() const { return _interval; }
	IloSegmentedFunctionI* getFunction() const { return _func; }
	Type getType() const { return (Type)(_type & 31L); }
	IloBool hasNonExecValueConst() const { return !hasNonExecValueExpr(); }
	IloBool hasNonExecValueExpr() const { return IloBitTest(_type, 6); }
	IloNum getNonExecValue() const {
		IloAssert(hasNonExecValueConst(), "getNonExecValue: nonExecValue is an expression.");
		return _nonExecVal._num;
	}
	IloNumExprI* getNonExecValueExpr() const {
		IloAssert(hasNonExecValueExpr(), "getNonExecValueExpr: nonExecValue is a number.");
		return _nonExecVal._expr;
	}
	IloBool hasOplPWFunction() const { return (0 != _pwf); }
	IloAdvPiecewiseFunctionExprI* getOplPWFunction() const { return _pwf; };

public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

typedef IloArray<IloIntervalVarEvalI*> IloIntervalVarEvalIArray;

class IloExecuteI: public IloConstraintI {
	ILOS_CPEXTR_DECL(IloExecuteI, IloConstraintI)
private:
	IloIntervalExprI* _ti;
public:
	IloExecuteI(IloEnvI* env, IloIntervalExprI* ti);
	virtual ~IloExecuteI() {}
	IloIntervalExprI* getInterval() const { return _ti; }
public:
#ifdef ILO_RTC_36064
        virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
	static IloEvalCP _evalCP;
#endif  
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloForbidTimesI: public IloConstraintI {
	ILOS_CPEXTR_DECL(IloForbidTimesI, IloConstraint)

public:
	enum Type {
		ForbidStart,
		ForbidEnd,
		ForbidExtent
	};

private:
	IloIntervalVarI*		_ti;
	Type				_type;
	IloParameterExtractableLinkI* _funcParamLink;

public:
	IloForbidTimesI(IloEnvI* env, IloIntervalVarI* ti, Type type, IloSegmentedFunctionI* func);
	virtual ~IloForbidTimesI() {}
	IloIntervalVarI* getInterval() const { return _ti; }
	Type getType() const { return _type; }
	IloSegmentedFunctionI* getFunction() const {
		return _funcParamLink ? (IloSegmentedFunctionI*)_funcParamLink->getParameter() : NULL;
	}

public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual void changeParameter(const IloParameterExtractableLinkI* param) ILO_OVERRIDE;
	virtual void endParameter(const IloParameterExtractableLinkI* link) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloPrecedenceI: public IloConstraintI {
	ILOS_CPEXTR_DECL(IloPrecedenceI, IloConstraintI)
private:
	union Delay {
		IloInt          _num;
		IloIntExprArgI* _expr;
		Delay(IloInt num): _num(num) {};
		Delay(IloIntExprArgI* expr): _expr(expr) {}
	};

	IloUInt           _type;
	IloIntervalExprI* _source;
	IloIntervalExprI* _target;
	Delay             _delay;

public:
	IloPrecedenceI(IloEnvI*                 env,
		IloIntervalExprI*         source,
		IloSched::PrecedenceType type,
		IloIntervalExprI*         target,
		IloInt                   delay);
	IloPrecedenceI(IloEnvI*                 env,
		IloIntervalExprI*         source,
		IloSched::PrecedenceType type,
		IloIntervalExprI*         target,
		IloIntExprArgI*          delay);
	virtual ~IloPrecedenceI();

	IloBool hasDelayExpr() const { return !IloBitTest(_type, IloSched::BitDelayNumExpr); }
	IloInt getDelay() const {
		IloAssert(!hasDelayExpr(), "getDelayNumber: delay is an expression.");
		return _delay._num;
	}
	IloIntExprArgI* getDelayExpr() const {
		IloAssert(hasDelayExpr(), "getDelayExpr: delay is a number.");
		return _delay._expr;
	}
	void setDelay(IloInt delay);
	void setDelay(IloIntExprArgI* expr);
	IloIntervalExprI* getTarget() const { return _target; }
	IloIntervalExprI* getSource() const { return _source; }
	IloUInt getType() const { return _type; }

private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloStatusRelationI: public IloConstraintI {
	ILOS_CPEXTR_DECL(IloStatusRelationI, IloConstraintI)

private:
	IloSched::StatusRelationType _type;
	IloIntervalExprI* _source;
	IloIntervalExprI* _target;

public:
	IloStatusRelationI(IloEnvI* env,
		IloIntervalExprI* source,
		IloSched::StatusRelationType type,
		IloIntervalExprI* target)
		:IloConstraintI(env, NULL), _type(type), _source(source), _target(target) {}
	virtual ~IloStatusRelationI() {}
	IloIntervalExprI* getSource() const { return _source; }
	IloIntervalExprI* getTarget() const { return _target; }
	IloSched::StatusRelationType getType()   const { return _type; }

private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloIntervalVarIArrayIterator {
	IloInt              _pos;
	IloExtractableArray _array;
public:
	IloIntervalVarIArrayIterator(IloExtractableArray array):
	  _pos(0), _array(array) {}
	  IloBool ok() const { return _pos < _array.getSize(); }
	  IloIntervalVarI* operator*() {
		  assert(ok());
		  return (IloIntervalVarI*)(_array[_pos].getImpl());
	  }
	  IloIntervalVarIArrayIterator& operator++() {
		  _pos++;
		  return *this;
	  }
};

class IloSpanI: public IloConstraintI {
	ILOS_CPEXTR_DECL(IloSpanI, IloConstraintI)

private:
	IloIntervalVarI* _master;
	IloExtractableArray _slaves;

public:
	IloSpanI(IloEnvI* env, IloIntervalVarI* master, const char* name)
		:IloConstraintI(env, name)
		,_master(master)
		,_slaves(env)
	{}
	IloSpanI(IloEnvI* env, IloIntervalVarI* master, IloExtractableArray bs, const char* name);
	IloSpanI(IloEnvI* env, IloIntervalVarI* master, IloAny bs, const char* name);
	virtual ~IloSpanI();
	IloIntervalVarI* getSpanningInterval() const { return _master; }
	IloExtractableArray getSlaves() const { return _slaves; }
	IloBool contains(const IloIntervalVarI* interval);
	void add(IloIntervalVarI* interval);
	void remove(IloIntervalVarI* interval);
	class IntervalIterator: public IloIntervalVarIArrayIterator {
	public:
		IntervalIterator(const IloSpanI* span):
		  IloIntervalVarIArrayIterator(span->getSlaves()) {}
	};

public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloSynchronizeI: public IloConstraintI {
	ILOS_CPEXTR_DECL(IloSynchronizeI, IloConstraintI)

private:
	IloIntervalVarI* _master;
	IloExtractableArray _slaves;

public:
	IloSynchronizeI(IloEnvI* env, IloIntervalVarI* master, const char* name)
		:IloConstraintI(env, name)
		,_master(master)
		,_slaves(env)
	{}
	IloSynchronizeI(IloEnvI* env, IloIntervalVarI* master, IloExtractableArray bs, const char* name);
	IloSynchronizeI(IloEnvI* env, IloIntervalVarI* master, IloAny bs, const char* name);
	virtual ~IloSynchronizeI();
	IloIntervalVarI* getSynchronizationInterval() const { return _master; }
	IloExtractableArray getSlaves() const { return _slaves; }
	IloBool contains(const IloIntervalVarI* interval);
	void add(IloIntervalVarI* interval);
	void remove(IloIntervalVarI* interval);
	class IntervalIterator: public IloIntervalVarIArrayIterator {
	public:
		IntervalIterator(const IloSynchronizeI* span):
		  IloIntervalVarIArrayIterator(span->getSlaves()) {}
	};

public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloAlternativeI: public IloConstraintI {
	ILOS_CPEXTR_DECL(IloAlternativeI, IloConstraintI)

private:
	IloIntervalVarI*    _master;
	IloExtractableArray _slaves;
	IloInt              _cardinality;
	IloIntExprArgI*     _cardinalityExpr;
public:
	IloAlternativeI(IloEnvI* env, IloIntervalVarI* master, const char* name)
		:IloConstraintI  (env, name)
		,_master         (master)
		,_slaves         (env)
		,_cardinality    (1)
		,_cardinalityExpr(0)
	{}
	IloAlternativeI(IloEnvI* env, IloIntervalVarI* master, IloAny bs, const char* name);
	IloAlternativeI(IloEnvI* env, IloIntervalVarI* master, IloExtractableArray bs, const char* name);
	IloAlternativeI(IloEnvI* env, IloIntervalVarI* master, IloExtractableArray bs, IloInt card, const char* name);
	IloAlternativeI(IloEnvI* env, IloIntervalVarI* master, IloExtractableArray bs, IloIntExprArgI* card, const char* name);
	virtual ~IloAlternativeI();
	IloIntervalVarI* getAlternativeInterval() const { return _master; }
	IloExtractableArray getSlaves() const { return _slaves; }
	IloBool contains(const IloIntervalVarI* interval);
	void add(IloIntervalVarI* interval);
	void remove(IloIntervalVarI* interval);
	IloBool hasCardinalityExpr() const { return (0!=_cardinalityExpr); }
	IloInt getCardinality() const { return _cardinality; }
	IloIntExprArgI* getCardinalityExpr() const { return _cardinalityExpr; }
	class IntervalIterator: public IloIntervalVarIArrayIterator {
	public:
		IntervalIterator(const IloAlternativeI* alt):
		  IloIntervalVarIArrayIterator(alt->getSlaves()) {}
	};

public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloIntervalMapConstraintI : public IloConstraintI {
  ILOS_CPEXTR_DECL(IloIntervalMapConstraintI, IloConstraintI)
private:
  IloExtractableArray _domain;
  IloExtractableArray _intervals;
  IloIntExprArray     _map;
  IloInt              _absVal;
  IloBool             _isomorphism;
  IloIntervalMapConstraintI(IloEnvI* env, IloBool isomorphism, IloInt absVal, const char* name);
public:
  IloIntervalMapConstraintI(IloEnvI* env, IloBool isomorphism,
		  const IloExtractableArray domain, 
		  const IloExtractableArray intervals, 
		  const IloIntExprArray map, 
		  IloInt absVal, 
		  const char* name);
  virtual ~IloIntervalMapConstraintI();
  IloExtractableArray getDomain()    const { return _domain; }
  IloExtractableArray getIntervals() const { return _intervals;}
  IloIntExprArray getMap()           const { return _map;}
  IloInt getAbsVal()                 const { return _absVal;}
  IloBool isIsomorphism()            const { return _isomorphism;}
public:
  virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloTransitionDistanceI : public IloParameterI {
public:
	IloTransitionDistanceI(IloEnvI *env, IloInt size, const char* name = 0);
	IloTransitionDistanceI(IloEnvI *env, IloIntArray2 ttable, const char* name = 0);
	IloInt  getSize() const { return _size; }
	IloBool checkTriangularInequality() const;

	void setValue(IloInt fromState, IloInt toState, IloInt value) {
		assert(fromState < _size);
		assert(toState < _size);
		_ttable[fromState][toState] = value;
		notifyChange();
	}

	IloInt getValue(IloInt fromState, IloInt toState) const {
		assert(fromState < _size);
		assert(toState < _size);
		return _ttable[fromState][toState];
	}

	virtual IloParameterI* makeCopy() const ILO_OVERRIDE;  
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

private:
	IloInt _size;
	IloInt **_ttable;
	void allocMem();  
   virtual IloBool isDecisionExpr() const {return IloFalse;}
};

class IloOverlapExprI: public IloIntExprI {
	ILOS_CPEXTR_DECL(IloOverlapExprI, IloIntExprI)
protected:
	IloIntervalExprI* _interval;
	IloIntExprI* _defaultValueExpr;
	IloInt _defaultValue;

public:
	IloOverlapExprI(IloEnvI*          env,
		IloIntervalExprI* interval,
		IloInt            defaultValue);
	IloOverlapExprI(IloEnvI*          env,
		IloIntervalExprI* interval,
		IloIntExprI*      defaultValue);
	virtual ~IloOverlapExprI();

	IloBool hasExprDefaultValue() const { return (0 != _defaultValueExpr); }
	IloInt getDefaultValue() const {
		IloAssert(!hasExprDefaultValue(),
			"getDefaultValue: defaultValue is an expression.");
		return _defaultValue;
	}
	IloIntExprI* getDefaultValueExpr() const {
		IloAssert(hasExprDefaultValue(),
			"getDefaultValueExpr: defaultValue is a number.");
		return _defaultValueExpr;
	}
	IloIntervalExprI* getInterval() const { return _interval; }
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloOverlapVarExprI: public IloOverlapExprI {
	ILOS_CPEXTR_DECL(IloOverlapVarExprI, IloOverlapExprI)
private:
	IloIntervalExprI* _interval2;

public:
	IloOverlapVarExprI(IloEnvI*          env,
		IloIntervalExprI* interval1,
		IloIntervalExprI* interval2,
		IloInt            defaultValue);
	IloOverlapVarExprI(IloEnvI*          env,
		IloIntervalExprI* interval1,
		IloIntervalExprI* interval2,
		IloIntExprI*      defaultValue);
	virtual ~IloOverlapVarExprI();
	virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
	static IloEvalCP _evalCP;

	IloIntervalExprI* getFirstInterval() const { return getInterval(); }
	IloIntervalExprI* getSecondInterval() const { return _interval2; }

private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE; 
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloOverlapConstExprI: public IloOverlapExprI {
	ILOS_CPEXTR_DECL(IloOverlapConstExprI, IloOverlapExprI)
private:
	IloIntExprI* _startExpr;
	IloIntExprI* _endExpr;
	IloInt _start;
	IloInt _end;

public:
	IloOverlapConstExprI(IloEnvI*          env,
		IloIntervalExprI* interval,
		IloInt            start,
		IloInt            end,
		IloInt            defaultValue);
	IloOverlapConstExprI(IloEnvI*          env,
		IloIntervalExprI* interval,
		IloIntExprI*      start,
		IloIntExprI*      end,
		IloIntExprI*      defaultValue);
	virtual ~IloOverlapConstExprI();
	virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
	static IloEvalCP _evalCP;

	IloBool hasExprStart() const { return (0 != _startExpr); }
	IloBool hasExprEnd() const { return (0 != _endExpr); }
	IloInt getStart() const {
		IloAssert(!hasExprStart(), "getStartNumber: start is an expression.");
		return _start;
	}
	IloIntExprArgI* getStartExpr() const {
		IloAssert(hasExprStart(), "getStartExpr: start is a number.");
		return _startExpr;
	}
	IloInt getEnd() const {
		IloAssert(!hasExprEnd(), "getEndNumber: end is an expression.");
		return _end;
	}
	IloIntExprArgI* getEndExpr() const {
		IloAssert(hasExprEnd(), "getEndExpr: end is a number.");
		return _endExpr;
	}

private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE; 
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

