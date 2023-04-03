// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilolinkedlist.h
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

#ifndef __CONCERT_ilolinkedlistH
#define __CONCERT_ilolinkedlistH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>

class IloAnyArray;
class IloIntArray;

template <class X> class IloLinkedList {
public:
	struct Element {
		Element*         _next;
		X* _obj;
		Element(X* obj)
			: _next(0), _obj(obj) {}
	};
protected:
	IloEnvI*  _env;
	Element*  _first;
	Element** _last;

public:
	IloLinkedList() : _env(0), _first(0), _last(&_first)  {}
	~IloLinkedList(){
		clear();
	}

	
	Element* getFirst() const { return (_first); }

	
	X* firstObject() const {
		return (_first ? _first->_obj : 0);
	}

	
	X* getObject(IloInt index) const{
		IloInt i = 0;
		for (Element* e = _first; e!=0; e = e->_next) {
			if (i == index) return e->_obj;
			i++;
		}
		return 0;
	}

	class Iterator {
		Element* _which;
	public:
		
		Iterator(const IloLinkedList& l): _which(l.getFirst()) { }
		
		void operator++() { _which = _which->_next; }
		
		X* operator*() const  { return (_which->_obj); }
		
		X* getObject() const  { return (_which->_obj); }
		
		X* operator->() const { return (_which->_obj); }
		
		IloBool  ok() const { return (_which != 0); }
	};

	
	IloBool has (const X* extobj) const{
		for (Element* e = _first; e!=0; e = e->_next) {
			if (e->_obj == extobj)
				return IloTrue;
		}
		return IloFalse;
	}

	
	IloInt getIndex(const X* extobj) const {
		IloInt index = 0;
		for (Element* e = _first; e!=0; e = e->_next) {
			if (e->_obj == extobj) return index;
			index++;
		}
		return -1;
	}

	
	void add (X* obj){
		if (!_env) _env=obj->getEnv();
		IloLocalEnvI* lenv = obj->getEnv()->getLocalEnv();
		if (lenv) {
			if (_last) lenv->saveValue((IloAny*)_last);
			lenv->saveValue((IloAny*)&_last);
		}
		*_last = new (obj->getEnv()) Element(obj);
		_last = &((*_last)->_next);
	}

	
	void remove(X* obj){
		Element** e = &_first;
		for( ; (*e)!=0; e = &((*e)->_next) )
			if ((*e)->_obj == obj)
				break;
		Element* del = *e;
		if (del==0) return;

		IloLocalEnvI* localEnv = _first->_obj->getEnv()->getLocalEnv();
		if ( del->_next == 0 ) {
			if (localEnv) localEnv->saveValue((IloAny*)&_last);
			_last = e;
		}
		if (localEnv) localEnv->saveValue((IloAny*)e);
		*e = del->_next;
		del->_obj->getEnv()->free(del, sizeof(Element));
	}

	
	void removeMarked(){
		if (_first==0) return;

		IloLocalEnvI* localEnv = _first->_obj->getEnv()->getLocalEnv();
		IloEnvI* env = _first->_obj->getEnv();

		Element** e;
		for (e = &_first; *e;) {
			if ((*e)->_obj->mustRemove() ) {
				if (localEnv) localEnv->saveValue((IloAny*)e);
				Element* del = *e;
				*e = del->_next;
				env->free(del, sizeof(Element));
			} else e = &((*e)->_next);
		}
		if (localEnv) localEnv->saveValue((IloAny*)&_last);
		_last = e;
	}

	
	void clear(){
		if (_first) {
			IloLocalEnvI* localEnv = _env->getLocalEnv();
			if (localEnv) {
				localEnv->saveValue((IloAny*)(void*)(&_first));
				localEnv->saveValue((IloAny*)(void*)(&_last));
			}
			Element* next;
			for (Element* e = _first; e; e = next) {
				next = e->_next;
				_env->free(e, sizeof(Element));
			}
			_first = 0;
		}
		_last = &_first;
	}

};

template <class X> class IloSimpleLinkedList {
public:
	struct Element {
		Element*         _next;
		X _obj;
		Element(X obj)
			: _next(0), _obj(obj) {}
	};
protected:
	IloEnvI*  _env;
	Element*  _first;
	Element** _last;
	IloInt _nb;

public:
	
	IloSimpleLinkedList(IloEnv env) : _env(env.getImpl()), _first(0), _last(&_first), _nb(0) {}
	virtual ~IloSimpleLinkedList(){
		clear();
	}

	IloEnvI* getEnv() const { return _env; }

	
	Element* getFirst() const { return (_first); }

	
	X firstObject() const {
		return (_first ? _first->_obj : 0);
	}

	
	X lastObject() const {
		Element* e = *_last;
		for (e = _first; e!= 0 && e->_next != 0; e = e->_next) {}
		return e->_obj;
	}

	
	X getNext(X obj) const {
		for (Element* e = _first; e!=0; e = e->_next) {
			if (e->_obj == obj) return e->_next->_obj;
		}
		return 0;
	}

	
	X getObject(IloInt index) const{
		IloInt i = 0;
		for (Element* e = _first; e!=0; e = e->_next) {
			if (i == index) return e->_obj;
			i++;
		}
		return 0;
	}

	class Iterator {
		Element* _which;
	public:
		
		Iterator(const IloSimpleLinkedList& l): _which(l.getFirst()) { }
		
		void             operator++()       { _which = _which->_next; }
		
		X operator*() const  { return (_which->_obj); }
		
		X getObject() const  { return (_which->_obj); }
		
		IloBool          ok() const         { return (_which != 0); }
	};

	
	IloInt getSize() const { return _nb; }

	
	IloBool has (const X extobj) const{
		for (Element* e = _first; e!=0; e = e->_next) {
			if (e->_obj == extobj)
				return IloTrue;
		}
		return IloFalse;
	}

	
	IloInt getIndex(const X extobj) const {
		IloInt index = 0;
		for (Element* e = _first; e!=0; e = e->_next) {
			if (e->_obj == extobj) return index;
			index++;
		}
		return -1;
	}

	
	void add (X obj){
		if (_last) _env->saveValue((IloAny*)_last);
		_env->saveValue((IloAny*)&_last);
		*_last = new (_env) Element(obj);
		_last = &((*_last)->_next);
		_nb++;
	}

	
	void remove(X obj){
		Element** e = &_first;
		for( ; (*e)!=0; e = &((*e)->_next) )
			if ((*e)->_obj == obj)
				break;
		Element* del = *e;
		if (del==0) return;
		if (_nb == 1){
			clear();
			return;
		}

		if ( del->_next == 0 ) {
			_env->saveValue((IloAny*)&_last);
			_last = e;
		}
		_env->saveValue((IloAny*)e);
		*e = del->_next;
		_env->free(del, sizeof(Element));
		_nb --;
	}

	
	void removeMarked(){
		if (_first==0) return;

		Element** e;
		for (e = &_first; *e;) {
			if ((*e)->_obj->mustRemove() ) {
				_env->saveValue((IloAny*)e);
				Element* del = *e;
				*e = del->_next;
				_env->free(del, sizeof(Element));
			} else e = &((*e)->_next);
		}
		_env->saveValue((IloAny*)&_last);
		_last = e;
		_nb --;
	}

	
	void clear(){
		if (_first) {
			_env->saveValue((IloAny*)&_first);
			_env->saveValue((IloAny*)&_last);
			Element* next;
			for (Element* e = _first; e; e = next) {
				next = e->_next;
				_env->free(e, sizeof(Element));
			}
			_first = 0;
			_last = &_first;
			_nb =0;
		}
	}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

