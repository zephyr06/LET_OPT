// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilsched/ilosatomi.h
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
#ifndef __CONCERT_ilosatomiH
#define __CONCERT_ilosatomiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>
#include <ilconcert/ilsched/ilosbasic.h>
#include <ilconcert/iloparami.h>
#include <ilconcert/iloevaluator.h>

class IloIntervalVarI;
class IloCumulAtomI;

class IloIntervalRangeI {
	friend class IloCumulAtomI;
	friend class IloAlwaysInI;
	friend class IloStateAlwaysI;
public:
	IloIntervalRangeI(IloIntervalVarI* ti, IloInt start, IloInt end);
	IloBool isConstant()            const { return IloBitTest(_status, 0); }
	IloBool isVariable()            const { return !IloBitTest(_status, 0); }
	IloBool isCondition()           const { return IloBitTest(_status, 1); }
	IloBool isChange()              const { return !IloBitTest(_status, 1); }
	IloBool isCapacity()            const { return IloBitTest(_status, 12); }
	IloBool isContinuous()          const { return IloBitTest(_status, 11); }
	IloBool isPulse()               const { return IloBitTest(_status, 10); }
	IloBool isStepFromStart()       const { return IloBitTest(_status, 8); }
	IloBool isStepFromEnd()         const { return IloBitTest(_status, 7); }
	IloBool isState()               const { return IloBitTest(_status, 24); }
	IloBool isStartAligned()        const { return IloBitTest(_status, 25);}
	IloBool isEndAligned()          const { return IloBitTest(_status, 26);}
	IloBool isAligned()             const { return IloBitTest(_status, 27);}
	IloBool isStateRange()          const { return IloBitTest(_status, 28);}
	IloBool isNoState()             const { return IloBitTest(_status, 29);}
	IloBool isLowerThanBuiltMax()   const { return IloBitTest(_status, 20);}
	IloBool isGreaterThanBuiltMax() const { return IloBitTest(_status, 21);}
	IloBool isLowerThanBuiltMin()   const { return IloBitTest(_status, 22);}
	IloBool isGreaterThanBuiltMin() const { return IloBitTest(_status, 23);}
	IloIntervalVarI* getInterval()  const { return (IloIntervalVarI*) _tiOrStart;}
	IloInt getStart() const { return (IloInt) _tiOrStart;}
	IloInt getEnd()   const { return _end;}
	IloInt getMin()   const { return _min;}
	IloInt getMax()   const { return _max;}
	void display(ILOSTD(ostream)& out) const;
protected:
	void setInterval(const IloIntervalVarI* const ti);
	void setInterval(IloInt start, IloInt end);
	void setCapacity(IloInt min, IloInt max); 
	void setCapacity(IloInt min, IloInt max, IloBool step, IloBool fromStart); 
	void setPossibleState(IloInt min, IloInt max);
	void setAligned(IloBool start, IloBool end);
	void setNoState();
private:
	IloAny  _tiOrStart;
	IloInt  _end;
	IloInt  _min;
	IloInt  _max;
	IloUInt _status;
public:
	IloUInt getStatus() const { return _status;}
private:
	void setVariable()             { IloBitUnset(_status, 0); }
	void setConstant()             { IloBitSet(_status, 0); }
	void setLowerThanBuiltMax()    { IloBitSet(_status, 20);}
	void setGreaterThanBuiltMax()  { IloBitSet(_status, 21);}
	void setLowerThanBuiltMin()    { IloBitSet(_status, 22);}
	void setGreaterThanBuiltMin()  { IloBitSet(_status, 23);}
};

class IloCumulAtomI : public IloCumulFunctionExprI { 
	ILOS_CPEXTR_DECL(IloCumulAtomI, IloCumulFunctionExprI)
        void visitAtoms(AtomVisitor * visitor, AtomVisitorContext * ctx) ILO_OVERRIDE;
	virtual IloBool isEmptyAtom() const ILO_OVERRIDE;
private:
	IloIntervalRangeI _range;
	
	IloCumulAtomI(IloEnvI*          env,
		IloIntervalVarI*  ti,
		IloInt t1, IloInt t2, 
		IloInt            min,
		IloInt            max,
		IloUInt           status,
		const char*       name);
public:
	IloCumulAtomI(IloEnvI* env,
		IloIntervalVarI*  ti, 
		IloInt            cmin,
		IloInt            cmax,
		IloBool           step,
		IloBool           fromstart,
		const char*       name);
	IloCumulAtomI(IloEnvI*          env,
		IloInt t1, IloInt t2, 
		IloInt            value,
		IloBool           step,
		IloBool           fromstart,
		const char*       name);
	IloIntervalRangeI& getRange() { return _range;}
	IloBool isConstant() const { return _range.isConstant(); }
	IloBool isVariable() const { return _range.isVariable(); }
	IloIntervalVarI* getInterval() const { return _range.getInterval();}
	IloInt getStart() const { return _range.getStart();}
	IloInt getEnd() const { return _range.getEnd();}
	IloInt getMin() const { return _range.getMin();}
	IloInt getMax() const { return _range.getMax();}
	IloBool isCapacity() const  { return _range.isCapacity(); }
	IloBool isContinuous() const { return _range.isContinuous(); }
	IloBool isPulse() const { return _range.isPulse(); }
	IloBool isStepFromStart()  const { return _range.isStepFromStart(); }
	IloBool isStepFromEnd() const { return _range.isStepFromEnd(); }
	virtual IloBool isAtomic() const ILO_OVERRIDE { return IloTrue; }
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloAlwaysInI : public IloConstraintI {
	ILOS_CPEXTR_DECL(IloAlwaysInI, IloConstraintI)
protected:
	IloAlwaysInI(IloEnvI* env,
		IloIntervalVarI* ti,
		IloInt t1, IloInt t2, 
		IloCumulFunctionExprI* func,
		IloInt min,
		IloInt max,
		IloUInt status,
		const char* name);
public:
	IloAlwaysInI(IloEnvI* env,
		IloIntervalVarI* ti, 
		IloCumulFunctionExprI* func,
		IloInt cmin, IloInt cmax,
		const char* name);
	IloAlwaysInI(IloEnvI* env,
		IloInt t1, IloInt t2, 
		IloCumulFunctionExprI* func,
		IloInt cmin, IloInt cmax,
		const char* name);

	IloBool isLowerThanBuiltMax()   const { return _range.isLowerThanBuiltMax();}
	IloBool isGreaterThanBuiltMax() const { return _range.isGreaterThanBuiltMax();}
	IloBool isLowerThanBuiltMin()   const { return _range.isLowerThanBuiltMin();}
	IloBool isGreaterThanBuiltMin() const { return _range.isGreaterThanBuiltMin();}
	void setLowerThanBuiltMax()    { _range.setLowerThanBuiltMax(); }
	void setGreaterThanBuiltMax()  { _range.setGreaterThanBuiltMax(); }
	void setLowerThanBuiltMin()    { _range.setLowerThanBuiltMin(); }
	void setGreaterThanBuiltMin()  { _range.setGreaterThanBuiltMin(); }
	IloIntervalRangeI& getRange() { return _range;}
	IloCumulFunctionExprI* getFunction() const { return _func;}
	IloIntervalVarI* getInterval() const { return _range.getInterval();}
	IloInt getStart() const { return _range.getStart();}
	IloInt getEnd() const { return _range.getEnd();}
	IloBool isConstant() const { return _range.isConstant(); }
	IloBool isVariable() const { return _range.isVariable(); }
	IloInt getMin() const { return _range.getMin();}
	IloInt getMax() const { return _range.getMax();}
	IloBool isCapacity() const  { return _range.isCapacity(); }
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;

private:
	IloIntervalRangeI _range;
	IloCumulFunctionExprI* _func;
	void displayRange(ILOSTD(ostream)& out) const;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloAddCumulFunctionsI : public IloCumulFunctionExprI {
	ILOS_CPEXTR_DECL(IloAddCumulFunctionsI, IloCumulFunctionExprI)
private:
	IloCumulFunctionExprI * _f1;
	IloCumulFunctionExprI * _f2;
public:
	IloAddCumulFunctionsI(IloCumulFunctionExprI * f1,
		IloCumulFunctionExprI * f2);
	IloCumulFunctionExprI* getLeftArg() const { return _f1;}
	IloCumulFunctionExprI* getRightArg() const { return _f2;}
	void visitAtoms(AtomVisitor * visitor, AtomVisitorContext * ctx) ILO_OVERRIDE;
	IloExtractableI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
public:
	virtual IloBool isAtomic() const ILO_OVERRIDE { return IloFalse; }
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloNegateCumulFunctionI : public IloCumulFunctionExprI {
	ILOS_CPEXTR_DECL(IloNegateCumulFunctionI, IloCumulFunctionExprI)
private:
	IloCumulFunctionExprI * _f;
public:
	IloNegateCumulFunctionI(IloCumulFunctionExprI * f);
	IloCumulFunctionExprI* getArg() const { return _f;}
	void visitAtoms(AtomVisitor * visitor, AtomVisitorContext * ctx) ILO_OVERRIDE;
	virtual IloBool isAtomic() const ILO_OVERRIDE { return IloFalse; }
	IloExtractableI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloCumulHeightExprI :public IloIntExprI {
	ILOS_CPEXTR_DECL(IloCumulHeightExprI, IloIntExprI)
public:
	IloCumulHeightExprI(IloEnvI* env,
		IloIntervalVarI* a,
		IloCumulFunctionExprI* f,
		IloBool onStart,
		IloInt dval);
	virtual ~IloCumulHeightExprI(){}
	IloIntervalVarI* getInterval() const { return _interval; }
	IloCumulFunctionExprI* getCumulFunction() const { return _cumul; }
	IloBool isOnStart() const { return _onStart; }
	IloInt getDefaultValue() const { return _dval; }
	virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
	static IloEvalCP _evalCP;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
	IloIntervalVarI*       _interval;
	IloCumulFunctionExprI* _cumul;
	IloBool                _onStart;
	IloInt                 _dval;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloStateFunctionI :public IloStateFunctionExprI {
	ILOS_CPEXTR_DECL(IloStateFunctionI, IloStateFunctionExprI)
public:
	enum ParamId {
		TransitionDistance =0
	};
public:
	IloStateFunctionI(IloEnvI* env, IloTupleSetExprArg tdist, const char* name=0);
	IloTupleSetExprI* getTransitionDistanceSetExpr() const {
		if (_tDistType != 0)
			return (IloTupleSetExprI*)_transitionDistance;
		return NULL;
	}
	IloTransitionDistanceI* getTransitionDistanceParam() const { 
		if ( (_tDistType == 0) && (0 != _transitionDistance) )
			return
			(IloTransitionDistanceI*)((IloParameterExtractableLinkI*)_transitionDistance)->getParameter();
		return NULL;
	}
	IloBool hasTransitionDistanceParam() const {return (0!=_transitionDistance);}
	IloBool transitionDistanceIsTupleSetExpr() const {return (0!=_tDistType);}
public:
	IloStateFunctionI(IloEnvI* env, IloTransitionDistanceI* tdist, const char* name);
	virtual ~IloStateFunctionI();
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual void changeParameter(const IloParameterExtractableLinkI* param) ILO_OVERRIDE;
	virtual void endParameter(const IloParameterExtractableLinkI* link) ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
	IloUInt _tDistType;
	void* _transitionDistance;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloStateAlwaysI : public IloConstraintI {
	ILOS_CPEXTR_DECL(IloStateAlwaysI, IloConstraintI)
private:
	IloIntervalRangeI _range;
	IloStateFunctionI* _f;
	IloStateAlwaysI(IloEnvI* env, 
		IloIntervalVarI* ti, IloInt t1, IloInt t2,
		IloStateFunctionI* f,
		IloUInt status, IloInt min, IloInt max,
		const char* name);
public:
	IloStateAlwaysI(IloEnvI* env, 
		IloIntervalVarI* ti,
		IloInt t1, IloInt t2,
		IloStateFunctionI* f,
		const char* name);
	IloStateAlwaysI(IloEnvI* env,
		IloIntervalVarI* ti,
		IloInt t1, IloInt t2,
		IloStateFunctionI* f,
		IloBool range, IloBool min, IloBool max,
		IloBool align, IloBool startAlign, IloBool endAlign,
		const char* name);
	IloStateFunctionI* getFunction() const { return _f;}
	IloIntervalVarI* getInterval() const { return _range.getInterval();}
	IloBool isConstant()           const { return _range.isConstant();}
	IloBool isNoState()            const { return _range.isNoState();}
	IloBool isRange()              const { return _range.isStateRange();}
	IloBool isAligned()            const { return _range.isAligned();}
	IloInt getStart()              const { return _range.getStart();}
	IloInt getEnd()                const { return _range.getEnd();}
	IloBool isVariable()           const { return _range.isVariable();}
	IloInt getMin()                const { return _range.getMin();}
	IloInt getMax()                const { return _range.getMax();}
	IloBool isStartAligned()       const { return _range.isStartAligned();}
	IloBool isEndAligned()         const { return _range.isEndAligned();}

	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloExprAlwaysInI : public IloConstraintI {
	ILOS_CPEXTR_DECL(IloExprAlwaysInI, IloConstraintI)
protected:
	IloCumulFunctionExprI* _function;
	IloIntExprI*           _levelMin; 
	IloIntExprI*           _levelMax; 
	IloUInt                _status;
	
	IloExprAlwaysInI(IloEnvI* env, IloCumulFunctionExprI* func,
		IloIntExprI* levelMin, IloIntExprI* levelMax,
		const char* name=0);
	void setStatus(IloUInt status) { _status = status; }
public:
	virtual ~IloExprAlwaysInI();
	IloCumulFunctionExprI* getFunction() const { return _function; }
	IloIntExprArg getLevelMin() const { return _levelMin; }
	IloIntExprArg getLevelMax() const { return _levelMax; }
	IloBool hasConstValue() const { return 0==_levelMax; }
	virtual IloIntExprArg getValue() const {
		IloAssert(hasConstValue(),
			"IloExprAlwaysInI cannot get a non-const value ");
		return getLevelMin();
	}
	void setLowerThanBuiltMax()    { IloBitSet(_status, 20);}
	void setGreaterThanBuiltMax()  { IloBitSet(_status, 21);}
	void setLowerThanBuiltMin()    { IloBitSet(_status, 22);}
	void setGreaterThanBuiltMin()  { IloBitSet(_status, 23);}
	IloBool isLowerThanBuiltMax()   const { return IloBitTest(_status, 20);} 
	IloBool isGreaterThanBuiltMax() const { return IloBitTest(_status, 21);} 
	IloBool isLowerThanBuiltMin()   const { return IloBitTest(_status, 22);} 
	IloBool isGreaterThanBuiltMin() const { return IloBitTest(_status, 23);} 
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloAlwaysInIntervalI : public IloExprAlwaysInI {
	ILOS_CPEXTR_DECL(IloAlwaysInIntervalI, IloExprAlwaysInI)
protected:
	IloIntExprI* _start; 
	IloIntExprI* _end; 
public:
	IloAlwaysInIntervalI(IloEnvI* env, IloCumulFunctionExprI* func,
		IloIntExprI* start, IloIntExprI* end,
		IloIntExprI* value, const char* name=0);
	IloAlwaysInIntervalI(IloEnvI* env, IloCumulFunctionExprI* func,
		IloIntExprI* start, IloIntExprI* end,
		IloIntExprI* min, IloIntExprI* max,
		const char* name=0);
	virtual ~IloAlwaysInIntervalI();
	IloIntExprArg getStart() const { return _start; }
	IloIntExprArg getEnd() const { return _end; }
private:
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
public:
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloCumulMinMaxExprI: public IloIntExprI {
	ILOS_CPEXTR_DECL(IloCumulMinMaxExprI, IloIntExprI)
private:
	IloBool _isMin;
	IloCumulFunctionExprI* _function;
	IloIntExprI* _startExpr;
	IloIntExprI* _endExpr;
	IloInt _start;
	IloInt _end;
public:
	IloCumulMinMaxExprI(IloEnvI* env, IloBool isMin, IloCumulFunctionExprI* f);
	IloCumulMinMaxExprI(IloEnvI* env, IloBool isMin, IloCumulFunctionExprI* f,
		IloInt start, IloInt end);
	IloCumulMinMaxExprI(IloEnvI* env, IloBool isMin, IloCumulFunctionExprI* f,
		IloIntExprI* start, IloIntExprI* end);
	virtual ~IloCumulMinMaxExprI();
	virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
	static IloEvalCP _evalCP;
	IloBool isMinExpr() const { return _isMin; }
	IloBool isMaxExpr() const { return !_isMin; }
	IloCumulFunctionExprI* getCumulFunction() const { return _function; }
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
