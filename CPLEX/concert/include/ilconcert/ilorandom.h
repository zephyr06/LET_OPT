// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilorandom.h
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

#ifndef __CONCERT_ilorandomH
#define __CONCERT_ilorandomH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>

#ifdef ILONEWRAND
#define ILORANDOM_SIZE 624
#else
#define ILORANDOM_SIZE 532
#endif

class IloRandomStateI {
public:
#ifdef ILONEWRAND
	typedef IloUInt Type;
#else
	class Element {
#ifdef ILO64
	private:
		IloUInt _value;
	public:
		Element() : _value(0) {}
#else
	private:
		IloUInt _low;
		IloUInt _high;
	public:
		Element() : _low(0), _high(0) {}
#endif
		void init(IloUInt, IloUInt);
		void combine(Element);
		IloNum get() const;
	};
	typedef Element Type;
#endif
protected:
	IloInt  _index;
	Type  * _values;
public:
	IloRandomStateI();
	virtual ~IloRandomStateI();
	void display(ILOSTD(ostream) & out) const;
  Type* getValues() const { return _values; }
  template<class RANDOMSTATE> void copy(const RANDOMSTATE * state) {
    memcpy(_values, state->getValues(), ILORANDOM_SIZE * sizeof(*_values));
    _index = state->getIndex();
  }
	Type getElement(IloInt i) const { return _values[i]; }
	IloInt getIndex() const { return _index; }
	void setElement(IloInt i, Type val) { _values[i] = val; }
	void setIndex(IloInt i) { _index = i; }
	void create(IloMemoryManagerI * mm);
	void destroy(IloMemoryManagerI * mm);
};

class IloConcertRandomStateI : public IloRandomStateI {
private:
	IloMemoryManagerI * _env;
public:
	typedef IloRandomStateI::Type Type;
	IloConcertRandomStateI(IloMemoryManagerI * env);
	virtual ~IloConcertRandomStateI();

	IloMemoryManagerI * getMemoryManager() const { return _env; }
	void operator delete(void *, size_t);
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloMemoryManagerI *);
	void operator delete(void *, const IloMemoryManager &);
#endif
};
class IloRandomBaseI {
protected:
  IloRandomStateI * _state;
public:
  IloRandomBaseI(IloRandomStateI * state = 0);
  virtual ~IloRandomBaseI();
  void advance(IloInt count = 1);
  template<class RANDOMBASE> void copy(const RANDOMBASE * rb) { copyState(rb->getState()); }
  template<class RANDOMSTATE> void copyState(const RANDOMSTATE * state) { _state->copy(state); }
  void displayState(ILOSTD(ostream) & out) const { _state->display(out); }
  virtual IloNum getFloat() = 0;
  IloNum getFloatAux();
  IloInt getInt(IloInt n);
  void reSeed(IloInt);

  IloRandomStateI::Type getElement(IloInt i) const {
    return _state->getElement(i);
  }
  IloInt getIndex() const { return _state->getIndex(); }
  void setElement(IloInt i, IloRandomStateI::Type value) {
    _state->setElement(i, value);
  }
  void setIndex(IloInt index) { _state->setIndex(index); }
  IloRandomStateI* getState() const { return _state; }
#ifdef ILONEWRAND
private:
  unsigned long get();
#endif
};

class IloRandomI : public IloRandomBaseI {
private:
	IloMemoryManagerI * _env;
public:
	IloRandomI(IloMemoryManagerI *env, IloInt seed = 0);
	virtual ~IloRandomI();
	virtual IloBool canDelete() const;
	void display(ILOSTD(ostream) &os) const;
	IloMemoryManagerI * getMemoryManager() const { return _env; }
	IloNum getFloat() ILO_OVERRIDE;
	IloRandomI * makeClone(IloMemoryManagerI* env) const;
	void operator delete(void *, size_t);
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloMemoryManagerI *);
	void operator delete(void *, const IloMemoryManager &);
#endif
	ILOEXTENSIONI
};

class IloUniqueRandomI : public IloRandomI {
public:
	IloUniqueRandomI(IloMemoryManagerI * env, IloInt seed = 0);
	IloBool canDelete() const ILO_OVERRIDE;
};

class ILO_EXPORTED IloRandom {
protected:
	IloRandomI *_impl;
public:
	
	IloRandom() : _impl(0) { }
	
	IloRandom(const IloEnv env, IloInt seed = 0);
	IloRandom(const IloMemoryManager env, IloInt seed = 0);
	
	IloRandom(IloRandomI *impl) : _impl(impl) { }
	
	IloRandom(const IloRandom& rand) : _impl(rand._impl) { }

	
	IloRandomI *getImpl() const { return _impl; }

	void copy(IloRandom r) {
		assert(_impl != 0);
		assert(r._impl != 0);
		_impl->copy(r._impl);
	}
	void advance(IloInt n = 1) const {
		assert(_impl != 0);
		_impl->advance(n);
	}
	void display(ILOSTD(ostream) &os) const {
		assert(_impl != 0);
		_impl->display(os);
	}

	
	void end() {
		if (_impl != 0 && _impl->canDelete())
			delete _impl;
		_impl = 0;
	}

	
	IloNum getFloat() const {
		assert(_impl != 0);
		return _impl->getFloat();
	}

	
	IloInt getInt(IloInt n) const {
		assert(_impl != 0);
		return _impl->getInt(n);
	}

	
	IloEnv getEnv() const {
		assert(_impl != 0);
		return IloEnv(IloMemoryManager(_impl->getMemoryManager()));
	}
	IloMemoryManager getMemoryManager() const {
		assert(_impl != 0);
		return IloMemoryManager(IloMemoryManager(_impl->getMemoryManager()));
	}
	IloRandom makeClone(IloMemoryManager env) const {
		assert(_impl != 0);
		assert(env.getImpl() != 0);
		return _impl->makeClone(env.getImpl());
	}

	
	void reSeed(IloInt seed) {
		assert(_impl != 0);
		_impl->reSeed(seed);
	}

	ILOEXTENSIONH(IloRandom)
};

ILOSTD(ostream)& operator << (ILOSTD(ostream) &os, const IloRandom& r);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
