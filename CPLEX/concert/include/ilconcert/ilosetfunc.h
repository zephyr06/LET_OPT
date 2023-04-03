// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilosetfunc.h
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

#ifndef __CONCERT_IloSetFuncH
#define __CONCERT_IloSetFuncH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloparam.h>
#include <ilconcert/ilosetfunci.h>

class IloNumToAnySetStepFunction : public IloParameter {
	ILOHANDLEMININOIMPL(IloNumToAnySetStepFunction,IloNumToAnySetStepFunctionI, IloParameter);
public:
	
	IloNumToAnySetStepFunction(const IloEnv env,
		IloNum xmin   = -IloInfinity,
		IloNum xmax   =  IloInfinity,
		const char* name = 0);
	
	IloNumToAnySetStepFunction(const IloEnv env,
		IloNum xmin,
		IloNum xmax,
		const IloAnySet dval,
		const char* name = 0);
	
	IloNumToAnySetStepFunction copy() const;

	IloNumToAnySetStepFunction makeCopy() const {
		return copy(); }

	
	IloNum getDefinitionIntervalMin() const;
	
	IloNum getDefinitionIntervalMax() const;
	
	void setPeriodic(const IloNumToAnySetStepFunction f, IloNum x0,
		IloNum n, const IloAnySet dval) const;
	
	void shift(IloNum dx, const IloAnySet dval) const;
	
	void dilate(IloNum k) const;
	
	void set(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complt =IloFalse) const;
	
	void set(IloNum xMin, IloNum xMax, IloAny elt) const;
	
	void fill(IloNum xMin, IloNum xMax) const;
	
	void empty(IloNum xMin, IloNum xMax) const;
	
	void remove(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complt =IloFalse) const;
	
	void remove(IloNum xMin, IloNum xMax, IloAny elt) const;
	
	void remove(const IloNumToAnySetStepFunction f) const;
	
	void add(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complt =IloFalse) const;
	
	void add(IloNum xMin, IloNum xMax, IloAny elt) const;
	
	void add(const IloNumToAnySetStepFunction f) const;
	
	void setIntersection(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complt =IloFalse) const;
	
	void setIntersection(IloNum xMin, IloNum xMax, IloAny elt) const;
	
	void setIntersection(const IloNumToAnySetStepFunction f) const;
	
	IloBool usesComplementaryRepresentation(IloNum x) const;
	
	IloBool isFull(IloNum x) const;
	
	IloBool isEmpty(IloNum x) const;
	
	IloAnySet getSet(IloNum x) const;
	
	IloAnySet getComplementSet(IloNum x) const;
	
	IloBool contains(IloNum x, IloAny elt) const;
	
	IloBool contains(IloNum x, const IloAnySet elts) const;
	
	IloBool alwaysContains(IloNum xMin, IloNum xMax, IloAny elt) const;
	
	IloBool alwaysContains(IloNum xMin, IloNum xMax, const IloAnySet elts) const;
	
	IloBool alwaysContains(const IloNumToAnySetStepFunction f) const;
	
	IloBool everContains (IloNum xMin, IloNum xMax, IloAny elt) const;
	
	IloBool everContains(IloNum xMin, IloNum xMax, const IloAnySet elts) const;
	
	IloBool everContains(const IloNumToAnySetStepFunction f) const;
	
	IloBool intersects(IloNum x, const IloAnySet elts) const;
	
	IloBool alwaysIntersects(IloNum xMin, IloNum xMax, const IloAnySet elts) const;
	
	IloBool alwaysIntersects(const IloNumToAnySetStepFunction f) const;
	
	IloBool everIntersects(IloNum xMin, IloNum xMax, const IloAnySet elts) const;
	
	IloBool everIntersects(const IloNumToAnySetStepFunction f) const;
};

inline IloNum
IloNumToAnySetStepFunction::getDefinitionIntervalMin() const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->getXMin(); }

inline IloNum
IloNumToAnySetStepFunction::getDefinitionIntervalMax() const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->getXMax(); }

inline IloBool
IloNumToAnySetStepFunction::usesComplementaryRepresentation(IloNum x) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->usesComplementaryRepresentation(x); }

inline void
IloNumToAnySetStepFunction::setPeriodic(const IloNumToAnySetStepFunction f,
										IloNum x0,
										IloNum n,
										const IloAnySet dval) const {
											IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
											getImpl()->setPeriodic(f.getImpl(), x0, IloTrunc(n), dval); }

inline void
IloNumToAnySetStepFunction::shift(IloNum dx, const IloAnySet dval) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->shift(dx, dval); }

inline void
IloNumToAnySetStepFunction::dilate(IloNum k) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->dilate(k);
}

inline void
IloNumToAnySetStepFunction::set(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complt) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->set(xMin, xMax, elts, complt);
}

inline void
IloNumToAnySetStepFunction::set(IloNum xMin,
								IloNum xMax,
								IloAny elt) const {
									IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
									getImpl()->set(xMin, xMax, elt);
}

inline void
IloNumToAnySetStepFunction::fill(IloNum xMin,
								 IloNum xMax) const {
									 IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
									 getImpl()->fill(xMin, xMax);
}

inline void
IloNumToAnySetStepFunction::empty(IloNum xMin,
								  IloNum xMax) const {
									  IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
									  getImpl()->empty(xMin, xMax);
}

inline void
IloNumToAnySetStepFunction::remove(IloNum xMin,
								   IloNum xMax,
								   const IloAnySet elts, IloBool complt) const {
									   IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
									   getImpl()->remove(xMin, xMax, elts, complt);
}

inline void
IloNumToAnySetStepFunction::remove(IloNum xMin,
								   IloNum xMax,
								   IloAny elt) const {
									   IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
									   getImpl()->remove(xMin, xMax, elt);
}

inline void
IloNumToAnySetStepFunction::remove(const IloNumToAnySetStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->remove(f.getImpl());
}

inline void
IloNumToAnySetStepFunction::add(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complt) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->add(xMin, xMax, elts, complt);
}

inline void
IloNumToAnySetStepFunction::add(IloNum xMin, IloNum xMax, IloAny elt) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->add(xMin, xMax, elt);
}

inline void
IloNumToAnySetStepFunction::add(const IloNumToAnySetStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->add(f.getImpl());
}

inline void
IloNumToAnySetStepFunction::setIntersection(IloNum xMin,
											IloNum xMax,
											const IloAnySet elts, IloBool complt) const {
												IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
												getImpl()->setIntersection(xMin, xMax, elts, complt);
}

inline void
IloNumToAnySetStepFunction::setIntersection(IloNum xMin,
											IloNum xMax,
											IloAny elt) const {
												IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
												getImpl()->setIntersection(xMin, xMax, elt);
}

inline void
IloNumToAnySetStepFunction::setIntersection(const IloNumToAnySetStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	getImpl()->setIntersection(f.getImpl());
}

inline IloBool
IloNumToAnySetStepFunction::isFull(IloNum x) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->isFull(x);
}

inline IloBool
IloNumToAnySetStepFunction::isEmpty(IloNum x) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->isEmpty(x);
}

inline IloAnySet
IloNumToAnySetStepFunction::getSet(IloNum x) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->getSet(x);
}

inline IloAnySet
IloNumToAnySetStepFunction::getComplementSet(IloNum x) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->getComplementSet(x);
}

inline IloBool
IloNumToAnySetStepFunction::contains(IloNum x, IloAny elt) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->contains(x, elt); }

inline IloBool
IloNumToAnySetStepFunction::contains(IloNum x, const IloAnySet elts) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->contains(x, elts); }

inline IloBool
IloNumToAnySetStepFunction::alwaysContains(IloNum xMin, IloNum xMax, IloAny elt) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->alwaysContains(xMin, xMax, elt);
}

inline IloBool
IloNumToAnySetStepFunction::alwaysContains(IloNum xMin, IloNum xMax, const IloAnySet elts) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->alwaysContains(xMin, xMax, elts);
}

inline IloBool
IloNumToAnySetStepFunction::alwaysContains(const IloNumToAnySetStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->alwaysContains(f.getImpl());
}

inline IloBool
IloNumToAnySetStepFunction::everContains(IloNum xMin, IloNum xMax, IloAny elt) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->everContains(xMin, xMax, elt);
}

inline IloBool
IloNumToAnySetStepFunction::everContains(IloNum xMin, IloNum xMax, const IloAnySet elts) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->everContains(xMin, xMax, elts);
}

inline IloBool
IloNumToAnySetStepFunction::everContains(const IloNumToAnySetStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->everContains(f.getImpl());
}

inline IloBool
IloNumToAnySetStepFunction::intersects(IloNum x,
									   const IloAnySet elts) const {
										   IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
										   return getImpl()->intersects(x, elts);
}

inline IloBool
IloNumToAnySetStepFunction::alwaysIntersects(IloNum xMin,
											 IloNum xMax,
											 const IloAnySet elts) const {
												 IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
												 return getImpl()->alwaysIntersects(xMin, xMax, elts);
}

inline IloBool
IloNumToAnySetStepFunction::alwaysIntersects(const IloNumToAnySetStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->alwaysIntersects(f.getImpl());
}

inline IloBool
IloNumToAnySetStepFunction::everIntersects(IloNum xMin, IloNum xMax, const IloAnySet elts) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->everIntersects(xMin, xMax, elts);
}

inline IloBool
IloNumToAnySetStepFunction::everIntersects(const IloNumToAnySetStepFunction f) const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return getImpl()->everIntersects(f.getImpl());
}

inline IloNumToAnySetStepFunction
IloNumToAnySetStepFunction::copy() const {
	IloAssert(getImpl() != 0, "Using empty handle integer to any set function");
	return (IloNumToAnySetStepFunctionI*)( getImpl()->makeCopy() );
}

ILOSTD(ostream)& operator<< (ILOSTD(ostream)& out, const IloNumToAnySetStepFunction& f);

IloNumToAnySetStepFunction IloUnion       (const IloNumToAnySetStepFunction f1,
										   const IloNumToAnySetStepFunction f2);

IloNumToAnySetStepFunction IloIntersection(const IloNumToAnySetStepFunction f1,
										   const IloNumToAnySetStepFunction f2);

IloNumToAnySetStepFunction IloDifference  (const IloNumToAnySetStepFunction f1,
										   const IloNumToAnySetStepFunction f2);

inline IloBool
operator==(const IloNumToAnySetStepFunction f1,
		   const IloNumToAnySetStepFunction f2) {
			   return f1.getImpl()->isSameAs(f2.getImpl());
}

inline IloBool
operator!=(const IloNumToAnySetStepFunction f1,
		   const IloNumToAnySetStepFunction f2) {
			   return !f1.getImpl()->isSameAs(f2.getImpl());
}

class IloNumToAnySetStepFunctionCursor {

private:
	IloNumToAnySetStepFunctionI* _function;
	IloAnySetSegmentI*           _cursor;

public:
	IloNumToAnySetStepFunctionCursor();
	
	IloNumToAnySetStepFunctionCursor(const IloNumToAnySetStepFunction);
	
	IloNumToAnySetStepFunctionCursor(const IloNumToAnySetStepFunction, IloNum x);
	
	IloNumToAnySetStepFunctionCursor(const IloNumToAnySetStepFunctionCursor&);
	~IloNumToAnySetStepFunctionCursor();
	
	void operator=(const IloNumToAnySetStepFunctionCursor&);
	
	IloBool ok() const { return (_cursor->getStart() < _function->getXMax()); }
	
	void operator++() {
		_cursor = (IloAnySetSegmentI*)_cursor->getNext();
	}
	
	void operator--() {
		if ((IloAnySetSegmentI*)_cursor->getNext() != _cursor)
			_cursor = (IloAnySetSegmentI*)_cursor->getPrev();
	}
	
	void seek(IloNum);
	
	IloNum getSegmentMin() const {
		assert (_cursor != 0);
		return _cursor->getXLeft(); }
	
	IloNum getSegmentMax() const {
		assert (_cursor != 0);
		return _cursor->getXRight(); }
	
	IloAnySet getComplementSet() const {
		assert (_cursor != 0);
		return (_cursor->getValue()); }
	
	IloAnySet getSet() const {
		assert (_cursor != 0);
		return (_cursor->getValue()); }
	
	IloBool usesComplementaryRepresentation() const {
		assert (_cursor != 0);
		return (_cursor->isComplementary()); }
	
	IloBool isFull() const {
		assert (_cursor != 0);
		return (_cursor->isFull()); }
	
	IloBool isEmpty() const {
		assert (_cursor != 0);
		return (_cursor->isEmpty()); }
	IloAnySetSegmentI*  getCursor() const {
		return _cursor; }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
