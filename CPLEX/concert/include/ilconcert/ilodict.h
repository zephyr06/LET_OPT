// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilodict.h
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

#ifndef __CONCERT_ilodictH
#define __CONCERT_ilodictH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/ilohash.h>

IloBool IloCompareWithoutCaseNChar(const char* str1, const char* str2, IloInt n);
IloBool IloCompareWithoutCase(const char* str1, const char* str2);

class IlodDictionaryBase {
protected: 
	const char* concatenate(const char* str1, const char* str2){
		char* msg = 0;
		IloInt length1 = strlen(str1);
		IloInt length2 = strlen(str2);
		ILOPROTECTNEW(msg, new char[length1+length2+1]);
		IloStrncpy(msg, length1+length2+1, str1, length1);
		IloStrncpy(msg+length1, length2+1, str2, length2+1);
		return (const char*)msg;
	}
protected:
	IlodDictionaryBase() {}
	static const char* MSG_DUPLICATED_KEY;
	static const char* MSG_MISSING_KEY_FOR_REPLACE;

public:
	
	static IloInt  HashString(const char* key, IloInt size) {
		return hash(key) % size;
	}
	
	static IloInt  HashInt(IloInt key, IloInt size) {
		return hash(key) % size;
	}
	
	static IloInt  HashPtr(const void* key,  IloInt size) {
		return hash(key) % size;
	}
	
	static IloBool CompareStrings(const char* k1, const char* k2);
	
	static IloBool CompareStringsCaseInsensitive(const char* k1, const char* k2);
	
	static IloBool CompareInts(IloInt l1, IloInt l2);

	static IloInt hash(const char* key);
	static IloInt hash(IloInt key);
	static IloInt hash(const void* key);
};

template <class T>
class IlodDictionary : public IlodDictionaryBase {
	typedef IloHashTable<const char*, T> IloDictionaryTable;
	IloDictionaryTable _table;

	class Exception : public IloException {
		const char* _stringmsg;
	public:
		Exception(const char* msg) : IloException(msg), _stringmsg(0){
			if (msg){
				IloInt length = strlen(msg);
				ILOPROTECTNEW(_stringmsg, new char[length+1]);
				IloStrncpy((char *)_stringmsg, length+1, msg, length+1);
			}
		}
	};

public:
	IlodDictionary(IloInt size = 127, IloBool caseSensitive = IloFalse)
		: _table( HashString, 
		caseSensitive? CompareStrings : CompareStringsCaseInsensitive, 
		size)  {  }

	~IlodDictionary() { end();}

	void end() { _table.clear();}

	IloInt getSize() const { return _table.getSize();}

	T getValue(const char* key, T fallBackValue) const {
		T value;

		if ( IloDictionaryTable::ILOHASHNOTFOUND == _table.find(key, value) ) {
			return fallBackValue;
		} else {
			return value;
		}
	}

	void add(const char* key, T value) {
		if ( _table.add(key, value) == IloDictionaryTable::ILOHASHDUPLICATEDKEY) {
			const char* msg = concatenate(MSG_DUPLICATED_KEY, key);
			throw Exception(msg);
		} else {
			
		}
	}

	void replace(const char* key, T value) {
		if ( _table.move(key, value, value) != IloDictionaryTable::ILOHASHOK) {
			const char* msg = concatenate(MSG_MISSING_KEY_FOR_REPLACE, key);
			throw Exception(msg);
		} else {
			
		}
	}

	void remove(const char* key) {
		_table.remove(key);
	}

	IloBool contains(const char* key) const {
		T dummy;
		return _table.find(key, dummy) != IloDictionaryTable::ILOHASHNOTFOUND;
	}

	class Iterator {
		typename IloDictionaryTable::Iterator _tableIter;
	public:
		Iterator(IlodDictionary<T>& dictionary):
		  _tableIter(&dictionary._table) {}
		  IloBool ok() const { return _tableIter.ok();}
		  void operator++() { ++_tableIter;}
		  T getValue() const { assert( ok() ) ;return *_tableIter;}
		  const char* getKey() { assert( ok()) ; return _tableIter.getKey();}
		  T operator*() const { return getValue();}
	};
	friend class Iterator;

}; 

class IlodIdTableBase : public IlodDictionaryBase {
private:
	IloInt _idMax;
protected:
	IlodIdTableBase():_idMax(-1) {}
	void notifyNewId(IloInt id);
public:
	IloInt getIdMax() const { return _idMax;}
	IloInt getNewId() const;

}; 

template <class T>
class IloIdTable : public IlodIdTableBase {
	typedef IloHashTable<IloInt, T> Table;
	Table _table;

	const char* id2String(IloInt id) const {
		char* idbuf = new char[32];
		IloSPrintf(idbuf, 32, "%d", id);
		return idbuf;
	}

	class Exception : public IloException {
		const char* _stringmsg;
	public:
		Exception(const char* msg) :IloException(msg), _stringmsg(0){
			if (msg){
				IloInt length = strlen(msg);
				ILOPROTECTNEW(_stringmsg, new char[length+1]);
				IloStrncpy((char *)_stringmsg, length+1, msg, length+1);
			}
		}
	};

	void duplicateKeyError(IloInt id) {
    const char* ID = id2String(id);
		const char* msg = concatenate("duplicated key: ", ID);
		delete[] ID;
		throw Exception(msg);
	}

public:
	IloIdTable(IloInt size = 127)
		: _table(HashInt, CompareInts, size) { }

	~IloIdTable() { _table.clear(); }

	IloInt getSize() const { return _table.getSize(); }
	void end() { _table.clear(); }

	T getValue(IloInt key) const {
		T value;
		if ( Table::ILOHASHNOTFOUND == _table.find(key, value)) {
			return 0;
		}
		else {
			return value;
		}
	}

	void add(IloInt key, T value) {
		if (_table.add(key, value) == Table::ILOHASHDUPLICATEDKEY) {
			duplicateKeyError(key);
		} else {
			notifyNewId(key);
		}
	}

	void remove(IloInt key) { _table.remove(key); }

	IloBool contains(IloInt key) const {
		T dummy;
		return _table.find(key, dummy) != Table::ILOHASHNOTFOUND;
	}

	class Iterator {
		typename Table::Iterator _tableIter;
	public:
		Iterator(const IloIdTable<T>& dict) :
		  _tableIter(&const_cast<IloIdTable<T>&>(dict) ._table) {
		  }
		  IloBool ok() const { return _tableIter.ok(); }
		  void operator++() { ++_tableIter; }
		  T getValue() const { return (ok() ? *_tableIter : 0); }
		  IloInt getId() { assert(ok()) ; return _tableIter.getKey(); }
		  T operator*() const { return getValue(); }
	};
	friend class Iterator;

}; 

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
