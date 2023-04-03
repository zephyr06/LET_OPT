// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilonumfunc.h
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

#ifndef __CONCERT_IloNumFuncH
#define __CONCERT_IloNumFuncH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloparam.h>
#include <ilconcert/ilonumfunci.h>

class IloNumToNumStepFunction : public IloParameter {
	ILOHANDLEMININOIMPL(IloNumToNumStepFunction, IloSegmentedFunctionI, IloParameter);
public:
	
	IloNumToNumStepFunction(const IloEnv env,
		IloNum xmin = -IloInfinity,
		IloNum xmax =  IloInfinity,
		IloNum dval =  0.0,
		const char* name = 0);
	
	IloNumToNumStepFunction(const IloEnv env,
		const IloNumArray x,
		const IloNumArray v,
		IloNum xmin   = -IloInfinity,
		IloNum xmax   =  IloInfinity,
		const char* name = 0);

	
	IloNumToNumStepFunction(const IloEnv env, 
		IloNum yl, 
		const IloNumArray x, 
		const IloNumArray y, 
		const char* name = 0); 

	
	IloNumToNumStepFunction copy() const;

	IloNumToNumStepFunction makeCopy() const {
		return copy(); }

	
	void setSteps(const IloNumArray x, const IloNumArray v) const;
	
	void setPeriodic(const IloNumToNumStepFunction f,
		IloNum x0,
		IloNum n = IloInfinity,
		IloNum dval = 0) const;

	
	void setPeriodicValue(IloNum x1, IloNum x2,
		const IloNumToNumStepFunction f,
		IloNum offset =0) const;

	
	void setValue(IloNum x1, IloNum x2,  IloNum v) const;
	void setValue(IloNum x1, IloNum x2, IloNumToNumStepFunction  f) const;
	
	void addValue(IloNum x1, IloNum x2, IloNum v) const;
	
	void setMax(IloNum x1, IloNum x2, IloNum v) const;
	
	void setMin(IloNum x1, IloNum x2, IloNum v) const;
	
	void shift(IloNum dx, IloNum dval = 0) const;
	
	IloNum getDefinitionIntervalMin() const;
	
	IloNum getDefinitionIntervalMax() const;
	
	IloNum getValue(IloNum x)  const;
	
	IloNum getMax(IloNum x1, IloNum x2)  const;
	
	IloNum getMin(IloNum x1, IloNum x2)  const;
	
	IloNum getArea(IloNum x1, IloNum x2)  const;
	
	void operator*=(IloNum k) const;
	
	void operator+=(const IloNumToNumStepFunction fct) const;
	
	void operator-=(const IloNumToNumStepFunction fct) const;
	
	void setMin(const IloNumToNumStepFunction fct) const;
	
	void setMax(const IloNumToNumStepFunction fct) const;
	
	void dilate(IloNum k) const;
};

inline void
IloNumToNumStepFunction::setSteps(const IloNumArray x, const IloNumArray v) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	IloAssert(x.getSize()+1 == v.getSize(), "Bad array sizes in step function");
	getImpl()->setSteps(x,v);
}

inline void
IloNumToNumStepFunction::setPeriodic(const IloNumToNumStepFunction f,
									 IloNum x0,
									 IloNum n,
									 IloNum dval) const {
										 IloAssert(getImpl() != 0, "Using empty handle step function");
										 getImpl()->setPeriodic(f.getImpl(),x0,n,dval);
}

inline void
IloNumToNumStepFunction::setPeriodicValue(IloNum x1, IloNum x2,
										  const IloNumToNumStepFunction f,
										  IloNum offset) const {
											  IloAssert(getImpl() != 0, "Using empty handle step function");
											  getImpl()->setPeriodicValue(x1, x2, f.getImpl(), offset);
}

inline void
IloNumToNumStepFunction::setValue(IloNum x1, IloNum x2,  IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setValue(x1,x2, v, 0.);
}

inline void
IloNumToNumStepFunction::setValue(IloNum x1, IloNum x2, IloNumToNumStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setValue(x1,x2, f.getImpl());
}

inline void
IloNumToNumStepFunction::addValue(IloNum x1, IloNum x2, IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->addValue(x1,x2,v, 0.);
}

inline void
IloNumToNumStepFunction::setMax(IloNum x1, IloNum x2, IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setMax(x1,x2,v, 0.);
}

inline void
IloNumToNumStepFunction::setMin(IloNum x1, IloNum x2, IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setMin(x1,x2,v, 0.);
}

inline void
IloNumToNumStepFunction::shift(IloNum dx, IloNum dval) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->shift(dx,dval);
}

inline IloNum
IloNumToNumStepFunction::getDefinitionIntervalMin() const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	return getImpl()->getXMin();
}

inline IloNum
IloNumToNumStepFunction::getDefinitionIntervalMax() const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	return getImpl()->getXMax();
}

inline void
IloNumToNumStepFunction::operator*=(IloNum k) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setMult(k);
}

inline void
IloNumToNumStepFunction::operator+=(const IloNumToNumStepFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setAdd(fct.getImpl());
}

inline void
IloNumToNumStepFunction::operator-=(const IloNumToNumStepFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setSub(fct.getImpl());
}

inline void
IloNumToNumStepFunction::setMin(const IloNumToNumStepFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setMin(fct.getImpl());
}

inline void
IloNumToNumStepFunction::setMax(const IloNumToNumStepFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setMax(fct.getImpl());
}

inline void
IloNumToNumStepFunction::dilate(IloNum k) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setXScale(k);
}

inline IloNumToNumStepFunction
IloNumToNumStepFunction::copy() const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	return (IloNumToNumStepFunctionI*)( getImpl()->makeCopy() );
}

ILOSTD(ostream)& operator<< (ILOSTD(ostream)& out, const IloNumToNumStepFunction& f);

IloNumToNumStepFunction operator*  (const IloNumToNumStepFunction f1,
									IloNum k);

IloNumToNumStepFunction operator*  (IloNum k,
									const IloNumToNumStepFunction f1);

IloNumToNumStepFunction operator+  (const IloNumToNumStepFunction f1,
									const IloNumToNumStepFunction f2);

IloNumToNumStepFunction operator-  (const IloNumToNumStepFunction f1,
									const IloNumToNumStepFunction f2);

IloNumToNumStepFunction IloMin     (const IloNumToNumStepFunction f1,
									const IloNumToNumStepFunction f2);

IloNumToNumStepFunction IloMax     (const IloNumToNumStepFunction f1,
									const IloNumToNumStepFunction f2);

inline IloBool
operator== (const IloNumToNumStepFunction f1,
			const IloNumToNumStepFunction f2) {
				return f1.getImpl()->isSameAs(f2.getImpl());
}

inline IloBool
operator!= (const IloNumToNumStepFunction f1,
			const IloNumToNumStepFunction f2) {
				return !f1.getImpl()->isSameAs(f2.getImpl());
}

inline IloBool
operator<= (const IloNumToNumStepFunction f1,
			const IloNumToNumStepFunction f2) {
				return f1.getImpl()->isLowerThan(f2.getImpl());
}

inline IloBool
operator>= (const IloNumToNumStepFunction f1,
			const IloNumToNumStepFunction f2) {
				return f2.getImpl()->isLowerThan(f1.getImpl());
}

class IloNumToNumStepFunctionCursor {
private:
	IloSegmentedFunctionI* _function;
	IloFunctionSegmentI*   _cursor;

public:
	IloNumToNumStepFunctionCursor();
	IloNumToNumStepFunctionCursor(const IloNumToNumStepFunction);

	
	IloNumToNumStepFunctionCursor(const IloNumToNumStepFunction, IloNum x);
	
	IloNumToNumStepFunctionCursor(const IloNumToNumStepFunctionCursor&);
	~IloNumToNumStepFunctionCursor();
	void operator=(const IloNumToNumStepFunctionCursor&);

	
	IloBool ok() const { return (_cursor->getStart() < _function->getXMax()); }
	
	void     operator++() {
		_cursor = (IloFunctionSegmentI*)_cursor->getNext();
	}
	
	void     operator--() {
		
		_cursor = (IloFunctionSegmentI*)_cursor->getPrev();
	}
	
	void seek(IloNum);

	
	IloNum getSegmentMin() const {
		assert (_cursor != 0);
		return IloMax(_cursor->getXLeft(), _function->getXMin());
	}
	
	IloNum getSegmentMax() const {
		assert (_cursor != 0);
		return IloMin(_cursor->getXRight(), _function->getXMax());
	}
	
	IloNum getValue()      const {
		assert (_cursor != 0);
		return _cursor->getValueLeft();
	}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
