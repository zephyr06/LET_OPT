// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilomapi.h
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

#ifndef __CONCERT_ilomapiH
#define __CONCERT_ilomapiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilocollection.h>
#include <ilconcert/ilotupleset.h>
#include <ilconcert/iloextractable.h>
#include <ilconcert/ilosymbol.h>

class IloMapI;
class IloAbstractTupleMapI;
class IloMapVectorIndexI;
class IloAbstractMapI;

#ifdef CPPREF_GENERATION
typedef IloArray<IloDiscreteDataCollection> IloMapIndexerBase;

class IloMapIndexer : public IloMapIndexerBase {
	
	IloMapIndexer(IloArrayI* i=0);
	
	IloMapIndexer(const IloMapIndexer& cpy);
	
	IloMapIndexer(const IloEnv env, IloInt n = 0);
};

#else

typedef IloArray<IloDiscreteDataCollection> IloMapIndexer;
#endif

class IloObjectBase {
	friend class IloMapI;
public:
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	enum Type {
		Anonymous=0,
		Extractable=1,
		IntConst =4,
		IntExpr  =5,
		IntIndex =7,
		NumConst =8,
		NumExpr  =9,
		NumIndex =11,
		SymbolConst=16,
		SymbolExpr = 17,
		SymbolIndex=19,
		TupleConst=32,
		TupleExpr = 33,
		TupleIndex=35,
		TuplePatternConst=64,
		TuplePattern=67,
		IntCollectionConst=128,
		IntCollectionExpr=129,
		IntCollectionIndex=131,
		NumCollectionConst=256,
		NumCollectionExpr=257,
		NumCollectionIndex=259,
		SymbolCollectionConst=512,
		SymbolCollectionExpr=513,
		SymbolCollectionIndex=515,
		TupleCollectionConst=1024,
		TupleCollectionExpr=1025,
		StringConst=2048,
		ConstraintExpr=4097,
		SubMap=32768,
		IntervalExpr=65537,
		PiecewiseFunctionExpr=131073,
		SequenceExpr=262145,
		CumulFunctionExpr=524289,
		StateFunctionExpr=1048577
	};
protected:
	Type   _type;
	union {
		IloInt _int;
		IloNum _num;
		IloAny _any;
	} _data;
	IloObjectBase(){
		_type = Anonymous;
		_data._int = 0;
	}
public:
	virtual ~IloObjectBase(){	}
	IloBool operator== (const IloObjectBase& i);

	IloObjectBase(IloObjectBase::Type t, IloAny e);
	
	IloObjectBase(const IloObjectBase& i);

	
	IloObjectBase(IloInt x);
	
	IloObjectBase(IloNum x);
	
	IloObjectBase(const char* x);
	
	IloObjectBase(IloSymbol x);
	IloObjectBase(IloIntCollection x);
	IloObjectBase(IloNumCollection x);
	
	IloObjectBase(IloAnyCollection x);
	IloObjectBase(IloNumRange x);

	
	Type getType() const { return _type; }

	IloObjectBase getClone(IloEnvI* env);
	void operator= (const IloObjectBase& i);
	
	IloBool isAnonymous() const { return !_type; }
	
	IloBool isConstant() const { return !(_type & 1); }
	
	IloBool isExtractable() const { return (_type & 1) || _type == 64; }
	
	IloBool isIndex() const { return (_type & 2); }
	
	IloBool isInt() const { return (_type & 4); }
	
	IloBool isNum() const { return (_type & 8); }
	
	IloBool isSymbol() const { return (_type & 16); }
	
	IloBool isTuple() const { return (_type & 32); }
	
	IloBool isPattern() const { return (_type & 64); }
	
	IloBool isIntCollection() const { return (_type & 128); }
	
	IloBool isNumCollection() const { return (_type & 256); }
	
	IloBool isAnyCollection() const { return isTupleCollection() || isSymbolCollection(); }
	
	IloBool isSymbolCollection() const { return (_type & 512); }
	IloBool isTupleCollection() const { return (_type & 1024); }
	
	IloBool isString() const { return (_type & 2048); }
	
	IloBool isConstraint() const { return (_type & 4096); }
	
	IloBool isSubMap() const { return (_type & 32768); }
	IloBool isInterval() const { return (_type & 65536); }
	IloBool isPiecewiseFunction() const { return (_type & 131072); }
	IloBool isSequence() const { return (_type & 262144); }
	IloBool isCumulFunctionExpr() const { return (_type & 524288); }
	IloBool isStateFunctionExpr() const { return (_type & 1048576); }
	
	IloBool isAny() const{ return (!isInt() && !isNum());}
	
	IloBool isTypeCompatible(Type t) const;

	
	IloInt asInt() const {
		IloAssert(_type == IntConst, "Map Item is not an integer.");
		return _data._int;
	}
	void getValue(IloInt& res) { res = asInt(); }
	
#ifdef ILO_WIN64
	IloNum asNum() const;
#else
	IloNum asNum() const {
		if (_type == NumConst) return _data._num;
		IloAssert(_type == IntConst, "Map Item is not a double nor an int.");
		IloNum res = IloNum(_data._int);
		return res;
	}
#endif
	void getValue(IloNum& res) {  res = asNum(); }
	
	const char* asString() const {
		if (isString()) return (const char*)_data._any;
		else return asSymbol().getImpl()->getString();
	}
	
	IloSymbol asSymbol() const {
		IloAssert(_type == SymbolConst, "Map Item is not a symbol.");
		return (IloSymbolI*)_data._any;
	}
	IloAny asAny() const{
		IloAssert((_type != NumConst) && (_type != IntConst), "Map Item can not be used as an IloAny");
		return _data._any;
	}
	
	IloExtractable asExtractable() const {
		IloAssert(isExtractable(), "Map Item is not an extractable.");
		return (IloExtractableI*)_data._any;
	}

	void display(ILOSTD(ostream)& out) const;

	static Type GetType(IloInt) { return IntConst; }
	static Type GetType(IloNum) { return NumConst; }
	static Type GetType(IloSymbol) { return SymbolConst; }
	static Type GetType(IloAnyCollection e) {
		if (!e.getImpl()) return Anonymous;
		if (e.getImpl()->isTupleCollection())
			return TupleCollectionConst;
		return SymbolCollectionConst;
	}
};

inline ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloObjectBase &) {
	out << "display of map item is not available";
	return (out);
}

class IloForAllRangePropI : public IloNamedPropertyI {
	ILORTTIDECL
		static const char* ForAllRange;
	IloArray<IloObjectBase> _indexes;
public:
	IloForAllRangePropI(IloMemoryManager env) : IloNamedPropertyI(env), _indexes(env){};
	IloForAllRangePropI(IloMemoryManager env, IloArray<IloObjectBase> indexes) : IloNamedPropertyI(env){
		_indexes = indexes.copy();
	};
	~IloForAllRangePropI(){	
		if (_indexes.getImpl()) _indexes.end();
	}
	IloNamedPropertyI * makeClone(IloMemoryManager env) const ILO_OVERRIDE {
		return new (env) IloForAllRangePropI(env, _indexes);
	}
	const char * getName() const ILO_OVERRIDE { 
		return IloForAllRangePropI::ForAllRange; 
	}
	void add(IloObjectBase obj){
		_indexes.add(obj);
	}
	IloObjectBase getItem(IloInt index) const{
		IloTestAndRaise(_indexes.getImpl() && index < _indexes.getSize(), "empty handle or error using the array");
		return _indexes[index];
	}
	IloInt getNbIndexes() const{
		return (_indexes.getImpl() ? _indexes.getSize() : 0);
	}
	void reset(){ _indexes = 0; }
	IloArray<IloObjectBase> getItems() const{ return _indexes; }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
