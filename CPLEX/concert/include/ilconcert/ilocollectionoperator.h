// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilocollectionoperator.h
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

#ifndef __CONCERT_ilocollectionoperatorH
#define __CONCERT_ilocollectionoperatorH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosymbol.h>
#include <ilconcert/iloanyexpri.h>
#include <ilconcert/iloset.h>
#include <ilconcert/ilomapi.h>

IloIntSet IloUnion(IloIntSet set1, IloIntSet set2);

IloNumSet IloUnion(IloNumSet set1, IloNumSet set2);

IloAnySet IloUnion(IloAnySet set1, IloAnySet set2);

IloInt IloFirst(IloIntSet set);

IloNum IloFirst(IloNumSet set);

IloAny IloFirst(IloAnySet set);

IloInt IloLast(IloIntSet set);

IloNum IloLast(IloNumSet set);

IloAny IloLast(IloAnySet set);

IloIntSet IloExclude(IloIntSet set1, IloIntSet set2);

IloNumSet IloExclude(IloNumSet set1, IloNumSet set2);

IloAnySet IloExclude(IloAnySet set1, IloAnySet set2);

IloIntSet IloSymExclude(IloIntSet set1, IloIntSet set2);

IloNumSet IloSymExclude(IloNumSet set1, IloNumSet set2);

IloAnySet IloSymExclude(IloAnySet set1, IloAnySet set2);

IloIntSet IloInter(IloIntSet set1, IloIntSet set2);

IloNumSet IloInter(IloNumSet set1, IloNumSet set2);

IloAnySet IloInter(IloAnySet set1, IloAnySet set2);

class IloOperatorException : public IloException {
private:
	IloObjectBase _cell;
	IloBool _isNext;
	IloBool _isCirc;

public:
	IloOperatorException( IloObjectBase value, IloBool isNext, IloBool isCirc = false );

	virtual ~IloOperatorException();

	const char* getOperatorName() const;
	IloObjectBase getItem() const { return _cell; }

	void printExpr(ILOSTD(ostream)& out) const;

	IloBool isTuple() const { return _cell.isTuple();}
	IloBool isInt() const { return _cell.isInt();}
	IloBool isNum() const { return _cell.isNum();}
	IloBool isExtractable() const { return _cell.isExtractable();}
	IloBool isSymbol() const { return _cell.isSymbol();}
	IloBool isDataCollection() const { return _cell.isIntCollection() || _cell.isNumCollection() || _cell.isAnyCollection() || _cell.isSymbolCollection() || _cell.isTupleCollection(); }
	IloBool isNext() const { return _isNext; }
	IloBool isCirc() const { return _isCirc; }
};

class IloCollectionElementNotFoundException : public IloOperatorException {
public:
	IloCollectionElementNotFoundException( IloObjectBase value, IloBool isNext, IloBool isCirc ) : IloOperatorException(value, isNext, isCirc){}
	virtual ~IloCollectionElementNotFoundException(){}
};

class IloOperatorNextElementNotFoundException : public IloOperatorException {
public:
	IloOperatorNextElementNotFoundException( IloObjectBase value, IloBool isNext ) : IloOperatorException(value, isNext){}
	virtual ~IloOperatorNextElementNotFoundException(){}
};

class IloFirstLastElementNotFoundException : public IloException {
	IloBool _isFirst;
public:
	IloFirstLastElementNotFoundException( IloBool _isFirst );
	virtual ~IloFirstLastElementNotFoundException();
	virtual const char* getOperatorName() const;
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual const char* getMessage() const ILO_OVERRIDE;
	IloBool isFirst() const { return _isFirst; }
};

class IloIndexOutOfBoundsException : public IloException {
	const IloDataCollectionI* _coll;
	const IloInt _index;
public:
	IloIndexOutOfBoundsException( const IloDataCollectionI* coll, const IloInt index );
	virtual ~IloIndexOutOfBoundsException();	
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	virtual const char* getMessage() const ILO_OVERRIDE;	

	const char* getName() const;
	IloInt getIndex() const;
};

IloIntSetI* IloUnion(IloIntSetI* set1, IloIntSetI* set2);
IloNumSetI* IloUnion(IloNumSetI* set1, IloNumSetI* set2);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
