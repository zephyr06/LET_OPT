// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilomemmgr.h
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

#ifndef __CONCERT_ilomemmgrH
#define __CONCERT_ilomemmgrH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/ilortti.h>

class IloAllocationStack;

class IloGenAlloc : public IloRtti {
	ILORTTIDECL
public:
	IloGenAlloc() {}
	virtual ~IloGenAlloc();
	virtual void * alloc(size_t size) const = 0;
	virtual void free(void * ptr, size_t size) const = 0;
	virtual void saveValue(IloAny * slot) const = 0;
	virtual void saveValue(IloInt * slot) const = 0;
	virtual void saveValue(IloNum * slot) const = 0;
	virtual const char* intern(const char* string) const = 0;
};

inline void * operator new (size_t sz, const IloGenAlloc * gen) {
	return gen->alloc(sz);
}
#ifdef ILODELETEOPERATOR
inline void operator delete(void*, const IloGenAlloc*) { }
#endif
#if defined(ILONEWOPERATOR)
inline void* operator new [] (size_t sz, const IloGenAlloc * gen) {
	return gen->alloc(sz);
}
# if defined(ILODELETEOPERATOR)
inline void operator delete[] (void*, const IloGenAlloc*) { }
# endif
#endif

class IloConcertAlloc : public IloGenAlloc {
	ILORTTIDECL
private:
	IloEnvI * _env;
public:
	IloConcertAlloc(IloEnvI * env);
	IloEnvI* getEnvI() const { return _env; }
	void * alloc(size_t size) const ILO_OVERRIDE;
	void free(void * ptr, size_t size) const ILO_OVERRIDE;
	void saveValue(IloAny * slot) const ILO_OVERRIDE;
	void saveValue(IloInt * slot) const ILO_OVERRIDE;
	void saveValue(IloNum * slot) const ILO_OVERRIDE;
	virtual const char* intern(const char* string) const ILO_OVERRIDE;
};

class IloAllocationStack;

class IloSymbolTable;

class IloStackGenAlloc : public IloGenAlloc {
	ILORTTIDECL
private:
	IloAllocationStack* _stack;
	IloSymbolTable*     _symbols;
public:
	IloStackGenAlloc(IloAllocationStack* s, IloSymbolTable* symbols = 0)
		:IloGenAlloc(),_stack(s), _symbols(symbols) {}
	void * alloc(size_t size) const ILO_OVERRIDE;
	void free(void * ptr, size_t size) const ILO_OVERRIDE;
	void saveValue(IloAny * slot) const ILO_OVERRIDE;
	void saveValue(IloInt * slot) const ILO_OVERRIDE;
	void saveValue(IloNum * slot) const ILO_OVERRIDE;
	virtual const char* intern(const char*) const ILO_OVERRIDE;
};

typedef IloGenAlloc IloMemoryManagerI;

class IloMemoryManager
{
private:
	IloMemoryManagerI * _impl;
public:
	IloMemoryManager() : _impl((IloMemoryManagerI*)0) { }
	virtual ~IloMemoryManager() {

	}
	
	IloMemoryManager(IloMemoryManagerI * impl) : _impl(impl) { }
	
	IloMemoryManagerI * getImpl() const { return _impl; }
	void * alloc(size_t size) const {
		IloAssert(getImpl() != 0, "IloMemoryManager : empty handle");
		return getImpl()->alloc(size);
	}
	void free(void * ptr, size_t size) const {
		IloAssert(getImpl() != 0, "IloMemoryManager : empty handle");
		getImpl()->free(ptr, size);
	}
	void saveValue(IloAny * slot) const {
		IloAssert(getImpl() != 0, "IloMemoryManager : empty handle");
		getImpl()->saveValue(slot);
	}
	void saveValue(IloInt * slot) const {
		IloAssert(getImpl() != 0, "IloMemoryManager : empty handle");
		getImpl()->saveValue(slot);
	}
	void saveValue(IloNum * slot) const {
		IloAssert(getImpl() != 0, "IloMemoryManager : empty handle");
		getImpl()->saveValue(slot);
	}
};

inline void* operator new (size_t sz, const IloMemoryManager& mm) {
	IloAssert(mm.getImpl(), "Using empty IloMemoryManager handle.");
	return mm.getImpl()->alloc(sz);
}
#ifdef ILODELETEOPERATOR
inline void operator delete(void*, const IloMemoryManager&) {}
inline void operator delete(void*, IloMemoryManager) {}
#endif

#if defined(ILONEWOPERATOR)
inline void* operator new[] (size_t s, const IloMemoryManager& mm) {
	IloAssert(mm.getImpl(), "Using empty IloMemoryManager handle.");
	return (operator new (s, mm.getImpl()));
}
# ifdef ILODELETEOPERATOR
inline void operator delete[] (void*, const IloMemoryManager&) {}
inline void operator delete[] (void*, IloMemoryManager) {}
# endif
#endif

class IloEnv IloGetEnv(IloMemoryManager mm);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
