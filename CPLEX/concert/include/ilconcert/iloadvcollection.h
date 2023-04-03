// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloadvcollection.h
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

#ifndef __CONCERT_iloadvcollectionH
#define __CONCERT_iloadvcollectionH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilocollectioni.h>

template <class IloHandle, class IloImpl> class IloHandleSet : public IloAnyCollection {
public:
	virtual ~IloHandleSet(){}
	
	IloHandleSet(const IloEnv env, const IloAnyArray array, IloBool withIndex=IloFalse)
		: IloAnyCollection(new (env.getImpl()) IloAnySetI(env.getImpl(), array, withIndex)) {}

	
	IloHandleSet(IloEnvI* env, IloAnySetI* impl)
		: IloAnyCollection(new (env) IloAnySetI(env, impl)) {}

	
	IloHandleSet(const IloEnv env, IloBool withIndex=IloFalse) : IloAnyCollection(new (env.getImpl()) IloAnySetI(env.getImpl(), withIndex)) {}

	
	IloHandleSet(IloAnySetI* impl=0) : IloAnyCollection(impl) {}

	
	IloAnySetI* getImpl() const {
		return (IloAnySetI*)IloAnyCollection::getImpl();
	}

	
	IloHandleSet copy() const {
		if (!getImpl()) throw IloEmptyHandleException("IloSet: Using empty handle");
		return IloHandleSet(getImpl()->getEnv(), getImpl());
	}

	
	IloInt getSize() const{
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		return getImpl()->getSize();
	}

	
	void add(IloHandle elt){
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->add(elt.getImpl());
	}
	void addWithoutCheck(IloHandle elt){
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->addWithoutCheck(elt.getImpl());
	}
	
	void add(IloHandleSet set){
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->add((IloAnySetI*)set.getImpl());
	}

	
	void remove(IloHandle elt) {
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->remove(elt.getImpl());
	}

	
	void remove(IloHandleSet set){
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->remove((IloAnySetI*)set.getImpl());
	}

	
	void setIntersection(IloHandle elt){
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->setIntersection(elt.getImpl());
	}

	
	void setIntersection(IloHandleSet set){
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->setIntersection((IloAnySetI*)set.getImpl());
	}

	
	void empty(){
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		getImpl()->empty();
	}

	
	IloBool contains(IloHandle elt) const{
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		return getImpl()->contains(elt.getImpl());
	}

	
	IloBool contains(IloHandleSet set) const{
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		return getImpl()->contains((IloAnySetI*)set.getImpl());
	}

	
	IloBool intersects(IloHandleSet set) const{
		if (!getImpl()) throw IloEmptyHandleException("Using empty IloSet handle.");
		return getImpl()->intersects((IloAnySetI*)set.getImpl());
	}

	
	IloHandle getFirst() const{
		IloAssert(getImpl() != 0, "IloSet: Using empty handle");
		return IloHandle((IloImpl*)((IloAnySetI*)_impl)->getFirst());
	}

	
	IloHandle getLast() const{
		IloAssert(getImpl() != 0, "IloSet: Using empty handle");
		return IloHandle((IloImpl*)((IloAnySetI*)_impl)->getLast());
	}

	
	IloHandle getNext(IloHandle value, IloInt n=1) const{
		IloAssert(getImpl() != 0, "IloSet: Using empty handle");
		return IloHandle((IloImpl*)((IloAnySetI*)_impl)->getNext(value.getImpl(),n));
	}
	
	IloHandle getPrevious(IloHandle value, IloInt n=1) const{
		IloAssert(getImpl() != 0, "IloSet: Using empty handle");
		return IloHandle((IloImpl*)((IloAnySetI*)_impl)->getPrevious(value.getImpl(),n));
	}
	
	IloHandle getNextC(IloHandle value, IloInt n=1) const{
		IloAssert(getImpl() != 0, "IloSet: Using empty handle");
		return IloHandle((IloImpl*)((IloAnySetI*)_impl)->getNextC(value.getImpl(),n));
	}
	
	IloHandle getPreviousC(IloHandle value, IloInt n=1) const{
		IloAssert(getImpl() != 0, "IloSet: Using empty handle");
		return IloHandle((IloImpl*)((IloAnySetI*)_impl)->getPreviousC(value.getImpl(),n));
	}

	
	virtual void display(ILOSTD(ostream)& os) const {
		os << "[ ";
		for (IloInt i=0; i< this->getSize(); i++){
			os << IloHandle((IloImpl*)((IloAnySetI*)_impl)->getAnyValue(i)) << " ";
		}
		os << "]";
	}

	
	class Iterator : public IloAnyDefaultDataIterator {
	public:
		
		Iterator(const IloAnySetI* coll = 0) : IloAnyDefaultDataIterator(coll->getEnv()->getGeneralAllocator(), coll) {
			reset();
		}
		
		Iterator(const IloHandleSet coll): IloAnyDefaultDataIterator(coll.getImpl()->getEnv()->getGeneralAllocator(),
			coll.getImpl()) {
				reset();
		}

		
		Iterator(IloGenAlloc* heap, const IloAnySetI* coll): IloAnyDefaultDataIterator(heap, coll) {
			reset();
		}

		virtual ~Iterator(){}

		
		IloHandle operator*() {
			return IloHandle((IloImpl*)IloAnyDefaultDataIterator::operator*());
		}
	};

	
	const IloHandle getValue(IloInt i) const{
		IloAssert(getImpl() != 0, "IloDiscreteDataCollection: Using empty handle");
		return IloHandle((IloImpl*)((IloAnySetI*)getImpl())->getValue(i));
	}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

