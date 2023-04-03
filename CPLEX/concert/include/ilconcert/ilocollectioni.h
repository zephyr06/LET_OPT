// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilocollectioni.h
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
#ifndef __CONCERT_ilocollectioniH
#define __CONCERT_ilocollectioniH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilocollection.h>
#include <ilconcert/ilosys.h>
#include <ilconcert/ilohash.h>
#include <ilconcert/iloany.h>
#include <ilconcert/ilolinkedlist.h>

class IloInt2IndexHashTable;
class IloNum2IndexHashTable;
class IloAny2IndexHashTable;

class IloObjectBase;

class IloDataCollectionI;
class IloContinuousDataI;
class IloNumRangeI;
class IloDiscreteDataCollectionI;
class IloIntCollectionI;
class IloIntAbstractSetI;
class IloIntSetI;
class IloIntRangeI;
class IloNumCollectionI;
class IloNumAbstractSetI;
class IloNumSetI;
class IloAnyCollectionI;
class IloAnyAbstractSetI;
class IloAnySetI;

class IloDataCollection;
class IloContinuousData;
class IloNumRange;
class IloDiscreteDataCollection;
class IloIntCollection;
class IloIntAbstractSet;
class IloIntSet;
class IloIntRange;
class IloNumCollection;
class IloNumAbstractSet;
class IloNumSet;
class IloAnyCollection;
class IloAnyAbstractSet;
class IloAnySet;

class IloDataIterator;

class IloNumVarDomainI;
class IloIntVarDomainI;

class IloDataException : public IloException {
public:
	IloDataException(const char * m) : IloException(m) { }
};

class IloIntArray;
class IloNumArray;
class IloDataIterator;

class IloDataCollectionI : public IloOplBaseObjectI {
	ILORTTIDECL
		ILOEXTENSIONI
private:
	IloInt _lock;

public:
	virtual IloBool isSymbol() const ILO_OVERRIDE { return IloFalse; }

	virtual void setLockable(IloBool flag) {
		if( flag ) {
			_lock += 1;
		} else {
			_lock -= 1;
		}
		IloAssert( _lock >= 0, "lock cannot be negative" );
	}

	IloBool isLocked() {
		return _lock>0;
	}

	static void sortAsc(IloIntArray& array, IloInt min, IloInt max, IloIntArray* oldPosition = 0);
	static void sortAsc(IloIntArray& array);
	static void sortAsc(IloNumArray& array, IloInt min, IloInt max, IloIntArray* oldPosition = 0);
	static void sortAsc(IloNumArray& array);
	static void sortDesc(IloIntArray& array, IloInt min, IloInt max, IloIntArray* oldPosition = 0);
	static void sortDesc(IloIntArray& array);
	static void sortDesc(IloNumArray& array, IloInt min, IloInt max, IloIntArray* oldPosition = 0);
	static void sortDesc(IloNumArray& array);

public:

	IloDataCollectionI(IloEnv env);

	virtual ~IloDataCollectionI();
	virtual IloDataCollectionI* copy() const = 0;
	virtual IloDataCollection::IloDataType getDataType() const = 0;
	virtual IloBool isIntRange() const;
	virtual IloBool isNumRange() const;
	virtual IloBool isIntSet() const;
	virtual IloBool isNumSet() const;
	virtual IloBool isSymbolSet() const;
	virtual IloBool isAnySet() const;
	virtual IloBool isIntDataColumn() const;
	virtual IloBool isNumDataColumn() const;
	virtual IloBool isAnyDataColumn() const;
	virtual IloBool isTupleCollection() const;
	virtual IloBool isTupleSet() const;
	virtual IloBool isTupleRefColumn() const;
	virtual IloBool isIntCollectionColumn() const;
	virtual IloBool isNumCollectionColumn() const;
	virtual IloBool isAnyCollectionColumn() const;
	virtual IloBool isIntCollection() const { return IloFalse; }
	virtual IloBool isNumCollection() const { return IloFalse; }
	virtual IloBool isAnyCollection() const { return IloFalse; }
	virtual IloBool isMapAsCollection() const { return IloFalse; }
	virtual void display(ILOSTD(ostream)& os) const=0;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const = 0;
	virtual IloRttiEnvObjectI* makeOplClone(IloEnvI* env) const ILO_OVERRIDE {
		return (IloRttiEnvObjectI*)makeClone(env);
	}
};

inline ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out,IloDataCollectionI* dom) {
	if (dom){
		dom->display(out);
	}
	return (out);
}

class IloDiscreteDataCollectionI : public IloDataCollectionI {
	ILORTTIDECL
public:
	virtual void processBeforeFill() {}
	virtual void processAfterFill( IloBool  = IloFalse ) {}
	virtual const IloIntArray getOldIndexPositions() { return 0; }
	virtual void endOldIndexes() {}
	virtual void clearAllCaches() {}
	virtual void empty() {}
	virtual IloBool hasReference() const { return IloFalse; }
	virtual IloInt getSize() const = 0;

	virtual IloObjectBase getMapItem(IloInt idx) const = 0;
	IloDiscreteDataCollectionI(IloEnv env): IloDataCollectionI(env) {}
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const;
	virtual IloInt getMapItemIndex(IloObjectBase value) const;
	virtual IloInt getIndex(IloInt value) const;
	virtual IloInt getIndex(IloNum value) const;
	virtual IloInt getIndex(IloAny value) const;
	virtual IloInt getIndex(IloSymbol value) const;
	virtual IloInt getIntValue(IloInt idx) const;
	virtual IloNum getNumValue(IloInt idx) const;
	virtual IloAny getAnyValue(IloInt idx) const;
	IloSymbol getSymbolValue(IloInt idx) const;
	const char* getStringValue(IloInt idx) const;
	virtual IloBool supportDuplicates() const;
	virtual IloBool isSortedAsc() const { return IloFalse; }
	virtual IloBool isSortedDesc() const { return IloFalse; }
	virtual void sort(IloBool  = IloTrue) {}
	virtual IloBool isOplRefCounted() const ILO_OVERRIDE { return IloTrue; }
	IloDataCollection::SortSense getSortSense() const;
	IloBool hasSortingProp() const{ return isSortedAsc() || isSortedDesc(); }
	IloBool isSortedSameSense( const IloDiscreteDataCollectionI& set ) const {
		return hasSortingProp() && getSortSense() == set.getSortSense();
	}
	IloBool isReversed() const{ return isSortedDesc(); }
	IloBool isSorted() const{ return isSortedAsc(); }
	IloBool isOrdered() const{ return !isSorted() && !isReversed(); }
};

class IloContinuousDataI : public IloDataCollectionI {
	ILORTTIDECL
public:
	IloContinuousDataI(IloEnv env): IloDataCollectionI(env) {}
};

class IloNumRangeI : public IloContinuousDataI {
	ILORTTIDECL
protected:
	IloNum _min;
	IloNum _max;
public:
	IloVarDomainI* getSharedDomain() const;
	IloNumRangeI(IloEnv env, IloNum min, IloNum max);
	IloNumRangeI(IloNumRangeI* obj);

	virtual ~IloNumRangeI();
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	virtual IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	virtual IloBool isNumRange() const ILO_OVERRIDE;
	void setUB(IloNum val){
		_max = val;
	}
	void setLB(IloNum val){
		_min = val;
	}
	virtual void display(ILOSTD(ostream)& os) const ILO_OVERRIDE {
		os << "[";
		os << _min << " ," << _max;
		os << "]";
	}
	IloNum getLB() const {
		return _min;
	}
	IloNum getUB() const {
		return _max;
	}
	virtual IloBool isOplRefCounted() const ILO_OVERRIDE { return IloTrue; }
};

class IloIntCollectionI : public IloDiscreteDataCollectionI {
	ILORTTIDECL
public:
	IloIntCollectionI(IloEnv env): IloDiscreteDataCollectionI(env) {}
	virtual IloInt getIndex(IloSymbol s) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	virtual IloInt getIntValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloNum getNumValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloAny getAnyValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloIntArray getArray() const = 0;
	virtual IloBool contains(IloInt e) const=0;
	virtual IloInt getValue(IloInt index) const=0;
	virtual IloBool isIntCollection() const ILO_OVERRIDE { return IloTrue; }
};

class IloIntAbstractSetI : public IloIntCollectionI {
	ILORTTIDECL
public:
	IloIntAbstractSetI(IloEnv env): IloIntCollectionI(env) {}
	virtual IloInt getNext(IloInt value, IloInt n=1) const = 0;
	virtual IloInt getPrevious(IloInt value, IloInt n=1) const = 0;
	virtual IloInt getNextC(IloInt value, IloInt n=1) const = 0;
	virtual IloInt getPreviousC(IloInt value, IloInt n=1) const = 0;
	virtual IloInt getFirst() const = 0;
	virtual IloInt getLast() const = 0;
	virtual IloBool supportDuplicates() const ILO_OVERRIDE;
	virtual IloInt getLB() const;
	virtual IloInt getUB() const;
};

class IloIntAbstractDataColumnI : public IloIntCollectionI {
	ILORTTIDECL
public:
	IloIntAbstractDataColumnI(IloEnv env): IloIntCollectionI(env) {}
	virtual IloIntArray makeIndexArray(IloInt value) const = 0;
};

class IloNumCollectionI : public IloDiscreteDataCollectionI {
	ILORTTIDECL
public:
	IloNumCollectionI(IloEnv env): IloDiscreteDataCollectionI(env) {}
	virtual IloNumArray getArray() const = 0;
	virtual IloInt getIndex(IloSymbol s) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	virtual IloInt getIntValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloNum getNumValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloAny getAnyValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloBool contains(IloNum e) const=0;
	virtual IloNum getValue(IloInt index) const=0;
	virtual IloBool isNumCollection() const ILO_OVERRIDE { return IloTrue; }
};

class IloNumAbstractSetI : public IloNumCollectionI {
	ILORTTIDECL
public:
	IloNumAbstractSetI(IloEnv env): IloNumCollectionI(env) {}

	IloInt getIndex(IloSymbol s) const ILO_OVERRIDE;
	IloInt getIndex(IloAny s) const ILO_OVERRIDE;
	IloInt getIndex(IloNum s) const ILO_OVERRIDE;

	IloInt getIndex(IloInt value) const ILO_OVERRIDE;

	virtual IloNum getNext(IloNum value, IloInt n=1) const = 0;
	virtual IloNum getPrevious(IloNum value, IloInt n=1) const = 0;
	virtual IloNum getNextC(IloNum value, IloInt n=1) const = 0;
	virtual IloNum getPreviousC(IloNum value, IloInt n=1) const = 0;
	virtual IloNum getFirst() const = 0;
	virtual IloNum getLast() const = 0;
	virtual IloBool supportDuplicates() const ILO_OVERRIDE;
	virtual IloNum getLB() const;
	virtual IloNum getUB() const;
};

class IloNumAbstractDataColumnI : public IloNumCollectionI {
	ILORTTIDECL
public:
	IloNumAbstractDataColumnI(IloEnv env): IloNumCollectionI(env) {}
	virtual IloIntArray makeIndexArray(IloNum value) const = 0;
};

class IloAnyCollectionI : public IloDiscreteDataCollectionI {
	ILORTTIDECL
public:
	IloAnyCollectionI(IloEnv env): IloDiscreteDataCollectionI(env) {}
	virtual IloAnyArray getArray() const = 0;
	virtual IloInt getIndex(IloSymbol s) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	virtual IloInt getIntValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloNum getNumValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloAny getAnyValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloBool contains(IloAny e) const=0;
	virtual IloAny getValue(IloInt index) const=0;
	virtual IloBool isAnyCollection() const ILO_OVERRIDE { return IloTrue; }
};

class IloAnyAbstractSetI : public IloAnyCollectionI {
	ILORTTIDECL
		IloDataCollection::IloDataType _type;
public:
	IloDataCollection::IloDataType getType() const { return _type;}
	IloAnyAbstractSetI(IloEnv env): IloAnyCollectionI(env), _type(IloDataCollection::AnySet) {}
	void setType(IloDataCollection::IloDataType type){ _type = type; }
	virtual IloAny getNext(IloAny value, IloInt n=1) const = 0;
	virtual IloAny getPrevious(IloAny value, IloInt n=1) const = 0;
	virtual IloAny getNextC(IloAny value, IloInt n=1) const = 0;
	virtual IloAny getPreviousC(IloAny value, IloInt n=1) const = 0;
	virtual IloAny getFirst() const = 0;
	virtual IloAny getLast() const = 0;
	virtual IloBool supportDuplicates() const ILO_OVERRIDE;
};

class IloAnyAbstractDataColumnI : public IloAnyCollectionI {
	ILORTTIDECL
public:
	IloAnyAbstractDataColumnI(IloEnv env): IloAnyCollectionI(env) {}
	virtual IloIntArray makeIndexArray(IloAny value) const = 0;
};

class IloIntRangeI : public IloIntAbstractSetI {
	ILORTTIDECL
protected:
	IloInt _min;
	IloInt _max;
public:
	IloVarDomainI* getSharedDomain() const;
	virtual IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	virtual IloInt getNext(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getPrevious(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getNextC(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getPreviousC(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getFirst() const ILO_OVERRIDE;
	virtual IloInt getLast() const ILO_OVERRIDE;
	virtual IloInt getMapItemIndex(IloObjectBase value) const ILO_OVERRIDE;
	IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	IloInt getIndex(IloSymbol value) const ILO_OVERRIDE;
	IloInt getIntValue(IloInt idx) const ILO_OVERRIDE;
	IloIntArray getArray() const ILO_OVERRIDE;
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const ILO_OVERRIDE;
	IloIntRangeI(IloEnv env);
	IloIntRangeI(IloEnv env, IloInt min, IloInt max);
	IloIntRangeI(IloIntRangeI* obj);
	virtual ~IloIntRangeI();
	virtual IloBool isIntRange() const ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& os) const ILO_OVERRIDE;
	virtual IloInt getSize() const ILO_OVERRIDE;
	virtual IloObjectBase getMapItem(IloInt idx) const ILO_OVERRIDE;
	virtual IloInt getLB() const ILO_OVERRIDE {
		return _min;
	}
	virtual IloInt getUB() const ILO_OVERRIDE {
		return _max;
	}
	virtual void setLB(IloInt min);
	virtual void setUB(IloInt max);

	virtual IloBool contains(IloInt e) const ILO_OVERRIDE;
	virtual IloInt getValue(IloInt index) const ILO_OVERRIDE;

	virtual IloBool isSortedAsc() const ILO_OVERRIDE { return IloTrue; }
};

class IloIntSetI : public IloIntAbstractSetI {
	ILORTTIDECL
protected:
	IloBool _hasIndex;
	IloIntArray _array;
	IloInt2IndexHashTable* _hashForGetIndex;
public:
	void createHashTable();
	IloInt2IndexHashTable* getOrMakeHashTable();
	virtual void removeIndexes();
	void updateIndexes();
	virtual void clearAllCaches() ILO_OVERRIDE;
protected:
	void removeFromArray(IloInt elt);

public:
	void addWithoutCheck(IloInt elt);

public:
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;

	virtual IloInt getNext(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getPrevious(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getNextC(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getPreviousC(IloInt value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloInt getFirst() const ILO_OVERRIDE;
	virtual IloInt getLast() const ILO_OVERRIDE;
	virtual IloInt getMapItemIndex(IloObjectBase value) const ILO_OVERRIDE;
	IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	IloInt getIndex(IloSymbol value) const ILO_OVERRIDE;
	IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	IloInt getIntValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const ILO_OVERRIDE;
	IloIntSetI(IloEnvI* env, const IloIntArray array, IloBool withIndex = IloFalse);
	IloIntSetI(IloEnvI* env, IloBool withIndex = IloFalse);
	IloIntSetI(IloEnvI* env,  IloIntSetI* set);
	virtual ~IloIntSetI();
	IloBool hasIndex() const {
		return  _hasIndex;
	}
	IloIntArray getArray() const ILO_OVERRIDE {return _array;}
	IloInt getSize() const ILO_OVERRIDE;
	virtual IloObjectBase getMapItem(IloInt idx) const ILO_OVERRIDE;
	virtual void add(IloInt elt);
	virtual void addWithoutSort(IloInt elt) { add(elt); }
	virtual void add(IloIntSetI* set);
	void remove(IloInt elt);
	void remove(IloIntSetI* set);
	void setIntersection(IloInt elt);
	virtual void setIntersection(IloIntSetI* set);
	virtual void empty() ILO_OVERRIDE;
	virtual IloBool contains(IloInt elt) const ILO_OVERRIDE;
	virtual IloInt getValue(IloInt index) const ILO_OVERRIDE;
	IloBool contains(IloIntSetI* set) const;
	virtual IloBool intersects(IloIntSetI* set) const;
	IloNumSetI * toNumSet() const;
	IloAnySetI * toAnySet() const;
	IloIntArray toArray() const { return _array.copy(); }
	IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	IloBool isIntSet() const ILO_OVERRIDE;
	void display(ILOSTD(ostream)& os) const ILO_OVERRIDE;
	virtual IloIntSetI* makeDiff(IloIntSetI* set1, IloIntSetI* set2);
	void setHasIndex(IloBool flag) { _hasIndex = flag; }
};

class IloNumSetI : public IloNumAbstractSetI {
	ILORTTIDECL
protected:
	IloBool _hasIndex;
	IloNumArray _array;
	IloNum2IndexHashTable* _hashForGetIndex;
public:
	void createHashTable();
	IloNum2IndexHashTable* getOrMakeHashTable();

	virtual void removeIndexes();
	void updateIndexes();
	virtual void clearAllCaches() ILO_OVERRIDE;
protected:
	void removeFromArray(IloNum elt);

public:
	void addWithoutCheck(IloNum elt);

public:
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	virtual IloNum  getNext(IloNum value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloNum  getPrevious(IloNum value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloNum  getNextC(IloNum value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloNum  getPreviousC(IloNum value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloNum getFirst() const ILO_OVERRIDE;
	virtual IloNum getLast() const ILO_OVERRIDE;
	virtual IloInt getMapItemIndex(IloObjectBase value) const ILO_OVERRIDE;
	IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	IloInt getIndex(IloSymbol value) const ILO_OVERRIDE;
	IloNum getNumValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const ILO_OVERRIDE;
	IloNumSetI(IloEnvI* env, const IloNumArray array, IloBool withIndex = IloFalse);
	IloNumSetI(IloEnvI* env, IloBool withIndex = IloFalse);
	IloNumSetI(IloEnvI* env,  IloNumSetI* set);
	virtual ~IloNumSetI();
	IloBool hasIndex() const {
		return  _hasIndex;
	}
	IloNumArray getArray() const ILO_OVERRIDE {return _array;}
	IloInt getSize() const ILO_OVERRIDE;
	virtual IloObjectBase getMapItem(IloInt idx) const ILO_OVERRIDE;
	virtual void add(IloNum elt);
	virtual void addWithoutSort(IloNum elt) { add(elt); }
	virtual void add(IloNumSetI* set);
	void add(IloIntSetI* set);
	void remove(IloNum elt);
	void remove(IloNumSetI* set);
	void setIntersection(IloNum elt);
	virtual void setIntersection(IloNumSetI* set);
	virtual void empty() ILO_OVERRIDE;
	virtual IloBool contains(IloNum elt) const ILO_OVERRIDE;
	virtual IloNum getValue(IloInt index) const ILO_OVERRIDE;
	virtual IloBool contains(IloNumSetI* set) const;
	virtual IloBool intersects(IloNumSetI* set) const;
	IloIntSetI * toIntSet() const;
	IloNumArray toArray() const { return _array.copy(); }
	IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	IloBool isNumSet() const ILO_OVERRIDE;
	void display(ILOSTD(ostream)& os) const ILO_OVERRIDE;
	virtual IloNumSetI* makeDiff(IloNumSetI* set1, IloNumSetI* set2);
	void setHasIndex(IloBool flag) { _hasIndex = flag; }
};

class IloAnySetI : public IloAnyAbstractSetI {
	ILORTTIDECL
protected:
	IloBool _hasIndex;
	IloAnyArray _array;
	IloAny2IndexHashTable* _hashForGetIndex;
public:
	void createHashTable();
	IloAny2IndexHashTable* getOrMakeHashTable();
	virtual void removeIndexes();
	void updateIndexes();
	virtual void clearAllCaches() ILO_OVERRIDE;
protected:
	void removeFromArray(IloAny elt);
public:
	void addWithoutCheck(IloAny elt);
public:
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	virtual IloAny getNext(IloAny value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloAny getPrevious(IloAny value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloAny getNextC(IloAny value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloAny getPreviousC(IloAny value, IloInt n=1) const ILO_OVERRIDE;
	virtual IloAny getFirst() const ILO_OVERRIDE;
	virtual IloAny getLast() const ILO_OVERRIDE;
	IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	virtual IloInt getMapItemIndex(IloObjectBase value) const ILO_OVERRIDE;
	IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	IloInt getIndex(IloSymbol value) const ILO_OVERRIDE;
	IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	IloAny getAnyValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const ILO_OVERRIDE;
	IloAnySetI(IloEnvI* env, const IloAnyArray array, IloBool withIndex = IloFalse);
	IloAnySetI(IloEnvI* env, IloBool withIndex = IloFalse);
	IloAnySetI(IloEnvI* env,  IloAnySetI* set);
	virtual ~IloAnySetI();
	IloBool hasIndex() const {
		return  _hasIndex;
	}
	IloAnyArray getArray() const ILO_OVERRIDE {return _array;}
	IloInt getSize() const ILO_OVERRIDE;
	virtual IloObjectBase getMapItem(IloInt idx) const ILO_OVERRIDE;
	virtual void add(IloAny elt);
	virtual void add(IloAnySetI* set);
	void remove(IloAny elt);
	void remove(IloAnySetI* set);
	void setIntersection(IloAny elt);
	void setIntersection(IloAnySetI* set);
	virtual void empty() ILO_OVERRIDE;
	virtual IloBool contains(IloAny elt) const ILO_OVERRIDE;
	virtual IloAny getValue(IloInt index) const ILO_OVERRIDE;
	IloBool contains(IloAnySetI* set) const;
	IloBool intersects(IloAnySetI* set) const;
	IloNumSetI * toNumSet() const;
	IloIntSetI * toIntSet() const;
	IloAnyArray toArray() const { return _array.copy(); }
	IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	IloBool isAnySet() const ILO_OVERRIDE;
	IloBool isSymbolSet() const ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& os) const ILO_OVERRIDE;
	virtual IloAnySetI* makeDiff(IloAnySetI* set1, IloAnySetI* set2);
	virtual void addWithoutSort(IloAny elt) { add(elt); }
	void setHasIndex(IloBool flag) { _hasIndex = flag; }
};

class IloIntArrayAsCollectionI : public IloIntCollectionI {
	ILORTTIDECL
private:
	IloIntArray _array;
public:
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	IloIntArrayAsCollectionI(IloEnvI* env, const IloIntArray array);
	virtual ~IloIntArrayAsCollectionI();
	virtual IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	virtual IloInt getSize() const ILO_OVERRIDE;
	virtual IloObjectBase getMapItem(IloInt idx) const ILO_OVERRIDE;
	virtual IloIntArray getArray() const ILO_OVERRIDE;
	virtual IloBool contains(IloInt e) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloSymbol value) const ILO_OVERRIDE;
	virtual IloInt getValue(IloInt index) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	virtual IloInt getIntValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& os) const ILO_OVERRIDE;
};

class IloNumArrayAsCollectionI : public IloNumCollectionI {
	ILORTTIDECL
private:
	IloNumArray _array;
public:
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	IloNumArrayAsCollectionI(IloEnvI* env, const IloNumArray array);
	virtual ~IloNumArrayAsCollectionI();
	virtual IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	virtual IloInt getSize() const ILO_OVERRIDE;
	virtual IloObjectBase getMapItem(IloInt idx) const ILO_OVERRIDE;
	virtual IloNumArray getArray() const ILO_OVERRIDE;
	virtual IloBool contains(IloNum e) const ILO_OVERRIDE;
	virtual IloNum getValue(IloInt index) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloSymbol value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	virtual IloNum getNumValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& os) const ILO_OVERRIDE;
};

class IloAnyArrayAsCollectionI : public IloAnyCollectionI {
	ILORTTIDECL
private:
	IloAnyArray _array;
public:
	virtual IloDataCollectionI* copy() const ILO_OVERRIDE;
	virtual IloDataCollectionI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	IloAnyArrayAsCollectionI(IloEnvI* env, const IloAnyArray array);
	virtual ~IloAnyArrayAsCollectionI();
	virtual IloDataCollection::IloDataType getDataType() const ILO_OVERRIDE;
	virtual IloInt getSize() const ILO_OVERRIDE;
	virtual IloObjectBase getMapItem(IloInt idx) const ILO_OVERRIDE;
	virtual IloAnyArray getArray() const ILO_OVERRIDE;
	virtual IloBool contains(IloAny e) const ILO_OVERRIDE;
	virtual IloAny getValue(IloInt index) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloInt value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloNum value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloSymbol value) const ILO_OVERRIDE;
	virtual IloInt getIndex(IloAny value) const ILO_OVERRIDE;
	virtual IloAny getAnyValue(IloInt idx) const ILO_OVERRIDE;
	virtual IloDataIterator* iterator(IloGenAlloc* heap) const ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)& os) const ILO_OVERRIDE;
};

class IloDummyIntDataIterator : public IloIntDataIterator {
public:
	IloDummyIntDataIterator(IloGenAlloc* heap);
	virtual ~IloDummyIntDataIterator();
	virtual IloBool next() ILO_OVERRIDE;
};
class IloDummyNumDataIterator : public IloNumDataIterator {
public:
	IloDummyNumDataIterator(IloGenAlloc* heap);
	virtual ~IloDummyNumDataIterator();
	virtual IloBool next() ILO_OVERRIDE;
};

class IloDummyAnyDataIterator : public IloAnyDataIterator {
public:
	IloDummyAnyDataIterator(IloGenAlloc* heap);
	virtual ~IloDummyAnyDataIterator();
	virtual IloBool next() ILO_OVERRIDE;
};

class IloDataChangeListenerI : public IloRttiEnvObjectI {
	ILORTTIDECL
		friend class IloDataChangeNotifierI;
protected :
	virtual void change() const = 0;
public:
	IloDataChangeListenerI(IloEnvI* env) : IloRttiEnvObjectI(env) {}
	virtual ~IloDataChangeListenerI() {};
};

class IloDataChangeNotifierI : public IloRttiEnvObjectI {
	ILORTTIDECL
protected:
	class Listener {
	public:
		Listener(IloDataChangeListenerI* listener, Listener* next) : _listener(listener), _next(next) {}
		IloDataChangeListenerI* _listener;
		Listener* _next;
	};
	Listener* _listeners;
protected:
	virtual void change() const;
public:
	IloDataChangeNotifierI(IloEnvI*);
	void registerListener(IloDataChangeListenerI*);
	void unregisterListener(IloDataChangeListenerI*);
};

class IloSortSetPropertyI : public IloNamedPropertyI {
	ILORTTIDECL
		static const char* SortSet;

public:
	IloDataCollection::SortSense _sense;

	IloSortSetPropertyI(IloMemoryManager env, IloDataCollection::SortSense sense)
		: IloNamedPropertyI(env), _sense(sense){};
	~IloSortSetPropertyI(){}
	IloBool isAsc() const{
		return _sense == IloDataCollection::ASC;
	}
	IloBool isDesc() const{
		return _sense == IloDataCollection::DESC;
	}
	IloNamedPropertyI * makeClone(IloMemoryManager) const ILO_OVERRIDE{
		throw IloWrongUsage("can not clone this property");
		ILOUNREACHABLE(return 0;)
	}
	const char * getName() const ILO_OVERRIDE { return IloSortSetPropertyI::SortSet; }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

