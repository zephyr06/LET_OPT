// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilothread.h
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

#ifndef __CONCERT_ilothreadH
#define __CONCERT_ilothreadH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>

#if defined(ILO_MSVC)
#include <windows.h>
#endif

class IloThread;
class IloFastMutex;
class IloSemaphore;
class IloCondition;
class IloBarrier;

class IloAgentError : public IloException {
public:
	IloAgentError(const char* msg): IloException(msg) {}
};

class IloAgentFailure : public IloAgentError {
	char msgbuf[128];
public:
	IloAgentFailure();
	explicit IloAgentFailure(int error);
	IloAgentFailure(char const *func, int error);
};

class IloMutexProblem : public IloAgentError {
public:
	
	IloMutexProblem(const char* msg) : IloAgentError(msg)  {}
};

class IloMutexDeadlock : public IloMutexProblem {
public:
	IloMutexDeadlock() : IloMutexProblem("Mutex already locked") {}
};

class IloMutexNotOwner : public IloMutexProblem {
public:
	IloMutexNotOwner() : IloMutexProblem("Not owner of the mutex") {}
};

#ifdef _WIN32
typedef CRITICAL_SECTION IloSystemMutex;
typedef DWORD            IloSystemThread;
#define IloSystemThreadSame(t1, t2) ((t1) == (t2))
#define IloGetSystemThread() GetCurrentThreadId()
#else
typedef pthread_mutex_t IloSystemMutex;
typedef pthread_t       IloSystemThread;
#define IloSystemThreadSame(t1, t2) (pthread_equal((t1), (t2)) != 0)
#define IloGetSystemThread() pthread_self()
#endif

class IloFastMutex {
	IloSystemMutex  _holderMutex;
	IloSystemMutex  _realMutex;
	IloSystemThread _holder;
	int             _holding;
	friend class IloCondition;
	IloBool isLockedUnsafe() const;
        inline void setHolder();
        inline void unsetHolder();
        inline void unsetHolderUnsafe();
private:
   IloFastMutex(IloFastMutex const &) ILO_DELETE;
   IloFastMutex& operator=(IloFastMutex const &) ILO_DELETE;
public:
	
	IloFastMutex();
	
	~IloFastMutex();
	
	int isLocked();
	
	void lock();
	
	void unlock();
};

#ifdef _WIN32

ILOPUSHNODEPRECATION
#endif

class ILODEPRECATED(20010000) IloSafeMutexLock
{
private:
	IloFastMutex* _mutex;
public:
        ILODEPRECATED(20010000)
	IloSafeMutexLock(IloFastMutex* mutex) : _mutex(mutex)
	{
		if (_mutex!=NULL)
		{
			
			if (!_mutex->isLocked())
			{
				_mutex->lock();
			}
			else
			{
				
				_mutex=NULL;
			}
		}
	}

        ILODEPRECATED(20010000)
	~IloSafeMutexLock()
	{
		if (_mutex!=NULL) _mutex->unlock();
	}
};

class ILODEPRECATED(20010000) IloCondition {
#if defined(ILO_MSVC)
	IloThread*     _head;
	IloThread*     _tail;
	IloFastMutex   _lock;
	long           _waiting;
#elif defined(ILOPTHREAD)
	pthread_cond_t _realCond;
#endif
public:
        ILODEPRECATED(20010000) 
	IloCondition();
        ILODEPRECATED(20010000) 
	~IloCondition();
        ILODEPRECATED(20010000) 
	void broadcast();
        ILODEPRECATED(20010000) 
	void notify();
        ILODEPRECATED(20010000) 
	void wait(IloFastMutex *m);
};

class ILODEPRECATED(20010000) IloSemaphore {
#if defined(ILO_MSVC)
	IloFastMutex    _lock;
	HANDLE       _realSem;
	long         _count;
#elif defined(ILOPTHREAD)
	IloFastMutex _lock;
        ILOPUSHNODEPRECATION
	IloCondition _go;
        ILOPOPNODEPRECATION
	long         _count;
#endif
	friend class IloCondition;
	void post(int x);
public:
        ILODEPRECATED(20010000) 
	IloSemaphore(int value = 0);
        ILODEPRECATED(20010000) 
	~IloSemaphore();
        ILODEPRECATED(20010000) 
	void post();
        ILODEPRECATED(20010000) 
	int tryWait();
        ILODEPRECATED(20010000) 
	void wait();
};

class ILODEPRECATED(20010000) IloBarrier {
	int       _number;
	int       _count;
	int       _ocount;
	int       _generation;
	IloFastMutex _mut;
        ILOPUSHNODEPRECATION
	IloCondition _cv;
        ILOPOPNODEPRECATION
public:
	IloBarrier(int count);
	int wait();
};

#if defined(ILO_MSVC)
#define PRI_FIFO_MAX THREAD_PRIORITY_ABOVE_NORMAL
#define PRI_FIFO_MIN THREAD_PRIORITY_BELOW_NORMAL
#define PRI_RR_MAX THREAD_PRIORITY_ABOVE_NORMAL
#define PRI_RR_MIN THREAD_PRIORITY_BELOW_NORMAL
#define PRI_OTHER_MAX THREAD_PRIORITY_ABOVE_NORMAL
#define PRI_OTHER_MIN THREAD_PRIORITY_BELOW_NORMAL
#define PRI_NORMAL    THREAD_PRIORITY_NORMAL
#elif defined(ILOPTHREAD)
#define PRI_FIFO_MAX 0
#define PRI_FIFO_MIN 0
#define PRI_RR_MAX 0
#define PRI_RR_MIN 0
#define PRI_OTHER_MAX 0
#define PRI_OTHER_MIN 0
#define PRI_NORMAL    0
#endif

class ILODEPRECATED(20010000) IloThread {
	friend class IloCondition;
	friend class IloThreadHelper;
public:
	enum ThreadType {detached, joinable};
	enum ThreadPolicy {fifo,rr,other};
	static void initialize();
	static void end();
private:
	static int _alreadyInitted;
#if defined(ILO_MSVC)
	IloFastMutex  _mon;      
	IloSemaphore  _go;       
#elif defined(ILOPTHREAD)
	pthread_mutex_t _runLock;
	pthread_mutex_t _mon;
	pthread_cond_t  _go;
	int          _runnable;
#endif
	int           _active;   
	int           _ready;    
	ThreadType    _type;     
#if defined(ILO_MSVC)
	HANDLE        _thdl;     
	HANDLE        _event;    
        ILOPUSHNODEPRECATION
	IloThread*     _link;     
	IloCondition* _waitingOn;
        ILOPOPNODEPRECATION
	unsigned long _tid;      
#elif defined(ILOPTHREAD)
	pthread_t     _tid;
#endif
        ILOPUSHNODEPRECATION
	static void realStartup(IloThread *);
        ILOPOPNODEPRECATION
protected:
	void monLock();
	void monUnlock();
#if defined(ILO_MSVC)
	IloThread(long);
#elif defined(ILOPTHREAD)
	IloThread(pthread_t);
#endif
	virtual void startup() = 0;
	virtual ~IloThread();
public:
	IloThread(ThreadType T = detached, size_t stackSize = 0,ThreadPolicy policy = other,int pr = PRI_NORMAL);
        ILODEPRECATED(20010000) 
	void setPriority(ThreadPolicy policy = other,int pr = PRI_NORMAL);
        ILODEPRECATED(20010000) 
	void makeRunnable();
        ILODEPRECATED(20010000) 
	void join();
        ILODEPRECATED(20010000) 
	int  running() const { return _ready;}
#if defined(ILO_MSVC)
        ILODEPRECATED(20010000) 
	HANDLE getThreadHandle() { return _thdl; }
#endif
        ILOPUSHNODEPRECATION
        ILODEPRECATED(20010000) 
	static IloThread* current();
        ILOPOPNODEPRECATION
        ILODEPRECATED(20010000) 
	static void exit();
        ILOPUSHNODEPRECATION
        ILODEPRECATED(20010000) 
#ifndef ILO_ZOS
	static unsigned long getAgentId() { return (unsigned long)current()->_tid; }
#else
	static pthread_t getAgentId() { return current()->_tid; }
#endif
        ILOPOPNODEPRECATION
};

#ifdef _WIN32
ILOPOPNODEPRECATION
#endif

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

