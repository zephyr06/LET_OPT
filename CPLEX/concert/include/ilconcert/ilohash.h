// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilohash.h
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

#ifndef __CONCERT_ilohashH
#define __CONCERT_ilohashH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloenv.h>

const IloInt IloDefaultHashSize=31;

template <class KeyType, class ValueType>
class IloHashTable 
{
public:
	typedef IloInt (*IloHashFunction) (KeyType, IloInt);
	typedef IloBool (*IloCompFunction) (KeyType, KeyType);
	enum Status {ILOHASHOK=0, ILOHASHNOTFOUND=1, ILOHASHDUPLICATEDKEY=2};

private:
	IloInt          _size;
	IloInt          _mod;
	IloHashFunction _hashFct;
	IloCompFunction _compFct;

public:
	class Item {
		Item*   _next;
		KeyType _key;
		ValueType _value;
	public:
		Item(KeyType key, ValueType value)
			: _next(0), _key(key), _value(value) {}
		Item*     getNext()  const          { return (_next); }
		KeyType   getKey()   const          { return (_key); }
		ValueType getValue() const          { return (_value); }
		void setNext(Item* next)            { _next  = next; }
		void setKey(KeyType key)            { _key   = key; }
		void setValue(ValueType val)        { _value = val; }
	};

private:
	Item** _table;

	void create() {
		if (!_table) {
			ILOPROTECTNEW(_table, new Item*[_mod]);
			IloInt i;
			for (i=0; i<_mod; i++) {
				_table[i] = 0;
			}
		}
	}

public:
	IloHashTable(IloHashFunction hashFct, IloCompFunction compFct, IloInt size)
		: _size(0)
		, _mod(size)
		, _hashFct(hashFct)
		, _compFct(compFct)
		, _table(0)
	{
		create();
	}

	~IloHashTable() { clear(); }

	IloInt getMod() const         { return (_mod); }
	IloInt getSize() const        { return (_size); }
	Item** getTable() const       { return (_table); }

	void clear() {
		if (_table != 0) {
			for (IloInt i = 0; i < _mod; i++) {
				Item * next;
				for (Item * item = _table[i]; item != 0; item = next) {
					next = item->getNext();
					delete item;
				}
			}
			ILOBADDELETE _table; 
			_table = 0;
		}
		_size = 0;
	}

	Status addWithoutCheck(KeyType key, ValueType value) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* oldItem = _table[hashIndex];
		Item* newItem;
		ILOPROTECTNEW(newItem, new Item(key, value));
		newItem->setNext(oldItem);
		_table[hashIndex] = newItem;
		++_size;
		if (_size > 2 * _mod) {
			reMod(_mod*2); 
		}
		return ILOHASHOK;
	}

	Status add(KeyType key, ValueType value) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		while (item && !_compFct(item->getKey(), key)) {
			item = item->getNext();
		}
		if (item) {
			return ILOHASHDUPLICATEDKEY;
		} else {
			Item* newItem;
			ILOPROTECTNEW(newItem, new Item(key, value));
			newItem->setNext(_table[hashIndex]);
			_table[hashIndex] = newItem;
			++_size;
			if (_size > 2 * _mod) {
				reMod(_mod*2); 
			}
			return ILOHASHOK;
		}
	}

	Status find(KeyType key, ValueType& value) const {
		if (_table) {
			IloInt hashIndex = _hashFct(key, _mod);
			IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
			Item* item = _table[hashIndex];
			while (item && !_compFct(item->getKey(), key)) {
				item = item->getNext();
			}
			if (item) {
				value = item->getValue();
				return ILOHASHOK;
			}
		}
		return ILOHASHNOTFOUND;
	}

	IloBool containsKey(KeyType key) const {
		Item* item = 0;
		if (_table) {
			IloInt hashIndex = _hashFct(key, _mod);
			IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
			item = _table[hashIndex];
			while (item && !_compFct(item->getKey(), key)) {
				item = item->getNext();
			}
		}
		return (item != 0);
	}

	ValueType operator[](KeyType key) const {
		
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		while (item && !_compFct(item->getKey(), key)) {
			item = item->getNext();
		}
		return (item->getValue());
	}

	Status remove(KeyType key) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		Item* previous = 0;
		while (item && !_compFct(item->getKey(), key)) {
			previous = item;
			item = item->getNext();
		}
		if (!item)
			return ILOHASHNOTFOUND;
		if (previous)
			previous->setNext(item->getNext());
		else
			_table[hashIndex] = item->getNext();
		delete item;
		--_size;
		return ILOHASHOK;
	}

	void reMod(IloInt newMod) {
		Item** oldTable = _table;
		IloInt oldMod = _mod;
		IloInt i;
		ILOPROTECTNEW(_table, new Item*[newMod]);
		for (i=0; i<newMod; i++) {
			_table[i] = 0;
		}
		_mod = newMod;
		if (oldTable != 0) {
			for (i = 0; i < oldMod; i++) {
				Item * next;
				for (Item * item = oldTable[i]; item != 0; item = next) {
					next = item->getNext();
					IloInt hashIndex = _hashFct(item->getKey(), _mod);
					IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
					item->setNext(_table[hashIndex]);
					_table[hashIndex] = item;
				}
			}
			ILOBADDELETE oldTable; 
		}
	}

	Status move(KeyType key, ValueType , ValueType newValue) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		while (item && !_compFct(item->getKey(), key)) {
			item = item->getNext();
		}
		if (item) {
			item->setValue(newValue);
			return ILOHASHOK;
		} else
			return ILOHASHNOTFOUND;
	}

	class Iterator {
		IloHashTable<KeyType,ValueType>* _hash;
		Item* _item;
		IloInt _index;

	public:
		Iterator(IloHashTable<KeyType,ValueType>* hash)
			: _hash(hash) 
			, _item(0)
			, _index(-1) {
				if (_hash->getTable()) {
					while (!_item && ++_index<_hash->getMod()) {
						_item = _hash->getTable()[_index];
					}
				}
		}
		IloBool ok() const  { return ((_hash != 0) && (_item!=0)); }
		void operator++ () { 
			if (_item->getNext()) 
				_item = _item->getNext();
			else {
				_item = 0;
				while (!_item && ++_index<_hash->getMod()) 
					_item = _hash->getTable()[_index];
			}
		}
		ValueType operator*() const { return (_item->getValue()); }
		KeyType   getKey() const    { return (_item->getKey()); }
	};
};

template <class KeyType, class ValueType>
class IloEnvHashTable : public IloMemoryManagerObjectI
{
public:
	typedef IloInt (*IloHashFunction) (KeyType, IloInt);
	typedef IloBool (*IloCompFunction) (KeyType, KeyType);
	enum Status {ILOHASHOK=0, ILOHASHNOTFOUND=1, ILOHASHDUPLICATEDKEY=2};

private:
	IloInt          _size;
	IloInt          _mod;
	IloHashFunction _hashFct;
	IloCompFunction _compFct;

public:
	class Item {
		Item*   _next;
		KeyType _key;
		ValueType _value;
	public:
		Item(KeyType key, ValueType value)
			: _next(0), _key(key), _value(value) {}

		Item*     getNext()  const          { return (_next); }
		KeyType   getKey()   const          { return (_key); }
		ValueType getValue() const          { return (_value); }
		void setNext(Item* next)            { _next  = next; }
		void setKey(KeyType key)            { _key   = key; }
		void setValue(ValueType val)        { _value = val; }
	};

private:
	Item** _table;

	void create() {
		if (!_table) {
			_table = (Item**) getMemoryManager()->alloc(sizeof(Item*)*_mod);
			IloInt i;
			for (i=0; i<_mod; i++) {
				_table[i] = 0;
			}
		}
	}

public:
	IloEnvHashTable(IloMemoryManager env, IloHashFunction hashFct, IloCompFunction compFct, IloInt size)
		: IloMemoryManagerObjectI(env.getImpl()),
		_size(0)
		, _mod(size)
		, _hashFct(hashFct)
		, _compFct(compFct)
		, _table(0)
	{
		create();
	}
	IloEnvHashTable(IloEnv env, IloHashFunction hashFct, IloCompFunction compFct, IloInt size)
		: IloMemoryManagerObjectI(env.getImpl()->getGeneralAllocator()),
		_size(0)
		, _mod(size)
		, _hashFct(hashFct)
		, _compFct(compFct)
		, _table(0)
	{
		create();
	}

	virtual ~IloEnvHashTable() { clear(); }

	IloInt getMod() const         { return (_mod); }
	IloInt getSize() const        { return (_size); }
	Item** getTable() const       { return (_table); }

	void clear() {
		if (_table != 0) {
			for (IloInt i = 0; i < _mod; i++) {
				Item * next;
				for (Item * item = _table[i]; item != 0; item = next) {
					next = item->getNext();
					getMemoryManager()->free(item, sizeof(Item));
				}
			}
			getMemoryManager()->free(_table, sizeof(Item*)*_mod);
			_table = 0;
		}
		_size = 0;
	}

	Status addWithoutCheck(KeyType key, ValueType value) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* oldItem = _table[hashIndex];
		Item* newItem = (Item*) getMemoryManager()->alloc(sizeof(Item));
		newItem->setKey(key);
		newItem->setValue(value);
		newItem->setNext(oldItem);
		_table[hashIndex] = newItem;
		++_size;
		if (_size > 2 * _mod) {
			reMod(_mod*2); 
		}
		return ILOHASHOK;
	}

	Status add(KeyType key, ValueType value) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		while (item && !_compFct(item->getKey(), key)) {
			item = item->getNext();
		}
		if (item) {
			return ILOHASHDUPLICATEDKEY;
		} 
		else {
			Item* newItem = (Item*) getMemoryManager()->alloc(sizeof(Item));
			newItem->setKey(key);
			newItem->setValue(value);
			newItem->setNext(_table[hashIndex]);
			_table[hashIndex] = newItem;
			++_size;
			if (_size > 2 * _mod) {
				reMod(_mod*2); 
			}
			return ILOHASHOK;
		}
	}

	Status getKey(KeyType key, KeyType& ret) const {
		if (_table) {
			IloInt hashIndex = _hashFct(key, _mod);
			IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
			Item* item = _table[hashIndex];
			while (item && !_compFct(item->getKey(), key)) {
				item = item->getNext();
			}
			if (item) {
				ret = item->getKey();
				return ILOHASHOK;
			}
		}
		return ILOHASHNOTFOUND;
	}

	Status find(KeyType key, ValueType& value) const {
		if (_table) {
			IloInt hashIndex = _hashFct(key, _mod);
			IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
			Item* item = _table[hashIndex];
			while (item && !_compFct(item->getKey(), key)) {
				item = item->getNext();
			}
			if (item) {
				value = item->getValue();
				return ILOHASHOK;
			}
		}
		return ILOHASHNOTFOUND;
	}

	IloBool containsKey(KeyType key) const {
		Item* item = 0;
		if (_table) {
			IloInt hashIndex = _hashFct(key, _mod);
			IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
			item = _table[hashIndex];
			while (item && !_compFct(item->getKey(), key)) {
				item = item->getNext();
			}
		}
		return (item != 0);
	}

	ValueType operator[](KeyType key) const {
		
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		while (item && !_compFct(item->getKey(), key)) {
			item = item->getNext();
		}
		return (item->getValue());
	}

	Status remove(KeyType key) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		Item* previous = 0;
		while (item && !_compFct(item->getKey(), key)) {
			previous = item;
			item = item->getNext();
		}
		if (!item)
			return ILOHASHNOTFOUND;
		if (previous)
			previous->setNext(item->getNext());
		else
			_table[hashIndex] = item->getNext();
		getMemoryManager()->free(item, sizeof(Item));
		--_size;
		return ILOHASHOK;
	}

	void reMod(IloInt newMod) {
		Item** oldTable = _table;
		IloInt oldMod = _mod;
		IloInt i;
		_table = (Item**) getMemoryManager()->alloc(sizeof(Item*)*newMod);
		for (i=0; i<newMod; i++) {
			_table[i] = 0;
		}
		_mod = newMod;
		if (oldTable != 0) {
			for (i = 0; i < oldMod; i++) {
				Item * next;
				for (Item * item = oldTable[i]; item != 0; item = next) {
					next = item->getNext();
					IloInt hashIndex = _hashFct(item->getKey(), _mod);
					IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
					item->setNext(_table[hashIndex]);
					_table[hashIndex] = item;
				}
			}
			getMemoryManager()->free(oldTable, sizeof(Item*)*oldMod);
		}
	}

	Status move(KeyType key, ValueType , ValueType newValue) {
		create();
		IloInt hashIndex = _hashFct(key, _mod);
		IloTestAndRaise(0 <= hashIndex && hashIndex < _mod, "Hash function returned bad index");
		Item* item = _table[hashIndex];
		while (item && !_compFct(item->getKey(), key)) {
			item = item->getNext();
		}
		if (item) {
			item->setValue(newValue);
			return ILOHASHOK;
		} else
			return ILOHASHNOTFOUND;
	}

	class Iterator {
		IloEnvHashTable<KeyType,ValueType>* _hash;
		Item* _item;
		IloInt _index;

	public:
		Iterator(IloEnvHashTable<KeyType,ValueType>* hash)
			: _hash(hash)
			, _item(0)
			, _index(-1) {
				if (_hash->getTable()) {
					while (!_item && ++_index<_hash->getMod()) {
						_item = _hash->getTable()[_index];
					}
				}
		}
		IloBool ok() const  { return ((_hash != 0) && (_item!=0)); }
		void operator++ () {
			if (_item->getNext())
				_item = _item->getNext();
			else {
				_item = 0;
				while (!_item && ++_index<_hash->getMod())
					_item = _hash->getTable()[_index];
			}
		}
		ValueType operator*() const { return (_item->getValue()); }
		KeyType   getKey() const    { return (_item->getKey()); }
	};
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloEnvI *) { }
	void operator delete(void *, const IloEnv&) { }
#endif
	void operator delete(void * obj, size_t size) {
		IloMemoryManagerObjectI::operator delete(obj, size);
	}
};

extern IloInt  IloStringHashFunction(const char* key, IloInt size);
extern IloInt  IloStringHashFunction2(const char* key, IloInt size);
extern IloBool IloStringCompFunction(const char* key1, const char* key2);
extern IloBool IloStringCompFunction2(const char* key1, const char* key2);

template <class ValueType>
class IloStringHashTable : public IloEnvHashTable<const char*, ValueType> {
public:
	IloStringHashTable(IloMemoryManager env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<const char*, ValueType>(env, 
		IloStringHashFunction, 
		IloStringCompFunction, 
		size) {
	}
	IloStringHashTable(IloEnv env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<const char*, ValueType>(env, 
		IloStringHashFunction, 
		IloStringCompFunction, 
		size) {
	}
	virtual ~IloStringHashTable() {
	}
};

extern IloInt  IloAddressHashFunction(IloAny key, IloInt size);
extern IloBool IloAddressCompFunction(IloAny key1, IloAny key2);

template <class ValueType>
class IloAddressHashTable : public IloEnvHashTable<IloAny, ValueType> {
public:
	IloAddressHashTable(IloMemoryManager env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloAny, ValueType>(env, 
		IloAddressHashFunction,
		IloAddressCompFunction, 
		size) {
	}
	IloAddressHashTable(IloEnv env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloAny, ValueType>(env, 
		IloAddressHashFunction,
		IloAddressCompFunction, 
		size) {
	}
	virtual ~IloAddressHashTable() {
	}
};

class IloSymbolI;

typedef IloStringHashTable<IloSymbolI*> IloSymbolTableBase;

class IloSymbolTable : public IloSymbolTableBase {
public:
	IloSymbolTable(IloMemoryManager env, IloInt size=IloDefaultHashSize);
	IloSymbolTable(IloEnv env, IloInt size=IloDefaultHashSize);
	virtual ~IloSymbolTable();

	IloSymbolI* get(const char* key) const;
	IloSymbolI* make(const char* key);
};

extern IloInt  IloSymbolHashFunction(IloSymbol key, IloInt size);
extern IloBool IloSymbolCompFunction(IloSymbol key1, IloSymbol key2);

template <class ValueType>
class IloSymbolHashTable : public IloEnvHashTable<IloSymbol, ValueType> {
public:
	IloSymbolHashTable(IloMemoryManager env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloSymbol, ValueType>(env, 
		IloSymbolHashFunction,
		IloSymbolCompFunction, 
		size) {
	}
	IloSymbolHashTable(IloEnv env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloSymbol, ValueType>(env, 
		IloSymbolHashFunction,
		IloSymbolCompFunction, 
		size) {
	}
	virtual ~IloSymbolHashTable() {
	}
};

extern IloInt  IloIntegerHashFunction(IloInt key, IloInt size);
extern IloBool IloIntegerCompFunction(IloInt key1, IloInt key2);

template <class ValueType>
class IloIntegerHashTable : public IloEnvHashTable<IloInt, ValueType> {
public:
	IloIntegerHashTable(IloMemoryManager env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloInt, ValueType>(env, 
		IloIntegerHashFunction,
		IloIntegerCompFunction, 
		size) {
	}
	IloIntegerHashTable(IloEnv env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloInt, ValueType>(env, 
		IloIntegerHashFunction,
		IloIntegerCompFunction, 
		size) {
	}
	virtual ~IloIntegerHashTable() {
	}
};

extern IloInt  IloFloatHashFunction(IloNum key, IloInt size);
extern IloBool IloFloatCompFunction(IloNum key1, IloNum key2);

template <class ValueType>
class IloFloatHashTable : public IloEnvHashTable<IloNum, ValueType> {
public:
	IloFloatHashTable(IloMemoryManager env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloNum, ValueType>(env, 
		IloFloatHashFunction,
		IloFloatCompFunction, 
		size) {
	}
	IloFloatHashTable(IloEnv env, IloInt size=IloDefaultHashSize)
		:IloEnvHashTable<IloNum, ValueType>(env, 
		IloFloatHashFunction,
		IloFloatCompFunction, 
		size) {
	}
	virtual ~IloFloatHashTable() {
	}
};

class IloUndefinedSymbol : public IloException {
	static char* MakeMessage(const char* key);
public:
	IloUndefinedSymbol(const char* symbol);
	virtual ~IloUndefinedSymbol();
};

class IloAny2AnyHashTable : public IloAddressHashTable<IloAny> {
public:
	IloAny2AnyHashTable(IloMemoryManager env, IloInt size=IloDefaultHashSize)
		: IloAddressHashTable<IloAny>(env, size) {}
	IloAny2AnyHashTable(IloEnv env, IloInt size=IloDefaultHashSize)
		: IloAddressHashTable<IloAny>(env, size) {}
	virtual ~IloAny2AnyHashTable() {}
};

class IloAny2BoolHashTable : public IloAddressHashTable<IloBool> {
public:
	IloAny2BoolHashTable(IloMemoryManager env, IloInt size=IloDefaultHashSize)
		: IloAddressHashTable<IloBool>(env, size) {}
	IloAny2BoolHashTable(IloEnv env, IloInt size=IloDefaultHashSize)
		: IloAddressHashTable<IloBool>(env, size) {}
	virtual ~IloAny2BoolHashTable() {}
};

extern IloInt  IloIntDataTableHashFunction(IloInt key, IloInt size);
extern IloBool IloIntDataTableCompFunction(IloInt key1, IloInt key2);
extern IloInt  IloNumDataTableHashFunction(IloNum, IloInt size);
extern IloBool IloNumDataTableCompFunction(IloNum key1, IloNum key2);
extern IloInt IloMyIntDataTableHashFunction(IloInt key);
extern IloInt IloMyNumDataTableHashFunction(IloNum key);

class IloInt2IndexHashTable : public IloIntegerHashTable<IloInt> {
public:
	IloInt2IndexHashTable(IloEnv env, IloInt size=IloDefaultHashSize) : IloIntegerHashTable<IloInt>(env, size) {}
	virtual ~IloInt2IndexHashTable() {}
};

class IloNum2IndexHashTable : public IloFloatHashTable<IloInt> {
public:
	IloNum2IndexHashTable(IloEnv env, IloInt size=IloDefaultHashSize) : IloFloatHashTable<IloInt>(env, size) {}
	virtual ~IloNum2IndexHashTable() {}
};

class IloAny2IndexHashTable : public IloAddressHashTable<IloInt> {
public:
	IloAny2IndexHashTable(IloEnv env, IloInt size=IloDefaultHashSize) : IloAddressHashTable<IloInt>(env, size) {}
	virtual ~IloAny2IndexHashTable() {}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
