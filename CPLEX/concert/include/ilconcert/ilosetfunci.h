// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilosetfunci.h
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

#ifndef __CONCERT_IloSetFuncIH
#define __CONCERT_IloSetFuncIH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloskiplist.h>
#include <ilconcert/iloanyset.h>

class IloAnySetSegmentI : public IloSkipListNodeI {
private:
	IloBool   _complementary;
	IloAnySet _elts;
public:
	IloAnySetSegmentI(IloEnvI* env, IloInt level);
	~IloAnySetSegmentI();

	void empty();
	void fill();
	void add(const IloAnySet elts, const IloEnv, IloBool complt = IloFalse);
	void remove(const IloAnySet elts, const IloEnv, IloBool complt = IloFalse);
	void setIntersection(const IloAnySet elts, const IloEnv, IloBool complt =IloFalse);
	void set(const IloAnySet elts, IloBool complt = IloFalse);
	IloBool isFull() const { return (_complementary && (_elts.getSize() == 0L)); }
	IloBool isEmpty() const { return (!_complementary && (_elts.getSize() == 0L)); }
	IloAnySet getValue() const { return _elts; }
	IloBool isComplementary() const { return _complementary; }
	IloBool contains(const IloAnySet elts, IloBool complmt = IloFalse) const;
	IloBool intersects(const IloAnySet elts, IloBool complmt = IloFalse) const;
	IloBool isSameAs(const IloAnySet elts, IloBool complt) const;
	void setSameAs(const IloSkipListNodeI* seg);
	void display(ILOSTD(ostream)& out) const;
};

class IloNumToAnySetStepFunctionI : public IloSkipListI {
private:
	IloNum              _xMin;
	IloNum              _xMax;
	IloAnySetSegmentI   _workSeg;
	IloAnySetSegmentI*  _cursor;

protected:
	virtual IloSkipListNodeI* allocateNode(NodeType type, IloInt level) ILO_OVERRIDE;
	virtual void destroyNode(IloSkipListNodeI* node) ILO_OVERRIDE;

public:
	IloNumToAnySetStepFunctionI(IloEnvI* env,
		IloNum xmin, 
		IloNum xmax,
		const char* name);
	IloNumToAnySetStepFunctionI(IloEnvI* env,
		IloNum xmin, 
		IloNum xmax,
		const IloAnySet dval,
		const char* name);
	virtual ~IloNumToAnySetStepFunctionI();

	virtual IloParameterI* makeCopy() const ILO_OVERRIDE;
	void copy(const IloNumToAnySetStepFunctionI* fct);
	IloBool setPeriodic(IloNumToAnySetStepFunctionI* f, IloNum x0, 
		IloInt n, const IloAnySet dval);
	IloBool doSet(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt =IloFalse);
	IloBool doRemove(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt = IloFalse);
	IloBool doAdd(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt = IloFalse);
	IloBool doSetIntersection(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt = IloFalse);
	IloBool shift(IloNum dx, const IloAnySet dval);
	IloBool set(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt =IloFalse);
	IloBool set(IloNum xMin, IloNum xMax, IloAny value); 
	IloBool fill(IloNum xMin, IloNum xMax);
	IloBool empty(IloNum xMin, IloNum xMax);
	IloBool remove(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt = IloFalse);
	IloBool remove(IloNum xMin, IloNum xMax, IloAny value); 
	IloBool remove(IloNumToAnySetStepFunctionI* f);
	IloBool add(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt = IloFalse);
	IloBool add(IloNum xMin, IloNum xMax, IloAny value); 
	IloBool add(IloNumToAnySetStepFunctionI* f);
	IloBool setIntersection(IloNum xMin, IloNum xMax, const IloAnySet values, IloBool complt = IloFalse);
	IloBool setIntersection(IloNum xMin, IloNum xMax, IloAny value); 
	IloBool setIntersection(IloNumToAnySetStepFunctionI* f);
	IloBool dilate(IloNum k);
	IloNum getXMin() const { return _xMin; }
	IloNum getXMax() const { return _xMax; }
	IloBool isFull(IloNum x);
	IloBool isEmpty(IloNum x);
	IloAnySet getSet(IloNum x);
	IloAnySet getComplementSet(IloNum x);
	IloBool usesComplementaryRepresentation(IloNum x);
	IloBool contains(IloNum x, IloAny elt);
	IloBool contains(IloNum x, const IloAnySet elts);
	IloBool alwaysContains(IloNum xMin, IloNum xMax, IloAny elt);
	IloBool alwaysContains(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complmt = IloFalse);
	IloBool alwaysContains(IloNumToAnySetStepFunctionI* f);
	IloBool everContains(IloNum xMin, IloNum xMax, IloAny elt);
	IloBool everContains(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complmt = IloFalse);
	IloBool everContains(IloNumToAnySetStepFunctionI* f);
	IloBool intersects(IloNum x, const IloAnySet elts);
	IloBool alwaysIntersects(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complmt = IloFalse);
	IloBool alwaysIntersects(IloNumToAnySetStepFunctionI* f);
	IloBool everIntersects(IloNum xMin, IloNum xMax, const IloAnySet elts, IloBool complmt = IloFalse);
	IloBool everIntersects(IloNumToAnySetStepFunctionI* f);
	IloBool isSameAs(IloNumToAnySetStepFunctionI* fct);
	void display(ILOSTD(ostream)&) const ILO_OVERRIDE;
	IloAnySetSegmentI* seek(IloNum x) { return (IloAnySetSegmentI*)searchSlice0(x); }
	void initCursor(IloNum x) { _cursor = (IloAnySetSegmentI*)searchSlice0(x); }
	IloAnySetSegmentI* getCursor() const { return _cursor; }
	void setCursor(IloAnySetSegmentI* cursor) { _cursor = cursor; }
	IloBool ok() const { return (_cursor->_start != IloInfinity); }
	void operator++() {
		assert(_cursor->_start != IloInfinity);
		_cursor = (IloAnySetSegmentI*)_cursor->getNext(); 
	}
	void operator--() {
		assert(_cursor->_start != -IloInfinity);
		_cursor = (IloAnySetSegmentI*)_cursor->getPrev();    
	}
	IloAnySetSegmentI* getBeginSegment() const { return (IloAnySetSegmentI*)_header; }
	IloAnySetSegmentI* getEndSegment() const { return (IloAnySetSegmentI*)(_tail->_prev); }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
