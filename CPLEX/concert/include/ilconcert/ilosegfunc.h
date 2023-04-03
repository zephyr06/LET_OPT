// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilosegfunc.h
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

#ifndef __CONCERT_IloSegFuncH
#define __CONCERT_IloSegFuncH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloparam.h>
#include <ilconcert/ilonumfunci.h>

class IloNumToNumStepFunction;

class IloNumToNumSegmentFunction : public IloParameter {
	ILOHANDLEMININOIMPL(IloNumToNumSegmentFunction, IloSegmentedFunctionI, IloParameter);
public:
	
	IloNumToNumSegmentFunction(const IloEnv env,
		IloNum xmin = -IloInfinity,
		IloNum xmax =  IloInfinity,
		IloNum dval =  0.0,
		const char* name = 0);
	
	IloNumToNumSegmentFunction(const IloEnv env,
		const IloNumArray x,
		const IloNumArray v,
		IloNum xmin   = -IloInfinity,
		IloNum xmax   =  IloInfinity,
		const char* name = 0);
	
	IloNumToNumSegmentFunction(const IloNumToNumStepFunction& numFunction);

	
	IloNumToNumSegmentFunction(const IloEnv env,
		IloNum sl,
		const IloNumArray x,
		const IloNumArray y,
		IloNum sr,
		const char* name = 0);

	
	IloBool isSemiConvex() const;

	
	IloNumToNumSegmentFunction copy() const;

	IloNumToNumSegmentFunction makeCopy() const {
		return copy(); }

	
	void setPeriodic(const IloNumToNumSegmentFunction f,
		IloNum x0,
		IloNum n = IloInfinity,
		IloNum dval = 0) const;

	
	void setPeriodicValue(IloNum x1, IloNum x2,
		const IloNumToNumSegmentFunction f,
		IloNum offset =0) const;
	
	void setPoints(const IloNumArray x, const IloNumArray v) const;
	
	void setSlope(IloNum x1, IloNum x2, IloNum v, IloNum slope) const;
	
	void setValue(IloNum x1, IloNum x2,  IloNum v) const;
	void setValue(IloNum x1, IloNum v1, IloNum x2,  IloNum v2) const;
	void setValue(IloNum x1, IloNum x2, IloNumToNumSegmentFunction  f) const;
	
	void addValue(IloNum x1, IloNum x2, IloNum v) const;
	
	void setMax(IloNum x1, IloNum x2, IloNum v) const;
	
	void setMax(IloNum x1, IloNum v1, IloNum x2,  IloNum v2) const;
	
	void setMin(IloNum x1, IloNum x2, IloNum v) const;
	
	void setMin(IloNum x1, IloNum v1, IloNum x2,  IloNum v2) const;
	
	void shift(IloNum dx, IloNum dval = 0) const;
	
	IloNum getDefinitionIntervalMin() const;
	
	IloNum getDefinitionIntervalMax() const;
	
	IloNum getValue(IloNum x)  const;
	
	IloNum getMax(IloNum x1, IloNum x2)  const;
	
	IloNum getMin(IloNum x1, IloNum x2)  const;
	
	IloNum getArea(IloNum x1, IloNum x2)  const;
	
	void operator*=(IloNum k) const;
	
	void operator+=(const IloNumToNumSegmentFunction fct) const;
	
	void operator-=(const IloNumToNumSegmentFunction fct) const;
	
	void setMin(const IloNumToNumSegmentFunction fct) const;
	
	void setMax(const IloNumToNumSegmentFunction fct) const;
	
	void dilate(IloNum k) const;
};

IloNumToNumSegmentFunction IloPiecewiseLinearFunction(const IloEnv env,
													  const IloNumArray point,
													  const IloNumArray slope,
													  IloNum a,
													  IloNum fa,
													  const char* name = 0);

inline IloBool
IloNumToNumSegmentFunction::isSemiConvex() const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	return getImpl()->isSemiConvex();
}

inline void
IloNumToNumSegmentFunction::setPoints(const IloNumArray x,
									  const IloNumArray v) const {
										  IloAssert(getImpl() != 0, "Using empty handle step function");
										  IloAssert(x.getSize() == v.getSize(), "Bad array sizes in step function");
										  getImpl()->setPoints(x,v);
}

inline void
IloNumToNumSegmentFunction::setPeriodic(const IloNumToNumSegmentFunction f,
										IloNum x0, IloNum n,
										IloNum dval) const {
											IloAssert(getImpl() != 0, "Using empty handle segment function");
											getImpl()->setPeriodic(f.getImpl(),x0,n,dval);
}

inline void
IloNumToNumSegmentFunction::setPeriodicValue(IloNum x1, IloNum x2,
											 const IloNumToNumSegmentFunction f,
											 IloNum offset) const {
												 IloAssert(getImpl() != 0, "Using empty handle step function");
												 getImpl()->setPeriodicValue(x1, x2, f.getImpl(), offset);
}

inline void
IloNumToNumSegmentFunction::setSlope(IloNum x1, IloNum x2,
									 IloNum v, IloNum slope) const {
										 IloAssert(getImpl() != 0, "Using empty handle segment function");
										 getImpl()->setValue(x1, x2, v, slope);
}

inline void
IloNumToNumSegmentFunction::setValue(IloNum x1, IloNum x2,  IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setValue(x1, x2, v, 0.);
}

inline void
IloNumToNumSegmentFunction::setValue(IloNum x1, IloNum v1,
									 IloNum x2, IloNum v2) const {
										 IloAssert(getImpl() != 0, "Using empty handle segment function");
										 getImpl()->setValue(x1,x2, v1, (v2-v1)/(x2-x1));
}

inline void
IloNumToNumSegmentFunction::setValue(IloNum x1, IloNum x2, IloNumToNumSegmentFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle step function");
	getImpl()->setValue(x1,x2, f.getImpl());
}

inline void
IloNumToNumSegmentFunction::addValue(IloNum x1, IloNum x2, IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->addValue(x1,x2,v, 0.);
}

inline void
IloNumToNumSegmentFunction::setMax(IloNum x1, IloNum x2, IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setMax(x1,x2,v, 0.);
}

inline void
IloNumToNumSegmentFunction::setMax(IloNum x1, IloNum v1,
								   IloNum x2, IloNum v2) const {
									   IloAssert(getImpl() != 0, "Using empty handle segment function");
									   getImpl()->setMax(x1,x2, v1, (v2-v1)/(x2-x1));
}

inline void
IloNumToNumSegmentFunction::setMin(IloNum x1, IloNum x2, IloNum v) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setMin(x1,x2,v, 0.);
}

inline void
IloNumToNumSegmentFunction::setMin(IloNum x1, IloNum v1,
								   IloNum x2, IloNum v2) const {
									   IloAssert(getImpl() != 0, "Using empty handle segment function");
									   getImpl()->setMin(x1,x2, v1, (v2-v1)/(x2-x1));
}

inline void
IloNumToNumSegmentFunction::shift(IloNum dx, IloNum dval) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->shift(dx,dval);
}

inline IloNum
IloNumToNumSegmentFunction::getDefinitionIntervalMin() const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	return getImpl()->getXMin();
}

inline IloNum
IloNumToNumSegmentFunction::getDefinitionIntervalMax() const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	return getImpl()->getXMax();
}

inline void
IloNumToNumSegmentFunction::operator*=(IloNum k) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setMult(k);
}

inline void
IloNumToNumSegmentFunction::operator+=(const IloNumToNumSegmentFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setAdd(fct.getImpl());
}

inline void
IloNumToNumSegmentFunction::operator-=(const IloNumToNumSegmentFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setSub(fct.getImpl());
}

inline void
IloNumToNumSegmentFunction::setMin(const IloNumToNumSegmentFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setMin(fct.getImpl());
}

inline void
IloNumToNumSegmentFunction::setMax(const IloNumToNumSegmentFunction fct) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setMax(fct.getImpl());
}

inline void
IloNumToNumSegmentFunction::dilate(IloNum k) const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	getImpl()->setXScale(k);
}

inline IloNumToNumSegmentFunction
IloNumToNumSegmentFunction::copy() const {
	IloAssert(getImpl() != 0, "Using empty handle segment function");
	return (IloNumToNumSegmentFunctionI*)( getImpl()->makeCopy() );
}

ILOSTD(ostream)& operator<< (ILOSTD(ostream)& out, const IloNumToNumSegmentFunction& f);

IloNumToNumSegmentFunction operator* (const IloNumToNumSegmentFunction f1,
									  IloNum k);
IloNumToNumSegmentFunction operator* (IloNum k,
									  const IloNumToNumSegmentFunction f1);
IloNumToNumSegmentFunction operator+ (const IloNumToNumSegmentFunction f1,
									  const IloNumToNumSegmentFunction f2);
IloNumToNumSegmentFunction operator- (const IloNumToNumSegmentFunction f1,
									  const IloNumToNumSegmentFunction f2);
IloNumToNumSegmentFunction IloMin    (const IloNumToNumSegmentFunction f1,
									  const IloNumToNumSegmentFunction f2);
IloNumToNumSegmentFunction IloMax    (const IloNumToNumSegmentFunction f1,
									  const IloNumToNumSegmentFunction f2);

inline IloBool
operator== (const IloNumToNumSegmentFunction f1,
			const IloNumToNumSegmentFunction f2) {
				return f1.getImpl()->isSameAs(f2.getImpl());
}

inline IloBool
operator!= (const IloNumToNumSegmentFunction f1,
			const IloNumToNumSegmentFunction f2) {
				return !f1.getImpl()->isSameAs(f2.getImpl());
}

inline IloBool
operator<= (const IloNumToNumSegmentFunction f1,
			const IloNumToNumSegmentFunction f2) {
				return f1.getImpl()->isLowerThan(f2.getImpl());
}

inline IloBool
operator>= (const IloNumToNumSegmentFunction f1,
			const IloNumToNumSegmentFunction f2) {
				return f2.getImpl()->isLowerThan(f1.getImpl());
}

class IloNumToNumSegmentFunctionCursor {
private:
	IloSegmentedFunctionI* _function;
	IloFunctionSegmentI*   _cursor;

public:
	IloNumToNumSegmentFunctionCursor();
	IloNumToNumSegmentFunctionCursor(const IloNumToNumSegmentFunction);
	
	IloNumToNumSegmentFunctionCursor(const IloNumToNumSegmentFunction, IloNum x);
	
	IloNumToNumSegmentFunctionCursor(const IloNumToNumSegmentFunctionCursor&);
	~IloNumToNumSegmentFunctionCursor();
	void operator=(const IloNumToNumSegmentFunctionCursor&);
	
	IloBool ok() const { return (_cursor->getStart() < _function->getXMax()); }
	
	void     operator++() {
		_cursor = (IloFunctionSegmentI*)_cursor->getNext();
	}
	
	void     operator--() {
		if ((IloFunctionSegmentI*)_cursor->getNext() != _cursor)
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
	
	IloNum getValueLeft()  const {
		assert (_cursor != 0);
		return _cursor->getValueLeft();
	}
	
	IloNum getValueRight() const {
		assert (_cursor != 0);
		return _cursor->getValueRight();
	}
	
	IloNum getValue(IloNum t) const {
		assert (_cursor != 0);
		assert ((getSegmentMin() <= t) && (getSegmentMax() >= t));
		return _cursor->getValue(t);
	}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
