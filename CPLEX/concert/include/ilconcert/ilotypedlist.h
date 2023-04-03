// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilotypedlist.h
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

#ifndef __CONCERT_ilotypedlistH
#define __CONCERT_ilotypedlistH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloextractable.h>

typedef enum {
	IloHashStatusOk=0,
	IloHashStatusNotFound=1,
	IloHashStatusDuplicatedKey=2
} IloHashStatus;

class IloSimpleHashTable {
protected:
	struct Item {
		IloAny _key;
		IloAny _value;
		Item*  _next;
		Item(IloAny k, IloAny v, Item* next=0);
	};
	IloEnvI* _env;
	Item*    _last;
	IloInt   _hSize;
	Item **  _buckets;
	Item     _nil;
	IloInt   _entries;
	inline IloInt hashFunction(IloAny key) const {
		return (((std::ptrdiff_t)key)>>3)&(_hSize-1);
	}
	IloHashStatus checkDuplicatedKey();

public:
	IloSimpleHashTable(const IloEnv env, IloInt size=64);
	~IloSimpleHashTable();
	void clear();
	void end();
	IloBool isEnded() const { return (_buckets==0); }
	IloInt getSize() const { return _entries; }

	
	void add(IloAny key, IloAny value);
	
	IloHashStatus addWithCheck(IloAny key, IloAny value);
	
	IloHashStatus remove(IloAny key);
	
	IloBool isIn(IloAny key);
	
	IloHashStatus getValue(IloAny key, IloAny& value);
	IloAny operator[](IloAny key);

	class Iterator;
	friend class Iterator; 

	class Iterator {
	private:
		IloInt _index;
		Item* const * _table;
		const Item* _nil;
		Item* _item;
	public:
		Iterator(const IloSimpleHashTable& table);
		IloBool ok() const;
		void operator++();
		IloAny operator* ();
	};
};

class IloSimpleHashTable2 : public IloSimpleHashTable {
protected:
	IloInt _nextResize;
	void reHash();
public:
	IloSimpleHashTable2(const IloEnv env,
		IloInt size=64, IloInt AverageHashedListSize=32)
		: IloSimpleHashTable(env,size)
		, _nextResize(_hSize*AverageHashedListSize) {}

	IloInt getHashTableSize() const { return _hSize; }

	void add(IloAny key, IloAny value);
	IloHashStatus addWithCheck(IloAny key, IloAny value);
};

class IloTypedList {
	friend class IloTypedListManagerI;
private:
	
	IloTypeInfo   _type;
	
	IloTypedList*       _father;
	
	IloTypedList*       _firstSubType;
	
	IloTypedList*       _next;
	
	IloExtractableList  _extractables;

public:
	IloTypeInfo getType() const { return _type; }

private:
	
	IloTypedList(IloTypeInfo);
	
	~IloTypedList();

	
	void setFather(IloTypedList* father);
	
	void setFirstSubType(IloTypedList* sub);
	
	void setNext(IloTypedList* next);

public:
	
	void add(IloExtractableI* ext);
	
	void remove(IloExtractableI* ext);

	const IloExtractableList& getExtractables() const { return _extractables; }
	IloTypedList* getFather() const { return _father; }
	IloTypedList* getFirstSubType() const { return _firstSubType; }
	IloTypedList* getNext() const { return _next; }
	void addSubList(IloTypedList*);
};

class IloTypedListManagerI {
private:
	IloEnvI* _env;

	
	IloBool    _cleared;

	IloExtractableList _unclassified; 
	IloSimpleHashTable _classified;

private:

	friend class IloTypedListManager;

	IloTypedListManagerI(IloEnvI*);
	~IloTypedListManagerI();

	void classifyAll();
	void classify(); 
	void insert(IloExtractableI*);  

	IloTypedList* getOrBuildList(IloTypeInfo); 

	void add(IloExtractableI* ext);
	void remove(IloExtractableI* ext);

public:
	void clear(); 
	class IloTypedList* getList(IloTypeInfo);
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif  
