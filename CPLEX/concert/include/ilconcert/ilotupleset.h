// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilotupleset.h
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

#ifndef __CONCERT_ilotuplesetH
#define __CONCERT_ilotuplesetH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloanyset.h>

class IloIntTableSetConstraintI;
class IloIntTableBinPredicateConstraintI;
class IloIntTableTernaryPredicateConstraintI;
class IloIntTableElementConstraint2I;

class IloIntTupleI : public IloEnvObjectI {
	typedef IloIntTupleI Myself; 
private:
	IloIntArray _vals;
	IloInt _hashCode;
	Myself* _next;
public:
	IloIntTupleI(const IloIntArray & val, const IloInt hashCode, Myself* next=0):
	  IloEnvObjectI(val.getEnv().getImpl()), _vals(val), _hashCode(hashCode), _next(next){}
	  ~IloIntTupleI(){}
	  void setNext(Myself* elt){_next=elt;}
	  IloIntArray getArray()const{return _vals;}
	  IloInt getHashCode()const{return _hashCode;}
	  Myself* getNext()const{return _next;}
};

class IloIntTupleSetI : public IloExtractableI {
	ILOEXTRDECL
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
private:
	const IloInt _arity;
	IloInt _nbTuple;
	IloInt _maskNbBucket;
	IloIntTupleI** _buckets;
	IloInt computeHashCode(const IloIntArray &) const;
	IloBool equal(const IloIntArray &, const IloIntArray &) const;
	IloBool isInBucket(const IloInt, const IloInt, const IloIntArray &)const;
	void rehash();
public:
	~IloIntTupleSetI();
	IloInt getMaskNbBucket() const {return _maskNbBucket;}
	IloIntTupleI* getFirst(const IloInt i){return _buckets[i];}
	IloInt getArity()const {return _arity;}
	IloIntTupleSetI(IloEnvI*,const IloInt);
	IloBool add(const IloIntArray vals);
	IloBool remove(const IloIntArray);
	IloBool isIn(const IloIntArray)const;
	IloInt getSize() const{ return _nbTuple; }
	IloInt getCardinality()const {return _nbTuple;}
	ILOEXTROTHERDECL
};

class IloIntTupleSetAddTuple : public IloChange {
	ILOCHANGEDECL
		IloIntTupleSetI* _set;
	IloIntArray _elt;
public:
	IloIntTupleSetAddTuple (IloIntTupleSetI* set, IloIntArray elt)
		: _set(set), _elt(elt) {}
	IloIntTupleSetI* getSet() const { return (_set); }
	IloIntArray getElement() const { return (_elt); }
};

class IloIntTupleSetRemoveTuple : public IloChange {
	ILOCHANGEDECL
		IloIntTupleSetI* _set;
	IloIntArray _elt;
public:
	IloIntTupleSetRemoveTuple(IloIntTupleSetI* set, IloIntArray elt)
		: _set(set), _elt(elt) {}
	IloIntTupleSetI* getSet() const { return (_set); }
	IloIntArray getElement() const { return (_elt); }
};

class IloTupleSetIteratorBase {
  IloIntTupleSetI* _tset;
  IloInt _index;
  IloIntTupleI* _elt;
public:
  IloTupleSetIteratorBase(IloIntTupleSetI* tset)
    : _tset(tset)
    , _index(-1)
    , _elt(0){
    _reset();
  }
  IloBool _ok() const { return (_elt!=0); }
  void _advance() {
    if (_elt->getNext() != 0)
      _elt = _elt->getNext();
    else {
      _index++;
      _elt=0;
      for (IloInt index=_index;index <= _tset->getMaskNbBucket(); index++) {
        if (_tset->getFirst(index) != 0) {
          _index=index;
          _elt=_tset->getFirst(index);
          break;
        }
      }
    }
  }
  IloIntArray _get() const { return (_elt->getArray());}
  void _reset() {
    _index = -1;
    _elt = 0;
    if (_tset){
      for (IloInt index=0;index <= _tset->getMaskNbBucket(); index++){
        if (_tset->getFirst(index) != 0){
          _index=index;
          _elt=_tset->getFirst(index);
          break;
        }
      } 
    }
  }
  
  void end() { }
};

class IloIntTupleSetI;

#define IloAnyTupleSetI IloIntTupleSetI

class IloIntTupleSet {
	IloIntTupleSetI* _impl;
public:
	IloIntTupleSet(IloIntTupleSetI* impl = 0) : _impl(impl){}
	
	IloIntTupleSet(const IloEnv env, const IloInt arity);

	
	IloIntTupleSetI* getImpl() const {
		return _impl;
	}

	
	IloBool add(const IloIntArray tuple) const;
	
	IloBool remove(const IloIntArray tuple) const;
	
	IloBool isIn(const IloIntArray tuple) const;
	
	IloInt getArity()const;
	
	void end();
	
	IloInt getCardinality()const;
};

ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out,
							const IloIntTupleSet& dom);

class IloIntTupleSetIterator : public IloTupleSetIteratorBase {
public:
	
	IloIntTupleSetIterator(const IloEnv, IloIntTupleSet tSet) 
          : IloTupleSetIteratorBase(tSet.getImpl()) {}
	
	IloBool ok() const { return _ok(); }

	
	void operator++ () { _advance(); }
	
	IloIntArray operator*() const { return _get(); }
	
	void reset() { _reset(); }
};

class IloAnyTupleSet {
	IloAnyTupleSetI* _impl;
public:
	operator IloExtractable();

	IloAnyTupleSet(IloAnyTupleSetI* impl = 0) : _impl(impl){}
	
	IloAnyTupleSet(const IloEnv env, const IloInt arity);

	
	IloAnyTupleSetI* getImpl() const {
		return _impl;
	}

	
	IloBool add(const IloAnyArray tuple) const;
	
	IloBool remove(const IloAnyArray tuple) const;
	
	IloBool isIn(const IloAnyArray tuple) const;
	
	IloInt getArity() const;
	
	IloInt getCardinality() const;
};

ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloAnyTupleSet& dom);

class IloAnyTupleSetIterator : public IloTupleSetIteratorBase {
public:
	
	IloAnyTupleSetIterator(const IloEnv, IloAnyTupleSet tSet)
          : IloTupleSetIteratorBase(tSet.getImpl()) { }
	
	IloBool ok() const { return _ok(); }
	
	void operator++ () { _advance(); }
	
	IloAnyArray  operator*() const { return _get().getImpl(); }
	
	void reset() { _reset(); }
};

class IloIntBinaryPredicateI {
public:
	virtual ~IloIntBinaryPredicateI(){}
	virtual IloBool isTrue(const IloInt val1, const IloInt val2)=0;
};

class IloIntTernaryPredicateI {
public:
	virtual ~IloIntTernaryPredicateI(){}
	virtual IloBool isTrue(const IloInt val1,
		const IloInt val2,
		const IloInt val3)=0;
};

class IloIntBinaryPredicate {
public:
	
	IloIntBinaryPredicate():_impl(0){}
	
	IloIntBinaryPredicate(IloIntBinaryPredicateI* impl):_impl(impl){}
        IloIntBinaryPredicate(const IloIntBinaryPredicate& other): _impl(other._impl){}
	~IloIntBinaryPredicate(){}
	
	IloIntBinaryPredicateI* getImpl() const {return _impl;}
	
	void operator=(const IloIntBinaryPredicate& h){_impl = h._impl;}
protected:
	IloIntBinaryPredicateI* _impl;
public:
	
	IloBool isTrue(const IloInt val1, const IloInt val2){
		assert(_impl !=0);
		return _impl->isTrue(val1,val2);
	}
};

class IloIntTernaryPredicate {
public:
	
	IloIntTernaryPredicate():_impl(0){}
	
	IloIntTernaryPredicate(IloIntTernaryPredicateI* impl):_impl(impl){}
        IloIntTernaryPredicate(const IloIntTernaryPredicate& other): _impl(other._impl){}
	~IloIntTernaryPredicate(){}
	
	IloIntTernaryPredicateI* getImpl() const {return _impl;}
	
	IloIntTernaryPredicate& operator=(const IloIntTernaryPredicate& h){_impl = h._impl; return *this;}
protected:
	IloIntTernaryPredicateI* _impl;
public:
	
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3){
		assert(_impl !=0);
		return _impl->isTrue(val1,val2,val3);
	}
};

class IloAnyBinaryPredicateI : public IloIntBinaryPredicateI {
public:
	virtual ~IloAnyBinaryPredicateI(){}
	IloAnyBinaryPredicateI() : IloIntBinaryPredicateI() {}
	virtual IloBool isTrue(const IloAny val1, const IloAny val2)=0;
	virtual IloBool isTrue(const IloInt val1, const IloInt val2) ILO_OVERRIDE {
		return isTrue((IloAny)val1, (IloAny)val2);
	}
};

class IloAnyTernaryPredicateI : public IloIntTernaryPredicateI {
public:
	virtual ~IloAnyTernaryPredicateI(){}
	IloAnyTernaryPredicateI() : IloIntTernaryPredicateI() {}
	virtual IloBool isTrue(const IloAny val1,
		const IloAny val2,
		const IloAny val3)=0;
	virtual IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3) ILO_OVERRIDE {
			return isTrue((IloAny)val1, (IloAny)val2, (IloAny)val3);
	}
};

class IloAnyBinaryPredicate {
public:
	
	IloAnyBinaryPredicate():_impl(0){}
	
	IloAnyBinaryPredicate(IloAnyBinaryPredicateI* impl):_impl(impl){}
        IloAnyBinaryPredicate(const IloAnyBinaryPredicate& other): _impl(other._impl){}
	~IloAnyBinaryPredicate(){}
	
	IloAnyBinaryPredicateI* getImpl() const {return _impl;}
	
	void operator=(const IloAnyBinaryPredicate& h){_impl = h._impl;}
protected:
	IloAnyBinaryPredicateI* _impl;
public:
	
	IloBool isTrue(const IloAny val1, const IloAny val2){
		assert(_impl !=0);
		return _impl->isTrue(val1,val2);
	}
};

class IloAnyTernaryPredicate {
public:
	
	IloAnyTernaryPredicate():_impl(0){}
	
	IloAnyTernaryPredicate(IloAnyTernaryPredicateI* impl):_impl(impl){}
        IloAnyTernaryPredicate(const IloAnyTernaryPredicate& other): _impl(other._impl){}
	~IloAnyTernaryPredicate(){}
	
	IloAnyTernaryPredicateI* getImpl() const {return _impl;}
	
	void operator=(const IloAnyTernaryPredicate& h){_impl = h._impl;}
protected:
	IloAnyTernaryPredicateI* _impl;
public:
	
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3){
		assert(_impl !=0);
		return _impl->isTrue(val1,val2,val3);
	}
};

#define ILOANYBINARYPREDICATENAME0(name, envName)\
class envName : public IloAnyBinaryPredicateI { \
public:\
	envName():IloAnyBinaryPredicateI(){}\
	IloBool isTrue(const IloAny val1, const IloAny val2);\
};\
	IloAnyBinaryPredicate name(IloEnv env){\
	return new (env) envName();\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2)

#define ILOANYBINARYPREDICATE0(name)\
	ILOANYBINARYPREDICATENAME0(name, name2(name,I))

#define ILOANYBINARYPREDICATENAME1(name, envName, type1, nameArg1)\
class envName : public IloAnyBinaryPredicateI { \
public:\
	type1 nameArg1; \
	envName(type1 IlcArg1);\
	IloBool isTrue(const IloAny val1, const IloAny val2);\
};\
	envName::envName(type1 IlcArg1)\
	:IloAnyBinaryPredicateI(),nameArg1(IlcArg1){}\
	IloAnyBinaryPredicate name(IloEnv env,type1 arg1){\
	return new (env) envName(arg1);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2)

#define ILOANYBINARYPREDICATE1(name, type1, nameArg1)\
	ILOANYBINARYPREDICATENAME1(name, name2(name,I), type1, nameArg1)

#define ILOANYBINARYPREDICATENAME2(name, envName, type1, nameArg1, type2, nameArg2)\
class envName : public IloAnyBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	envName(type1 IlcArg1, type2 IlcArg2);\
	IloBool isTrue(const IloAny val1, const IloAny val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2)\
	:IloAnyBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2){}\
	IloAnyBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2){\
	return new (env) envName(arg1, arg2);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2)

#define ILOANYBINARYPREDICATE2(name, type1, nameArg1, type2, nameArg2)\
	ILOANYBINARYPREDICATENAME2(name, name2(name,I), type1, nameArg1, type2, nameArg2)

#define ILOANYBINARYPREDICATENAME3(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
class envName : public IloAnyBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3);\
	IloBool isTrue(const IloAny val1, const IloAny val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3)\
	:IloAnyBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3){}\
	IloAnyBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3){\
	return new (env) envName(arg1, arg2, arg3);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2)

#define ILOANYBINARYPREDICATE3(name, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
	ILOANYBINARYPREDICATENAME3(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3)

#define ILOANYBINARYPREDICATENAME4(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
class envName : public IloAnyBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4);\
	IloBool isTrue(const IloAny val1, const IloAny val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4)\
	:IloAnyBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4){}\
	IloAnyBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4){\
	return new (env) envName(arg1, arg2, arg3, arg4);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2)

#define ILOANYBINARYPREDICATE4(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
	ILOANYBINARYPREDICATENAME4(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)

#define ILOANYBINARYPREDICATENAME5(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
class envName : public IloAnyBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5);\
	IloBool isTrue(const IloAny val1, const IloAny val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5)\
	:IloAnyBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5){}\
	IloAnyBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2)

#define ILOANYBINARYPREDICATE5(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
	ILOANYBINARYPREDICATENAME5(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)

#define ILOANYBINARYPREDICATENAME6(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
class envName : public IloAnyBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	type6 nameArg6; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6);\
	IloBool isTrue(const IloAny val1, const IloAny val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6)\
	:IloAnyBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5), nameArg6(IlcArg6){}\
	IloAnyBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5, arg6);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2)

#define ILOANYBINARYPREDICATE6(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
	ILOANYBINARYPREDICATENAME6(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)

#define ILOANYTERNARYPREDICATENAME0(name, envName)\
class envName : public IloAnyTernaryPredicateI { \
public:\
	envName():IloAnyTernaryPredicateI(){}\
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3);\
};\
	IloAnyTernaryPredicate name(IloEnv env){\
	return new (env) envName();\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2, const IloAny val3)

#define ILOANYTERNARYPREDICATE0(name)\
	ILOANYTERNARYPREDICATENAME0(name, name2(name,I))

#define ILOANYTERNARYPREDICATENAME1(name, envName, type1, nameArg1)\
class envName : public IloAnyTernaryPredicateI { \
public:\
	type1 nameArg1; \
	envName(type1 IlcArg1);\
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3);\
};\
	envName::envName(type1 IlcArg1)\
	:IloAnyTernaryPredicateI(),nameArg1(IlcArg1){}\
	IloAnyTernaryPredicate name(IloEnv env,type1 arg1){\
	return new (env) envName(arg1);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2, const IloAny val3)

#define ILOANYTERNARYPREDICATE1(name, type1, nameArg1)\
	ILOANYTERNARYPREDICATENAME1(name, name2(name,I), type1, nameArg1)

#define ILOANYTERNARYPREDICATENAME2(name, envName, type1, nameArg1, type2, nameArg2)\
class envName : public IloAnyTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	envName(type1 IlcArg1, type2 IlcArg2);\
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2)\
	:IloAnyTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2){}\
	IloAnyTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2){\
	return new (env) envName(arg1, arg2);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2, const IloAny val3)

#define ILOANYTERNARYPREDICATE2(name, type1, nameArg1, type2, nameArg2)\
	ILOANYTERNARYPREDICATENAME2(name, name2(name,I), type1, nameArg1, type2, nameArg2)

#define ILOANYTERNARYPREDICATENAME3(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
class envName : public IloAnyTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3);\
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3)\
	:IloAnyTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3){}\
	IloAnyTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3){\
	return new (env) envName(arg1, arg2, arg3);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2, const IloAny val3)

#define ILOANYTERNARYPREDICATE3(name, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
	ILOANYTERNARYPREDICATENAME3(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3)

#define ILOANYTERNARYPREDICATENAME4(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
class envName : public IloAnyTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4);\
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4)\
	:IloAnyTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4){}\
	IloAnyTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4){\
	return new (env) envName(arg1, arg2, arg3, arg4);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2, const IloAny val3)

#define ILOANYTERNARYPREDICATE4(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
	ILOANYTERNARYPREDICATENAME4(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)

#define ILOANYTERNARYPREDICATENAME5(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
class envName : public IloAnyTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5);\
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5)\
	:IloAnyTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5){}\
	IloAnyTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2, const IloAny val3)

#define ILOANYTERNARYPREDICATE5(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
	ILOANYTERNARYPREDICATENAME5(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)

#define ILOANYTERNARYPREDICATENAME6(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
class envName : public IloAnyTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	type6 nameArg6; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6);\
	IloBool isTrue(const IloAny val1, const IloAny val2, const IloAny val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6)\
	:IloAnyTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5), nameArg6(IlcArg6){}\
	IloAnyTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5, arg6);\
}\
	IloBool envName ::isTrue(const IloAny val1, const IloAny val2, const IloAny val3)

#define ILOANYTERNARYPREDICATE6(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
	ILOANYTERNARYPREDICATENAME6(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)

#define ILOINTBINARYPREDICATENAME0(name, envName)\
class envName : public IloIntBinaryPredicateI { \
public:\
	envName():IloIntBinaryPredicateI(){}\
	IloBool isTrue(const IloInt val1, const IloInt val2) ILO_OVERRIDE;\
};\
	IloIntBinaryPredicate name(IloEnv env){\
	return new (env) envName();\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2)

#define ILOINTBINARYPREDICATE0(name)\
	ILOINTBINARYPREDICATENAME0(name, name2(name,I))

#define ILOINTBINARYPREDICATENAME1(name, envName, type1, nameArg1)\
class envName : public IloIntBinaryPredicateI { \
public:\
	type1 nameArg1; \
	envName(type1 IlcArg1);\
	IloBool isTrue(const IloInt val1, const IloInt val2);\
};\
	envName::envName(type1 IlcArg1)\
	:IloIntBinaryPredicateI(),nameArg1(IlcArg1){}\
	IloIntBinaryPredicate name(IloEnv env,type1 arg1){\
	return new (env) envName(arg1);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2)

#define ILOINTBINARYPREDICATE1(name, type1, nameArg1)\
	ILOINTBINARYPREDICATENAME1(name, name2(name,I), type1, nameArg1)

#define ILOINTBINARYPREDICATENAME2(name, envName, type1, nameArg1, type2, nameArg2)\
class envName : public IloIntBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	envName(type1 IlcArg1, type2 IlcArg2);\
	IloBool isTrue(const IloInt val1, const IloInt val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2)\
	:IloIntBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2){}\
	IloIntBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2){\
	return new (env) envName(arg1, arg2);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2)

#define ILOINTBINARYPREDICATE2(name, type1, nameArg1, type2, nameArg2)\
	ILOINTBINARYPREDICATENAME2(name, name2(name,I), type1, nameArg1, type2, nameArg2)

#define ILOINTBINARYPREDICATENAME3(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
class envName : public IloIntBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3);\
	IloBool isTrue(const IloInt val1, const IloInt val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3)\
	:IloIntBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3){}\
	IloIntBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3){\
	return new (env) envName(arg1, arg2, arg3);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2)

#define ILOINTBINARYPREDICATE3(name, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
	ILOINTBINARYPREDICATENAME3(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3)

#define ILOINTBINARYPREDICATENAME4(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
class envName : public IloIntBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4);\
	IloBool isTrue(const IloInt val1, const IloInt val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4)\
	:IloIntBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4){}\
	IloIntBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4){\
	return new (env) envName(arg1, arg2, arg3, arg4);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2)

#define ILOINTBINARYPREDICATE4(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
	ILOINTBINARYPREDICATENAME4(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)

#define ILOINTBINARYPREDICATENAME5(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
class envName : public IloIntBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5);\
	IloBool isTrue(const IloInt val1, const IloInt val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5)\
	:IloIntBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5){}\
	IloIntBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2)

#define ILOINTBINARYPREDICATE5(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
	ILOINTBINARYPREDICATENAME5(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)

#define ILOINTBINARYPREDICATENAME6(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
class envName : public IloIntBinaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	type6 nameArg6; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6);\
	IloBool isTrue(const IloInt val1, const IloInt val2);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6)\
	:IloIntBinaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5), nameArg6(IlcArg6){}\
	IloIntBinaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5, arg6);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2)

#define ILOINTBINARYPREDICATE6(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
	ILOINTBINARYPREDICATENAME6(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)

#define ILOINTTERNARYPREDICATENAME0(name, envName)\
class envName : public IloIntTernaryPredicateI { \
public:\
	envName():IloIntTernaryPredicateI(){}\
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3) ILO_OVERRIDE;\
};\
	IloIntTernaryPredicate name(IloEnv env){\
	return new (env) envName();\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2, const IloInt val3)

#define ILOINTTERNARYPREDICATE0(name)\
	ILOINTTERNARYPREDICATENAME0(name, name2(name,I))

#define ILOINTTERNARYPREDICATENAME1(name, envName, type1, nameArg1)\
class envName : public IloIntTernaryPredicateI { \
public:\
	type1 nameArg1; \
	envName(type1 IlcArg1);\
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3);\
};\
	envName::envName(type1 IlcArg1)\
	:IloIntTernaryPredicateI(),nameArg1(IlcArg1){}\
	IloIntTernaryPredicate name(IloEnv env,type1 arg1){\
	return new (env) envName(arg1);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2, const IloInt val3)

#define ILOINTTERNARYPREDICATE1(name, type1, nameArg1)\
	ILOINTTERNARYPREDICATENAME1(name, name2(name,I), type1, nameArg1)

#define ILOINTTERNARYPREDICATENAME2(name, envName, type1, nameArg1, type2, nameArg2)\
class envName : public IloIntTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	envName(type1 IlcArg1, type2 IlcArg2);\
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2)\
	:IloIntTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2){}\
	IloIntTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2){\
	return new (env) envName(arg1, arg2);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2, const IloInt val3)

#define ILOINTTERNARYPREDICATE2(name, type1, nameArg1, type2, nameArg2)\
	ILOINTTERNARYPREDICATENAME2(name, name2(name,I), type1, nameArg1, type2, nameArg2)

#define ILOINTTERNARYPREDICATENAME3(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
class envName : public IloIntTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3);\
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3)\
	:IloIntTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3){}\
	IloIntTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3){\
	return new (env) envName(arg1, arg2, arg3);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2, const IloInt val3)

#define ILOINTTERNARYPREDICATE3(name, type1, nameArg1, type2, nameArg2, type3, nameArg3)\
	ILOINTTERNARYPREDICATENAME3(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3)

#define ILOINTTERNARYPREDICATENAME4(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
class envName : public IloIntTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4);\
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4)\
	:IloIntTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4){}\
	IloIntTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4){\
	return new (env) envName(arg1, arg2, arg3, arg4);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2, const IloInt val3)

#define ILOINTTERNARYPREDICATE4(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)\
	ILOINTTERNARYPREDICATENAME4(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4)

#define ILOINTTERNARYPREDICATENAME5(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
class envName : public IloIntTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5);\
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5)\
	:IloIntTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5){}\
	IloIntTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2, const IloInt val3)

#define ILOINTTERNARYPREDICATE5(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)\
	ILOINTTERNARYPREDICATENAME5(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5)

#define ILOINTTERNARYPREDICATENAME6(name, envName, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
class envName : public IloIntTernaryPredicateI { \
public:\
	type1 nameArg1; \
	type2 nameArg2; \
	type3 nameArg3; \
	type4 nameArg4; \
	type5 nameArg5; \
	type6 nameArg6; \
	envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6);\
	IloBool isTrue(const IloInt val1, const IloInt val2, const IloInt val3);\
};\
	envName::envName(type1 IlcArg1, type2 IlcArg2, type3 IlcArg3, type4 IlcArg4, type5 IlcArg5, type6 IlcArg6)\
	:IloIntTernaryPredicateI(),nameArg1(IlcArg1), nameArg2(IlcArg2), nameArg3(IlcArg3), nameArg4(IlcArg4), nameArg5(IlcArg5), nameArg6(IlcArg6){}\
	IloIntTernaryPredicate name(IloEnv env,type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6){\
	return new (env) envName(arg1, arg2, arg3, arg4, arg5, arg6);\
}\
	IloBool envName ::isTrue(const IloInt val1, const IloInt val2, const IloInt val3)

#define ILOINTTERNARYPREDICATE6(name, type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)\
	ILOINTTERNARYPREDICATENAME6(name, name2(name,I), type1, nameArg1, type2, nameArg2, type3, nameArg3, type4, nameArg4, type5, nameArg5, type6, nameArg6)

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVarArray vars,
								 const IloIntTupleSet set,
								 IloBool compatible);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVar var1,
								 const IloIntVar var2,
								 const IloIntTupleSet set,
								 IloBool compatible);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVar var1,
								 const IloIntVar var2,
								 const IloIntVar var3,
								 const IloIntTupleSet set,
								 IloBool compatible);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVarArray vars,
								 const IloIntTernaryPredicate pred);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVar var1,
								 const IloIntVar var2,
								 const IloIntVar var3,
								 const IloIntTernaryPredicate pred);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVarArray vars,
								 const IloIntBinaryPredicate pred);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVar var1,
								 const IloIntVar var2,
								 const IloIntBinaryPredicate pred);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloIntVar y,
								 const IloIntArray a,
								 const IloNumVar x);

IloConstraint IloAllowedAssignments(const IloEnv env,
                                    const IloIntVarArray vars,
                                    const IloIntTupleSet set);

IloConstraint IloAllowedAssignments(const IloEnv env,
                                    const IloIntVar x,
                                    const IloIntVar y,
                                    const IloIntTupleSet set);

IloConstraint IloAllowedAssignments(const IloEnv env,
                                    const IloIntVar x,
                                    const IloIntVar y,
                                    const IloIntVar z,
                                    const IloIntTupleSet set);

IloConstraint IloAllowedAssignments(const IloEnv env,
                                    const IloIntExprArg x,
                                    const IloIntArray values);

IloConstraint IloAllowedAssignments(const IloEnv env,
                                    const IloIntVar x,
                                    const IloIntVar y,
                                    const IloIntBinaryPredicate pred);

IloConstraint IloAllowedAssignments(const IloEnv env,
                                    const IloIntVar x,
                                    const IloIntVar y,
                                    const IloIntVar z,
                                    const IloIntTernaryPredicate pred);

IloConstraint IloForbiddenAssignments(const IloEnv env,
                                      const IloIntVarArray vars,
                                      const IloIntTupleSet set);

IloConstraint IloForbiddenAssignments(const IloEnv env,
                                      const IloIntVar x,
                                      const IloIntVar y,
                                      const IloIntTupleSet set);

IloConstraint IloForbiddenAssignments(const IloEnv env,
                                      const IloIntVar x,
                                      const IloIntVar y,
                                      const IloIntVar z,
                                      const IloIntTupleSet set);

IloConstraint IloForbiddenAssignments(const IloEnv env,
                                      const IloIntExprArg x,
                                      const IloIntArray values);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloAnyVarArray vars,
								 const IloAnyTupleSet set,
								 IloBool compatible);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloAnyVarArray vars,
								 const IloAnyTernaryPredicate pred);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloAnyVarArray vars,
								 const IloAnyBinaryPredicate pred);

IloConstraint IloTableConstraint(const IloEnv env,
								 const IloAnyVar y,
								 const IloAnyArray a,
								 const IloNumVar x);

class IloIntTableSetConstraintI : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntTableSetConstraintI, IloConstraintI,
		IloIntVarArray, _vars,
		IloIntTupleSet, _set,
		IloBool, _compatible)
		virtual ~IloIntTableSetConstraintI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntVarArray getVars() const {return _vars;}
	IloIntTupleSet getSet() const {return _set;}
	IloExtractableI* getExtractable() const {return _set.getImpl();}
	

	IloBool getCompatible() const { return _compatible; }
};

class IloIntTableSetConstraint : public IloConstraint {
	ILOEXTRHANDLE(IloIntTableSetConstraint,IloConstraint)
public:
	IloIntTableSetConstraint(const IloEnv env,
		const IloIntVarArray vars,
		const IloIntTupleSet set,
		IloBool compatible,
		const char* name=0) :
	IloConstraint(new (env) IloIntTableSetConstraintI(env.getImpl(),
		vars.copy(),
		set,
		compatible,
		name)) {
			IloAssert(vars.getImpl(), "Using empty IloIntVarArray handle.");
			IloAssert(set.getImpl(), "Using empty IloIntTupleSet handle.");
	}
};

class IloIntTableBinPredicateConstraintI : public IloConstraintI {
	ILOEXTRMEMBERS2DECL(IloIntTableBinPredicateConstraintI, IloConstraintI,
		IloIntVarArray, _vars,
		IloIntBinaryPredicateI*, _pred)
		virtual ~IloIntTableBinPredicateConstraintI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntBinaryPredicateI* getPredicate() const{return _pred;}
	IloIntVarArray getVars() const {return _vars;}
};

class IloIntTableBinPredicateConstraint : public IloConstraint {
	ILOEXTRHANDLE(IloIntTableBinPredicateConstraint,IloConstraint)
public:
	IloIntTableBinPredicateConstraint(const IloEnv env,
		const IloIntVarArray vars,
		const IloIntBinaryPredicate pred,
		const char* name=0) :
	IloConstraint(new (env) IloIntTableBinPredicateConstraintI(env.getImpl(),
		vars.copy(),
		pred.getImpl(),
		name)) {
			IloAssert(vars.getImpl(), "Using empty IloIntVarArray handle.");
	}
};

class IloIntTableTernaryPredicateConstraintI : public IloConstraintI {
	ILOEXTRMEMBERS2DECL(IloIntTableTernaryPredicateConstraintI, IloConstraintI,
		IloIntVarArray, _vars,
		IloIntTernaryPredicateI*, _pred)
		virtual ~IloIntTableTernaryPredicateConstraintI();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntTernaryPredicateI* getPredicate()const {return _pred;}
	IloIntVarArray getVars() const {return _vars;}
};

class IloIntTableTernaryPredicateConstraint : public IloConstraint {
	ILOEXTRHANDLE(IloIntTableTernaryPredicateConstraint,IloConstraint)
public:
	IloIntTableTernaryPredicateConstraint(const IloEnv env,
		const IloIntVarArray vars,
		const IloIntTernaryPredicate pred,
		const char* name=0) :
	IloConstraint(new (env) IloIntTableTernaryPredicateConstraintI(env.getImpl(),
		vars.copy(),
		pred.getImpl(),
		name)) {
			IloAssert(vars.getImpl(), "Using empty IloIntVarArray handle.");
	}
};

class IloIntTableElementConstraint2I : public IloConstraintI {
	ILOEXTRMEMBERS3DECL(IloIntTableElementConstraint2I, IloConstraintI,
		IloIntVar, _y,
		IloIntArray, _a,
		IloNumVar, _x)
		virtual ~IloIntTableElementConstraint2I();
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntVar getYVar() const {return _y;}
	IloNumVar getXVar() const {return _x;}
	IloIntArray getArray() const {return _a;}
};

class IloIntTableElementConstraint2 : public IloConstraint {
	ILOEXTRHANDLE(IloIntTableElementConstraint2,IloConstraint)
public:
	IloIntTableElementConstraint2(const IloEnv env,
		const IloIntVar y,
		const IloIntArray a,
		const IloNumVar x,
		const char* name=0):
	IloConstraint(new (env) IloIntTableElementConstraint2I(env.getImpl(),
		y, a.copy(), x,name)) {
	}
};

#define IloAnyTableSetConstraintI IloIntTableSetConstraintI

class IloAnyTableSetConstraint : public IloConstraint {
	ILOEXTRHANDLE(IloAnyTableSetConstraint,IloConstraint)
public:
	IloAnyTableSetConstraint(const IloEnv env,
		const IloAnyVarArray vars,
		const IloAnyTupleSet set,
		IloBool compatible,
		const char* name=0) :
	IloConstraint(new (env) IloIntTableSetConstraintI(env.getImpl(),
		vars.copy(),
		set.getImpl(),
		compatible,
		name)) {
			IloAssert(vars.getImpl(), "Using empty IloAnyVarArray handle.");
			IloAssert(set.getImpl(), "Using empty IloAnyTupleSet handle.");
	}
};

#define IloAnyTableBinPredicateConstraintI IloIntTableBinPredicateConstraintI

class IloAnyTableBinPredicateConstraint : public IloConstraint {
	ILOEXTRHANDLE(IloAnyTableBinPredicateConstraint,IloConstraint)
public:
	IloAnyTableBinPredicateConstraint(const IloEnv env,
		const IloAnyVarArray vars,
		const IloAnyBinaryPredicate pred,
		const char* name=0) :
	IloConstraint(new (env) IloIntTableBinPredicateConstraintI(env.getImpl(),
		vars.copy(),
		pred.getImpl(),
		name)) {
			IloAssert(vars.getImpl(), "Using empty IloAnyVarArray handle.");
	}
};

#define IloAnyTableTernaryPredicateConstraintI IloIntTableTernaryPredicateConstraintI

class IloAnyTableTernaryPredicateConstraint : public IloConstraint {
	ILOEXTRHANDLE(IloAnyTableTernaryPredicateConstraint,IloConstraint)
public:
	IloAnyTableTernaryPredicateConstraint(const IloEnv env,
		const IloAnyVarArray vars,
		const IloAnyTernaryPredicate pred,
		const char* name=0) :
	IloConstraint(new (env) IloAnyTableTernaryPredicateConstraintI(env.getImpl(),
		vars.copy(),
		pred.getImpl(),
		name)) {
			IloAssert(vars.getImpl(), "Using empty IloAnyVarArray handle.");
	}
};

#define IloAnyTableElementConstraint2I IloIntTableElementConstraint2I

class IloAnyTableElementConstraint2 : public IloConstraint {
	ILOEXTRHANDLE(IloAnyTableElementConstraint2,IloConstraint)
public:
	IloAnyTableElementConstraint2(const IloEnv env,
		const IloAnyVar y,
		const IloAnyArray a,
		const IloNumVar x,
		const char* name=0);
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
