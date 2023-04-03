// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilodomaini.h
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

#ifndef __CONCERT_ilodomainiH
#define __CONCERT_ilodomainiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>

class IloVarDomainI {
public:
	IloVarDomainI() {}
	virtual ~IloVarDomainI() {}
	virtual IloNum getLB() const =0;
	virtual void setLB(IloNum LB)=0;
	virtual IloNum getUB() const =0;
	virtual void setUB(IloNum UB)=0;
	virtual IloNumVar::Type getType() const =0;
	virtual IloInt getMin() const =0;
	virtual IloInt getMax() const =0;
	virtual IloBool isEnumerated() const =0;
	virtual IloBool isAny() const = 0;
	virtual IloNumArray getNumValues() const =0;
	virtual IloIntArray getIntValues() const =0;
	virtual void setNumValues(IloNumArray ar) =0;
	virtual void setIntValues(IloIntArray ar) =0;
	virtual void suicide(IloEnvI*, void* ptr) =0;
	virtual void display(ILOSTD(ostream)& out) = 0;
	virtual IloVarDomainI* makeClone(IloEnvI* env) = 0;
	virtual IloBool isShared() const { return IloFalse; }
	virtual IloBool isIntDomain() const { return IloFalse; }
	virtual IloBool isNumDomain() const { return IloFalse; }
	virtual IloBool isBoolDomain() const { return IloFalse; }
};

class IloNumVarDomainI : public IloVarDomainI {
	IloNum _lb;
	IloNum _ub;
protected:
	IloNumArray _array;
public:
	virtual ~IloNumVarDomainI(){}
	IloNumVarDomainI(const IloNumRangeI* coll);
	IloNumVarDomainI(IloNum lb, IloNum ub);
	IloNumVarDomainI(IloNumArray ar);
	virtual IloNum getLB() const ILO_OVERRIDE;
	virtual void setLB(IloNum LB) ILO_OVERRIDE;
	virtual IloNum getUB() const ILO_OVERRIDE;
	virtual void setUB(IloNum UB) ILO_OVERRIDE;
	virtual IloNumVar::Type getType() const ILO_OVERRIDE;
	virtual IloInt getMin() const ILO_OVERRIDE;
	virtual IloInt getMax() const ILO_OVERRIDE;
	virtual IloBool isEnumerated() const ILO_OVERRIDE;
	virtual IloBool isAny() const ILO_OVERRIDE;
	virtual IloNumArray getNumValues() const ILO_OVERRIDE;
	virtual IloIntArray getIntValues() const ILO_OVERRIDE;
	virtual void setNumValues(IloNumArray ar) ILO_OVERRIDE;
	virtual void setIntValues(IloIntArray) ILO_OVERRIDE;
	virtual void suicide(IloEnvI* env, void* ptr) ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& out) ILO_OVERRIDE;
	virtual IloVarDomainI* makeClone(IloEnvI* env) ILO_OVERRIDE;
	virtual IloBool isNumDomain() const ILO_OVERRIDE { return IloTrue; }
};

class IloIntVarDomainI : public IloVarDomainI {
	IloInt      _lb;
	IloInt      _ub;
protected:
	IloIntArray _array;
private:
	IloBool     _any;
public:
	virtual ~IloIntVarDomainI(){}
	IloIntVarDomainI(const IloIntRangeI* coll);
	IloIntVarDomainI(IloInt lb, IloInt ub, IloBool any = IloFalse);
	IloIntVarDomainI(IloIntArray ar, IloBool any = IloFalse);
	void cleanArray(IloInt lb, IloInt ub);
	virtual IloNum getLB() const ILO_OVERRIDE;
	virtual void setLB(IloNum LB) ILO_OVERRIDE;
	virtual IloNum getUB() const ILO_OVERRIDE;
	virtual void setUB(IloNum UB) ILO_OVERRIDE;
	virtual IloNumVar::Type getType() const ILO_OVERRIDE;
	virtual IloInt getMin() const ILO_OVERRIDE;
	virtual IloInt getMax() const ILO_OVERRIDE;
	virtual IloBool isEnumerated() const ILO_OVERRIDE;
	virtual IloBool isAny() const ILO_OVERRIDE;
	virtual IloNumArray getNumValues() const ILO_OVERRIDE;
	virtual IloIntArray getIntValues() const ILO_OVERRIDE;
	virtual void setNumValues(IloNumArray) ILO_OVERRIDE;
	virtual void setIntValues(IloIntArray ar) ILO_OVERRIDE;
	virtual void suicide(IloEnvI* env, void* ptr) ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& out) ILO_OVERRIDE;
	virtual IloVarDomainI* makeClone(IloEnvI* env) ILO_OVERRIDE;
	virtual IloBool isIntDomain() const ILO_OVERRIDE { return IloTrue; }
	virtual IloBool isBoolDomain() const ILO_OVERRIDE;
};

class IloBoolVarDomainI : public IloVarDomainI {
	IloInt      _lb;
	IloInt      _ub;
public:
	virtual ~IloBoolVarDomainI(){}
	IloBoolVarDomainI(IloInt lb=0, IloInt ub=1);
	virtual IloNum getLB() const ILO_OVERRIDE;
	virtual void setLB(IloNum LB) ILO_OVERRIDE;
	virtual IloNum getUB() const ILO_OVERRIDE;
	virtual void setUB(IloNum UB) ILO_OVERRIDE;
	virtual IloNumVar::Type getType() const ILO_OVERRIDE;
	virtual IloInt getMin() const ILO_OVERRIDE;
	virtual IloInt getMax() const ILO_OVERRIDE;
	virtual IloBool isEnumerated() const ILO_OVERRIDE;
	virtual IloBool isAny() const ILO_OVERRIDE;
	virtual IloNumArray getNumValues() const ILO_OVERRIDE;
	virtual IloIntArray getIntValues() const ILO_OVERRIDE;
	virtual void setNumValues(IloNumArray) ILO_OVERRIDE;
	virtual void setIntValues(IloIntArray) ILO_OVERRIDE;
	virtual void suicide(IloEnvI* env, void* ptr) ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& out) ILO_OVERRIDE;
	virtual IloVarDomainI* makeClone(IloEnvI* env) ILO_OVERRIDE;
	virtual IloBool isBoolDomain() const ILO_OVERRIDE { return IloTrue; }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
