// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilocollection.h
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

#ifndef __CONCERT_ilocollectionH
#define __CONCERT_ilocollectionH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#if !defined(__CONCERT_iloenvH)
#include <ilconcert/iloenv.h>
#endif

class IloIntRangeI;
class IloNumRangeI;
class IloAnyCollectionI;
class IloDataCollectionI;
class IloDiscreteDataCollectionI;
class IloDataIterator;
class IloIntDataIterator;
class IloNumDataIterator;
class IloAnyDataIterator;
class IloContinuousDataI;
class IloIntCollectionI;
class IloNumCollectionI;
class IloAnyCollectionI;

class IloIntRange;
class IloNumRange;
class IloAnySet;
class IloIntSet;
class IloNumSet;

class IloObjectBase;

class IloDataCollection {
protected:
	IloDataCollectionI* _impl;
public:
	
	enum IloDataType {
		UndefinedData = 0,
		IntData = 1,
		NumData = 2,
		SymbolSet = 3,
		IntSet = 4,
		NumSet = 5,
		AnySet = 6,
		IntDataColumn = 7,
		NumDataColumn = 8,
		AnyDataColumn = 9,
		SymbolDataColumn = 10,
		TupleCollection = 11,
		TupleSet = 12,
		IntCollectionColumn = 13,
		NumCollectionColumn = 14,
		AnyCollectionColumn = 15,
		TupleRefColumn = 16,
		IntArray = 21,
		NumArray = 22,
		AnyArray = 23,
		SymbolArray = 24
	};

	enum SortSense {
		ORDERED = 10,
		ASC = 11,
		DESC = 12
	};

	class ImmutableException : public IloException {
	private:
		IloDataCollectionI* _coll;
	public:
		ImmutableException(IloDataCollectionI* coll);
		virtual const char* getMessage() const ILO_OVERRIDE;
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
		~ImmutableException();
		IloDataCollectionI* getCollection() const { return _coll; }
	};

	
	IloEnv getEnv();

	
	static void sort(IloIntArray& array, IloInt min, IloInt max);

	
	static void sort(IloIntArray& array);

	
	static void sort(IloNumArray& array, IloInt min, IloInt max);

	
	static void sort(IloNumArray& array);

	
	IloDataCollection();

	
	IloDataCollection(IloDataCollectionI* impl);

	
	IloDataCollectionI* getImpl() const;

	
	void end();

	
	IloDataCollection::IloDataType getDataType() const;

	
	IloBool isIntRange() const;

	
	IloBool isNumRange() const;

	
	IloBool isIntSet() const;

	
	IloBool isNumSet() const;

	
	IloBool isSymbolSet() const;

	
	IloBool isIntDataColumn() const;

	
	IloBool isNumDataColumn() const;

	
	IloBool isAnyDataColumn() const;

	
	IloBool isAnySet() const;

	
	IloBool isTupleSet() const;

	
	IloBool isIntCollectionColumn() const;

	
	IloBool isNumCollectionColumn() const;

	
	IloBool isAnyCollectionColumn() const;

	
	void display(ILOSTD(ostream)& os) const;

	
	const char* getName() const;
};

ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloDataCollection& dom);

class IloDiscreteDataCollection : public IloDataCollection {
public:
	
	IloInt getSize() const;

	
	IloDiscreteDataCollection();

	
	IloDiscreteDataCollection(IloDiscreteDataCollectionI* impl);

	
	IloDataIterator* iterator() const;

	
	IloDiscreteDataCollectionI* getImpl() const;

	
	IloInt getIndex(IloInt value) const;

	
	IloInt getIndex(IloNum value) const;

	
	IloInt getIndex(IloAny value) const;

	
	IloInt getIndex(IloSymbol value) const;

	
	IloInt getIndex(const char* value) const;
	
	IloObjectBase getMapItem(IloInt idx) const;

	
	IloBool isReversed() const;
	
	IloBool isSorted() const;
	
	IloBool isOrdered() const;
};

class IloContinuousData : public IloDataCollection {
public:
	
	IloContinuousData(IloContinuousDataI* impl);

	IloContinuousData();

	
	IloContinuousDataI* getImpl() const;
};

class IloIntCollection : public IloDiscreteDataCollection {
public:
	
	IloIntCollection(IloIntCollectionI* impl);

	IloIntCollection();

	

	
	IloIntCollectionI* getImpl() const;

	
	IloInt getValue(IloInt index) const;
};

class IloNumCollection : public IloDiscreteDataCollection {
public:
	
	IloNumCollection(IloNumCollectionI* impl);

	IloNumCollection();

	

	
	IloNumCollectionI* getImpl() const;

	
	IloNum getValue(IloInt index) const;
};

class IloAnyCollection : public IloDiscreteDataCollection {
public:
	
	IloAnyCollection(IloAnyCollectionI* impl);

	IloAnyCollection();

	

	
	IloAnyCollectionI* getImpl() const;

	IloAny getValue(IloInt index) const;
};

class IloDataIterator : public IloMemoryManagerObjectI {
protected:
	const IloDiscreteDataCollectionI* _coll;
	IloBool _ok;
public:
	virtual void setCollection(const IloDiscreteDataCollectionI* coll);
	
	virtual IloBool next() = 0;

	
	virtual void reset(IloBool catchInvalidCollection = IloFalse);

	
	IloDataIterator(IloGenAlloc* heap, const IloDiscreteDataCollectionI* coll);

	
	IloDataIterator(const IloDiscreteDataCollectionI* coll = 0);

	
	IloDataIterator(const IloDiscreteDataCollection coll);

	virtual ~IloDataIterator();

	virtual IloBool isOpen() const;

	
	IloGenAlloc* getHeap() const { return _mem; }

	
	const IloDiscreteDataCollectionI* getCollection() const { return _coll; }

	
	IloBool ok() const {
		return _ok;
	}

	void setNotOk() { _ok = IloFalse; }

	
	void operator++() {
		_ok = next();
	}
};

class IloIntDataIterator : public IloDataIterator {
protected:
	IloInt _wmin;
	IloInt _wmax;
	IloInt _curr;
	IloInt _xcurr;
	void checkMinMax(IloInt min, IloInt max);
public:
	
	IloIntDataIterator(IloGenAlloc* heap, const IloIntCollectionI* coll = 0,
		IloInt wmin=IloIntMin, IloInt wmax=IloIntMax);
	
	IloIntDataIterator(const IloIntCollectionI* coll = 0,
		IloInt wmin=IloIntMin, IloInt wmax=IloIntMax);

	virtual ~IloIntDataIterator();
	
	virtual IloBool next() ILO_OVERRIDE = 0;

	
	IloInt operator*() {
		return _curr;
	}
	
	virtual IloInt recomputeMin() const;
	
	virtual IloInt recomputeMax() const;
	
	virtual void reset(IloBool catchInvalidCollection = IloFalse) ILO_OVERRIDE;
	
	virtual void reset(IloInt wmin, IloInt wmax, IloBool catchInvalidCollection = IloFalse);
};

class IloNumDataIterator : public IloDataIterator {
protected:
	IloNum _wmin;
	IloNum _wmax;
	IloNum _curr;
	IloInt _xcurr;
	void checkMinMax(IloNum min, IloNum max);
public:
	
	IloNumDataIterator(IloGenAlloc* heap, const IloNumCollectionI* coll = 0);
	
	IloNumDataIterator(const IloNumCollectionI* coll = 0);

	virtual ~IloNumDataIterator();
	
	virtual IloBool next() ILO_OVERRIDE = 0;

	
	IloNum operator*() { return _curr; }

	
	virtual void reset(IloBool catchInvalidCollection = IloFalse) ILO_OVERRIDE;
	
	virtual void reset(IloNum wmin, IloNum wmax, IloBool catchInvalidCollection = IloFalse);
	
	virtual IloNum recomputeLB() const;
	
	virtual IloNum recomputeUB() const;
};

class IloAnyDataIterator : public IloDataIterator {
protected:
	IloAny _curr;
	IloInt _xcurr;
public:
	
	IloAnyDataIterator(IloGenAlloc* heap, const IloAnyCollectionI* coll = 0);
	IloAnyDataIterator(const IloAnyCollectionI* coll = 0);

	virtual ~IloAnyDataIterator();
	
	virtual IloBool next() ILO_OVERRIDE = 0;

	
	IloAny operator*() { return _curr; }

	
	virtual void reset(IloBool catchInvalidCollection = IloFalse) ILO_OVERRIDE;
	
	virtual void reset(IloAny value, IloBool catchInvalidCollection = IloFalse);
	void setCurrentToNull(){ _curr = 0; }
};

class IloIntDefaultDataIterator : public IloIntDataIterator {
public:
	
	IloIntDefaultDataIterator(const IloIntCollectionI* coll = 0,
		IloInt wmin=IloIntMin, IloInt wmax=IloIntMax)
		: IloIntDataIterator(coll, wmin, wmax){}
		
		IloIntDefaultDataIterator(IloGenAlloc* alloc, const IloIntCollectionI* coll = 0,
			IloInt wmin=IloIntMin, IloInt wmax=IloIntMax)
			: IloIntDataIterator(alloc, coll, wmin, wmax){}
			
			virtual IloBool next() ILO_OVERRIDE;
			
			virtual IloInt recomputeMin() const ILO_OVERRIDE;
			
			virtual IloInt recomputeMax() const ILO_OVERRIDE;
};

class IloNumDefaultDataIterator : public IloNumDataIterator {
public:
	
	IloNumDefaultDataIterator(const IloNumCollectionI* coll = 0) : IloNumDataIterator(coll){}
	
	IloNumDefaultDataIterator(IloGenAlloc* alloc, const IloNumCollectionI* coll = 0) : IloNumDataIterator(alloc, coll){}
	
	virtual IloBool next() ILO_OVERRIDE;
	virtual IloNum recomputeLB() const ILO_OVERRIDE;
	virtual IloNum recomputeUB() const ILO_OVERRIDE;
};

class IloAnyDefaultDataIterator : public IloAnyDataIterator {
public:
	
	IloAnyDefaultDataIterator(const IloAnyCollectionI* coll = 0)
		: IloAnyDataIterator(coll){}
		
		IloAnyDefaultDataIterator(IloGenAlloc* alloc, const IloAnyCollectionI* coll = 0)
			: IloAnyDataIterator(alloc, coll){}
			
			virtual IloBool next() ILO_OVERRIDE;
};

class IloIntRange : public IloIntCollection {
public:
	
	IloIntRangeI* getImpl() const;

	IloIntRange();

	
	IloInt getFirst() const;

	
	IloInt getLast() const;

	
	IloInt getNext(IloInt value, IloInt n=1) const;

	
	IloInt getPrevious(IloInt value, IloInt n=1) const;

	
	IloInt getNextC(IloInt value, IloInt n=1) const;

	
	IloInt getPreviousC(IloInt value, IloInt n=1) const;

	
	IloIntRange(IloIntRangeI* impl);

	
	IloIntRange(IloEnv env);

	
	IloIntRange(IloEnv env, IloInt min, IloInt max);

	
	IloIntRange(const IloIntRange& obj);

	
	IloInt getLB() const;

	
	IloInt getUB() const;

	
	class Iterator : public IloIntDataIterator {
	public:
		Iterator(IloGenAlloc* heap, const IloIntRangeI* coll,
			IloInt wmin=IloIntMin, IloInt wmax=IloIntMax);

		Iterator(const IloIntRangeI* coll,
			IloInt wmin=IloIntMin, IloInt wmax=IloIntMax);

		
		Iterator(const IloIntRange coll,
			IloInt wmin=IloIntMin, IloInt wmax=IloIntMax);

		virtual ~Iterator();

		IloInt recomputeMin() const ILO_OVERRIDE;
		IloInt recomputeMax() const ILO_OVERRIDE;

		virtual void reset(IloBool catchInvalidCollection = IloFalse) ILO_OVERRIDE;

		virtual void reset(IloInt wmin, IloInt wmax, IloBool catchInvalidCollection = IloFalse) ILO_OVERRIDE;

		
		IloInt operator*() { return _curr; }

		virtual IloBool next() ILO_OVERRIDE;
	};
};

typedef IloIntRange::Iterator IloIntRangeIterator;

class IloNumRange : public IloContinuousData {
public:
	IloNumRange();

	
	IloNumRange(IloNumRangeI* impl);

	
	IloNumRange(IloEnv env, IloNum min, IloNum max);

	
	IloNumRange(IloEnv env);

	
	IloNumRange(const IloNumRange& obj);

	
	IloNum getLB() const;

	
	IloNum getUB() const;

	
	IloNumRangeI* getImpl() const;
};

typedef IloArray<IloDataCollection> IloDataCollectionArrayBase;

class IloDataCollectionArray : public IloDataCollectionArrayBase {
public:
	typedef IloDefaultArrayI ImplClass;
	IloDataCollectionArray(IloDefaultArrayI* i=0) : IloDataCollectionArrayBase(i) {}
	
	IloDataCollectionArray(const IloDataCollectionArray& copy) : IloDataCollectionArrayBase(copy) {}
	
	IloDataCollectionArray(const IloEnv env, IloInt n = 0) : IloDataCollectionArrayBase(env, n) {}
	IloDataCollectionArray(const IloMemoryManager env, IloInt n = 0) : IloDataCollectionArrayBase(env, n) {}

	
	IloDataCollectionArray(const IloEnv env, IloInt n, const IloDataCollection v0);
};

class IloCollectionHelper{
public:
	
	static IloIntRange asIntRange(IloDiscreteDataCollection);

	
	static IloNumRange asNumRange(IloDiscreteDataCollection);

	
	static IloIntSet asIntSet(IloDiscreteDataCollection);

	
	static IloNumSet asNumSet(IloDiscreteDataCollection);

	
	static IloAnySet asAnySet(IloDiscreteDataCollection);
};

IloBool operator == (const IloIntCollection, const IloIntCollection);
IloBool operator == (const IloNumCollection, const IloNumCollection);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

