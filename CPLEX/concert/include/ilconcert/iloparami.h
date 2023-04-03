// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloparami.h
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

#ifndef __CONCERT_IloParamIH
#define __CONCERT_IloParamIH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilobasic.h>
#include <ilconcert/iloextractable.h>

class IloParameterI;
class IloExtractableI;
class IloParameterManagerI;
class IloParameterExtractedObjectI;
class IloParameterNodeI;

class IloParameterExtractableLinkI {
	friend class IloParameterI;

private:
	IloParameterExtractableLinkI* _prev;
	IloParameterExtractableLinkI* _next;
	IloParameterI*                _param;
	IloExtractableI*  _extr;
	IloAny                        _port;

protected:
	IloParameterExtractableLinkI(IloExtractableI* extr,
		IloParameterI* param,
		IloAny port)
		: _prev  (0)
		, _next  (0)
		, _param (param)
		, _extr  (extr)
		, _port  (port) {}
	~IloParameterExtractableLinkI() {}

	void setPrev(IloParameterExtractableLinkI* prev) { _prev = prev; }
	void setNext(IloParameterExtractableLinkI* next) { _next = next; }

public:
	IloParameterI* getParameter() const { return _param; } 
	IloParameterExtractableLinkI* getPrev() const { return _prev; }
	IloParameterExtractableLinkI* getNext() const { return _next; }
	IloExtractableI* getExtractable() const { return _extr; }
	IloAny getPort() const { return _port; }

	void removeAndDestroyOrphan();
	IloParameterI* remove();
	inline IloAny getOwner() const;
	inline void setOwner(IloAny owner) const;
	inline IloBool isUserCreated() const;
};

#define IloParametrizedExtractableI IloExtractableI

class IloParameterI : public 
#ifdef ILO_SEGF_GENERIC_ALLOC
	IloExtensibleRttiMemoryManagerObjectI
#else
	IloExtensibleRttiEnvObjectI 
#endif
{
	ILORTTIDECL
		friend class IloParameterManagerI;
	friend class IloParameterExtractableLinkI;
	friend class IloParametrizedExtractableIterator;

#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloEnvI *);
	void operator delete(void *, const IloEnv&);
#endif
	void operator delete(void * obj, size_t size);

private:
	IloParameterExtractableLinkI* _links;
	IloParameterExtractedObjectI* _objects;
	IloAny                        _owner;     

protected:
	
	
	void unlinkExtractable(IloParameterExtractableLinkI* link);

	
	
	
	void notifyChange();

public:

	
	class Exception : public IloException {
	public:
		Exception() : IloException("Generic parameter exception") {}
		Exception(const char* msg) : IloException(msg) {}
		virtual ~Exception() {}
	};

	IloParameterExtractableLinkI* getLinks() const {
		return _links; }

	
	class ParameterExtractableLinkIterator {
	protected:
		IloParameterExtractableLinkI* _cursor;
	public:
		ParameterExtractableLinkIterator(IloParameterI* parameter)
			: _cursor(parameter->getLinks()) {} 
		
		IloBool ok() const { 
			return _cursor != 0; 
		}
		IloParameterExtractableLinkI* operator*() { 
			return _cursor; 
		}
		IloExtractableI* getExtractable() { 
			return _cursor->getExtractable(); 
		}
		IloAny getPort() const { 
			return _cursor->getPort(); 
		}
		ParameterExtractableLinkIterator& operator++() {
			_cursor = _cursor->getNext();
			return (*this);
		}
	};

	IloParameterI(IloEnvI* env, const char* name = 0, IloAny owner = 0);
#ifdef ILO_SEGF_GENERIC_ALLOC
	IloParameterI(IloMemoryManagerI* env, const char* name = 0, IloAny owner = 0);
#endif  
	
	
	
	virtual ~IloParameterI();

	virtual IloParameterI* makeCopy() const = 0;

	virtual void display(ILOSTD(ostream)&) const;

	
	
	IloAny getExtractedObject(const IloParameterManagerI* mng) const;

	
	
	void setExtractedObject(IloParameterManagerI* mng, IloAny obj);

	
	
	
	
	void unextract(IloParameterManagerI* mng);

	virtual void unsetExtractedObject(IloParameterManagerI*, IloAny) {}

	

	
	
	
	IloParameterExtractableLinkI* 
		linkExtractable(IloExtractableI* extr, IloAny port =(IloAny)0);

	
	
	IloBool isLocal() const { return !isUserCreated() && ((0 != _links) && (0 == _links->getNext())); };

	IloBool isOrphan() const { return (0 == _links) && !isUserCreated(); }

	IloAny getOwner() const { return _owner; }
	void setOwner(IloAny owner) { _owner = owner; }
	IloBool isUserCreated() const { return (getOwner()==0); }

	
	IloInt getNbReference() const;
	IloBool isParameterOf(const IloExtractableI* extr, IloAny port) const;
};

inline IloAny IloParameterExtractableLinkI::getOwner() const {
	assert(_param!=0);
	return _param->getOwner();
}
inline void IloParameterExtractableLinkI::setOwner(IloAny owner) const {
	assert(_param!=0);
	_param->setOwner( owner );
}
inline IloBool IloParameterExtractableLinkI::isUserCreated() const { 
	assert(_param!=0);
	return _param->isUserCreated();
}

class IloParameterManagerI
{
private:
	IloEnv             _env;
	IloParameterNodeI* _params;

public:
	IloParameterManagerI(IloEnv env);
	~IloParameterManagerI();
	void clear();

	
	
	
	void manage(IloParameterI*, IloAny obj);

	
	void unManage(IloParameterI*);

	
	
	IloAny getExtractedObject(const IloParameterI*) const;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif  
