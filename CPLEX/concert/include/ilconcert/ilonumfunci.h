// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilonumfunci.h
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

#ifndef __CONCERT_IloNumFuncIH
#define __CONCERT_IloNumFuncIH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilointervalsi.h>

class IloFunctionSegmentI : public IloSkipListNodeI {
	friend class IloSegmentedFunctionI;
private:
	IloNum _valueRef;
	IloNum _slope;
public:
	IloFunctionSegmentI();  
	IloFunctionSegmentI(IloEnvI* env, IloInt level);  
#ifdef ILO_SEGF_GENERIC_ALLOC
	IloFunctionSegmentI(IloMemoryManagerI* env, IloInt level);  
#endif
	~IloFunctionSegmentI() {}

	IloNum getValueRef() const  { return _valueRef; }
	void setValueRef(IloNum value) { _valueRef = value; }
	void addValueRef(IloNum value) { _valueRef += value; }
	IloNum getValueLeft() const;
	IloNum getValueRight() const;
	IloNum getSlope() const  { return _slope; }
	IloNum getMin() const;
	IloNum getMax() const;
	IloNum getArea() const;
	IloNum getValue(IloNum x) const;
	void setSlope(IloNum slope) { _slope = slope; }
	void addSlope(IloNum slope) { _slope += slope; }
	void setSameAs(const IloSkipListNodeI* seg);
	IloBool isSameAs(IloNum x, IloNum val, IloNum slope) const;
	IloBool minInterval(IloNum x1, IloNum x2, IloNum val, IloNum slope, 
		IloNum& ax1, IloNum& ax2);
	IloBool maxInterval(IloNum x1, IloNum x2, IloNum val, IloNum slope, 
		IloNum& ax1, IloNum& ax2);
	void display(ILOSTD(ostream)& ost) const;
};

class IloSegmentedFunctionI : public IloSkipListI {
private:
	IloNum               _xMin;
	IloNum               _xMax;
	IloNum               _xRefPoint;
	IloNum               _vRefPoint;
	IloBool              _valuesComputed;
	IloFunctionSegmentI* _cursor;
	IloIntervalListI*    _intervals;

private:
	void doRestrictInterval(IloNum& x1, IloNum& x2, IloNum& v1, IloNum s);
	IloBool doSetValue(IloNum x1, IloNum x2,  IloNum v1, IloNum s);
	IloBool doAddValue(IloNum x1, IloNum x2, IloNum v1, IloNum s);
	IloBool doSetMax(IloNum x1, IloNum x2, IloNum v1, IloNum s);
	IloBool doSetMin(IloNum x1, IloNum x2, IloNum v1, IloNum s);
	IloBool doShift(IloNum dx, IloNum dval);

protected:
	virtual IloSkipListNodeI* allocateNode(NodeType type, IloInt level) ILO_OVERRIDE;
	virtual void destroyNode(IloSkipListNodeI* node) ILO_OVERRIDE;

public:
#ifndef ILO_SEGF_GENERIC_ALLOC
	IloSegmentedFunctionI(IloEnvI* env,
		IloNum xMin, 
		IloNum xMax,
		IloNum dvalue,
		const char* name);
#else
	IloSegmentedFunctionI(IloEnvI* env,
		IloNum xMin, 
		IloNum xMax,
		IloNum dvalue,
		const char* name);
	IloSegmentedFunctionI(IloMemoryManagerI* env,
		IloNum xMin, 
		IloNum xMax,
		IloNum dvalue,
		IloRandomI* random,
		const char* name);
#endif
	virtual ~IloSegmentedFunctionI();
	void ensureIntervals();
	virtual IloParameterI* makeCopy() const ILO_OVERRIDE;
#ifdef ILO_SEGF_GENERIC_ALLOC
	IloSegmentedFunctionI* makeClone(IloMemoryManagerI* mgr) const;
#endif
	void copy(const IloSegmentedFunctionI* fct);
	virtual void display(ILOSTD(ostream)& ost) const ILO_OVERRIDE;
	IloNum getXMin() const { return _xMin; }
	IloNum getXMax() const { return _xMax; }
	IloBool isSemiConvex();
	void definePiecewiseLinear(IloNumArray x, IloNumArray s,
		IloNum x0, IloNum y0);
	void setPiecewiseLinear(IloNumArray x, IloNumArray s,
		IloNum x0, IloNum y0);
	IloInt countSegments();
	void setSlope(IloNum x, IloNum s);
	void setReferenceValue(IloNum x0, IloNum v0);
	IloNum getReferenceX() const { return _xRefPoint; }
	IloNum getReferenceValue() const { return _vRefPoint; }
	void setPoints(IloNumArray x, IloNumArray v);
	void setPoints(IloNum sl, IloNumArray x, IloNumArray v, IloNum sr);
	IloBool setPeriodic(IloSegmentedFunctionI* f, IloNum x0, 
		IloNum n, IloNum dval);
	void setSteps(IloNumArray x, IloNumArray v);
	void setSteps(IloNum yl, IloNumArray x, IloNumArray v);
	IloNum getValue(IloNum x);
	IloNum getSlope(IloNum x);
	IloNum getMax(IloNum x1, IloNum x2); 
	IloNum getMin(IloNum x1, IloNum x2); 
	IloNum getArea(IloNum x1, IloNum x2); 
	IloBool setValue(IloNum x1, IloNum x2, IloNum v1, IloNum s);
	IloBool setValue(IloNum x1, IloNum x2, IloSegmentedFunctionI* f);
	IloBool addValue(IloNum x1, IloNum x2, IloNum v1, IloNum s);
	IloBool setMax(IloNum x1, IloNum x2, IloNum v1, IloNum s);
	IloBool setMin(IloNum x1, IloNum x2, IloNum v1, IloNum s);
	IloBool shift(IloNum dx, IloNum dval);
	IloBool setXScale(IloNum k);
	IloBool setMult(IloNum k);
	IloBool setAdd(IloSegmentedFunctionI* fct);
	IloBool setSub(IloSegmentedFunctionI* fct);
	IloBool setMin(IloSegmentedFunctionI* fct);
	IloBool setMax(IloSegmentedFunctionI* fct);
	IloBool setPeriodicValue(IloNum x1, IloNum x2, IloSegmentedFunctionI* fct, IloNum offset =0);
	IloBool isSameAs(IloSegmentedFunctionI* fct);
	IloBool isLowerThan(IloSegmentedFunctionI* fct);
	void needValues();
	IloFunctionSegmentI* seek(IloNum x) { return (IloFunctionSegmentI*)searchSlice0(x); }
	void initCursor(IloNum x) { _cursor = (IloFunctionSegmentI*)searchSlice0(x); }
	IloFunctionSegmentI* getCursor() const { return _cursor; }
	IloBool ok() const { return (_cursor->_start != IloInfinity); }
	void setCursor(IloFunctionSegmentI* cursor) { _cursor = cursor; }
	IloNum getSegmentMin() const;
	IloNum getSegmentMax() const;
	IloNum getValue()      const;
	void nextSegment() {
		assert(_cursor->_start != IloInfinity);
		_cursor = (IloFunctionSegmentI*)_cursor->getNext(); 
	}
	void operator++() {
		assert(_cursor->_start != IloInfinity);
		_cursor = (IloFunctionSegmentI*)_cursor->getNext(); 
	}
	void operator--() {
		assert(_cursor->_start != -IloInfinity);
		_cursor = (IloFunctionSegmentI*)_cursor->getPrev();    
	}
	IloNum getValueLeft(IloNum x);
	IloFunctionSegmentI* getBeginSegment() const { return (IloFunctionSegmentI*)_header; }
	IloFunctionSegmentI* getEndSegment() const { return (IloFunctionSegmentI*)(_tail->_prev); }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
