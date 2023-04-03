// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilointervalsi.h
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

#ifndef __CONCERT_IloIntervalsIH
#define __CONCERT_IloIntervalsIH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloskiplist.h>

class IloIntervalI : public IloSkipListNodeI {
	friend class IloIntervalListI;
private:
	IloNum _end;
	IloNum _type;

public:
	IloIntervalI();
#ifndef ILO_SEGF_GENERIC_ALLOC
	IloIntervalI(IloEnvI* env, IloInt level);
#else
	IloIntervalI(IloMemoryManagerI* env, IloInt level);
#endif
	~IloIntervalI() {}

	IloNum getEnd() const { return _end; }
	IloNum getType() const { return _type; }
	void setEnd(IloNum end) { _end = end; }
	void setType(IloNum type) { _type = type; }

	void display(ILOSTD(ostream)& out) const;
};

class IloIntervalListI : public IloSkipListI {
private:
	IloBool _keepOpen;
	IloNum  _origin;
	IloNum  _horizon;
protected:
	
	virtual IloSkipListNodeI* allocateNode(NodeType type, IloInt level) ILO_OVERRIDE;
	virtual void destroyNode(IloSkipListNodeI* node) ILO_OVERRIDE;

public:
	
	IloIntervalListI(IloEnvI* env,
		IloNum origin    =-IloInfinity,
		IloNum horizon   =+IloInfinity,
		const char* name =0);

	
	IloIntervalListI (IloEnvI* env,
		const IloNumArray times, 
		const IloNumArray types,
		const char* name);

	virtual ~IloIntervalListI();

	
	void keepOpen(IloBool val);

	
	IloBool isKeptOpen() const { return _keepOpen; }

	
	void addInterval(IloNum start, IloNum end, IloNum type) {
		if (doAddInterval(start, end, type))
			notifyChange();
	}

	
	void addIntervalOnDuration(IloNum start, IloNum duration, IloNum type) {
		if (doAddInterval(start, start+duration, type))
			notifyChange();
	}

	
	void addPeriodicInterval(IloNum start,
		IloNum duration,
		IloNum period,
		IloNum end, 
		IloNum type);

	
	void removeInterval(IloNum start, IloNum end) {
		if (doRemoveInterval(start, end))
			notifyChange();
	}

	
	void removeIntervalOnDuration(IloNum start, IloNum duration) {
		if (doRemoveInterval(start, start+duration))
			notifyChange();
	}

	
	void removePeriodicInterval(IloNum start,
		IloNum duration,
		IloNum period,
		IloNum end);

	
	IloBool isEmpty() const {
		return (_header->getNext() == _tail); }

	
	void setPeriodic(IloIntervalListI* l, 
		IloNum x0,
		IloInt n);

	
	void shift(IloNum dx);

	
	void dilate(IloNum k);

	
	void setUnion(IloIntervalListI* l);

	
	void setDifference(IloIntervalListI* l);

	
	IloIntervalI* getFirstInterval() const {
		return (IloIntervalI*)_header->getNext(); }

	
	IloIntervalI* getLastInterval() const {
		return (IloIntervalI*)_tail->getPrev(); }

	
	IloNum getDefinitionIntervalMin() const {
		return _origin; }

	
	IloNum getDefinitionIntervalMax() const {
		return _horizon; }

	IloBool contains(IloIntervalListI* l);

	IloBool isSameAs(IloIntervalListI* l);

	
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

	
	IloBool doAddInterval    (IloNum start, IloNum end, IloNum type);
	IloBool doRemoveInterval (IloNum start, IloNum end);

	IloBool contains    (IloNum start, IloNum end, IloNum type);

	
	virtual IloParameterI* makeCopy() const ILO_OVERRIDE;

};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
