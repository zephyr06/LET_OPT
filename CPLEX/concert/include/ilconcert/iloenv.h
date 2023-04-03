// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloenv.h
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

#ifndef __CONCERT_iloenvH
#define __CONCERT_iloenvH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/ilortti.h>
#include <ilconcert/ilomemmgr.h>
#include <ilconcert/iloallocator.h>
#ifdef ILO_LINUX
#include <cstring>
#endif
#if !defined(ILO_WINDOWS)
#include <pthread.h>
#endif

class IloVarDomainI;
class IloIntSharedDomainHash;
class IloNumSharedDomainHash;
class IloNumSharedVarDomainI;
class IloIntSharedVarDomainI;
class IloIntRangeI;
class IloNumRangeI;
class IloSchedGlobalExtractableI;
class IloArrayI;

#ifdef ILO_RS6000
void* operator new (size_t sz, const IloEnv& env);
void* operator new[] (size_t s, const IloEnv& env);
#endif

class IloIntCollection;
class IloNumCollection;

class IloScopedSetLocale {
	int _cat;
	char* _orig;
public:
	IloScopedSetLocale(int cat, const char* locale);
	~IloScopedSetLocale();
};

class IloEmptyHandleException : public IloException {
public:
	
	IloEmptyHandleException();
	
	IloEmptyHandleException(const char * message);
	virtual ~IloEmptyHandleException();
};

class IloNotImplemented : public IloException {
public:
	IloNotImplemented();
	IloNotImplemented(const char* message);
	virtual ~IloNotImplemented();
};

class IloEnvAllocationDisabled : public IloException {
public:
	IloEnvAllocationDisabled();
	IloEnvAllocationDisabled(const char* message);
	virtual ~IloEnvAllocationDisabled();
};

class IloEnvAllocatorThreadMismatch : public IloException {
public:
	IloEnvAllocatorThreadMismatch();
	IloEnvAllocatorThreadMismatch(const char* message);
	virtual ~IloEnvAllocatorThreadMismatch();
};

class IloNotSupportedOperation : public IloException {
public:
	IloNotSupportedOperation();
	IloNotSupportedOperation(const char* message);
	virtual ~IloNotSupportedOperation();
};

class IloBadConversion : public IloException {
public:
	IloBadConversion();
	IloBadConversion(const char* message);
	virtual ~IloBadConversion();
};

class IloEnvironmentMismatch : public IloException {
public:
	
	IloEnvironmentMismatch();
	
	IloEnvironmentMismatch(const char * message);
	
	virtual ~IloEnvironmentMismatch();
};

class IloParsingNumberMismatch : public IloException {
public:
	enum Type{
		PARSING_INT,
		PARSING_NUM,
		PARSING_OVERFLOW
	};
private:
	char* _str;
	Type _type;

public:
	IloParsingNumberMismatch(char* str, Type type);
	virtual ~IloParsingNumberMismatch(){};
	char* getBadString() const { return _str;};
	IloBool isInt() const { return _type == PARSING_INT;}
	IloBool isNum() const { return _type == PARSING_NUM;}
	IloBool isOverflow() const { return _type == PARSING_OVERFLOW;}
};

#ifndef ILO_STRINGIZE
# define ILO_STRINGIZE(s) #s
#endif

class IloLocalEnvI {
	friend class IloEnvI;
	IloEnvI*        _env;
	IloLocalEnvI*   _previousEnv;
public:
	IloLocalEnvI(IloEnvI*);
	virtual ~IloLocalEnvI(){}
	virtual IloAny alloc(size_t sz) = 0;
	virtual void free(void*, size_t) = 0;
	virtual IloInt init(IloExtractableI* ext);
	virtual void saveValue(IloAny*) = 0;
	virtual void saveValue(IloInt*) = 0;
	virtual void saveValue(IloNum*) = 0;
	IloInt bookId();
	virtual void releaseId(IloInt id);
	IloEnvI* getEnv() const {return _env;}
};

class IloNoChangeDuringSearchException : public IloException {
public:
	IloNoChangeDuringSearchException()
		: IloException("This change is not allowed during a search.") {}
	virtual ~IloNoChangeDuringSearchException();
};

#define IloGlobal(env_t, what_t) \
	IloLocalEnvI* localEnv_t = env_t.getImpl()->getLocalEnv(); \
	env_t.getImpl()->setLocalEnv(0); \
	what_t; \
	env_t.getImpl()->setLocalEnv(localEnv_t);

class IloBaseEnvMutex {
public:
	
	virtual void lock();
	
	virtual void unlock();
	virtual ~IloBaseEnvMutex(){}
};

void IloInitMT();

void IloInitMT(IloBaseEnvMutex*);
IloBool IloIsMTEnabled();

void IloEndMT();

class IloEnv;
class IloEnvI;

class ILO_EXPORTED IloTimer {
	IloMemoryManagerI* _env;
	double  _total;
	double  _start;
	IloBool _running;

	static IloNum getCPUtime();
	IloTimer() { reset(); }
public:
	
	IloTimer(const IloEnv env);
	IloTimer(const IloMemoryManager env);
	
	IloEnv getEnv() const;
	IloMemoryManager getMemoryManager() const;

	
	IloNum getTime() const {
		if ( _running ) return (_total + getCPUtime() - _start);
		else            return (_total);
	}

	
	void reset() {
		_total   =  0.0;
		_start   = -1.0;
		_running = IloFalse;
	}

	
	IloNum stop();
	
	IloNum start();
	
	IloNum restart() {
		IloNum time = getTime();
		reset();
		start();
		return (time);
	}
};

class IloIdManager {
	union Cell { Cell* _next; IloExtractableI* _extr; };
	IloMemory* _memory;
	IloInt _datasize;
	IloInt _datamax;
	Cell* _data;
	Cell  _free;
	void growData(IloInt size, IloInt freeFrom=0);
public:
	
	IloIdManager(IloMemory*);
	
	void end();
	
	IloInt getMemoryUsage() const;
	
	IloInt getSize() const { return _datasize; }
	
	IloExtractableI* getExtractable(IloInt id) const {
		IloTestAndRaise(isValidId(id), "this ID is not valid");
		return _data[id]._extr;
	}
	IloExtractableI* getExtractableOrNull(IloInt id) const {
		return isValidId(id) ? _data[id]._extr : 0;
	}
	
	void setExtractable(IloInt id, IloExtractableI* obj) { _data[id]._extr = obj; }
	
	IloBool isValidId(IloInt id) const {
		return (_data[id]._next<_data || _data[id]._next>=&_data[_datamax]);
	}
	
	void freeId(IloInt);
	
	IloInt newId(IloExtractableI* obj);
	
	~IloIdManager();

	
	class Iterator {
		IloIdManager* _ids;
		IloInt _id;
		void nextValid();
	public:
		Iterator(IloIdManager* ids) : _ids(ids), _id(-1) {nextValid();}
		void             operator++()       { nextValid(); }
		IloExtractableI* operator*()        { return _ids->getExtractable(_id); }
		IloBool          ok() const         { return (_id<_ids->getSize());}
	};
};

class ILO_EXPORTED IloTypedListManager
{
private:
	friend class IloEnvI;
	class IloTypedListManagerI* _manager;
	IloBool hasManager() const { return (_manager!=0); }

	IloTypedListManager() : _manager(0) {}
	~IloTypedListManager();

	void add(IloExtractableI* ext);
	void remove(IloExtractableI* ext);
	void end(class IloEnvI*);
	static class IloTypedListManagerI* getOrMake(IloTypedListManager &lm,
		IloEnvI* env);
};

class IloNamedPropertyI;
class IloNamedProperty;
class IloNamedPropertySetI;

class IloExtension {
	struct IloObjectExtension {
		IloAny                 _object;
		IloNamedPropertySetI * _nps;

		IloObjectExtension() : _object(0), _nps(0) { }
		void createProperties(IloMemoryManagerI * mem);
		void end();
	};

private:
	char *               _name;
	IloObjectExtension * _objectExt;
public:
	IloExtension() : _name(0), _objectExt(0) {}
	IloExtension(IloMemoryManagerI* mem, const char* name = 0, IloAny object = 0);
	IloExtension(IloEnvI* mem, const char* name = 0, IloAny object = 0);
	void end(IloMemoryManagerI * mem);
	void end(IloEnvI * mem);
	~IloExtension();
	static IloExtension* getOrMake(IloMemoryManagerI * mem, IloExtension ** ext);
	static IloExtension* getOrMake(IloEnvI * mem, IloExtension ** ext);

	void setName(IloMemoryManagerI* mem, const char* name);
	void setName(IloEnvI* env, const char* name);
	void setObject(IloMemoryManagerI * mem, IloAny object);
	void setObject(IloEnvI * mem, IloAny object);
	void setProperties(IloNamedPropertySetI * np);
	const char * getName() const { return _name; }
	IloAny getObject() const {
		return (_objectExt == 0) ? 0 : _objectExt->_object;
	}
	IloNamedPropertySetI * getProperties() const {
		return (_objectExt == 0) ? 0 : _objectExt->_nps;
	}
        void setLocation(IloMemoryManagerI* mem, const char* fileName, int lineNumber);
};

class ILO_EXPORTED IloPrivateBuf : public ILOSTD(streambuf) {
#if defined(ILO_MSVC) || defined(ILO_HP11)  || defined(ILO_RS6000)
	int overflow(int) { return 0;}
	int underflow(void) { return 0;}
#endif
public:
};

class ILO_EXPORTED IloNullStream : public ILOSTD(ostream) {
public:
	IloNullStream(ILOSTD(streambuf)& buf);
};

enum IloDeleterMode {
	IloLinearDeleterMode = 0,
	IloSafeDeleterMode = 1,
	IloRecursiveDeleterMode = 2,
	IloSmartDeleterMode = 2
};

#define ILOENDEXTENSION(ENV) \
	if (_ext != 0) { \
	_ext->end(ENV); \
	ENV->free(_ext, sizeof(IloExtension)); \
	}

#define ILOEXTENSIONI \
public: \
	const char * getName() const { return (_ext != 0) ? _ext->getName() : 0; } \
	void setName(const char *); \
	IloAny getObject() const { return (_ext != 0) ? _ext->getObject() : 0; } \
	void setObject(IloAny); \
	IloNamedPropertySetI * getProperties() const { \
	return (_ext != 0) ? _ext->getProperties() : 0; \
	} \
	IloNamedPropertyI * getProperty(const char *) const; \
	IloBool hasProperty(const char *) const; \
	void addProperty(IloNamedPropertyI *); \
	void addProperties(IloNamedPropertySetI *); \
	void addClonedProperty(IloNamedPropertyI *); \
	void removeProperty(const char *); \
	void removeProperty(IloNamedPropertyI * np); \
	void removeAllProperties(); \
	void setProperties(IloNamedPropertySetI *); \
	IloInt getIntProperty(const char * name) const; \
	IloNum getNumProperty(const char * name) const; \
	IloAny getAnyProperty(const char * name) const; \
	const char * getStringProperty(const char * name) const; \
	void setIntProperty(const char * name, IloInt i); \
	void setNumProperty(const char * name, IloNum n); \
	void setAnyProperty(const char * name, IloAny a); \
	void setStringProperty(const char * name, const char * str); \
        void setLocation(const char* fileName, int lineNumber); \
private: \
	IloExtension * _ext;

class IloLinExprTermI;
class IloQuadExprTermI;
class IloPlugInManager;
class IloBaseDeleterI;
class IloRandomI;
class IloRandom;
class IloNumExprI;
class IloAnyExprI;
class IloSymbol;
class IloSymbolI;
class IloSymbolTable;
class IloConcertAlloc;
class IloHomomorphismErrorHandlerI;

extern IloBaseDeleterI* IloSetBaseDeleter(IloEnvI*, IloDeleterMode);

class IloArrayI;
class IloEndCollectionCallbackI;
class IloOplDisplayI;
class IloComprehensionI;
class IloGeneratorI;

class IloModelEvaluatorI;
class IloExpanderI;
typedef	void (*IloMyOPLExtractors)(IloModelEvaluatorI*);
typedef void (*IloMyOPLExpanders)(IloExpanderI*);

class IloAny2BoolHashTable;

typedef IloModelEvaluatorI* (*IloMyModelEvaluatorPop)(IloEnvI*);
typedef IloExpanderI* (*IloMyExpanderPop)(IloEnvI*);

class IloRttiEnvObjectI;

class ILO_EXPORTED IloEnvI {
	friend class IloNumVarI;
	friend class IloModelI;
protected:
	
	IloUInt _flags;

	
	IloBool _useMemory;
	IloMemory _mem;
	IloBool   _memoryManagement;

	friend class IloEnv;
	friend class IloLocalEnvI;

	IloLocalEnvI*      _localEnv;
	IloBool            _safeAllocation;
        IloBool            _allocationDisabled;
        IloBool            _allocatorThreadSpecified;
        void*              _allocatorThreadId;

	
	IloBool            _mtSafeNormalization;
	IloAny*            _marks;

	void    freeMarks();
	void    initMarkArray(IloAny *mark);

	
	IloSymbolTable* _symbols;

	
	IloBool _abortRequested;
public:
	void requestAbort() { _abortRequested = IloTrue; }
	void resetAbortRequest() { _abortRequested = IloFalse; }
	IloBool isAbortRequested() const {return _abortRequested;}

protected:
	IloHomomorphismErrorHandlerI* _homomorphismErrorHandler;
public:
	void setHomomorphismErrorHandler(IloHomomorphismErrorHandlerI* handler) {
		_homomorphismErrorHandler = handler;
	}
	IloHomomorphismErrorHandlerI* getHomomorphismErrorHandler() const {
		return _homomorphismErrorHandler;
	}

public:
	void pushLocalEnv(IloLocalEnvI* localEnv) {
		localEnv->_previousEnv = _localEnv;
		_localEnv = localEnv;
	}
	IloLocalEnvI* popLocalEnv() {
		IloLocalEnvI* localEnv = _localEnv;
		_localEnv = localEnv->_previousEnv;
		return localEnv;
	}
	IloLocalEnvI* getLocalEnv() const {return _localEnv;}
	void setLocalEnv(IloLocalEnvI* localEnv) {_localEnv = localEnv;}
	void useMTSafeAllocation(IloBool use);
	void useMTSafeNormalization(IloBool use);
	IloBool isMTSafeNormalization() const { return _mtSafeNormalization; }

	IloAny* getMarkArray();
	void    releaseMarkArray(IloAny* mark);

	void saveValue(IloAny*);
	void saveValue(IloInt*);
	void saveValue(IloNum*);

	IloSymbolI* getSymbol(const char* key) const;
	IloBool hasSymbol(const char* key) const;
	IloSymbolI* makeSymbol(const char* key);
	IloSymbolTable* getSymbolTable() const { return _symbols; }
	void makeSymbolTable();

	IloVarDomainI* getOrMakeSharedDomain(IloNumRangeI* R);
	IloVarDomainI* getOrMakeSharedDomain(IloNum lb, IloNum ub);
	IloVarDomainI* getOrMakeSharedDomain(IloIntRangeI* R);
	IloVarDomainI* getOrMakeSharedDomain(IloInt lb, IloInt ub);

	const char* strDup(const char* str);
	void strFree(char* str);

	
protected:
	
	
	
	class IloSolverEnvI* _solverEnv;
	
public:
	
	
	
	
	
	
	
	IloSolverEnvI* getSolverEnv() const { return _solverEnv; }
	void setSolverEnv(IloSolverEnvI * solverEnv) { _solverEnv = solverEnv; }
	
	

	const char* getVersion() const;

private:
	class IloIdManager* _ids;
	IloTypedListManager _typedListManager;
	IloAlgorithmI* _firstAlgorithm;
	IloBool        _assumeNormalizedLinExpr;
	IloBool        _assumeNormalizedQuadExpr;
	IloInt         _sizeOfLinExprTermI;
	IloInt         _sizeOfQuadExprTermI;
	IloFixedSizeAllocatorI* _allocatorOfLinExprTermI;
	IloFixedSizeAllocatorI* _allocatorOfQuadExprTermI;
	IloPlugInManager* _plugins;

	ILOSTD(ostream)* _out;
	ILOSTD(ostream)* _war;
	ILOSTD(ostream)* _err;
	IloPrivateBuf _privateBuff;
	IloNullStream _nullstream;

	IloTimer _time;

	
	class IloCloneManager* _cloneManager;
	class IloDestroyableList* _destroyables;

	IloBaseDeleterI* _deleter;
	IloDeleterMode _deleterMode;
	IloBool _isInDestructor;
	IloRandomI * _random;
	IloConcertAlloc * _genAllocator;
	class IloExpanderI* _expander;
	IloAny2BoolHashTable* _isDecisionExprMap;
public:
	~IloEnvI();
	IloEnvI();
	void addAlgorithm(IloAlgorithmI* m);
	void removeAlgorithm(IloAlgorithmI* m);
	void emancipateAlgorithm(IloAlgorithmI* algo);
	void addDestroyable(class IloDestroyableI*);
	void removeDestroyable(class IloDestroyableI*);

	const char * intern(const char * name);

	void removeVerticalEnv(void * vertialEnv);
	void end();
	IloBool isCloningLocked() const;
	IloBool lockCloning();
	void unlockCloning();
	IloInt lockCloningLevel();
	void unlockCloningLevel(IloInt level);

	ILOSTD(ostream)& getNullStream() const   { return (((IloEnvI*)this)->_nullstream); }

	ILOSTD(ostream)& out() const       { return *(((IloEnvI*)this)->_out); }
	void setOut(ILOSTD(ostream)& s)    { _out = &s; }
	ILOSTD(ostream)& warning() const   { return *(((IloEnvI*)this)->_war); }
	void setWarning(ILOSTD(ostream)& s){ _war = &s; }
	ILOSTD(ostream)& error() const     { return *(((IloEnvI*)this)->_err); }
	void setError(ILOSTD(ostream)& s)  { _err = &s; }

	IloNum getTime() const        { return (_time.getTime()); }
	void   printTime() const;

	IloBool assumeNormalizedLinExpr() const { return _assumeNormalizedLinExpr; }
	void    assumeNormalizedLinExpr(IloBool v) { _assumeNormalizedLinExpr = v; }

	IloBool assumeNormalizedQuadExpr() const    { return _assumeNormalizedQuadExpr; }
	void    assumeNormalizedQuadExpr(IloBool v) { _assumeNormalizedQuadExpr = v; }

	IloInt getMemoryUsage() const;
	IloInt getTotalMemoryUsage() const;
	IloInt getIdManagerMemoryUsage() const;

	IloBool useDetailedDisplay() const;
	void useDetailedDisplay(IloBool b);

	IloDeleterMode getDeleterMode() const { return _deleterMode; }
	IloBaseDeleterI* getDeleter() const {return _deleter;}
	void setConcreteDeleter(IloBaseDeleterI* deleter) {_deleter = deleter;}
	void setDeleter(IloDeleterMode mode) {
		_deleterMode = mode;
		if (mode != IloLinearDeleterMode)
			IloSetBaseDeleter(this, mode);
		else
			unsetDeleter();
	}
	void unsetDeleter();
	void inhibitDeleter(IloBool);
	void removeAndDelete(IloExtractableI*, IloBool);
	void removeAndDelete(const IloExtractableArray, IloBool);
	void setDeleterVerbosityLevel(IloInt level);

	void init(IloExtractableI* ext);
	void finit(IloExtractableI* ext);
	IloIdManager* getIdManager() const { return _ids; }

	IloInt getMaxId() const { return _ids->getSize(); }
	IloBool isValidId(IloInt id) const {
		return _ids->isValidId(id);
	}
	IloIdManager::Iterator getIdIterator() const {
		return IloIdManager::Iterator(_ids);
	}
	IloExtractableI* getExtractable(IloInt id) const {
		return (_ids->getExtractable(id));
	}

	IloPlugInManager* getPlugInManager() const;

	
	void change(const IloExtractableI*     what, const IloChange& chg);
	void change(const IloChange& chg);

	
	void markRemove(IloInt id) {
		_ids->setExtractable(id, (IloExtractableI*)-1);
	}
	void unmarkRemove(IloInt id, IloExtractableI* ext) {
		_ids->setExtractable(id, ext);
	}
	IloBool mustRemove(IloInt id) const {
		return ((id == -1) || (_ids->getExtractableOrNull(id) == (IloExtractableI*)-1));
	}

	void destroy(IloExtractableI*, IloBool inDelete, IloBool throwChange = IloTrue);
	void removeFromAll(IloExtractableI*);
	void removeFromAll(const IloExtractableArray);
	void removeFromAllAndDestroy(IloExtractableI*, IloBool inDelete);
	void removeFromAllAndDestroy(const IloExtractableArray, IloBool inDelete, IloBool removeFromAll = IloTrue);

        
        IloBool hasAllocatorThread() const;
        void setAllocatorThread();
        void clearAllocatorThread();
        IloBool matchesAllocatorThread() const;
        IloBool isAllocationDisabled() const 
        { return _allocationDisabled; }
        void setAllocationDisabled(IloBool b) 
        { _allocationDisabled = b; }
	void* alloc(size_t sz) const {
          if(! matchesAllocatorThread())
            throw IloEnvAllocatorThreadMismatch();
          if(_allocationDisabled)
            throw IloEnvAllocationDisabled();
		IloAny ptr =  (_localEnv
			? _localEnv->alloc(sz)
			: ((IloEnvI*)this)->_mem.alloc(sz));
		return ptr;
	}
	void free(void* data, size_t sz) const {
          if(! matchesAllocatorThread())
            throw IloEnvAllocatorThreadMismatch();
          if(_allocationDisabled)
            throw IloEnvAllocationDisabled();
		(_localEnv
			? _localEnv->free(data, sz)
			: ((IloEnvI*)this)->_mem.free(data, sz));
	}

	const IloMemory* getGlobalHeap() const { return &_mem; }
	void* globalAlloc(size_t sz) const {
		return ((IloEnvI*)this)->_mem.alloc(sz);
	}
	void globalFree(void* data, size_t sz) const {
		((IloEnvI*)this)->_mem.free(data, sz);
	}
	IloBool isInDestructor() {
		return _isInDestructor;
	}
	void release(IloNumExprI* expr);
	void release(IloAnyExprI* expr);
	void release(IloComprehensionI* expr);
	void release(IloGeneratorI* expr);

	class IloTypedListManagerI* getTypedListManager() {
		return IloTypedListManager::getOrMake(_typedListManager, this);
	}

	IloBool memoryManagement() const { return _memoryManagement; }

	
	IloBool hasClone(IloExtractableI* obj) const;
	IloExtractableI* substitute(IloExtractableI* obj);
	IloExtractableI* getClone(IloExtractableI* obj);
	IloRttiEnvObjectI* getOplClone(IloRttiEnvObjectI* obj);
	void setClone(IloExtractableI* original, IloExtractableI* clone);

	
	IloNumLinExprTermI* newTerm(IloNumVarI* var, IloNum coef);
	void recycleTerm(IloNumLinExprTermI* term);

	
	IloNumQuadExprTermI* newTerm(IloNumVarI* var1, IloNumVarI* var2, IloNum coef);
	void recycleTerm(IloNumQuadExprTermI* term);

	
	IloRandomI * createRandom();
	IloRandomI * getRandom() const {
		return (_random != 0) ? _random : ((IloEnvI*)this)->createRandom();
	}

	
	IloConcertAlloc * getGeneralAllocator() const { return _genAllocator; }

	
	IloModelEvaluatorI* getModelEvaluator();
	IloExpanderI* getExpander();

	
#ifdef ILOUSEMT
	friend void IloInitMT();
#endif
	friend void IloInitMT(IloBaseEnvMutex*);
	friend IloBool IloIsMTEnabled();
	friend void IloEndMT();
	static IloBaseEnvMutex* Mutex;
	IloBaseEnvMutex* _mutex;
	static IloBool DeleteMutex;
#ifdef ILO_WINDOWS
        
        
        static long GlobalMutex;
        static void GlobalLock();
        static void GlobalUnlock();
#else
        static pthread_mutex_t GlobalMutex;
        static void GlobalLock() { pthread_mutex_lock(&GlobalMutex); }
        static void GlobalUnlock() { pthread_mutex_unlock(&GlobalMutex); }
#endif
public:
        static void Lock() {if (Mutex) Mutex->lock(); else GlobalLock(); }
        static void Unlock() {if (Mutex) Mutex->unlock(); else GlobalUnlock(); }
	void setMutex(IloBaseEnvMutex* mutex) {_mutex = mutex;}
	IloBaseEnvMutex* getMutex() const {return _mutex;}
	void lock() {if (_mutex) _mutex->lock();}
	void unlock() {if (_mutex) _mutex->unlock();}
protected:
	char* _sccsid;
public:
	IloBool useMemory() const {
		return _useMemory;
	}
	IloEnvI * getEnv() const { return (IloEnvI*)this; }

	ILOEXTENSIONI
protected:
	class IloRecycleBinI* _recycleBin;
	class IloRecycleBinI* _recycleBin2;
public:
	IloBool hasRecycleBin() const { return _recycleBin != 0; }
	class IloIntRangeI* _sharedIntRange;
	class IloNumRangeI* _sharedNumRange;
	IloIntSharedDomainHash* _intSharedDomainHash;
	IloNumSharedDomainHash* _numSharedDomainHash;
	IloMemoryManagerI* getMemoryManager() const { return getGeneralAllocator(); }
protected:
	IloArrayI* _sharedDeletedArray;
	IloExtractableArray getOrMakeEmptySharedDeletedArray();
	IloEndCollectionCallbackI* _endCollectionCallback;
	friend class IloEndCollectionCallbackI;
	IloOplDisplayI* _oplDisplay;
	friend class IloOplDisplayI;

protected:
	IloSchedGlobalExtractableI* _schedGlobalExtr;
public:
	IloSchedGlobalExtractableI* getSchedGlobalExtractable() const {
		return _schedGlobalExtr;
	}
	IloBool hasSchedGlobalExtractable() const {
		return 0 != _schedGlobalExtr;
	}
	void makeSchedGlobalExtractable();
	IloBool hasIntervalVars() const {
		return hasSchedGlobalExtractable();
	}
protected:
	class IloOplRefCountManager* _oplrefcount;
	IloMyOPLExtractors _myOPLExtractors;
	IloMyOPLExpanders _myOPLExpanders;
	IloMyModelEvaluatorPop _myModelEvaluatorPop;
	IloMyExpanderPop _myExpanderPop;
        IloBool _expandQuadraticTerms;

public:
	IloOplRefCountManager* getOplRefCounter() const{ return _oplrefcount; }
	void setOplRefCounter(IloOplRefCountManager* refcount){ _oplrefcount = refcount; }

	void setExtractors(IloMyOPLExtractors fct);
	void setExpanders(IloMyOPLExpanders fct);
	IloMyOPLExtractors getExtractors() const;
	IloMyOPLExpanders getExpanders() const;

	void setModelEvaluatorPop(IloMyModelEvaluatorPop fct);
	void setExpanderPop(IloMyExpanderPop fct);
	IloMyModelEvaluatorPop getModelEvaluatorPop() const;
	IloMyExpanderPop getExpanderPop() const;

	IloAny2BoolHashTable* getDecisionExprMap();
	IloAny2BoolHashTable* getDecisionExprMapOrNull();

        IloBool expandQuadraticTerms() const;
        void expandQuadraticTerms(IloBool doExpand);

};

inline void* operator new (size_t sz, const IloEnvI* env) {
	void* ptr = env->alloc(sz);
	return ptr;
}
#ifdef ILODELETEOPERATOR
inline void operator delete(void*, const IloEnvI*) { }
#endif
#if defined(ILONEWOPERATOR)
inline void* operator new [] (size_t sz, const IloEnvI* env) {
	return env->alloc(sz);
}
# if defined(ILODELETEOPERATOR)
inline void operator delete[] (void*, const IloEnvI*) { }
# endif
#endif

class IloMTSafeLocalEnvI : public IloLocalEnvI {
public:
	virtual ~IloMTSafeLocalEnvI(){}
	IloMTSafeLocalEnvI(IloEnvI*);
	IloAny alloc(size_t sz) ILO_OVERRIDE;
	void free(void* data, size_t sz) ILO_OVERRIDE;
	IloInt init(IloExtractableI*) ILO_OVERRIDE;
	void releaseId(IloInt) ILO_OVERRIDE;
	void saveValue(IloAny*) ILO_OVERRIDE {}
	void saveValue(IloInt*) ILO_OVERRIDE {}
	void saveValue(IloNum*) ILO_OVERRIDE {}
};

class IloEnvObjectI {
protected:
	IloEnvI * _env;
public:
	IloEnvObjectI(IloEnvI * env) : _env(env) {
	}
	virtual ~IloEnvObjectI();
	IloEnvI * getEnv() const { return _env; }
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloEnvI *);
	void operator delete(void *, const IloEnv&);
#endif
	void operator delete(void * obj, size_t size);
	IloMemoryManagerI* getMemoryManager() const;
};

class ILO_EXPORTED IloRttiEnvObjectI : public IloRtti {
	ILORTTIDECL
protected:
	IloEnvI * _env;
public:
	IloRttiEnvObjectI(IloEnvI * env) : _env(env) {
	}
	virtual ~IloRttiEnvObjectI();
	IloEnvI * getEnv() const { return _env; }
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloEnvI *);
	void operator delete(void *, const IloEnv&);
	inline void operator delete(void*, IloAllocationStack*) {}
#endif
	void operator delete(void * obj, size_t size);
	IloMemoryManagerI* getMemoryManager() const;
	virtual IloBool isOplRefCounted() const { return IloFalse; }
	virtual IloRttiEnvObjectI* makeOplClone(IloEnvI* env) const;
	virtual void garbage() {}
};

class IloExtensibleRttiEnvObjectI : public IloRttiEnvObjectI {
public:
	IloExtensibleRttiEnvObjectI(IloEnvI* env)
		: IloRttiEnvObjectI(env), _ext(0) { }
	~IloExtensibleRttiEnvObjectI();
	ILORTTIDECL
		ILOEXTENSIONI
};

class IloMemoryManagerObjectI {
protected:
	IloMemoryManagerI * _mem;
public:
	IloMemoryManagerObjectI(IloMemoryManagerI * mem) : _mem(mem) { }
	virtual ~IloMemoryManagerObjectI();
	IloMemoryManagerI * getMemoryManager() const { return _mem; }
	IloEnvI* getEnv() const;
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloMemoryManagerI *);
	void operator delete(void *, const IloMemoryManager&);
#endif
	void operator delete(void * obj, size_t size);
};

class ILO_EXPORTED IloRttiMemoryManagerObjectI : public IloRtti {
	ILORTTIDECL
protected:
	IloMemoryManagerI * _mem;
public:
	IloRttiMemoryManagerObjectI(IloMemoryManagerI * mem) : _mem(mem) { }
	virtual ~IloRttiMemoryManagerObjectI();
	IloMemoryManagerI * getMemoryManager() const { return _mem; }
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloMemoryManagerI *);
	void operator delete(void *, const IloMemoryManager&);
	inline void operator delete(void*, IloAllocationStack*) {}
#endif
	void operator delete(void * obj, size_t size);
	IloEnvI* getEnv() const;
};

class IloExtensibleRttiMemoryManagerObjectI : public IloRttiMemoryManagerObjectI {
public:
	IloExtensibleRttiMemoryManagerObjectI(IloMemoryManagerI* mem)
		: IloRttiMemoryManagerObjectI(mem), _ext(0) { }
	~IloExtensibleRttiMemoryManagerObjectI();
	ILORTTIDECL
		ILOEXTENSIONI
};

template<class IMPL> inline IMPL* Ilo_static_cast_IloRtti(IloRtti* ptr) {
	return static_cast<IMPL*>(ptr);
}
template<class IMPL> inline IMPL* Ilo_static_cast_IloRtti(const IloRtti* ptr) {
	return static_cast<const IMPL*>(ptr);
}
template<class IMPL> inline IMPL* Ilo_static_cast_IloRtti(void* ptr) {
	IloRtti* obj = static_cast<IloRtti*>(ptr);
	return Ilo_static_cast_IloRtti<IMPL>((IloRtti*)obj);
}
template<class IMPL> inline IMPL* Ilo_dynamic_cast_IloRtti(IloRtti* ptr) {
#ifdef NDEBUG
	return (IMPL*)ptr;
#else
	if (! ptr )
          throw IloWrongUsage("dynamic cast: null pointer");
	if ( !ptr->isType(IMPL::GetTypeInfo()) )
          throw IloWrongUsage(ptr->getRttiTypeName());
        return static_cast<IMPL*>(ptr);
#endif
}
template<class IMPL>
inline IMPL* Ilo_dynamic_cast_IloRtti(const IloRtti* ptr) {
#ifdef NDEBUG
	return (IMPL*)ptr;
#else
	if (! ptr )
          throw IloWrongUsage("dynamic cast: null pointer");
	if ( !ptr->isType(IMPL::GetTypeInfo()) )
          throw IloWrongUsage(ptr->getRttiTypeName());
        return static_cast<IMPL*>((IloRtti*)ptr);
#endif
}

template<class IMPL>
inline IMPL* Ilo_dynamic_cast_IloRtti(void* ptr) {
	IloRtti* obj =  static_cast<IloRtti*>(ptr);
	return Ilo_dynamic_cast_IloRtti<IMPL>(obj);
}

class IloOplBaseObjectI : public IloRttiEnvObjectI {
	ILORTTIDECL
public:
	IloOplBaseObjectI(IloEnvI* env) : IloRttiEnvObjectI(env){}
	virtual ~IloOplBaseObjectI(){}
	virtual IloBool isSymbol() const { return IloFalse; }
};

#define ILO_PLUGIN_HAS_ENV
class IloPlugIn : public IloRttiEnvObjectI {
	ILORTTIDECL
		IloPlugIn* _next;
	void setNext(IloPlugIn* next) { _next = next; }
	IloPlugIn* getNext() const { return _next; }
public:
	IloPlugIn(IloEnvI * env) : IloRttiEnvObjectI(env), _next(0) { }
};

class IloNotifiedPlugIn : public IloPlugIn {
	ILORTTIDECL
		IloAlgorithmI* _alg;
public:
	IloNotifiedPlugIn(IloEnvI * env) : IloPlugIn(env), _alg(0) { }
	virtual void apply(const IloChange& chg, const IloExtractableI* obj) = 0;
	IloAlgorithmI* getAlgorithm() const{ return _alg; }
	void setAlgorithm(IloAlgorithmI* alg){ _alg = alg; }
	virtual void removeAlgorithm(IloAlgorithmI*) {}
};

class IloPlugInAddon : public IloRttiEnvObjectI {
	ILORTTIDECL
		IloPlugInAddon* _next;
public:
	IloPlugInAddon(IloEnvI * env) : IloRttiEnvObjectI(env), _next(0) {}
	virtual void init(IloPlugIn*) = 0;
	virtual void end(IloPlugIn*) = 0;
	void setNext(IloPlugInAddon* addon) { _next = addon; }
	IloPlugInAddon* getNext() const { return _next; }
};

class IloPlugInManager {
public:
	class IloCell {
		friend class IloPlugInManager;
	private:
		const char* _name;
		IloPlugIn* _plug;
		IloPlugInAddon* _addon;
		IloCell* _next;
	public:
		IloCell(const char* name) : _name(name), _plug(0), _addon(0), _next(0) {}
		void setNext(IloCell* cell) { _next = cell; }
		void add(IloPlugIn* plug) { plug->setNext(_plug); _plug = plug; }
		void add(IloPlugInAddon* addon) { addon->setNext(_addon); _addon = addon; }
		IloCell* getNext() const { return _next; }
		IloPlugInAddon* getAddons() const { return _addon; }
		IloPlugIn* getPlugIns() const { return _plug; }
		const char* getName() const { return _name; }
		void end();
	};
private:
	IloCell* _list;
	IloEnvI* _env;

	class IloNotifiedCell {
		friend class IloPlugInManager;
	private:
		IloNotifiedPlugIn* _plug;
		IloNotifiedCell* _next;
	public:
		IloNotifiedCell(IloNotifiedPlugIn* plug, IloNotifiedCell* next=0) : _plug(plug), _next(next) {}
		IloNotifiedPlugIn* getNotifiedPlugIn() const {return _plug;}
		IloNotifiedCell* getNext() const {return _next;}
	};
	IloNotifiedCell* _notifiedList;
public:
	IloPlugInManager(IloEnvI* env) : _list(0), _notifiedList(0) {
		_env = env;
	}
	~IloPlugInManager();
	void add(IloPlugIn* plug, const char* name);
	void remove(IloPlugIn* plug, const char* name);
	void addNotified(IloNotifiedPlugIn* plug);
	void removeNotified(IloNotifiedPlugIn* plug);
	void add(IloPlugInAddon* addon, const char* name);
	void remove(IloPlugInAddon* addon, const char* name);
	IloCell* findCell(const char* name);
	IloCell* findOrCreateCell(const char* name);
	IloBool isCreated(IloTypeInfo, const char* name) const;
	void apply(const IloChange& chg, const IloExtractableI* obj);
	void removeAlgorithm(IloAlgorithmI* algo);
};

class IloDestroyableI : public IloRttiEnvObjectI {
	ILORTTIDECL
protected:
	void owns(IloDestroyableI * d) const { _env->removeDestroyable(d); }
public:
	IloDestroyableI(IloEnvI*);
	virtual ~IloDestroyableI();
};

class IloDestroyable {
	IloDestroyableI* _impl;
public:
	IloDestroyable(IloDestroyableI* impl=0) : _impl(impl) {}
	IloDestroyableI* getImpl() const {return _impl;}
	void end() {
		delete _impl;
		_impl = 0;
	}
};

class IloRttiEnvObjectI;
class IloOplDisplayI : public IloDestroyableI {
	ILORTTIDECL
public:
	IloOplDisplayI(IloEnvI* env) : IloDestroyableI(env) {}
	virtual char* display(const IloRttiEnvObjectI* obj) = 0;
	static void Register(IloEnvI* env, IloOplDisplayI* cb);
	static void Unregister(IloEnvI* env, IloOplDisplayI* cb);
	static IloBool HasInstance(IloEnvI* env);
	static IloOplDisplayI* GetInstance(IloEnvI* env);
};

class IloNamedPropertyI : public IloRttiMemoryManagerObjectI {
	ILORTTIDECL
private:
	IloInt _refCount;
protected:
	char * cloneString(const char * s) const;
	void freeString(char * s) const;

public:
	
	IloNamedPropertyI(IloMemoryManager env);
	
	virtual ~IloNamedPropertyI();
	
	virtual void display(ILOSTD(ostream)& out) const;
	
	virtual const char * getName() const = 0;
	
	virtual IloNamedPropertyI * makeClone(IloMemoryManager env) const;

	class Exception : public IloException {
	public: Exception(const char * m) : IloException(m) { }
	};

	void increaseRef() { _refCount++; }
	void decreaseRef() { _refCount--; }
	IloInt getReferences() const { return _refCount; }
};

class IloNamedProperty {
protected:
	IloNamedPropertyI * _impl;
public:
	
	IloNamedProperty(IloNamedPropertyI * impl = 0) : _impl(impl) { }
	IloNamedPropertyI * getImpl() const { return _impl; }
	void display(ILOSTD(ostream) & stream) const {
		IloAssert(getImpl() != 0, "IloNamedProperty: using empty handle");
		getImpl()->display(stream);
	}
	void end() {
		if (_impl != 0 && _impl->getReferences() == 0)
			delete _impl;
		_impl = 0;
	}
	const char * getName() const {
		IloAssert(getImpl() != 0, "IloNamedProperty: using empty handle");
		return getImpl()->getName();
	}
	inline IloNamedProperty makeClone(IloMemoryManager env) const;

	typedef IloNamedPropertyI ImplClass;
};

ILOSTD(ostream) & operator << (ILOSTD(ostream) & stream, IloNamedProperty np);

class IloPreNamedPropertyI : public IloNamedPropertyI {
	ILORTTIDECL
private:
	const char * _name;
public:
	IloPreNamedPropertyI(IloMemoryManager env, const char * name);
	IloNamedPropertyI * makeClone(IloMemoryManager env) const ILO_OVERRIDE;
	~IloPreNamedPropertyI();
	const char * getName() const ILO_OVERRIDE { return _name; }
};

#define ILONAMEDPROPERTYINLINEHANDLE(NM,T) \
class NM : public IloNamedProperty { \
public: \
	typedef name2(NM, I) ImplClass; \
	NM(IloMemoryManager env, const char * name, T value); \
	NM(name2(NM, I) * impl = 0) : IloNamedProperty(impl) { } \
	name2(NM, I) * getImpl() const { \
	return (name2(NM, I) *)IloNamedProperty::getImpl(); \
} \
	T getValue() const { \
	IloAssert(getImpl() != 0, "IloNamedProperty: using empty handle"); \
	return getImpl()->getValue(); \
} \
	void setValue(T value) const { \
	IloAssert(getImpl() != 0, "IloNamedProperty: using empty handle"); \
	getImpl()->setValue(value); \
} \
	NM makeClone(IloMemoryManager env) const; \
	typedef T EmbeddedType; \
};

#define ILONAMEDPROPERTYH(NM, T) \
class name2(NM, I) : public IloPreNamedPropertyI { \
	ILORTTIDECL \
private: \
	T _value; \
public: \
	name2(NM, I)(IloMemoryManager env, const char * name, T value); \
	~name2(NM, I)(); \
	void display(ILOSTD(ostream) & out) const ILO_OVERRIDE; \
	T getValue() const { return _value; } \
	void setValue(T value); \
	IloNamedPropertyI * makeClone(IloMemoryManager env) const ILO_OVERRIDE; \
	typedef T EmbeddedType; \
}; \
	ILONAMEDPROPERTYINLINEHANDLE(NM,T)

ILONAMEDPROPERTYH(IloNamedIntProperty, IloInt)
ILONAMEDPROPERTYH(IloNamedNumProperty, IloNum)
ILONAMEDPROPERTYH(IloNamedAnyProperty, IloAny)
ILONAMEDPROPERTYH(IloNamedStringProperty, const char *)

#define ILONAMEDPROPERTYCPPDTOR(NM, T) \
	ILORTTI(name2(NM, I), IloPreNamedPropertyI) \
	name2(NM, I)::name2(NM, I)(IloMemoryManager env, const char * name, T value) \
	: IloPreNamedPropertyI(env, name), _value(value) { } \
	void name2(NM, I)::display(ostream & out) const { \
	out << getName() << " (" << _value << ")"; \
} \
	void name2(NM, I)::setValue(T value) { _value = value; } \
	IloNamedPropertyI * name2(NM, I)::makeClone(IloMemoryManager env) const { \
	return new (env) name2(NM, I)(env, getName(), _value); \
} \
	NM::NM(IloMemoryManager env, const char * name, T value) \
	: IloNamedProperty(new (env) name2(NM, I)(env, name, value)) { } \
	NM NM::makeClone(IloMemoryManager env) const { \
	return (name2(NM, I) *)getImpl()->makeClone(env.getImpl());	\
} \
	name2(NM, I)::~name2(NM, I)()

#define ILONAMEDPROPERTYCPP(NM, T) ILONAMEDPROPERTYCPPDTOR(NM, T) { }

class IloNamedPropertySetI : public IloMemoryManagerObjectI {
private:
	struct PropertyList {
		IloNamedPropertyI * _np;
		PropertyList * _next;
		PropertyList * _prev;

		PropertyList(IloNamedPropertyI * np) : _np(np), _next(0), _prev(0) { }

		void end(IloMemoryManagerI * env);
	};

	PropertyList * _list;

	PropertyList * getElem(const char * name) const;
	PropertyList * getElem(IloNamedPropertyI * np) const;
	void remove(PropertyList * elem);
	void fastAdd(IloNamedPropertyI * np);
public:
	IloNamedPropertySetI(IloMemoryManager env);
	~IloNamedPropertySetI();
	void addProperty(IloNamedPropertyI * np);
	void addProperties(IloNamedPropertySetI * nps);
	void addClonedProperty(IloNamedPropertyI * np);
	void removeAllProperties();
	IloBool hasProperty(const char * name) const { return getElem(name) != 0; }
	IloNamedPropertyI * getProperty(const char * name) const;
	IloNamedPropertyI * getPropertyOrNull(const char * name) const;
	IloNamedPropertySetI * makeClone(IloMemoryManager env) const;
	void removeProperty(const char * name);
	void removeProperty(IloNamedPropertyI * property);

	class Iterator;
	friend class Iterator;
	class Iterator {
	private:
		PropertyList * _list;
	public:
		Iterator(IloNamedPropertySetI * nps) : _list(0) {
			if (nps != 0)
				_list = nps->_list;
		}
		IloNamedPropertyI * operator * () const {
			IloAssert(_list != 0, "IloNamedPropertySetI::Iterator overflow");
			return _list->_np;
		}
		IloBool ok() const { return _list != 0; }
		Iterator& operator++() {
			IloAssert(_list != 0, "IloNamedPropertySetI::Iterator overflow");
			_list = _list->_next;
			return *this;
		}
	};
};

class IloNamedPropertySet {
private:
	IloNamedPropertySetI * _impl;
public:
	IloNamedPropertySet(IloMemoryManager env);
	IloNamedPropertySet(IloNamedPropertySetI * impl = 0) : _impl(impl) { }
	IloNamedPropertySetI * getImpl() const { return _impl; }
	void end() {
		delete _impl;
		_impl = 0;
	}
	void addProperty(IloNamedProperty np) {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		IloAssert(np.getImpl() != 0, "IloNamedProperty: using empty handle");
		_impl->addProperty(np.getImpl());
	}
	void addProperties(IloNamedPropertySet nps) {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		IloAssert(nps.getImpl() != 0, "IloNamedPropertySet: using empty handle");
		_impl->addProperties(nps.getImpl());
	}
	void addClonedProperty(IloNamedProperty np) {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		IloAssert(np.getImpl() != 0, "IloNamedProperty: using empty handle");
		_impl->addClonedProperty(np.getImpl());
	}
	inline IloMemoryManager getMemoryManager() const;
	IloNamedProperty getProperty(const char * name) const {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		IloAssert(name != 0, "IloNamedPropertySet: using null string");
		return _impl->getProperty(name);
	}
	void removeAllProperties() {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		_impl->removeAllProperties();
	}
	IloBool hasProperty(const char * name) const {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		IloAssert(name != 0, "IloNamedPropertySet: using null string");
		return _impl->hasProperty(name);
	}
	inline IloNamedPropertySet makeClone(IloMemoryManager env) const;
	void removeProperty(const char * name) const {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		IloAssert(name != 0, "IloNamedPropertySet: using null string");
		_impl->removeProperty(name);
	}
	void removeProperty(IloNamedProperty np) const {
		IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
		IloAssert(np.getImpl() != 0, "IloNamedProperty: using empty handle");
		_impl->removeProperty(np.getImpl());
	}

	class Iterator : public IloNamedPropertySetI::Iterator {
	public:
		Iterator(IloNamedPropertySet nps)
			: IloNamedPropertySetI::Iterator(nps.getImpl()) { }
	};
	class BadType : public IloException {
	public:
		BadType(const char * name) : IloException(name) {
			IloAssert(name != 0, "IloNamedPropertySet::BadType - empty name");
		}
		void print(ILOSTD(ostream) & out) const ILO_OVERRIDE;
	};

	class AlreadyPresent : public IloException {
	public:
		AlreadyPresent(const char * name) : IloException(name) {
			IloAssert(name != 0, "IloNamedPropertySet::AlreadyPresent - empty name");
		}
		void print(ILOSTD(ostream) & out) const ILO_OVERRIDE;
	};
	class NotPresent : public IloException {
	private:
		const IloNamedPropertyI * _np;
	public:
		NotPresent(const char * name) : IloException(name) {
			IloAssert(name != 0, "IloNamedPropertySet::NotPresent - empty name");
		}
		NotPresent(const IloNamedPropertyI * np)
			: IloException(np->getName()), _np(np) { }
		void print(ILOSTD(ostream) & out) const ILO_OVERRIDE;
	};
};

template <class NP, class O>
void IloGetPropertyI(const char * name, const O * obj, NP & npref) {
	typedef typename NP::ImplClass NPImplClass;

	IloAssert(name != 0, "IloGetProperty: empty name");
	IloNamedPropertyI * np = obj->getProperty(name);
	if (np == 0)
		throw IloNamedPropertySet::NotPresent(name);
	if (np->isType(NPImplClass::GetTypeInfo()))
		npref = NP((NPImplClass *)np);
	else
		throw IloNamedPropertySet::BadType(name);
}

template <class NP, class O>
void IloGetProperty(const char * name, const O obj, NP & npref) {
	IloGetPropertyI<NP>(name, obj.getImpl(), npref);
}

template <class NP, class O>
void IloGetPropertyValueI(const char * name,
						  const O * obj,
						  typename NP::EmbeddedType & t) {
							  NP np;
							  IloGetPropertyI(name, obj, np);
							  t = np.getValue();
}

template <class NP, class O>
void IloGetPropertyValue(const char * name, \
						 const O obj,
						 typename NP::EmbeddedType & t) {
							 IloGetPropertyValueI<NP>(name, obj.getImpl(), t);
}

template <class NP, class O>
void IloSetPropertyValueI(const char * name,
						  O * obj,
						  typename NP::EmbeddedType t) {
							  if (obj->hasProperty(name)) {
								  NP np;
								  IloGetPropertyI(name, obj, np);
								  np.setValue(t);
							  } else {
								  NP np(obj->getMemoryManager(), name, t);
								  obj->addProperty(np.getImpl());
							  }
}

template <class NP, class O>
void IloSetPropertyValue(const char * name,
						 O obj,
						 typename NP::EmbeddedType t) {
							 IloSetPropertyValueI<NP>(name, obj.getImpl(), t);
}

#define ILOEXTENSIONHNONINLINE(hname) \
	 \
	const char * getName() const; \
	 \
	void setName(const char * name) const; \
	 \
	IloAny getObject() const; \
	 \
	void setObject(IloAny object) const; \
	 \
        void setLocation(const char* fileName, int lineNumber) const; \
	IloNamedPropertySet getProperties() const; \
	IloNamedProperty getProperty(const char * name) const; \
	IloNamedPropertyI * getPropertyI(const char * name) const; \
	IloBool hasProperty(const char * name) const; \
	void addProperty(IloNamedProperty np) const; \
	void addProperties(IloNamedPropertySet nps) const; \
	void addClonedProperty(IloNamedProperty np) const; \
	void removeProperty(const char * name) const; \
	void removeProperty(IloNamedProperty np) const; \
	void removeAllProperties() const; \
	void setProperties(IloNamedPropertySet nps) const; \
	IloInt getIntProperty(const char * name) const; \
	IloNum getNumProperty(const char * name) const; \
	IloAny getAnyProperty(const char * name) const; \
	const char * getStringProperty(const char * name) const; \
	void setIntProperty(const char * name, IloInt) const; \
	void setNumProperty(const char * name, IloNum) const; \
	void setAnyProperty(const char * name, IloAny) const; \
	void setStringProperty(const char * name, const char *) const;

#define ILOEXTENSIONH(hname) \
	 \
	const char * getName() const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	return getImpl()->getName(); \
} \
	 \
	void setName(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->setName(name); \
} \
	 \
	IloAny getObject() const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	return getImpl()->getObject(); \
} \
	 \
	void setObject(IloAny obj) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->setObject(obj); \
} \
	 \
        void setLocation(const char* fileName, int lineNumber) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->setLocation(fileName, lineNumber); \
} \
	IloBool hasProperty(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->hasProperty(name); \
} \
	IloNamedPropertySet getProperties() const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	return getImpl()->getProperties(); \
} \
	IloNamedProperty getProperty(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getProperty(name); \
} \
	IloNamedPropertyI * getPropertyI(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getProperty(name); \
} \
	void addProperty(IloNamedProperty np) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(np.getImpl() != 0, "IloNamedProperty: Using empty handle"); \
	getImpl()->addProperty(np.getImpl()); \
} \
	void addProperties(IloNamedPropertySet nps) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(nps.getImpl() != 0, "IloNamedPropertySet: Using empty handle"); \
	getImpl()->addProperties(nps.getImpl()); \
} \
	void addClonedProperty(IloNamedProperty np) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(np.getImpl() != 0, "IloNamedProperty: Using empty handle"); \
	getImpl()->addClonedProperty(np.getImpl()); \
} \
	void removeProperty(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->removeProperty(name); \
} \
	void removeProperty(IloNamedProperty np) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(np.getImpl() != 0, "IloNamedProperty: Using empty handle"); \
	getImpl()->removeProperty(np.getImpl()); \
} \
	void removeAllProperties() const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->removeAllProperties(); \
} \
	void setProperties(IloNamedPropertySet nps) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(nps.getImpl() != 0, "IloNamedPropertySet: Using empty handle"); \
	getImpl()->setProperties(nps.getImpl()); \
} \
	IloInt getIntProperty(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getIntProperty(name); \
} \
	IloNum getNumProperty(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getNumProperty(name); \
} \
	IloAny getAnyProperty(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getAnyProperty(name); \
} \
	const char * getStringProperty(const char * name) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getStringProperty(name); \
} \
	void setIntProperty(const char * name, IloInt i) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setIntProperty(name, i); \
} \
	void setNumProperty(const char * name, IloNum n) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setNumProperty(name, n); \
} \
	void setAnyProperty(const char * name, IloAny a) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setAnyProperty(name, a); \
} \
	void setStringProperty(const char * name, const char * str) const { \
	IloAssert(getImpl() != 0, ILO_STRINGIZE(hname) ": Using empty handle"); \
	IloAssert(name != 0, ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setStringProperty(name, str); \
}

class ILO_EXPORTED IloEnv {
	IloEnvI* _impl;
public:
	enum Flag {
		UseMemory = 1,
		SafeAllocation = 2,
		MtSafeNormalization = 4,
		AssumeNormalizedLinExpr = 8,
		UseDetailedDisplay = 32
	};

	
	IloEnv();
	
	IloEnv(IloEnvI* impl) : _impl(impl) {}
	
	IloEnvI* getImpl() const {
		return _impl;
	}

	
	const char* getVersion() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->getVersion());
	}

	void requestAbort() const {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		getImpl()->requestAbort();
	}

	class AbortException : public IloException {
	public:
		AbortException();
	};

	
	void end() {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		delete _impl;
		_impl = 0;
	}
	
	ILOSTD(ostream)& getNullStream() const   {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->getNullStream());
	}

	
	ILOSTD(ostream)& out() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->out());
	}
	
	void setOut(ILOSTD(ostream)& s) {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		getImpl()->setOut(s);
	}

	
	ILOSTD(ostream)& warning() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->warning());
	}
	
	void setWarning(ILOSTD(ostream)& s) {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		getImpl()->setWarning(s);
	}

	
	ILOSTD(ostream)& error() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->error());
	}
	IloConcertAlloc * getGeneralAllocator() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return getImpl()->getGeneralAllocator();
	}

	IloBool hasSymbol(const char* key) const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return getImpl()->hasSymbol(key);
	}
	IloSymbol getSymbol(const char* key) const;

	IloSymbol makeSymbol(const char* key) const;

	const char * intern(const char * name) const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return getImpl()->intern(name);
	}
	
	IloRandom getRandom() const;
	
	void setError(ILOSTD(ostream)& s) {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		getImpl()->setError(s);
	}

	
	IloNum getTime() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->getTime());
	}
	
	void printTime() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		getImpl()->printTime();
	}

	
	void assumeNormalizedLinExpr(IloBool val) const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		getImpl()->assumeNormalizedLinExpr(val);
	}
	
	IloBool assumeNormalizedLinExpr() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->assumeNormalizedLinExpr());
	}

	
	void assumeNormalizedQuadExpr(IloBool val) const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		getImpl()->assumeNormalizedQuadExpr(val);
	}
	
	IloBool assumeNormalizedQuadExpr() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->assumeNormalizedQuadExpr());
	}

	
	void setNormalizer(IloBool val) const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		getImpl()->assumeNormalizedLinExpr((val == IloFalse));
		getImpl()->assumeNormalizedQuadExpr((val == IloFalse));
	}

	IloBool useNormalizer() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->assumeNormalizedLinExpr() == IloFalse);
	}
	

	IloInt getMemoryUsage() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->getMemoryUsage());
	}
	

	IloInt getTotalMemoryUsage() const {
		IloAssert (getImpl(), "Using empty IloEnv handle.");
		return (getImpl()->getTotalMemoryUsage());
	}

	
	void setDeleter(IloDeleterMode mode) const {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		getImpl()->setDeleter(mode);
	}
	
	void unsetDeleter() const {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		getImpl()->unsetDeleter();
	}
	void inhibitDeleter(IloBool v) const {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		IloAssert(getImpl()->getDeleter(), "IloEnv has no deleter.");
		getImpl()->inhibitDeleter(v);
	}
	void setDeleterVerbosityLevel(IloInt level) const {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		IloAssert(getImpl()->getDeleter(), "IloEnv has no deleter.");
		getImpl()->setDeleterVerbosityLevel(level);
	}
	
	IloExtractableI* getExtractable(IloInt id) {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		IloTestAndRaise(getImpl()->isValidId(id), "This ID is not a valid ID: no extractable exists");
		return getImpl()->getExtractable(id);
	}

	
	IloInt getMaxId() const {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		return getImpl()->getMaxId();
	}
	
	IloBool isValidId(IloInt id) const {
		IloAssert(getImpl(), "Using empty IloEnv handle.");
		return getImpl()->isValidId(id);
	}

	ILOEXTENSIONHNONINLINE(IloEnv)

		
	class IncompatibleMemoryManagerException : public IloException
	{ public: IncompatibleMemoryManagerException(); };

	
	IloEnv(IloMemoryManager memoryManager);

	
	operator IloMemoryManager () const { return getGeneralAllocator(); }
};

inline IloBool IloEnvI::useDetailedDisplay() const {
	return (_flags & IloEnv::UseDetailedDisplay);
}
inline void IloEnvI::useDetailedDisplay(IloBool b) {
	if (b)
		_flags |= IloEnv::UseDetailedDisplay;
	else
		_flags &= ~IloEnv::UseDetailedDisplay;
}

inline void* operator new (size_t sz, const IloEnv& env) {
	IloAssert(env.getImpl(), "Using empty IloEnv handle.");
	return (env.getImpl()->alloc(sz));
}
#ifdef ILODELETEOPERATOR
inline void operator delete(void*, const IloEnv&) {}
inline void operator delete(void*, IloEnv) {}
#endif

#if defined(ILONEWOPERATOR)
inline void* operator new[] (size_t s, const IloEnv& env) {
	IloAssert(env.getImpl(), "Using empty IloEnv handle.");
	return (operator new (s, env.getImpl()));
}
# ifdef ILODELETEOPERATOR
inline void operator delete[] (void*, const IloEnv&) {}
inline void operator delete[] (void*, IloEnv) {}
# endif
#endif

inline IloNamedProperty IloNamedProperty::makeClone(IloMemoryManager env) const {
	IloAssert(getImpl() != 0, "IloNamedProperty: using empty handle");
	IloAssert(env.getImpl() != 0, ILO_STRINGIZE(IloMemoryManager) ": using empty handle");
	return getImpl()->makeClone(env.getImpl());
}

inline IloNamedPropertySet IloNamedPropertySet::makeClone(IloMemoryManager env) const {
	IloAssert(getImpl() != 0, "IloNamedPropertySet: using empty handle");
	IloAssert(env.getImpl() != 0, ILO_STRINGIZE(IloMemoryManager) ": using empty handle");
	return getImpl()->makeClone(env.getImpl());
}

inline IloMemoryManager IloNamedPropertySet::getMemoryManager() const {
	IloAssert(_impl != 0, "IloNamedPropertySet: using empty handle");
	return _impl->getMemoryManager();
}

class IloDiscreteDataCollectionI;
class IloEndCollectionCallbackI;
class IloOplDisplayI;

#include <math.h>

#ifndef M_LOG2E
#define M_LOG2E 1.4426950408889634074
#endif

IloInt IloDownDiv(IloInt a, IloInt b);
IloInt IloUpDiv(IloInt a, IloInt b);

inline IloNum IloFloor(IloNum val) {
	return floor(val);
}

inline IloNum IloCeil(IloNum val) {
	return ceil(val);
}
IloInt IloTrunc(IloNum val);

IloNum IloRound(IloNum val);
IloNum IloOplRound(IloNum x);

inline IloNum IloAbs(IloNum val) {return fabs(val);}
inline IloInt IloAbs(IloInt val) {return (val > 0 ? val : -val);}
inline IloInt IloAbs(int val) {return (val > 0 ? val : -val);}

inline IloNum IloCos(IloNum val) {return cos(val);}

inline IloNum IloSin(IloNum val) {return sin(val);}

inline IloNum IloTan(IloNum val) {return tan(val);}

inline IloNum IloArcCos(IloNum val) {return acos(val);}

inline IloNum IloArcSin(IloNum val) {return asin(val);}

inline IloNum IloArcTan(IloNum val) {return atan(val);}

inline IloNum IloSquare(IloNum val) {return val*val;}

inline IloInt IloSquare(IloInt val) {return (IloInt)val*val;}

inline IloInt IloSquare(int val) {return val*val;}

inline IloNum IloExponent(IloNum val) {return exp(val);}

inline IloNum IloLog(IloNum val) {return log(val);}

inline IloNum IloPower(IloNum val1, IloNum val2) { return pow(val1, val2); }

inline IloNum IloMin(IloNum val1, IloNum val2) {
	return (val1<val2) ? val1 : val2;
}
inline IloInt IloMin(IloInt val1, IloInt val2) {
	return (val1<val2) ? val1 : val2;
}
inline IloInt IloMin(int val1, IloInt val2) {
	return (val1<val2) ? val1 : val2;
}
inline IloInt IloMin(IloInt val1, int val2) {
	return (val1<val2) ? val1 : val2;
}
inline IloInt IloMin(int val1, int val2) {
	return (val1<val2) ? val1 : val2;
}
inline IloNum IloMin(int val1, IloNum val2) {
	return (val1<val2) ? val1 : val2;
}
inline IloNum IloMin(IloNum val1, int val2) {
	return (val1<val2) ? val1 : val2;
}
inline IloNum IloMin(IloInt val1, IloNum val2) {
	return (val1<val2) ? IloNum(val1) : val2;
}
inline IloNum IloMin(IloNum val1, IloInt val2) {
	return (val1<val2) ? val1 : IloNum(val2);
}

inline IloNum IloMax(IloNum val1, IloNum val2) {
	return (val1>val2) ? val1 : val2;
}
inline IloInt IloMax(IloInt val1, IloInt val2) {
	return (val1>val2) ? val1 : val2;
}
inline IloInt IloMax(int val1, IloInt val2) {
	return (val1>val2) ? val1 : val2;
}
inline IloInt IloMax(IloInt val1, int val2) {
	return (val1>val2) ? val1 : val2;
}
inline IloInt IloMax(int val1, int val2) {
	return (val1>val2) ? val1 : val2;
}
inline IloNum IloMax(int val1, IloNum val2) {
	return (val1>val2) ? val1 : val2;
}
inline IloNum IloMax(IloNum val1, int val2) {
	return (val1>val2) ? val1 : val2;
}
inline IloNum IloMax(IloInt val1, IloNum val2) {
	return (val1>val2) ? IloNum(val1) : val2;
}
inline IloNum IloMax(IloNum val1, IloInt val2) {
	return (val1>val2) ? val1 : IloNum(val2);
}

IloBool IloNumIsInteger(IloNum val);

inline IloNum IloFract(IloNum x) {
	return x>=0 ? x-IloFloor(x) : IloCeil(x)-x;
}

inline IloNum IloDistToInt(IloNum x) { return IloAbs( IloRound(x)-x ); }

inline IloInt IloNumToInt(IloNum x) {
	if (IloNumIsInteger(x)) return (IloInt)x;
	throw IloBadConversion();
	ILOUNREACHABLE(return 0);
}

inline IloNum IloLog10(IloNum x) { return log10(x); }

inline IloNum IloLog2(IloNum x) { return IloLog(x)/IloLog(2); }

class IloArrayError : public IloException {
public:
	IloArrayError(const char* message) : IloException(message) {}
};

class IloArrayI : public IloMemoryManagerObjectI {
	friend class IloMapI;
private:
	IloInt       _max;    
	IloInt const _sizeof; 
	IloInt       _size;   
	IloInt       _mod;    
	IloInt       _shift;  
	char**       _data;   

	void setShiftMod();
	void allocData(IloInt);
	void grow(IloInt toSize);
	inline void* getData(IloInt i) const {
		return (void*)(&_data[i>>_shift][(i&_mod)*_sizeof]);
	}
	IloInt getShift() const {  return _shift; }
public:
	IloArrayI(IloMemoryManagerI*, IloInt, IloInt);
	IloArrayI(IloMemoryManagerI*, IloArrayI*);
	IloArrayI(IloEnvI*, IloInt, IloInt);
	IloArrayI(IloEnvI*, IloArrayI*);

	~IloArrayI();
	void zeroData();

	IloInt getSize() const { return _size; }

	inline void* getBase(IloInt i) const { return (void*)(_data[i>>_shift]); }
	inline IloInt getMod() const { return _mod; }

	void clear();
	void remove(IloInt first, IloInt max);
	void setSize(IloInt size) {
		IloAssert(size>=0, "invalid size in IloArrayI::setSize");
		if ( (size>=_max) || (_data[size>>_shift]==(char*)0) )
			grow(size);
		_size = size;
	}
	void addArray(IloArrayI* ar);
	void addArray(IloArrayI* ar, IloInt begin, IloInt end);
};

template <class X> class IloArray  {
private:
	IloArrayI* _impl;
public:
	inline X* data(IloInt i) const {
		return &((X*)getImpl()->getBase(i))[i&getImpl()->getMod()];
	}
	IloArray(IloArrayI* impl) : _impl(impl) {}
	IloArray() : _impl(0) {}
	IloArrayI* getImpl() const { return _impl; }
	
	IloArray(IloEnv env, IloInt max=0) {
		_impl = new (env.getImpl()->getGeneralAllocator()) IloArrayI(env.getImpl()->getGeneralAllocator(), max, (IloInt)sizeof(X)) ;
	}
	IloArray(IloMemoryManager env, IloInt max=0) {
		_impl = new (env.getImpl()) IloArrayI(env.getImpl(), max, (IloInt)sizeof(X)) ;
	}
	
	void end() {
		IloAssert(_impl, "Empty Handle in IloArray::end");
		delete _impl;
		_impl = 0;
	}

	
	IloInt getSize() const {
		IloAssert(_impl, "Empty Handle in IloArray::getSize");
		return _impl->getSize();
	}
	
	IloEnv getEnv() const {
		IloAssert(_impl, "Empty Handle in IloArray::getEnv");
		return _impl->getEnv();
	}
	IloMemoryManager getMemoryManager() const {
		IloAssert(_impl, "Empty Handle in IloArray::getEnv");
		return _impl->getMemoryManager();
	}

	
	const X& operator[] (IloInt i) const {
		IloAssert (_impl, "Empty Handle in IloArray::operator[]");
		IloAssert ( i>=0, "Index is out of bounds operation: negative index");
		IloAssert ( i < _impl->getSize(), "const X& IloArray::operator[] (IloInt i) const : Out of bounds operation: index superior to size of array");
		return *data(i);
	}
	

	X& operator[] (IloInt i) {
		IloAssert (_impl, "Empty Handle in IloArray::operator[]");
		IloAssert ( i>=0, "Index out of bounds operation: negative index");
		IloAssert ( i < _impl->getSize(), "X& IloArray::operator[] (IloInt i) : Out of bounds operation: index superior to size of array");
		return *data(i);
	}

	
	void add(X x) const {
		IloAssert (_impl, "Empty Handle in IloArray::add");
		IloInt size = _impl->getSize();
		_impl->setSize(size+1);
		*data(size) = x;
	}
	
	void add(IloInt more, X x) const {
		IloAssert (_impl, "Empty Handle in IloArray::add");
		IloAssert(more>=0, "Invalid parameter in IloArray::add");
		if (more==0) return;
		IloInt size = _impl->getSize();
		_impl->setSize(size+more);
		while(more-->0)
			*data(size++) = x;
	}
	
	void add(IloArray<X> ax) const {
		IloAssert (_impl, "Empty Handle in IloArray::add");
		_impl->addArray(ax._impl);
	}

	void add(IloArray<X> ax, IloInt begin, IloInt end) const {
		IloAssert (_impl, "Empty Handle in IloArray::add");
		_impl->addArray(ax._impl, begin, end);
	}

	
	void remove(IloInt first, IloInt nb = 1) {
		IloAssert (_impl, "Empty Handle in IloArray::remove");
		_impl->remove(first, nb);
	}

	
	void clear() {
		IloAssert (_impl, "Empty Handle in IloArray::clear");
		_impl->clear();
	}
	IloArrayI* copy() const {
		IloAssert (_impl, "Empty Handle in IloArray::copy");
		return new (_impl->getMemoryManager()) IloArrayI(_impl->getMemoryManager(), _impl);
	}
	void setSize(IloInt size) const {
		IloAssert (_impl, "Empty Handle in IloArray::setSize");
		_impl->setSize(size);
	}
	IloArrayI* makeClone(IloMemoryManagerI* env) const {
		IloAssert (_impl, "Empty Handle in IloArray::copy");
		return new (env) IloArrayI(env, _impl);
	}
	IloArrayI* makeClone(IloEnvI* env) const {
		IloAssert (_impl, "Empty Handle in IloArray::copy");
		return makeClone(env->getMemoryManager());
	}
	void setImpl(IloArrayI* impl){ _impl = impl; }
};

template <class X>

inline ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloArray<X>& a) {
	if (a.getImpl()) {
		out << '[';
		IloInt i, n=a.getSize()-1;
		for (i = 0; i < n; ++i) {
			out << a[i];
			out << ',';
			if ( (i+1) % 10 ) out << ' ';
			else              out << ILOSTD(endl);
		}
		if (n>=0)
			out << a[n];
		out << ']';
	}
	return out;
}

#define IloSimpleArray IloArray
#define IloDefaultArrayI IloArrayI

#include <stdlib.h>
#include <ctype.h>

#define IloRONumArray IloNumArray
#define IloROIntArray IloIntArray
#define IloROBoolArray IloBoolArray

#if (defined(ILO_HP11) && defined(ILO64)) || (defined(ILO_LINUX) && defined(ILO64)) || (defined(ILO_AMD64))
class IloVarArgsNum {
	IloNum _value;
public:
	IloVarArgsNum(int value) : _value((IloNum)value) {}
	IloVarArgsNum(IloInt value) : _value((IloNum)value) {}
	IloVarArgsNum(IloNum value) : _value(value) {}
	IloNum getValue() const {return _value;}
};
class IloVarArgsInt {
	IloInt _value;
public:
	IloVarArgsInt(int value) : _value(value) {}
	IloVarArgsInt(IloInt value) : _value(value) {}
	IloInt getValue() const {return _value;}
};
#endif

typedef IloArray<IloNum> IloNumArrayBase;
class IloIntArray;
class IloIntIndex;

class IloNumArray : public IloNumArrayBase {
public:
	typedef IloDefaultArrayI ImplClass;
    
	IloNumArray() : IloNumArrayBase() {}
	IloNumArray(IloArrayI* i) : IloNumArrayBase(i) {}
	
	IloNumArray(const IloNumArray& cpy) : IloNumArrayBase(cpy)    {}
	
	IloNumArray(const IloEnv env, IloInt n = 0);
	IloNumArray(const IloMemoryManager env, IloInt n = 0);
#ifndef NO_SINGLETON
	IloNumArray(const IloEnv env, IloInt n, IloNum v0);
#endif
#if (defined(ILO_HP11) && defined(ILO64)) || (defined(ILO_LINUX) && defined(ILO64)) || (defined(ILO_AMD64))
	IloNumArray(const IloEnv env, IloInt n, int v, IloVarArgsNum v0 ...);
	IloNumArray(const IloEnv env, IloInt n, IloInt v, IloVarArgsNum v0 ...);
	IloNumArray(const IloEnv env, IloInt n, IloNum v, IloVarArgsNum v0 ...);
#else
	
#ifndef ILO_ZOS
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, ...);
#else
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12, IloNum f13);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12, IloNum f13, IloNum f14);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12, IloNum f13, IloNum f14, IloNum f15);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12, IloNum f13, IloNum f14, IloNum f15, IloNum f16 );
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12, IloNum f13, IloNum f14, IloNum f15, IloNum f16, IloNum f17);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12, IloNum f13, IloNum f14, IloNum f15, IloNum f16, IloNum f17, IloNum f18);
	IloNumArray(const IloEnv env, IloInt n, IloNum f0, IloNum f1, IloNum f2, IloNum f3, IloNum f4, IloNum f5, IloNum f6, IloNum f7, IloNum f8, IloNum f9, IloNum f10 , IloNum f11, IloNum f12, IloNum f13, IloNum f14, IloNum f15, IloNum f16, IloNum f17, IloNum f18, IloNum f19);
#endif 
#ifndef ILO_ZOS
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1 ...);
#else
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2);
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6);
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12, IloNum v13 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12, IloNum v13, IloNum v14 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12, IloNum v13, IloNum v14, IloNum v15 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12, IloNum v13, IloNum v14, IloNum v15, IloNum v16 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12, IloNum v13, IloNum v14, IloNum v15, IloNum v16, IloNum v17 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12, IloNum v13, IloNum v14, IloNum v15, IloNum v16, IloNum v17, IloNum v18 );
	IloNumArray(const IloEnv env, int n, IloNum v0, IloNum v1, IloNum v2, IloNum v3, IloNum v4, IloNum v5, IloNum v6, IloNum v7, IloNum v8, IloNum v9, IloNum v10, IloNum v11, IloNum v12, IloNum v13, IloNum v14, IloNum v15, IloNum v16, IloNum v17, IloNum v18, IloNum v19 );
#endif 
#ifndef ILO_ZOS
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1 ...);
#else
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17, IloInt v18 );
	IloNumArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1,IloInt v2,IloInt v3,IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17, IloInt v18, IloInt v19 );
#endif 
#ifndef ILO_ZOS
	IloNumArray(const IloEnv env, int n, int v0, int v1 ...);
#else
	IloNumArray(const IloEnv env, int n, int v0, int v1);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16, int v17);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16, int v17, int v18);
	IloNumArray(const IloEnv env, int n, int v0, int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16, int v17, int v18, int v19);
#endif 
#ifndef ILO_ZOS
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1 ...);
#else
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16, int v17 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16, int v17, int v18 );
	IloNumArray(const IloEnv env, IloInt n, int v0, int v1, int v2,int v3, int v4, int v5,int v6,int v7,int v8,int v9, int v10, int v11, int v12, int v13, int v14, int v15, int v16, int v17, int v18, int v19 );
#endif 
#ifndef ILO_ZOS
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1 ...);
#else
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17, IloInt v18 );
	IloNumArray(const IloEnv env, int n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17, IloInt v18, IloInt v19 );
#endif 
#endif
	
	IloNumExprArg operator[] (IloIntExprArg intExp) const;

	const IloNum& operator[] (IloInt i) const {
		return IloNumArrayBase::operator[](i);
	}
	IloNum& operator[] (IloInt i) {
		return IloNumArrayBase::operator[](i);
	}

	IloBool areElementsInteger() const;
	IloBool areElementsBoolean() const;
        IloNumArray& operator=(const IloNumArray& other) {
          setImpl(other.getImpl());
          return *this;
        }

	
	IloIntArray toIntArray() const;

	
	IloBool contains(IloNum value) const;
};

typedef IloDefaultArrayI IloNumArrayI;

inline ILOSTD(istream)& operator>>(ILOSTD(istream)& in, IloNumArray& a) {
	IloAssert (a.getImpl(), "Using empty IloNumArray handle.");
	char ch;
	in >> ch;
	if (ch != '[') {
		in.putback(ch);
		return (in);
	}
	a.clear();
	in >> ch;
	if (ch == ']') {
		return (in);
	}
	else {
		in.putback(ch);
	}
	for(;;) {
		IloNum x;
		in >> x >> ch;
		a.add(x);
		if (ch == ']') break;
		if (ch == ';') break;
		if (ch != ',') {
			in.clear(ILOSTD(ios)::badbit);
			break;
		}
	}
	return (in);
}

#define ILOARRAY_INPUT_OPERATOR(_element, _name) \
	inline ILOSTD(istream)& operator>>(ILOSTD(istream)& in, _name & a) { \
	IloAssert (a.getImpl(), "Using empty " #_name " handle."); \
	char ch;          \
	in >> ch;         \
	if (ch != '[') {  \
	in.putback(ch); \
	return (in);    \
	}                 \
	a.clear();        \
	in >> ch;         \
	if (ch == ']') {  \
	return (in);    \
	} else {          \
	in.putback(ch); \
	}                 \
	for(;;) {         \
	_element x(a.getMemoryManager());  \
	in >> x >> ch;  \
	a.add(x);       \
	if (ch == ']') break; \
	if (ch == ';') break; \
	if (ch != ',') {      \
	in.clear(ILOSTD(ios)::badbit);  \
	break;        \
	}               \
	}                 \
	return (in);      \
}

#define ILOARRAY(_element,_name) \
	typedef IloArray<_element> _name;

typedef IloArray<IloNumArray> IloNumArray2Base;

class IloNumArray2 : public IloNumArray2Base {
public:
	typedef IloDefaultArrayI ImplClass;
	IloNumArray2(IloDefaultArrayI* i=0) : IloNumArray2Base(i) {}
	IloNumArray2(const IloNumArray2& copy) : IloNumArray2Base(copy) {}
	IloNumArray2(const IloEnv env, IloInt n = 0) : IloNumArray2Base(env, n) {}
	IloNumArray2(const IloMemoryManager env, IloInt n = 0) : IloNumArray2Base(env, n) {}
	IloNumArray2(const IloEnv env, IloInt n, const IloNumArray v0);
        IloNumArray2& operator=(const IloNumArray2& other) { setImpl(other.getImpl()); return *this; }
	using IloArray<IloNumArray>::add;
	static IloBool equal(IloNumArray array1, IloNumArray array2);
	IloBool contains(const IloNumArray array) const;
};

ILOARRAY_INPUT_OPERATOR(IloNumArray,IloNumArray2Base)

ILOARRAY(IloNumArray2,IloNumArray3)
ILOARRAY_INPUT_OPERATOR(IloNumArray2,IloNumArray3)
ILOARRAY(IloNumArray3,IloNumArray4)
ILOARRAY_INPUT_OPERATOR(IloNumArray3,IloNumArray4)

#define IloFloatArray IloNumArray

typedef IloArray<IloInt> IloIntArrayBase;

class IloIntArray : public IloIntArrayBase {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloIntArray(IloArrayI* i=0) : IloIntArrayBase(i) {}
	IloIntArray(const IloIntArray& cpy) : IloIntArrayBase(cpy)    {}
	
	IloIntArray(const IloEnv env, IloInt n = 0);
	IloIntArray(const IloMemoryManager env, IloInt n = 0);
#ifndef NO_SINGLETON
	IloIntArray(const IloEnv env, IloInt n, IloInt v0);
#endif
#if (defined(ILO_HP11) && defined(ILO64)) || (defined(ILO_LINUX) && defined(ILO64)) || (defined(ILO_AMD64))
	IloIntArray(const IloEnv env, IloInt n, int v, IloVarArgsInt v0 ...);
	IloIntArray(const IloEnv env, IloInt n, IloInt v, IloVarArgsInt v0 ...);
#else
	
#ifndef ILO_ZOS
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1 ...);
#else
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17, IloInt v18);
	IloIntArray(const IloEnv env, IloInt n, IloInt v0, IloInt v1, IloInt v2, IloInt v3, IloInt v4, IloInt v5, IloInt v6, IloInt v7, IloInt v8, IloInt v9, IloInt v10, IloInt v11, IloInt v12, IloInt v13, IloInt v14, IloInt v15, IloInt v16, IloInt v17, IloInt v18, IloInt v19);
#endif
#endif
	
	IloNumArray toNumArray() const;
	IloBool areElementsBoolean() const;

	const IloInt& operator[] (IloInt i) const {
		return IloIntArrayBase::operator[](i);
	}
	IloInt& operator[] (IloInt i) {
		return IloIntArrayBase::operator[](i);
	}

	
	IloIntExprArg operator[] (IloIntExprArg intExp) const;

	IloAnyArray toAnyArray() const;

        IloIntArray& operator=(const IloIntArray& other) { setImpl(other.getImpl()); return *this; }

	
	void discard(IloInt value);

	
	void discard(IloIntArray ax);

	
	IloBool contains(IloInt value) const;

	
	IloBool contains(IloIntArray ax) const;

};

typedef IloDefaultArrayI IloIntArrayI;

inline ILOSTD(istream)& operator>>(ILOSTD(istream)& in, IloIntArray& a) {
	IloAssert (a.getImpl(), "Using empty IloIntArray handle.");
	char ch;
	in >> ch;
	if (ch != '[') {
		in.putback(ch);
		return (in);
	}
	a.clear();
	in >> ch;
	if (ch == ']') {
		return (in);
	}
	else {
		in.putback(ch);
	}
	for(;;) {
		IloInt x;
#if defined(ILO_MSVC)
		IloNum y;
		in >> y >> ch;
		x = (IloInt)y;
#else
		in >> x >> ch;
#endif
		a.add(x);
		if (ch == ']') break;
		if (ch == ';') break;
		if (ch != ',') {
			in.clear(ILOSTD(ios)::badbit);
			break;
		}
	}
	return (in);
}

#define ILOARRAY(_element,_name) \
	typedef IloArray<_element> _name;

typedef IloArray<IloIntArray> IloIntArray2;

class IloIntArrayArray : public IloIntArray2 {
public:
	typedef IloDefaultArrayI ImplClass;
	IloIntArrayArray(IloDefaultArrayI* i=0) : IloIntArray2(i) {}
	IloIntArrayArray(const IloIntArrayArray& copy) : IloIntArray2(copy) {}
	
	IloIntArrayArray(const IloEnv env, IloInt n = 0) : IloIntArray2(env, n) {}
	IloIntArrayArray(const IloMemoryManager env, IloInt n = 0) : IloIntArray2(env, n) {}
	IloIntArrayArray(const IloEnv env, IloInt n, const IloIntArray v0);
        IloIntArrayArray& operator=(const IloIntArrayArray& other) { setImpl(other.getImpl()); return *this; }
	using IloArray<IloIntArray>::add;
	static IloBool equal(IloIntArray array1, IloIntArray array2);
	IloBool contains(const IloIntArray array) const;
};

ILOARRAY_INPUT_OPERATOR(IloIntArray,IloIntArray2)

ILOARRAY(IloIntArray2,IloIntArray3)
ILOARRAY_INPUT_OPERATOR(IloIntArray2,IloIntArray3)
ILOARRAY(IloIntArray3,IloIntArray4)
ILOARRAY_INPUT_OPERATOR(IloIntArray3,IloIntArray4)

class IloBoolArray : public IloIntArray {
	IloBool areElementsBoolean() const;
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloBoolArray(IloArrayI* i=0) : IloIntArray(i) {}
	
	IloBoolArray(const IloEnv env, IloInt n = 0) : IloIntArray(env, n) {}
	IloBoolArray(const IloMemoryManager env, IloInt n = 0) : IloIntArray(env, n) {}

	
	IloBoolArray(const IloEnv env, IloInt n, const IloBool v0);
#if (defined(ILO_HP11) && defined(ILO64)) || (defined(ILO_LINUX) && defined(ILO64)) || (defined(ILO_AMD64))
	IloBoolArray(const IloEnv env, IloInt n, const IloBool v0, IloVarArgsInt v1 ...);
#else
	

	IloBoolArray(const IloEnv env, IloInt n, const IloBool v0, const IloBool v1 ...);
#endif

	
	void add(const IloBoolArray x)         { IloIntArray::add (x); }
	
	void add(const IloBool x)              { IloIntArray::add (x); }
	
	void add(IloInt more, const IloBool x) { IloIntArray::add (more, x); }
};

typedef IloDefaultArrayI IloBoolArrayI;

class IloBaseDeleterI : public IloNotifiedPlugIn {
public:
	IloBaseDeleterI(IloEnvI *env) : IloNotifiedPlugIn(env) { }
	virtual void removeFromAll(IloInt id)=0;
	virtual void addToBeExtracted(const IloExtractable extractable)=0;
	virtual void removeToBeExtracted(const IloExtractable extractable)=0;
	virtual void inhibit(IloBool b)=0;
	virtual void end(IloExtractableI* object, IloBool)=0;
	virtual void fastEnd(IloExtractableI* object)=0;
	virtual void end(const IloExtractableArray objects, IloBool)=0;
	virtual ~IloBaseDeleterI();
	virtual void setVerbosityLevel(IloInt)=0;
	virtual void addLink(IloExtractableI* super, IloExtractableI* sub)=0;
	virtual void removeLink(IloExtractableI* super, IloExtractableI* sub)=0;
	virtual IloBool hasLink(IloExtractableI* user, IloExtractableI* object)=0;
};

#define ILOEXTENSIONCPP0(iname, env, template) \
	template \
	void iname::setName(const char * name) { \
	if (_ext != 0) \
	_ext->setName(env, name); \
	else if (name != 0) \
	IloExtension::getOrMake(env, &_ext)->setName(env, name); \
} 

#define ILOEXTENSIONCPP1(iname, env, template) \
	template \
	void iname::setObject(IloAny object) { \
	if (_ext != 0) \
	_ext->setObject(env, object); \
	else if (object != 0)  \
	IloExtension::getOrMake(env, &_ext)->setObject(env, object); \
} \
	template \
	void iname::setLocation(const char* fileName, int lineNumber) { \
	IloExtension::getOrMake(env, &_ext)->setLocation(env, fileName, lineNumber); \
} \
	template \
	IloBool iname::hasProperty(const char * name) const { \
	if (_ext != 0) { \
	IloNamedPropertySetI * nps = _ext->getProperties(); \
	if (nps != 0) \
	return nps->hasProperty(name); \
	} \
	return IloFalse; \
} \
	template \
	IloNamedPropertyI * iname::getProperty(const char * name) const { \
	IloNamedPropertyI * np = 0; \
	if (_ext != 0) { \
	IloNamedPropertySetI * nps = _ext->getProperties(); \
	if (nps != 0) \
	np = nps->getProperty(name); \
	} \
	if (np == 0) \
	throw IloNamedPropertySet::NotPresent(name); \
	return np; \
} \
	template \
	void iname::setProperties(IloNamedPropertySetI * nps) { \
	if (_ext != 0) \
	_ext->setProperties(nps); \
	else if (nps != 0) \
	IloExtension::getOrMake(env, &_ext)->setProperties(nps); \
} \
	template \
	void iname::addProperty(IloNamedPropertyI * np) { \
	if (_ext == 0) \
	(void)IloExtension::getOrMake(env, &_ext); \
	if (_ext->getProperties() == 0) \
	_ext->setProperties(new (env) IloNamedPropertySetI(env)); \
	_ext->getProperties()->addProperty(np); \
} \
	template \
	void iname::addClonedProperty(IloNamedPropertyI * np) { \
	if (_ext == 0) \
	(void)IloExtension::getOrMake(env, &_ext); \
	if (_ext->getProperties() == 0) \
	_ext->setProperties(new (env) IloNamedPropertySetI(env)); \
	_ext->getProperties()->addClonedProperty(np); \
} \
	template \
	void iname::addProperties(IloNamedPropertySetI * nps) { \
	for (IloNamedPropertySetI::Iterator it(nps); it.ok(); ++it) \
	addClonedProperty(*it); \
} \
	template \
	void iname::removeProperty(const char * name) { \
	if (_ext != 0) { \
	IloNamedPropertySetI * nps = _ext->getProperties(); \
	if (nps != 0) \
	nps->removeProperty(name); \
	else \
	throw IloNamedPropertySet::NotPresent(name); \
	} \
	else \
	throw IloNamedPropertySet::NotPresent(name); \
}\
	template \
	void iname::removeProperty(IloNamedPropertyI * np) { \
	if (_ext != 0) { \
	IloNamedPropertySetI * nps = _ext->getProperties(); \
	if (nps != 0) \
	nps->removeProperty(np); \
	else \
	throw IloNamedPropertySet::NotPresent(np); \
	} \
	else \
	throw IloNamedPropertySet::NotPresent(np); \
}\
	template \
	IloInt iname::getIntProperty(const char * name) const { \
	IloInt ret; \
	IloGetPropertyValueI<IloNamedIntProperty, iname>(name, this, ret); \
	return ret; \
} \
	template \
	IloNum iname::getNumProperty(const char * name) const { \
	IloNum ret; \
	IloGetPropertyValueI<IloNamedNumProperty, iname>(name, this, ret); \
	return ret; \
} \
	template \
	IloAny iname::getAnyProperty(const char * name) const { \
	IloAny ret; \
	IloGetPropertyValueI<IloNamedAnyProperty, iname>(name, this, ret); \
	return ret; \
} \
	template \
	const char * iname::getStringProperty(const char * name) const { \
	const char * ret; \
	IloGetPropertyValueI<IloNamedStringProperty, iname>(name, this, ret); \
	return ret; \
} \
	template \
	void iname::setIntProperty(const char * name, IloInt i) { \
	IloSetPropertyValueI<IloNamedIntProperty, iname>(name, this, i); \
} \
	template \
	void iname::setNumProperty(const char * name, IloNum n) { \
	IloSetPropertyValueI<IloNamedNumProperty, iname>(name, this, n); \
} \
	template \
	void iname::setAnyProperty(const char * name, IloAny a) { \
	IloSetPropertyValueI<IloNamedAnyProperty, iname>(name, this, a); \
} \
	template \
	void iname::setStringProperty(const char * name, const char * str) { \
	IloSetPropertyValueI<IloNamedStringProperty, iname>(name, this, str); \
} \
	template \
	void iname::removeAllProperties() { setProperties(0); }

#define ILOEXTENSIONCPP(iname,env) \
	ILOEXTENSIONCPP0(iname, env, ILOEMPTYMACROARGUMENT) \
	ILOEXTENSIONCPP1(iname,env,ILOEMPTYMACROARGUMENT)
#define ILOEXTENSIONCPPTEMPLATE(iname,env) \
	ILOEXTENSIONCPP0(iname, env, template<>) \
	ILOEXTENSIONCPP1(iname,env,template<>)
#define ILOEXTENSIONCPPTEMPLATEGENERIC(INAME,ENV,NAME) \
	ILOEXTENSIONCPP0(iname, env, template<NAME>) \
	ILOEXTENSIONCPP1(INAME,ENV,template<NAME>)

#if 0
void iname::removeAllProperties() { setProperties(0); } \
void iname::setProperties(IloNamedPropertySetI * nps) { \
if (nps != 0 && _ext == 0) \
IloExtension::getOrMake(env, &_ext); \
if (_ext != 0) \
_ext->setProperties(nps); \
} \

#endif

#define ILOEXTENSIONHANDLEMETHODS(hname) \
	const char * hname::getName() const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	return getImpl()->getName(); \
} \
	void hname::setName(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->setName(name); \
} \
	IloAny hname::getObject() const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	return getImpl()->getObject(); \
} \
	IloBool hname::hasProperty(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->hasProperty(name); \
} \
	void hname::setObject(IloAny object) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->setObject(object); \
} \
	void hname::setLocation(const char* fileName, int lineNumber) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->setLocation(fileName, lineNumber); \
} \
	IloNamedPropertySet hname::getProperties() const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	return getImpl()->getProperties(); \
} \
	IloNamedProperty hname::getProperty(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getProperty(name); \
} \
	IloNamedPropertyI * hname::getPropertyI(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getProperty(name); \
} \
	void hname::addProperty(IloNamedProperty np) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (np.getImpl() == 0) \
	throw IloEmptyHandleException("IloNamedProperty: Using empty handle"); \
	getImpl()->addProperty(np.getImpl()); \
} \
	void hname::addProperties(IloNamedPropertySet nps) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (nps.getImpl() == 0) \
	throw IloEmptyHandleException("IloNamedPropertySet: Using empty handle"); \
	getImpl()->addProperties(nps.getImpl()); \
} \
	void hname::addClonedProperty(IloNamedProperty np) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (np.getImpl() == 0) \
	throw IloEmptyHandleException("IloNamedProperty: Using empty handle"); \
	getImpl()->addClonedProperty(np.getImpl()); \
} \
	void hname::removeProperty(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->removeProperty(name); \
} \
	void hname::removeAllProperties() const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	getImpl()->removeAllProperties(); \
} \
	void hname::setProperties(IloNamedPropertySet nps) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (nps.getImpl() == 0) \
	throw IloEmptyHandleException("IloNamedPropertySet: Using empty handle"); \
	getImpl()->setProperties(nps.getImpl()); \
} \
	IloInt hname::getIntProperty(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getIntProperty(name); \
} \
	IloNum hname::getNumProperty(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getNumProperty(name); \
} \
	IloAny hname::getAnyProperty(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getAnyProperty(name); \
} \
	const char * hname::getStringProperty(const char * name) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	return getImpl()->getStringProperty(name); \
} \
	void hname::setIntProperty(const char * name, IloInt i) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setIntProperty(name, i); \
} \
	void hname::setNumProperty(const char * name, IloNum n) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setNumProperty(name, n); \
} \
	void hname::setAnyProperty(const char * name, IloAny a) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setAnyProperty(name, a); \
} \
	void hname::setStringProperty(const char * name, const char * str) const { \
	if (getImpl() == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty handle"); \
	if (name == 0) \
	throw IloEmptyHandleException(ILO_STRINGIZE(hname) ": Using empty string"); \
	getImpl()->setStringProperty(name, str); \
}

#define ILOEXTENSIONCPPNONINLINE(hname, iname, env) \
	ILOEXTENSIONCPP(iname, env) \
	ILOEXTENSIONHANDLEMETHODS(hname)

class IloConcertVersion {
public:
	static const char* _ILO_NAME;
	static const int _ILO_MAJOR_VERSION;
	static const int _ILO_MINOR_VERSION;
	static const int _ILO_TECH_VERSION;
	static const IloReleaseType _ILO_RELEASE_TYPE;
	static const char* _ILO_COMMENT;
	static const int _ILO_BUILD_NUMBER;
	static const char* _ILO_DATE;
};

template<class X> inline void deleteFromEnv(IloEnvI* env, X* obj, IloInt size =1) {
	if ( obj ) {
#ifndef ILO_RS6000
		obj->X::~X();
#else
		obj->~X();
#endif
		if ( env && !env->isInDestructor() ) {
			env->free(obj,sizeof(X)*size);
		}
	}
}

double IloStrtod( const char *str, char **endPtr );

#if defined(ILO_WINDOWS) && defined(ILO_MSVC) && !defined(_M_IA64)
template< int X >
inline char* IloStrcpy( char (&dest)[X], const char* src ) {
	strcpy_s< X >( dest, src );
	return (char*)dest;
}

inline char* IloStrcpy( char* dest, size_t destSize, const char* src ) {
	strcpy_s( dest, destSize, src );
	return dest;
}

inline char* IloStrncpy( char* dest, size_t destSize, const char* src, size_t size ) {
	strncpy_s( dest, destSize, src, size );
	return dest;
}

inline char* IloStrcat( char* dest, size_t destSize, const char* src ) {
	strcat_s( dest, destSize, src );
	return dest;
}

inline char* IloStrncat( char* dest, size_t destSize, const char* src, size_t size ) {
	strncat_s( dest, destSize, src, size );
	return dest;
}

inline char* IloStrtok( char* str, const char* delim, char** context ) {
	return strtok_s(str, delim, context );
}

inline int IloSPrintf( char *dest, size_t destSize, const char* format, ... ) {

	va_list ptr;
	va_start( ptr, format );

	return vsprintf_s( dest, destSize, format, ptr );
}

#else

template< int X >
inline char* IloStrcpy( char (&dest)[X], const char* src ) {
	strcpy( (char*)dest, src );
	return (char*)dest;
}

inline char* IloStrcpy( char* dest, size_t, const char* src ) {
	return strcpy( dest, src );
}

inline char* IloStrncpy( char* dest, size_t, const char* src, size_t size ) {
	return strncpy( dest, src, size );
}

inline char* IloStrcat( char* dest, size_t, const char* src ) {
	return strcat( dest, src );
}

inline char* IloStrncat( char* dest, size_t, const char* src, size_t size ) {
	return strncat( dest, src, size );
}

inline char* IloStrtok( char* str, const char* delim, char** ) {
	return strtok(str, delim );
}

inline int IloSPrintf( char *dest, size_t, const char* format, ... ) {

	va_list ptr;
	va_start( ptr, format );

	return vsprintf( dest, format, ptr );
}

#endif	

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
