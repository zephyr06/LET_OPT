// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilsched/ilosstructi.h
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

#ifndef __CONCERT_ilosstructiH
#define __CONCERT_ilosstructiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include<ilconcert/ilomemmgr.h>

#define ILOS_LISTI_MIN_SIZE 4

class IlosBaseListI {
private:
	IloUInt _size;
	IloAny* _array;
	IloUInt _max;

protected:
        
        
	IlosBaseListI(IloMemoryManagerI* m = NULL, IloUInt max = 0);
	~IlosBaseListI(){}
	void addPtr(IloMemoryManagerI* m, IloAny elt); 
	IloBool removePtr(IloAny elt);
public:
	IloUInt getSize()            const { return _size; }
	IloAny* getArray()           const { return _array; }
	IloAny  getElt(IloUInt i)    const { return _array[i]; }
	IloBool contains(IloAny elt) const { 
		for (IloUInt i=0; i<_size; ++i) 
			if (_array[i] == elt)
				return IloTrue;
		return IloFalse;
	}
	void stackElt(IloMemoryManagerI* m, IloAny elt) {
		if (_max <= _size)
			resize(m);
		_array[_size] = elt;
		_size++;
	}
	IloAny unstackElt() {
		IloAny elem = _array[_size-1];
		_size--;
		return elem;
	}
	void clear() { _size = 0; }
	void removeElement(IloUInt i) {
		const IloUInt l = _size-1;
		_array[i]=_array[l];
		_size = l;
	}
	void end(IloMemoryManagerI* m);
        void prealloc(IloMemoryManagerI* m, IloUInt max);
private:
	void resize(IloMemoryManagerI* m);
};

template <class T> 
class IlosListI :public IlosBaseListI {
public:
        
        
	IlosListI(IloMemoryManagerI* m = NULL, IloUInt max = 0) :IlosBaseListI(m, max) {}
	~IlosListI(){}
	void add(IloMemoryManagerI* m, T* elt) { addPtr(m,(IloAny)elt); }
	IloBool remove(T* elt) { return removePtr((IloAny)elt); }
	T* getElement(IloUInt i) const { return (T*)getElt(i); }
	T* pop() { T* first = (T*)getElement(0); removeElement(0); return first; }
	void stack(IloMemoryManagerI* m, T* elt) { stackElt(m, (IloAny)elt); }
	T* unstack() { return (T*)unstackElt(); }
	IloBool isEmpty() const { return (getSize()==0); }
	class Iterator {
	private:
		IloAny* _curr;
		IloAny* _end;
	public:
		Iterator()
			:_curr  (0), _end(0) {}
		Iterator(IlosListI<T>* list) 
			:_curr (list->getArray()),
			_end  (0){
				_end   = _curr + list->getSize();
		}
		Iterator(const IlosListI<T>& list) 
			:_curr (list.getArray()),
			_end  (0){
				_end   = _curr + list.getSize();
		}
		Iterator(const Iterator& ite) 
			:_curr  (ite._curr),
			_end   (ite._end){}
		~Iterator(){}    
		void init(const IlosListI<T>* list) {
			_curr  = list->getArray();
			_end   = _curr + list->getSize();
		}
		void init(const IlosListI<T>& list) {
			_curr  = list.getArray();
			_end   = _curr + list.getSize();
		}
		IloBool ok() const     { return (_curr != _end); }
		Iterator& operator++() { _curr++; return *this; }
		T* operator*() const   { return (T*)(*_curr); }
	};  
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
