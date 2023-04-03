// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilosolution.h
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

#ifndef __CONCERT_ilosolutionH
#define __CONCERT_ilosolutionH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#if !defined(ILO_NO_EA_SOLUTION)
# define ILO_EA_SOLUTION
#endif

#include <ilconcert/iloalg.h>
#include <ilconcert/ilolinear.h>
#include <ilconcert/iloany.h>
#include <ilconcert/iloset.h>
#include <ilconcert/iloanyset.h>
#include <ilconcert/ilodomaini.h>
#include <ilconcert/ilosmodel.h>
#include <ilconcert/ilsched/ilostimei.h>
#include <ilconcert/iloalg.h>
#include <ilconcert/ilohash.h>

class IloSavedIntervalI;
class IloSavedSequenceI;
class IloSavedAtomI;

IloNumVar IloMapToNumVar(IloObjective);
extern const IloNum IloNumMax;
extern const IloNum IloNumMin;

#define ILOSOLUTIONPRECISION (1e-10)

class IloSolutionI;
class IloSolution;
class IloAnd;

class IloSolutionElementI : public IloMemoryManagerObjectI {
private:
	const IloExtractableI *_original;
	IloAny                 _object;
	unsigned               _flags;

public:
	IloSolutionElementI(IloMemoryManager env, IloExtractable extr);
	virtual ~IloSolutionElementI();
	void clearFlags(unsigned f) { _flags = _flags & ~f; }
	virtual void copy(const IloSolutionElementI *element) = 0;
	IloSolutionElementI *copyAll(IloSolutionElementI *element) const;
	virtual void display(ILOSTD(ostream) &) const;
	virtual IloConstraint getConstraint() const; 
	IloExtractable getExtractable() const { return (IloExtractableI*)_original; }
	const IloExtractableI *getExtractableI() const { return _original; }
	unsigned getFlags() const { return _flags; }
	unsigned getFlags(unsigned f) const { return _flags & f; }
	IloAny getObject() const { return _object; }
	virtual IloBool isEquivalent(const IloSolutionElementI *element) const = 0;
	IloBool isFeasible(IloAlgorithmI *, const IloSolutionManagerI * = 0) const;
	virtual IloBool isRestorable() const;
	virtual IloSolutionElementI *makeClone(IloMemoryManagerI*) const = 0;
	void restore(IloAlgorithmI *, const IloSolutionManagerI * = 0) const;
	void setFlags(unsigned f) { _flags = _flags | f; }
	void copyAttributes(const IloSolutionElementI * e) {
		_flags = e->_flags;
		_object = e->_object;
	}
	virtual void setNonRestorable();
	void setObject(IloAny object) { _object = object; }
	virtual void setRestorable();
	void store(IloAlgorithmI *, const IloSolutionManagerI * = 0);

	
	virtual IloBool isFeasibleNull(IloAlgorithmI *a) const;
	virtual void restoreNull(IloAlgorithmI *a) const;
	virtual void storeNull(IloAlgorithmI *a);
};

class IloSolutionElement {
protected:
	IloSolutionElementI *_impl;
public:
	IloSolutionElement() : _impl(0) { }
	IloSolutionElement(IloSolutionElementI *impl) : _impl(impl) { }
	IloSolutionElement(const IloSolutionElement& elem) : _impl(elem._impl) { }
	IloSolutionElementI *getImpl() const { return _impl; }
	void operator = (const IloSolutionElement& h) { _impl = h._impl; }
	void copy(IloSolutionElement e) {
		assert(_impl != 0);
		assert(e._impl != 0);
		_impl->copy(e._impl);
	}
	void display(ILOSTD(ostream)& stream) const {
		assert(_impl != 0);
		_impl->display(stream);
	}
	IloSolutionElement makeClone(IloMemoryManager env) const {
		assert(_impl != 0);
		return _impl->makeClone(env.getImpl());
	}
	IloConstraint getConstraint() const { 
		assert(_impl != 0);
		return _impl->getConstraint();
	}
	IloEnv getEnv() const {
		assert(_impl != 0);
		return _impl->getEnv();
	}
	IloMemoryManager getMemoryManager() const {
		assert(_impl != 0);
		return _impl->getMemoryManager();
	}
	IloExtractable getExtractable() const {
		assert(_impl != 0);
		return _impl->getExtractable();
	}
	IloAny getObject() const {
		assert(getImpl() != 0);
		return _impl->getObject();
	}
	IloBool isEquivalent(IloSolutionElement e) const {
		assert(_impl != 0);
		assert(e._impl != 0);
		return _impl->isEquivalent(e._impl);
	}
	IloBool isFeasible(IloAlgorithm algorithm) const {
		assert(_impl != 0);
		return _impl->isFeasible(algorithm.getImpl());
	}
	IloBool isRestorable() const {
		assert(_impl != 0);
		return _impl->isRestorable();
	}
	void restore(IloAlgorithm algorithm) const {
		assert(_impl != 0);
		_impl->restore(algorithm.getImpl());
	}
	void store(IloAlgorithm algorithm) {
		assert(_impl != 0);
		_impl->store(algorithm.getImpl());
	}
	IloBool isFeasible(IloAlgorithm algorithm, IloSolution parent) const;
	void restore(IloAlgorithm algorithm, IloSolution parent) const;
	void store(IloAlgorithm algorithm, IloSolution parent);
	void setObject(IloAny object) {
		assert(_impl != 0);
		_impl->setObject(object);
	}
	void end() {
		delete _impl;
		_impl = 0;
	}
};

class IloSolutionOperationsI : public IloMemoryManagerObjectI {
private:
	IloTypeIndex _typeIndex;
	unsigned     _opFlags;

public:
	IloSolutionOperationsI(IloMemoryManagerI *env, IloTypeIndex t)
		: IloMemoryManagerObjectI(env), _typeIndex(t), _opFlags(0) { }
	IloSolutionOperationsI(IloEnvI *env, IloTypeIndex t)
		: IloMemoryManagerObjectI(env->getGeneralAllocator()), _typeIndex(t), _opFlags(0) { }
	virtual ~IloSolutionOperationsI();
	IloTypeIndex getTypeIndex() const { return _typeIndex; }
	virtual IloSolutionOperationsI *makeClone(IloMemoryManagerI *env) const = 0;
	IloSolutionOperationsI *makeClone(IloEnvI *env) const{
		return makeClone(env->getGeneralAllocator());
	}

	void setForceCallStore() { _opFlags |= 1; }
	void setForceCallRestore() { _opFlags |= 2; }
	void setForceCallIsFeasible() { _opFlags |= 4; }
	void clearForceCallStore() { _opFlags &= ~1; }
	void clearForceCallRestore() { _opFlags &= ~2; }
	void clearForceCallIsFeasible() { _opFlags &= ~4; }
	IloBool forceCallStore() const { return (_opFlags & 1) != 0; }
	IloBool forceCallRestore() const { return (_opFlags & 2) != 0; }
	IloBool forceCallIsFeasible() const { return (_opFlags & 4) != 0; }

	virtual void store(IloAlgorithmI *a,
		IloSolutionElementI *e) const = 0;
	virtual void restore(IloAlgorithmI *a,
		const IloSolutionElementI *e) const = 0;
	virtual IloBool isFeasible(IloAlgorithmI *a,
		const IloSolutionElementI *e) const = 0;
};

class IloSolutionManagerPlugIn : public IloPlugInAddon {
	ILORTTIDECL
protected:
	IloSolutionOperationsI *_operations;
public:
	IloSolutionManagerPlugIn(IloEnvI * env);
	void init(IloPlugIn* plug) ILO_OVERRIDE;
	void end(IloPlugIn* plugin) ILO_OVERRIDE;
};

#define ILOSOLUTIONOPERATIONS1(FUNCNAME, A, EX, SE, U) \
class name2(IloSolutionOperations, name2(A, EX)) : public IloSolutionOperationsI { \
	const U * const _operationsClass; \
public: \
	name2(IloSolutionOperations, name2(A, EX))(IloMemoryManagerI *env, IloTypeIndex tIndex, const U *uc) : \
	IloSolutionOperationsI(env, tIndex), _operationsClass(uc) { } \
	name2(IloSolutionOperations, name2(A, EX))(IloEnvI *env, IloTypeIndex tIndex, const U *uc) : \
	IloSolutionOperationsI(env->getGeneralAllocator(), tIndex), _operationsClass(uc) { } \
	void store(IloAlgorithmI *a, IloSolutionElementI *e) const ILO_OVERRIDE { \
	if (a->uses(e->getExtractableI()->getId()) || forceCallStore()) \
	_operationsClass->store((name2(A, I) *)a, (SE *)e); \
} \
	void restore(IloAlgorithmI *a, const IloSolutionElementI *e) const ILO_OVERRIDE { \
	if (a->uses(e->getExtractableI()->getId()) || forceCallRestore()) \
	_operationsClass->restore((name2(A, I) *)a, (const SE *)e); \
} \
	IloBool isFeasible(IloAlgorithmI *a, const IloSolutionElementI *e) const ILO_OVERRIDE { \
	if (a->uses(e->getExtractableI()->getId()) || forceCallIsFeasible()) \
	return _operationsClass->isFeasible((name2(A, I) *)a, (const SE *) e); \
	else \
	return IloTrue; \
} \
	virtual IloSolutionOperationsI *makeClone(IloMemoryManagerI *env) const ILO_OVERRIDE { \
	IloSolutionOperationsI * ops = \
	new (env) name2(IloSolutionOperations, name2(A, EX))(*this); \
	return ops; \
} \
	const U * getUserOperations() const { return _operationsClass; } \
};

#define ILOSOLUTIONOPERATIONS2(FUNCNAME, A, EX, U) \
class name2(IloSolutionOperationsAddon, name2(A, EX)) : \
	public IloSolutionManagerPlugIn {\
	ILORTTIDECL \
	const U * _uc; \
public: \
	name2(IloSolutionOperationsAddon, name2(A, EX))(IloEnvI *env, const U *uc) \
	: IloSolutionManagerPlugIn(env), _uc(uc) { \
	_operations = new (env) name2(IloSolutionOperations, name2(A, EX)) \
	(env, name2(EX, I)::GetTypeIndex(), uc); \
} \
	name2(~IloSolutionOperationsAddon, name2(A, EX))() { \
	delete _operations; \
	_env->free((U *)_uc, sizeof(*_uc)); \
} \
};

#define ILOSOLUTIONOPERATIONS3(FUNCNAME, A, EX) \
	ILORTTI(name2(IloSolutionOperationsAddon, name2(A, EX)), \
	IloSolutionManagerPlugIn)

#define ILOSOLUTIONOPERATIONS4(FUNCNAME, A, NM, EX, U) \
	void FUNCNAME(IloEnv env, const U * uc) { \
	IloEnvI *envi = env.getImpl(); \
	IloPlugInManager* pm = envi->getPlugInManager(); \
	if (!pm->isCreated(name2(IloSolutionOperationsAddon,name2(A, EX))::GetTypeInfo(), NM)) { \
	IloPlugInAddon* ops = new (envi) \
	name2(IloSolutionOperationsAddon, name2(A, EX))(envi, uc); \
	pm->add(ops, NM); \
	} \
}
#define ILOSOLUTIONOPERATIONS5(FUNCNAME, A, NM, EX, U) \
	void FUNCNAME(IloEnv env) { \
	IloEnvI *envi = env.getImpl(); \
	IloPlugInManager * pm = envi->getPlugInManager(); \
	if (!pm->isCreated(name2(IloSolutionOperationsAddon,name2(A, EX))::GetTypeInfo(), NM)) { \
	U * uc = new (envi) U; \
	IloPlugInAddon* ops = new (envi) \
	name2(IloSolutionOperationsAddon, name2(A, EX))(envi, uc); \
	pm->add(ops, NM); \
	} \
}

#define ILOSOLUTIONOPERATIONS(FUNCNAME, A, NM, EX, SE, U) \
	ILOSOLUTIONOPERATIONS1(FUNCNAME, A, EX, SE, U) \
	ILOSOLUTIONOPERATIONS2(FUNCNAME, A, EX, U) \
	ILOSOLUTIONOPERATIONS3(FUNCNAME, A, EX) \
	ILOSOLUTIONOPERATIONS4(FUNCNAME, A, NM, EX, U) \
	ILOSOLUTIONOPERATIONS5(FUNCNAME, A, NM, EX, U)

#if defined(ILO_HP_aCC)
#define ILOINHERITCOPYCTOR(Hname, Bname) Hname(const Hname& h):Bname(h){}
#else
#define ILOINHERITCOPYCTOR(Hname, Bname)
#endif

#define ILOINHERITHANDLEMINI(Hname, Iname, Bname)\
public:\
	Hname() : Bname() {}\
	Hname(Iname* impl) : Bname(impl) {} \
	ILOINHERITCOPYCTOR(Hname, Bname)\
	Iname* getImpl() const {return (Iname*)Bname::getImpl();}

class IloSavedNumDomainI : public IloMemoryManagerObjectI {
private:
	struct Entry {
		IloNum _value;
		Entry * _next;
		Entry(IloNum value, Entry *next = 0) : _value(value), _next(next) { }
	};
	IloInt    _numRows;
	IloInt    _logNumRows;
	IloInt    _size;
	Entry  ** _table;
	IloNum    _min;
	IloNum    _max;

	IloBool   _boundsValid;

	IloInt getRow(IloNum num) const {
		return hash(num, _logNumRows) & (_numRows - 1);
	}
	void rehash(IloInt numRows);
	static IloInt hash(IloNum num, IloInt bits);
	void updateBounds();
	void resetBounds();
	void sizeUp();
	void sizeDown();
	static void sort(IloNumArray &a);
	static void sort(IloNumArray &a, IloInt min, IloInt max);

public:
	IloSavedNumDomainI(IloMemoryManagerI *);
	~IloSavedNumDomainI();

	void add(IloNum num);
	void add(IloInt min, IloInt max);
	void clear();
	IloBool contains(IloNum num) const;
	IloBool contains(const IloSavedNumDomainI * dom) const;
	void copy(const IloSavedNumDomainI *dom);
	IloNumArray getArray() const;
	IloNumArray getSortedArray() const;
	IloNumSet getSet() const;
	IloInt getSize() const { return _size; }
	IloSavedNumDomainI * makeClone(IloMemoryManagerI* env) const;
	void remove(IloNum num);
	void remove(IloInt min, IloInt max);
	void setSet(IloNumSet);
	IloNum getMin() const {
		if (!_boundsValid)
			((IloSavedNumDomainI *)this)->updateBounds();
		return _min;
	}
	IloNum getMax() const {
		if (!_boundsValid)
			((IloSavedNumDomainI *)this)->updateBounds();
		return _max;
	}
	IloBool isEquivalent(IloSavedNumDomainI * dom) const;
	class Iterator;
	friend class IloSavedNumDomainI::Iterator;
	class Iterator {
	private:
		IloSavedNumDomainI * _dom;
		IloInt               _row;
		Entry **             _entry;

		void advance();
	public:
		Iterator(IloSavedNumDomainI *dom);
		IloBool ok() { return _row != -2; }
		IloNum operator * () const { return (*_entry)->_value; }
		Iterator& operator ++() { advance(); return *this; }
	};
	Iterator* iterator(){
		return new (getMemoryManager()) Iterator(this);
	}
};

class IloSavedIntDomainI : public IloRttiMemoryManagerObjectI {
public:
	class VisitorI {
	public:
		virtual ~VisitorI();
		virtual IloBool needsMoreVisit() const;
		virtual void visit(IloInt value) = 0;
	};

	IloSavedIntDomainI(IloMemoryManagerI* mmgr);
	virtual ~IloSavedIntDomainI();
	virtual void add(IloInt num) = 0;
	virtual void add(IloInt min, IloInt max) = 0;
	virtual void clear() = 0;
	virtual IloBool contains(IloInt num) const = 0;
	virtual IloBool contains(const IloSavedIntDomainI * dom) const = 0;
	virtual void copy(const IloSavedIntDomainI *dom)= 0;
	virtual IloIntArray getArray() const = 0;
	virtual IloIntArray getSortedArray() const = 0;
	virtual IloIntSet getSet() const = 0;
	virtual IloInt getSize() const = 0;
	virtual IloSavedIntDomainI * makeClone(IloMemoryManagerI* env) const = 0;
	virtual void remove(IloInt num) = 0;
	virtual void remove(IloInt min, IloInt max) = 0;
	virtual void setSet(IloIntSet) = 0;
	virtual IloInt getMin() const = 0;
	virtual IloInt getMax() const = 0;
	virtual IloBool isEquivalent(IloSavedIntDomainI * dom) const = 0;
	virtual void accept(VisitorI* visitor) = 0;
	ILORTTIDECL
};

#define ILO_HASH_INT_DOMAIN_IMPL IloHashSavedIntDomainI
#define ILO_HASH_INT_DOMAIN_SUPER_IMPL IloSavedIntDomainI

class ILO_HASH_INT_DOMAIN_IMPL : public ILO_HASH_INT_DOMAIN_SUPER_IMPL
{
private:
	struct Entry {
		IloInt _value;
		Entry * _next;
		Entry(IloInt value, Entry *next = 0) : _value(value), _next(next) { }
	};

	IloInt    _numRows;
	IloInt    _logNumRows;
	IloInt    _size;
	Entry  ** _table;
	IloInt    _min;
	IloInt    _max;
	IloBool   _boundsValid;

	IloInt getRow(IloInt num) const {
		return hash(num, _logNumRows) & (_numRows - 1);
	}
	void rehash(IloInt numRows);
	static IloInt hash(IloInt num, IloInt bits);
	void updateBounds();
	void resetBounds();
	void sizeUp();
	void sizeDown();
	static void sort(IloIntArray &a);
	static void sort(IloIntArray &a, IloInt min, IloInt max);

public:
	ILO_HASH_INT_DOMAIN_IMPL(IloMemoryManagerI *);
	~ILO_HASH_INT_DOMAIN_IMPL();

	void add(IloInt num) ILO_OVERRIDE;
	void add(IloInt min, IloInt max) ILO_OVERRIDE;
	void clear() ILO_OVERRIDE;
	IloBool contains(IloInt num) const ILO_OVERRIDE;
	IloBool contains(const IloSavedIntDomainI * dom) const ILO_OVERRIDE;
	void copy(const IloSavedIntDomainI *dom) ILO_OVERRIDE;
	IloIntArray getArray() const ILO_OVERRIDE;
	IloIntArray getSortedArray() const ILO_OVERRIDE;
	IloIntSet getSet() const ILO_OVERRIDE;
	IloInt getSize() const ILO_OVERRIDE { return _size; }
	IloSavedIntDomainI * makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
	void remove(IloInt num) ILO_OVERRIDE;
	void remove(IloInt min, IloInt max) ILO_OVERRIDE;
	void setSet(IloIntSet) ILO_OVERRIDE;
	IloInt getMin() const ILO_OVERRIDE {
		if (!_boundsValid)
			((ILO_HASH_INT_DOMAIN_IMPL *)this)->updateBounds();
		return _min;
	}
	IloInt getMax() const ILO_OVERRIDE {
		if (!_boundsValid)
			((ILO_HASH_INT_DOMAIN_IMPL *)this)->updateBounds();
		return _max;
	}
	IloBool isEquivalent(IloSavedIntDomainI * dom) const ILO_OVERRIDE;
	void accept(IloSavedIntDomainI::VisitorI* visitor) ILO_OVERRIDE;
	ILORTTIDECL
};

class IloSavedNumVarI : public IloSolutionElementI {
private:
	union {
		struct {
			IloNum    _min;
			IloNum    _max;
		} _float;
		struct {
			IloInt	_min;
			IloInt	_max;
		} _int;
	} _bounds;
	IloSavedNumDomainI * _fltDom;
	IloSavedIntDomainI * _intDom;

	void deleteDomain();
	IloBool hasDomain() const {
		return isInt() ? _intDom != 0 :_fltDom != 0;
	}
	void initFloat();
	void initInt();
	void initAny();
	void increaseMax(IloInt max);
	void increaseMin(IloInt min);
	void decreaseMax(IloInt max);
	void decreaseMin(IloInt min);

public:
	IloSavedNumVarI(IloMemoryManager env, IloNumVar var);
	IloSavedNumVarI(IloMemoryManager env, IloIntVar var);
	IloSavedNumVarI(IloMemoryManager env, IloAnyVar var);
	~IloSavedNumVarI();
	void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
	void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;
	IloSolutionElementI *makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
	IloSavedIntDomainI * getIntDomain();
	IloSavedNumDomainI * getNumDomain();
	IloNumSet getDomainAsNumSet() const;
	IloIntSet getDomainAsIntSet() const;
	IloSavedNumDomainI * getNumDomain() const {
		return isFloat() ? _fltDom : 0;
	}
	IloSavedIntDomainI * getIntDomain() const {
		return isInt() ? _intDom : 0;
	}
	IloInt getNumberOfDomainElements() const;
	IloNum getMax() const;
	IloNum getMin() const;

	IloNum getValue(IloNum prec = ILOSOLUTIONPRECISION) const;
	IloInt getIntValue() const;
	IloConstraint getConstraint() const ILO_OVERRIDE;
	IloNumVarI * getVar() const { return (IloNumVarI*)getExtractableI(); }
	IloBool isBound(IloNum prec = ILOSOLUTIONPRECISION) const;
	IloBool isFloat() const { return _fltDom != 0 || getVar()->getType() == IloNumVar::Float; }
	IloBool isInt() const   { return _intDom != 0 || getVar()->getType() == IloNumVar::Int || getVar()->getType() == ILOBOOL;  }
	IloBool isAny() const {
		IloVarDomainI * dom = getVar()->getDomain();
		return dom != 0 && dom->isAny();
	}
	IloBool isIntBound() const;
	IloBool isEnumerated() const;
	IloBool isEquivalent(const IloSolutionElementI *) const ILO_OVERRIDE;
	IloBool isInDomain(IloNum element) const;
	IloBool isInIntDomain(IloInt element) const;
	void setMax(IloNum max);
	void setMin(IloNum min);
	void setValue(IloNum value);
	void setValue(IloInt value);
	void setDomain(IloNumSet domain);
	void setDomain(IloIntSet domain);
	void setDomain(IloNum min, IloNum max);
	void forceDomain(IloNum min, IloNum max);
	void replaceIntDomain(IloSavedIntDomainI* domain) {
		if(_intDom != 0)
			delete _intDom;
		_intDom = domain;
	}
};

typedef IloSavedNumVarI IloSavedIntVarI;
typedef IloSavedNumVarI IloSavedAnyVarI;

class IloSavedNumVar : public IloSolutionElement {
	ILOINHERITHANDLEMINI(IloSavedNumVar, IloSavedNumVarI, IloSolutionElement)
public:
	IloSavedNumVar(IloNumVar var);
	IloSavedNumVar(IloMemoryManager env, IloNumVar var);
	IloNumSet getDomainAsNumSet() const {
		assert(getImpl() != 0);
		return getImpl()->getDomainAsNumSet();
	}
	IloIntSet getDomainAsIntSet() const {
		assert(getImpl() != 0);
		return getImpl()->getDomainAsIntSet();
	}
	IloInt getNumberOfDomainElements() const {
		assert(getImpl() != 0);
		return getImpl()->getNumberOfDomainElements();
	}
	IloNum getMax() const {
		assert(getImpl() != 0);
		return getImpl()->getMax();
	}
	IloNum getMin() const {
		assert(getImpl() != 0);
		return getImpl()->getMin();
	}
	IloNum getValue(IloNum prec = ILOSOLUTIONPRECISION) const {
		assert(getImpl() != 0);
		return getImpl()->getValue(prec);
	}
	IloNumVar getVar() const {
		assert(getImpl() != 0);
		return getImpl()->getVar();
	}
	IloBool isBound(IloNum prec = ILOSOLUTIONPRECISION) const {
		assert(getImpl() != 0);
		return getImpl()->isBound(prec);
	}
	IloBool isInDomain(IloNum value) const {
		assert(getImpl() != 0);
		return getImpl()->isInDomain(value);
	}
	IloBool isInDomain(IloInt value) const {
		assert(getImpl() != 0);
		return getImpl()->isInDomain((IloNum)value);
	}
	void setMax(IloNum max) {
		assert(getImpl() != 0);
		getImpl()->setMax(max);
	}
	void setMin(IloNum min) {
		assert(getImpl() != 0);
		getImpl()->setMin(min);
	}
	void setValue(IloNum value) {
		assert(getImpl() != 0);
		getImpl()->setValue(value);
	}
	void setValue(IloInt value) {
		assert(getImpl() != 0);
		getImpl()->setValue(value);
	}
	void setDomain(IloNumSet dom) {
		assert(getImpl() != 0);
		getImpl()->setDomain(dom);
	}
	void setDomain(IloIntSet dom) {
		assert(getImpl() != 0);
		getImpl()->setDomain(dom);
	}
	void setDomain(IloNum min, IloNum max) {
		assert(getImpl() != 0);
		getImpl()->setDomain(min, max);
	}
};

class IloSavedIntVar : public IloSolutionElement {
	ILOINHERITHANDLEMINI(IloSavedIntVar, IloSavedNumVarI, IloSolutionElement)
public:
	IloSavedIntVar(IloIntVar var);
	IloSavedIntVar(IloMemoryManager env, IloIntVar var);
	IloIntSet getDomain() const {
		assert(getImpl() != 0);
		return getImpl()->getDomainAsIntSet();
	}
	IloInt getNumberOfDomainElements() const {
		assert(getImpl() != 0);
		return getImpl()->getNumberOfDomainElements();
	}
	IloInt getMax() const {
		assert(getImpl() != 0);
		return (IloInt)getImpl()->getMax();
	}
	IloInt getMin() const {
		assert(getImpl() != 0);
		return (IloInt)getImpl()->getMin();
	}
	IloInt getValue() const {
		assert(getImpl() != 0);
		return (IloInt)getImpl()->getIntValue();
	}
	IloIntVar getVar() const {
		assert(getImpl() != 0);
		return getImpl()->getVar();
	}
	IloBool isBound() const {
		assert(getImpl() != 0);
		return getImpl()->isIntBound();
	}
	IloBool isInDomain(IloInt value) const {
		assert(getImpl() != 0);
		return getImpl()->isInDomain((IloNum)value);
	}
	void setMax(IloInt max) {
		assert(getImpl() != 0);
		getImpl()->setMax((IloNum)max);
	}
	void setMin(IloInt min) {
		assert(getImpl() != 0);
		getImpl()->setMin((IloNum)min);
	}
	void setValue(IloInt value) {
		assert(getImpl() != 0);
		getImpl()->setValue(value);
	}
	void setDomain(IloIntSet dom) {
		assert(getImpl() != 0);
		IloNumSet ns = dom.toNumSet();
		getImpl()->setDomain(ns);
		ns.end();
	}
	void setDomain(IloInt min, IloInt max) {
		assert(getImpl() != 0);
		getImpl()->setDomain((IloNum)min, (IloNum)max);
	}
};

class IloSavedAnyVar : public IloSolutionElement {
	ILOINHERITHANDLEMINI(IloSavedAnyVar, IloSavedNumVarI, IloSolutionElement)
public:
	IloSavedAnyVar(IloAnyVar var);
	IloSavedAnyVar(IloMemoryManager env, IloAnyVar var);
	IloAnySet getDomain() const {
		assert(getImpl() != 0);
		IloIntSet dom = getImpl()->getDomainAsIntSet();
		IloAnySet ret = dom.toAnySet();
		dom.end();
		return ret;
	}
	IloInt getNumberOfDomainElements() const {
		assert(getImpl() != 0);
		return getImpl()->getNumberOfDomainElements();
	}
	IloAny getValue() const {
		assert(getImpl() != 0);
		return (IloAny)getImpl()->getIntValue();
	}
	IloAnyVar getVar() const {
		assert(getImpl() != 0);
		return (IloNumVarI*)getImpl()->getVar();
	}
	IloBool isBound() const {
		assert(getImpl() != 0);
		return getImpl()->isIntBound();
	}
	IloBool isInDomain(IloAny value) const;
	void setDomain(IloAnySet domain) {
		assert(getImpl() != 0);
		IloIntSet set = domain.toIntSet();
		getImpl()->setDomain(set);
		set.end();
	}
	void setValue(IloAny value) {
		assert(getImpl() != 0);
		getImpl()->setValue((IloInt)value);
	}
};

class IloSavedObjectiveI : public IloSolutionElementI {
private:
        IloInt _n;
	IloNum * _value;
        void clean();
        void build();
public:
	IloSavedObjectiveI(IloEnv env, IloObjective obj);
	IloSavedObjectiveI(IloMemoryManager env, IloObjective obj);
	~IloSavedObjectiveI();
	void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
	void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;
	IloSolutionElementI *makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
	IloBool isEquivalent(const IloSolutionElementI *) const ILO_OVERRIDE;
	IloBool isRestorable() const ILO_OVERRIDE;
	void setRestorable() ILO_OVERRIDE;
	void setNonRestorable() ILO_OVERRIDE;

	IloObjective getObjective() const {
          return (IloObjectiveI *)getExtractableI();
	}
	IloNumVar getVar() const { return IloMapToNumVar(getObjective()); }
	IloNum getValue() const;
	void setValue(IloNum value);

        IloInt getNumCriteria() const { return _n; }
        IloNum getCriterionValue(IloInt i) const { return _value[i]; }
        void setCriterionValue(IloInt i, IloNum v) const { _value[i] = v; }
        void getValues(IloNumArray v) const;
	void setValues(IloNumArray values);

        IloBool isBetterThan(const IloSavedObjectiveI *) const;
};

class IloSavedObjective : public IloSolutionElement {
	ILOINHERITHANDLEMINI(IloSavedObjective, IloSavedObjectiveI,
		IloSolutionElement)
public:
	IloSavedObjective(IloObjective var);
	IloSavedObjective(IloEnv env, IloObjective var);
	IloSavedObjective(IloMemoryManager env, IloObjective var);
	IloNum getValue() const {
		assert(getImpl() != 0);
		return getImpl()->getValue();
	}
	IloObjective getObjective() const {
		assert(getImpl() != 0);
		return getImpl()->getObjective();
	}
	IloNumVar getVar() const {
		assert(getImpl() != 0);
		return getImpl()->getVar();
	}
	void setValue(IloNum value) {
		assert(getImpl() != 0);
		getImpl()->setValue(value);
	}
        void getValues(IloNumArray v) const {
          assert(getImpl() != 0);
          getImpl()->getValues(v);
        }
	void setValues(IloNumArray v) {
          assert(getImpl() != 0);
          getImpl()->setValues(v);
        }
};

class IloSavedIntSetVarI : public IloSolutionElementI {
private:
	IloSavedIntDomainI * _required;
	IloSavedIntDomainI * _possible;
public:
	IloSavedIntSetVarI(IloMemoryManager env, IloIntSetVar var);
	IloSavedIntSetVarI(IloMemoryManager env, IloAnySetVar var);
	~IloSavedIntSetVarI();
	void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
	void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;
	IloSolutionElementI *makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
	IloIntSet getRequiredSet() const;
	IloIntSet getPossibleSet() const;
	IloSavedIntDomainI *getInternalRequiredSet() const { return _required; }
	IloSavedIntDomainI *getInternalPossibleSet() const { return _possible; }
	IloConstraint getConstraint() const ILO_OVERRIDE;
	IloIntSetVar getVar() const;
	IloBool isBound() const;
	IloBool isEquivalent(const IloSolutionElementI *e) const ILO_OVERRIDE;
	IloBool isPossible(IloInt) const;
	IloBool isRequired(IloInt) const;
	void setRequiredSet(IloIntSet reqd);
	void setPossibleSet(IloIntSet poss);
};

typedef IloSavedIntSetVarI IloSavedNumSetVarI;
typedef IloSavedIntSetVarI IloSavedAnySetVarI;

class IloSavedIntSetVar : public IloSolutionElement {
	ILOINHERITHANDLEMINI(IloSavedIntSetVar, IloSavedIntSetVarI, IloSolutionElement)
public:
	IloSavedIntSetVar(IloMemoryManager env, IloIntSetVar var);
	IloSavedIntSetVar(IloIntSetVar var);
	IloBool isBound() const {
		assert(getImpl() != 0);
		return getImpl()->isBound();
	}
	IloBool isPossible(IloInt value) const {
		assert(getImpl() != 0);
		return getImpl()->isPossible(value);
	}
	IloBool isRequired(IloInt value) const {
		assert(getImpl() != 0);
		return getImpl()->isRequired(value);
	}
	IloIntSet getPossibleSet() const {
		assert(getImpl() != 0);
		return getImpl()->getPossibleSet();
	}
	IloIntSet getRequiredSet() const {
		assert(getImpl() != 0);
		return getImpl()->getRequiredSet();
	}
	IloIntSetVar getVar() const {
		assert(getImpl() != 0);
		return getImpl()->getVar();
	}
	void setPossibleSet(IloIntSet possible) {
		assert(getImpl() != 0);
		getImpl()->setPossibleSet(possible);
	}
	void setRequiredSet(IloIntSet required) {
		assert(getImpl() != 0);
		getImpl()->setRequiredSet(required);
	}
};

class IloSavedAnySetVar : public IloSolutionElement {
	ILOINHERITHANDLEMINI(IloSavedAnySetVar, IloSavedAnySetVarI, IloSolutionElement)
public:
	IloSavedAnySetVar(IloMemoryManager env, IloAnySetVar var);
	IloBool isBound() const {
		assert(getImpl() != 0);
		return getImpl()->isBound();
	}
	IloBool isPossible(IloAny value) const {
		assert(getImpl() != 0);
		return getImpl()->isPossible((IloInt)value);
	}
	IloBool isRequired(IloAny value) const {
		assert(getImpl() != 0);
		return getImpl()->isRequired((IloInt)value);
	}
	IloAnySet getPossibleSet() const {
		assert(getImpl() != 0);
		IloIntSet poss = getImpl()->getPossibleSet();
		IloAnySet ret = poss.toAnySet();
		poss.end();
		return ret;
	}
	IloAnySet getRequiredSet() const {
		assert(getImpl() != 0);
		IloIntSet poss = getImpl()->getRequiredSet();
		IloAnySet ret = poss.toAnySet();
		poss.end();
		return ret;
	}
	IloAnySetVar getVar() const {
		assert(getImpl() != 0);
		return getImpl()->getVar().getImpl();
	}
	void setPossibleSet(IloAnySet possible) {
		assert(getImpl() != 0);
		IloIntSet poss = possible.toIntSet();
		getImpl()->setPossibleSet(poss);
		poss.end();
	}
	void setRequiredSet(IloAnySet required) {
		assert(getImpl() != 0);
		IloIntSet req = required.toIntSet();
		getImpl()->setRequiredSet(req);
		req.end();
	}
};

class IloSolutionHashTableI;

class IloSolutionElementListI : public IloMemoryManagerObjectI {
	friend class IloSolutionI;
	friend class IloSolutionHashTableI;
private:
	IloSolutionElementI     *_elem;
	IloSolutionElementListI *_next;
	IloSolutionElementListI *_prev;
	IloSolutionElementListI *_nextHashEntry;
public:
	IloSolutionElementListI(IloMemoryManagerI *env, IloSolutionElementI *e) :
	  IloMemoryManagerObjectI(env), _elem(e), _next(0), _prev(0), _nextHashEntry(0) { }
	  ~IloSolutionElementListI();

	  IloSolutionElementI     *getElement() const { return _elem; }
	  IloSolutionElementListI *getNext() const { return _next; }
	  IloSolutionElementListI *getNextHashEntry() const { return _nextHashEntry; }
	  IloSolutionElementListI *getPrev() const { return _prev; }
	  void setNext(IloSolutionElementListI *n) { _next = n; }
	  void setNextHashEntry(IloSolutionElementListI *n) { _nextHashEntry = n; }
	  void setPrev(IloSolutionElementListI *p) { _prev = p; }
};

class IloSolutionI : public IloExtensibleRttiMemoryManagerObjectI {
	ILORTTIDECL
private:
	IloSolutionElementListI * _first;
	IloSolutionElementListI * _last;
	IloInt                    _nbOfElements;
	IloObjectiveI *           _obj;
	IloSolutionHashTableI *   _hash;
	IloSolutionManagerI *     _overrides;
	IloBool                   _overridesShared;
	IloNum                    _precision;
#ifdef ILO_EA_SOLUTION
	IloMemoryManagerObjectI*  _eaData;
#endif

	IloSolutionElementI *getSafeElement(const IloExtractableI *) const;
	IloSavedNumVar getSafeNumVar(IloNumVar) const;
	IloSavedIntVar getSafeIntVar(IloIntVar) const;
	IloSavedAnyVar getSafeAnyVar(IloAnyVar) const;
	IloSavedIntSetVar getSafeIntSetVar(IloIntSetVar) const;
	IloSavedAnySetVar getSafeAnySetVar(IloAnySetVar) const;
	IloSavedObjective getSafeObjective(IloObjective) const;

	
	
	IloSavedNumVar getAndAddNumVar(IloNumVar);
	IloSavedIntVar getAndAddIntVar(IloIntVar);
	IloSavedAnyVar getAndAddAnyVar(IloAnyVar);
	IloSavedIntSetVar getAndAddIntSetVar(IloIntSetVar);
	IloSavedAnySetVar getAndAddAnySetVar(IloAnySetVar);
	IloSavedObjective getAndAddObjective(IloObjective);

protected:
	void pull(IloSolutionElementListI *);
	IloInt getMaxNoHashSize() const { return 13; }
	void buildHash();

public:
	IloSolutionI(IloMemoryManager env, const char* name = 0);
	virtual ~IloSolutionI();
	void makeMTSafe();
	void add(IloSolutionElementI *element);
#ifdef ILO_EA_SOLUTION
	void setEAData(IloMemoryManagerObjectI* data) {
		_eaData = data;
	}
	IloMemoryManagerObjectI *getEAData() const {
		return _eaData;
	}
#endif
	IloSolutionElementListI * getElementList(const IloExtractableI *ext) const;
	IloBool contains(const IloExtractableI *extr) const {
		return getElementList(extr) != 0;
	}
	virtual void copy(const IloSolutionI *solution);
	virtual void display(ILOSTD(ostream)& os) const;
	IloSolutionI* makeClone(IloEnvI* env) {
		return makeClone(env->getGeneralAllocator());
	}
	virtual IloSolutionI* makeClone(IloMemoryManagerI* env) const;
	IloSolutionElementI* getElement(const IloExtractableI *ext) const {
		IloSolutionElementListI *listbit = getElementList(ext);
		return listbit ? listbit->_elem : 0;
	}
	IloSolutionElementListI *getFirst() const { return _first; }
	IloSolutionManagerI *getAllOverrides() const { return _overrides; }
	IloBool hasOverrides() const { return _overrides != 0; }
	IloSolutionOperationsI *getOverrides(const IloExtractableI *ex) const;
	IloInt getNumberOfElements() const { return _nbOfElements; }
	IloInt getNbOfElements() const { return getNumberOfElements(); }
	IloAnd getConstraint() const;
	IloSolutionElementListI *getLast() const { return _last; }
	IloNum getPrecision() const { return _precision; }
	virtual IloBool isBetterThan(const IloSolutionI *s) const;
	virtual IloBool isEquivalent(const IloSolutionI *solution) const;
	virtual IloBool isFeasible(IloAlgorithm a) const;
	IloBool isRestorable(const IloExtractableI *extr) const;
	IloBool isWorseThan(const IloSolutionI *) const;
	void remove(const IloExtractableI *extr);
	void remove(IloExtractableArray array);
	void remove(IloSolutionElementListI *);
	virtual void restore(IloAlgorithm algorithm,
		const IloSolutionI *exclude = 0) const;
	virtual void store(IloAlgorithm algorithm);
	void setPrecision(IloNum prec) { _precision = prec; }

	void useOverridesFrom(const IloSolutionI *s);
	void copyOverridesFrom(const IloSolutionI *s);
	void setOverrides(IloSolutionOperationsI *ops);
	void removeOverrides(IloTypeIndex ti);
	void removeOverrides();
	void setNonRestorable(const IloExtractableI *extr);
	void setNonRestorable(IloExtractableArray array);
	void setRestorable(const IloExtractableI *extr);
	void setRestorable(IloExtractableArray array);

	
	void add(IloObjective obj);
	IloBool isObjectiveSet() const { return _obj != 0; }
	IloObjective getObjective() const { return _obj; }
	IloNumVar getObjectiveVar() const {
		if (_obj == 0) return 0;
		else           return IloMapToNumVar(_obj);
	}
	IloSavedObjectiveI * getElement(IloObjective obj) const;
	void unsetObjective() { _obj = 0; }
	void setObjective(IloObjective objective);
	IloNum getValue(IloObjective objective) const;
	IloNum getObjectiveValue() const {
		assert(_obj != 0);
		return getValue(_obj);
	}
	void setValue(IloObjective obj, IloNum value);
        void getObjectiveValues(IloNumArray values) const;
        void setObjectiveValues(IloNumArray values) const;

	
	virtual void copy(IloExtractableI *extr, IloSolutionI *soln);
	virtual void display(IloExtractableI *extr, ILOSTD(ostream)& stream);
	IloConstraint getConstraint(IloExtractableI *extr);
	IloBool isEquivalent(IloExtractableI *extr, IloSolutionI *soln);
	virtual void store(IloExtractableI *extr, IloAlgorithm alg);
	virtual void restore(IloExtractableI *extr, IloAlgorithm alg);
	virtual IloBool isFeasible(IloExtractableI *extr, IloAlgorithm alg);

	
	void add(IloNumVar var);
	void add(IloNumVarArray array);
	IloBool isBound(IloNumVar var) const;
	IloSavedNumVarI * getElement(IloNumVar var) const;
	IloNumSet getDomain(IloNumVar var) const;
	IloInt getNumberOfDomainElements(IloNumVar var) const;
	IloNum getMin(IloNumVar var) const;
	IloNum getMax(IloNumVar var) const;
	IloNum getValue(IloNumVar var) const;
	IloBool isInDomain(IloNumVar var, IloNum value) const;
	void setMin(IloNumVar var, IloNum min);
	void setMax(IloNumVar var, IloNum max);
	void setValue(IloNumVar var, IloNum value);
	void setDomain(IloNumVar var, IloNumSet domain);
	void setDomain(IloNumVar var, IloNum min, IloNum max);

	
	void add(IloIntVar var);
	void add(IloIntVarArray array);
	IloBool isBound(IloIntVar var) const;
	IloSavedNumVarI * getElement(IloIntVar var) const;
	IloIntSet getDomain(IloIntVar var) const;
	IloInt getNumberOfDomainElements(IloIntVar var) const;
	IloInt getMin(IloIntVar var) const;
	IloInt getMax(IloIntVar var) const;
	IloInt getValue(IloIntVar var) const;
	IloBool isInDomain(IloIntVar var, IloInt value) const;
	void setMin(IloIntVar var, IloInt min);
	void setMax(IloIntVar var, IloInt max);
	void setValue(IloIntVar var, IloInt value);
	void setDomain(IloIntVar var, IloIntSet domain);
	void setDomain(IloIntVar var, IloInt min, IloInt max);

	
	void add(IloAnyVar var);
	void add(IloAnyVarArray array);
	IloSavedNumVarI * getElement(IloAnyVar var) const;
	IloAnySet getDomain(IloAnyVar var) const;
	IloInt getNumberOfDomainElements(IloAnyVar var) const;
	IloAny getValue(IloAnyVar var) const;
	IloBool isBound(IloAnyVar var) const;
	IloBool isInDomain(IloAnyVar var, IloAny value) const;
	void setValue(IloAnyVar var, IloAny value);
	void setDomain(IloAnyVar var, IloAnySet domain);

	
	void add(IloIntSetVar var);
	void add(IloIntSetVarArray array);
	IloBool isBound(IloIntSetVar var) const;
	IloBool isPossible(IloIntSetVar var, IloInt value) const;
	IloBool isRequired(IloIntSetVar var, IloInt value) const;
	IloSavedIntSetVarI *getElement(IloIntSetVar var) const;
	IloIntSet getPossibleSet(IloIntSetVar v) const;
	IloIntSet getRequiredSet(IloIntSetVar v) const;
	void setPossibleSet(IloIntSetVar v, IloIntSet possible);
	void setPossibleSet(IloIntSetVar v, IloNumSet possible);
	void setRequiredSet(IloIntSetVar v, IloIntSet required);
	void setRequiredSet(IloIntSetVar v, IloNumSet required);

	
	void add(IloAnySetVar v);
	void add(IloAnySetVarArray array);
	IloBool isBound(IloAnySetVar var) const;
	IloBool isPossible(IloAnySetVar var, IloAny value) const;
	IloBool isRequired(IloAnySetVar var, IloAny value) const;
	IloSavedIntSetVarI * getElement(IloAnySetVar var) const;
	IloAnySet getPossibleSet(IloAnySetVar v) const;
	IloAnySet getRequiredSet(IloAnySetVar v) const;
	void setPossibleSet(IloAnySetVar v, IloAnySet possible);
	void setRequiredSet(IloAnySetVar v, IloAnySet required);

	
	
	
	void add(IloConstraint con);

	class Iterator {
	private:
		IloSolutionI            *_solution;
		IloSolutionElementListI *_current;

	public:
		Iterator() : _current(0) { }
		Iterator(IloSolutionI *solution);
		IloBool ok() const { return _current != 0; }
		IloExtractable operator * () const {
			return _current ? _current->getElement()->getExtractable()
				: IloExtractable();
		}
		IloSolutionElementI* getElement() const {
			return _current ? _current->getElement() : 0;
		}
		Iterator& operator++() {
			if (_current) _current = _current->getNext();
			return *this;
		}
		void remove() { _solution->remove(_current); }
	};
};

class IloTypedSolutionIteratorBase {
protected:
	IloTypeInfo               _type;
	IloSolutionElementListI * _curr;

	IloBool typeMatch(const IloExtractableI * ex) const {
		return ex != 0 && ex->isType(_type);
	}
public:
	IloTypedSolutionIteratorBase(IloSolutionI *solution, IloTypeInfo ti);
	IloBool ok() const { return _curr != 0; }
	IloSolutionElement getElement() const {
		return _curr->getElement();
	}
	IloSolutionElementI *getElementI() const {
		return _curr->getElement();
	}
	IloExtractableI *getExtractableI() const {
		return (IloExtractableI *)_curr->getElement()->getExtractableI();
	}
	void advance() {
		while (_curr != 0) {
			if (typeMatch(_curr->getElement()->getExtractableI()))
				break;
			_curr = _curr->getNext();
		}
	}
	void operator ++ () {
		if (_curr != 0) {
			_curr = _curr->getNext();
			advance();
		}
	}
};

class IloSolution {
protected:
	IloSolutionI *_impl;

public:
	typedef IloSolutionI ImplClass;
	

	class Exception : public IloException {
	public: Exception(const char *s) : IloException(s) { }
	};
	class NotPresentException : public Exception {
	private:
		IloSolutionI    *_soln;
		IloExtractableI *_ex;
	public:
		NotPresentException(IloSolution s, IloExtractable ex);
		IloSolution getSolution() const { return _soln; }
		IloExtractable getExtractable() const { return (IloExtractableI *)_ex; }
	};
	class AlreadyPresentException : public Exception {
	private:
		IloSolutionI    *_soln;
		IloExtractableI *_ex;
	public:
		AlreadyPresentException(IloSolution s, IloExtractable ex);
		IloSolution getSolution() const { return _soln; }
		IloExtractable getExtractable() const { return _ex; }
	};
	class OperationNotSupportedException : public Exception {
	private:
		IloAlgorithmI *_alg;
		const char    *_opName;
		const char    *_exName;
	public:
		OperationNotSupportedException(IloAlgorithm a,
			const char *opName,
			const char *exName = 0);
		IloAlgorithm getAlgorithm() const { return _alg; }
		const char *getOperationName() const { return _opName; }
		const char *getExtractableName() const { return _exName; }
		void print(ILOSTD(ostream) &) const ILO_OVERRIDE;
	};

	
	
	IloSolution() : _impl(0) { }
	
	IloSolution(IloSolutionI *impl) : _impl(impl) { }
	
	IloSolution(const IloSolution& solution) : _impl(solution._impl) { }
	
	IloSolutionI *getImpl() const { return _impl; }
	
	void operator=(const IloSolution& solution) { _impl = solution._impl; }

	

	IloSolution(IloEnv mem, const char* name = 0);
	IloSolution(IloMemoryManager mem, const char* name = 0);

	void add(IloSolutionElement s) const {
		assert(_impl != 0);
		_impl->add(s.getImpl());
	}
	
	IloBool contains(IloExtractable extr) const {
		assert(_impl != 0);
		return _impl->contains(extr.getImpl());
	}
	
	void copy(IloSolution solution) const {
		assert(_impl != 0);
		assert(solution._impl != 0);
		_impl->copy(solution.getImpl());
	}
	void display(ILOSTD(ostream)& stream) const {
		assert(_impl != 0);
		_impl->display(stream);
	}
	
	IloSolution makeClone(IloEnv env) const {
		assert(_impl != 0);
		assert(env.getImpl() != 0);
		return _impl->makeClone(env.getImpl());
	}
	IloSolution makeClone(IloMemoryManager env) const {
		assert(_impl != 0);
		assert(env.getImpl() != 0);
		return _impl->makeClone(env.getImpl());
	}
	void makeMTSafe() const {
		assert(_impl != 0);
		_impl->makeMTSafe();
	}
	
	void end() {
		delete _impl;
		_impl = 0;
	}
	IloSolutionElement getElement(IloExtractable extr) const {
		assert(_impl != 0);
		return _impl->getElement(extr.getImpl());
	}
	
	IloEnv getEnv() const {
		assert(_impl != 0);
		return _impl->getEnv();
	}
	IloMemoryManager getMemoryManager() const {
		assert(_impl != 0);
		return _impl->getMemoryManager();
	}
	IloInt getNumberOfElements() const {
		assert(_impl != 0);
		return _impl->getNbOfElements();
	}
	IloInt getNbOfElements() const { return getNumberOfElements(); }
	IloNum getPrecision() const {
		assert(_impl != 0);
		return _impl->getPrecision();
	}
	IloAnd getConstraint() const;
	
	IloBool isBetterThan(IloSolution solution) const {
		assert(_impl != 0);
		assert(solution._impl != 0);
		return _impl->isBetterThan(solution.getImpl());
	}
	
	IloBool isEquivalent(IloSolution solution) const {
		assert(_impl != 0);
		assert(solution._impl != 0);
		return _impl->isEquivalent(solution._impl);
	}
	IloBool isFeasible(IloAlgorithm a) const {
		assert(_impl != 0);
		return _impl->isFeasible(a.getImpl());
	}
	
	IloBool isRestorable(IloExtractable extr) const {
		assert(_impl != 0);
		assert(extr.getImpl() != 0);
		return _impl->isRestorable(extr.getImpl());
	}
	
	IloBool isWorseThan(IloSolution solution) const {
		assert(_impl != 0);
		assert(solution._impl != 0);
		return _impl->isWorseThan(solution.getImpl());
	}
	
	void remove(IloExtractable extr) const {
		assert(_impl != 0);
		assert(extr.getImpl() != 0);
		_impl->remove(extr.getImpl());
	}
	
	void remove(IloExtractableArray extr) const {
		assert(_impl != 0);
		assert(extr.getImpl() != 0);
		_impl->remove(extr);
	}
	
	void restore(IloAlgorithm algorithm) const {
		assert(_impl != 0);
		_impl->restore(algorithm);
	}
	void restore(IloAlgorithm algorithm, IloSolution exclude) const {
		assert(_impl != 0);
		_impl->restore(algorithm, exclude._impl);
	}
	
	void store(IloAlgorithm algorithm) const {
		assert(_impl != 0);
		_impl->store(algorithm);
	}
	void setPrecision(IloNum prec) const {
		assert(_impl != 0);
		_impl->setPrecision(prec);
	}
	
	void setNonRestorable(IloExtractable extr) const {
		assert(_impl != 0);
		assert(extr.getImpl() != 0);
		_impl->setNonRestorable(extr.getImpl());
	}
	
	void setNonRestorable(IloExtractableArray array) const {
		assert(_impl != 0);
		assert(array.getImpl() != 0);
		_impl->setNonRestorable(array);
	}
	
	void setRestorable(IloExtractable ex) const {
		assert(_impl != 0);
		assert(ex.getImpl() != 0);
		_impl->setRestorable(ex.getImpl());
	}
	
	void setRestorable(IloExtractableArray array) const {
		assert(_impl != 0);
		assert(array.getImpl() != 0);
		_impl->setRestorable(array);
	}

	
	
	
	IloSolutionManagerI *getAllOverrides() const {
		assert(_impl != 0);
		return _impl->getAllOverrides();
	}
	IloSolutionOperationsI *getOverrides(IloExtractable ex) const {
		assert(_impl != 0);
		return _impl->getOverrides(ex.getImpl());
	}
	void useOverridesFrom(IloSolution s) const {
		assert(_impl != 0);
		assert(s.getImpl() != 0);
		_impl->useOverridesFrom(s.getImpl());
	}
	void copyOverridesFrom(IloSolution s) const {
		assert(_impl != 0);
		assert(s.getImpl() != 0);
		_impl->copyOverridesFrom(s.getImpl());
	}
	IloBool hasOverrides() const {
		assert(_impl != 0);
		return _impl->hasOverrides();
	}
	void setOverrides(IloSolutionOperationsI *ops) const {
		assert(_impl != 0);
		_impl->setOverrides(ops);
	}
	void removeOverrides(IloTypeIndex ti) const {
		assert(_impl != 0);
		_impl->removeOverrides(ti);
	}
	void removeOverrides() const {
		assert(_impl != 0);
		_impl->removeOverrides();
	}
	
	
	
	
	void copy(IloExtractable extr, IloSolution solution) const {
		assert(_impl != 0);
		_impl->copy(extr.getImpl(), solution.getImpl());
	}
	void display(IloExtractable extr, ILOSTD(ostream)& stream) const {
		assert(_impl != 0);
		_impl->display(extr.getImpl(), stream);
	}
	IloConstraint getConstraint(IloExtractable extr) const {
		assert(_impl != 0);
		return _impl->getConstraint(extr.getImpl());
	}
	
	IloBool isEquivalent(IloExtractable extr, IloSolution solution) const {
		assert(_impl != 0);
		return _impl->isEquivalent(extr.getImpl(), solution.getImpl());
	}
	
	void store(IloExtractable extr, IloAlgorithm algorithm) const {
		assert(_impl != 0);
		_impl->store(extr.getImpl(), algorithm);
	}
	
	void restore(IloExtractable extr, IloAlgorithm algorithm) const {
		assert(_impl != 0);
		_impl->restore(extr.getImpl(), algorithm);
	}
	IloBool isFeasible(IloExtractable extr, IloAlgorithm alg) const {
		assert(_impl != 0);
		return _impl->isFeasible(extr.getImpl(), alg);
	}

	
	
	

	
	void add(IloObjective objective) const {
		assert(_impl != 0);
		_impl->add(objective);
	}
	
	IloNum getObjectiveValue() const {
		assert(_impl != 0);
		return _impl->getObjectiveValue();
	}
	void setObjectiveValue(IloNum val) const { 
		assert(_impl != 0);
		_impl->setValue(_impl->getObjective(), val);
	}
	
	IloNumVar getObjectiveVar() const {
		assert(_impl != 0);
		return _impl->getObjectiveVar();
	}
	
	IloObjective getObjective() const {
		assert(_impl != 0);
		return _impl->getObjective();
	}
	void storeObjective(IloAlgorithm alg) const { 
		store(getObjective(), alg);
	}
	
	IloNum getValue(IloObjective obj) const {
		assert(_impl != 0);
		return _impl->getValue(obj);
	}
	
	IloBool isObjectiveSet() const {
		assert(_impl != 0);
		return _impl->isObjectiveSet();
	}
	void setObjective() const { 
		assert(_impl != 0);
		_impl->unsetObjective();
	}
	
	void unsetObjective() const {
		assert(_impl != 0);
		_impl->unsetObjective();
	}
	
	void setObjective(IloObjective objective) const {
		assert(_impl != 0);
		_impl->setObjective(objective);
	}
	
	void setValue(IloObjective objective, IloNum value) const {
		assert(_impl != 0);
		_impl->setValue(objective, value);
	}

	
	
	
#ifndef CPPREF_CPO
	
#else
	
#endif
	void add(IloNumVar var) const {
		assert(_impl != 0);
		_impl->add(var);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	void add(IloNumVarArray a) const {
		assert(_impl != 0);
		assert(a.getImpl() != 0);
		_impl->add(a);
	}
	IloNumSet getDomain(IloNumVar v) const {
		assert(_impl != 0);
		return _impl->getDomain(v);
	}
	IloInt getNumberOfDomainElements(IloNumVar v) const {
		assert(_impl != 0);
		return _impl->getNumberOfDomainElements(v);
	}
	IloSavedNumVar getElement(IloNumVar v) const {
		assert(_impl != 0);
		return _impl->getElement(v);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	IloBool isBound(IloNumVar var) const {
		assert(_impl != 0);
		return _impl->isBound(var);
	}
	IloBool isFixed(IloNumVar var) const {
		assert(_impl != 0);
		return _impl->isBound(var);
	}
	IloBool isInDomain(IloNumVar var, IloNum value) const {
		assert(_impl != 0);
		return _impl->isInDomain(var, value);
	}
	IloBool isInDomain(IloNumVar var, IloInt value) const {
		return isInDomain(var, (IloNum)value);
	}
	IloBool isInDomain(IloNumVar var, int value) const {
		return isInDomain(var, (IloNum)value);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	IloNum getMin(IloNumVar var) const {
		assert(_impl != 0);
		return _impl->getMin(var);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	IloNum getMax(IloNumVar var) const {
		assert(_impl != 0);
		return _impl->getMax(var);
	}

#ifndef CPPREF_CPO
	
#else
	
#endif
	IloNum getValue(IloNumVar var) const {
		assert(_impl != 0);
		return _impl->getValue(var);
	}
	void setDomain(IloNumVar var, IloNumSet domain) const {
		assert(_impl != 0);
		_impl->setDomain(var, domain);
	}
	void setDomain(IloNumVar var, IloNum min, IloNum max) const {
		assert(_impl != 0);
		_impl->setDomain(var, min, max);
	}
	void setDomain(IloNumVar var, IloInt min, IloInt max) const {
		setDomain(var, IloNum(min), IloNum(max));
	}
	void setDomain(IloNumVar var, int min, int max) const {
		setDomain(var, IloNum(min), IloNum(max));
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	void setMin(IloNumVar var, IloNum min) const {
		assert(_impl != 0);
		_impl->setMin(var, min);
	}
	void setMin(IloNumVar var, IloInt min) const { setMin(var, IloNum(min)); }
	void setMin(IloNumVar var, int min) const { setMin(var, IloNum(min)); }
#ifndef CPPREF_CPO
	
#else
	
#endif
	void setMax(IloNumVar var, IloNum max) const {
		assert(_impl != 0);
		_impl->setMax(var, max);
	}
	void setMax(IloNumVar var, IloInt max) const { setMax(var, IloNum(max)); }
	void setMax(IloNumVar var, int max) const { setMax(var, IloNum(max)); }
#ifndef CPPREF_CPO
	
#else
	
#endif
	void setValue(IloNumVar var, IloNum value) const {
		assert(_impl != 0);
		_impl->setValue(var, value);
	}
	void setValue(IloNumVar var, IloInt val) const { setValue(var, IloNum(val)); }
	void setValue(IloNumVar var, int val) const { setValue(var, IloNum(val)); }

	
	
	

	
	void add(IloIntVar var) const {
		assert(_impl != 0);
		_impl->add(var);
	}
	
	void add(IloIntVarArray a) const {
		assert(_impl != 0);
		assert(a.getImpl() != 0);
		_impl->add(a);
	}
	IloIntSet getDomain(IloIntVar v) const {
		assert(_impl != 0);
		return _impl->getDomain(v);
	}
	IloInt getNumberOfDomainElements(IloIntVar v) const {
		assert(_impl != 0);
		return _impl->getNumberOfDomainElements(v);
	}
	IloSavedIntVar getElement(IloIntVar v) const {
		assert(_impl != 0);
		return _impl->getElement(v);
	}
	IloBool isBound(IloIntVar var) const {
		assert(_impl != 0);
		return _impl->isBound(var);
	}
	
	IloBool isFixed(IloIntVar var) const {
		assert(_impl != 0);
		return _impl->isBound(var);
	}
	
	IloBool isInDomain(IloIntVar var, IloInt value) const {
		assert(_impl != 0);
		return _impl->isInDomain(var, value);
	}
	IloBool isInDomain(IloIntVar var, int value) const {
		return isInDomain(var, IloInt(value));
	}
	void setDomain(IloIntVar var, IloIntSet domain) const {
		assert(_impl != 0);
		_impl->setDomain(var, domain);
	}
	
	void setDomain(IloIntVar var, IloInt min, IloInt max) const {
		assert(_impl != 0);
		_impl->setDomain(var, min, max);
	}
	
	void setMin(IloIntVar var, IloInt min) const {
		assert(_impl != 0);
		_impl->setMin(var, min);
	}
	
	void setMin(IloIntVar var, int min) const { setMin(var, IloInt(min)); }
	
	void setMax(IloIntVar var, IloInt max) const {
		assert(_impl != 0);
		_impl->setMax(var, max);
	}

	
	void setMax(IloIntVar var, int max) const { setMax(var, IloInt(max)); }
	
	void setValue(IloIntVar var, IloInt value) const {
		assert(_impl != 0);
		_impl->setValue(var, value);
	}
	void setValue(IloIntVar var, int value) const {
		setValue(var, IloInt(value));
	}
	
	IloInt getMin(IloIntVar var) const {
		assert(_impl != 0);
		return (IloInt)_impl->getMin(var);
	}
	
	IloInt getMax(IloIntVar var) const {
		assert(_impl != 0);
		return (IloInt)_impl->getMax(var);
	}

	
	IloInt getValue(IloIntVar var) const {
		assert(_impl != 0);
		return (IloInt)_impl->getValue(var);
	}

	
	
	
	IloBool getValue(IloBoolVar var) const {
		assert(_impl != 0);
		if (!_impl->isBound(var))
			throw Exception("Boolean variable not bound in solution");
		return (IloBool)_impl->getValue(var);
	}
	
	void setFalse(IloBoolVar var) const {
		assert(_impl != 0);
		_impl->setValue(var, 0);
	}
	
	void setTrue(IloBoolVar var) const {
		assert(_impl != 0);
		_impl->setValue(var, 1);
	}

	
	
	

#ifndef CPPREF_CPO
	
#else
	
#endif
	void add(IloAnyVar var) const {
		assert(_impl != 0);
		_impl->add(var);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	void add(IloAnyVarArray a) const {
		assert(_impl != 0);
		assert(a.getImpl() != 0);
		_impl->add(a);
	}
	IloSavedAnyVar getElement(IloAnyVar v) const {
		assert(_impl != 0);
		return _impl->getElement(v);
	}
	IloAnySet getDomain(IloAnyVar var) const {
		assert(_impl != 0);
		return _impl->getDomain(var);
	}
	IloInt getNumberOfDomainElements(IloAnyVar var) const {
		assert(_impl != 0);
		return _impl->getNumberOfDomainElements(var);
	}

#ifndef CPPREF_CPO
	
#else
	
#endif
	IloAny getValue(IloAnyVar var) const {
		assert(_impl != 0);
		return _impl->getValue(var);
	}
	IloBool isBound(IloAnyVar var) const {
		assert(_impl != 0);
		return _impl->isBound(var);
	}
	IloBool isInDomain(IloAnyVar var, IloAny value) const {
		assert(_impl != 0);
		return _impl->isInDomain(var, value);
	}
	void setDomain(IloAnyVar var, IloAnySet domain) const {
		assert(_impl != 0);
		_impl->setDomain(var, domain);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	void setValue(IloAnyVar var, IloAny value) const {
		assert(_impl != 0);
		_impl->setValue(var, value);
	}

	
	
	
	void add(IloIntSetVar var) const {
		assert(_impl != 0);
		_impl->add(var);
	}
	void add(IloIntSetVarArray a) const {
		assert(_impl != 0);
		assert(a.getImpl() != 0);
		_impl->add(a);
	}
	IloBool isBound(IloIntSetVar var) const {
		assert(_impl != 0);
		return _impl->isBound(var);
	}
	IloBool isPossible(IloIntSetVar var, IloInt value) const {
		assert(_impl != 0);
		return _impl->isPossible(var, value);
	}
	IloBool isRequired(IloIntSetVar var, IloInt value) const {
		assert(_impl != 0);
		return _impl->isRequired(var, value);
	}
	IloIntSet getPossibleSet(IloIntSetVar var) const {
		assert(_impl != 0);
		return _impl->getPossibleSet(var);
	}
	IloIntSet getRequiredSet(IloIntSetVar var) const {
		assert(_impl != 0);
		return _impl->getRequiredSet(var);
	}
	void setPossibleSet(IloIntSetVar var, IloIntSet possible) const {
		assert(_impl != 0);
		_impl->setPossibleSet(var, possible);
	}
	void setPossibleSet(IloIntSetVar var, IloNumSet possible) const {
		assert(_impl != 0);
		_impl->setPossibleSet(var, possible);
	}
	void setRequiredSet(IloIntSetVar var, IloIntSet required) const {
		assert(_impl != 0);
		_impl->setRequiredSet(var, required);
	}
	void setRequiredSet(IloIntSetVar var, IloNumSet required) const {
		assert(_impl != 0);
		_impl->setRequiredSet(var, required);
	}

	
	
	
#ifndef CPPREF_CPO
	
#else
	
#endif
	void add(IloAnySetVar var) const {
		assert(_impl != 0);
		_impl->add(var);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	void add(IloAnySetVarArray a) const {
		assert(_impl != 0);
		assert(a.getImpl() != 0);
		_impl->add(a);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	IloBool isBound(IloAnySetVar var) const {
		assert(_impl != 0);
		return _impl->isBound(var);
	}
	IloBool isPossible(IloAnySetVar var, IloAny value) const {
		assert(_impl != 0);
		return _impl->isPossible(var, value);
	}
	IloBool isRequired(IloAnySetVar var, IloAny value) const {
		assert(_impl != 0);
		return _impl->isRequired(var, value);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	IloAnySet getPossibleSet(IloAnySetVar var) const {
		assert(_impl != 0);
		return _impl->getPossibleSet(var);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	IloAnySet getRequiredSet(IloAnySetVar var) const {
		assert(_impl != 0);
		return _impl->getRequiredSet(var);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	void setPossibleSet(IloAnySetVar var, IloAnySet possible) const {
		assert(_impl != 0);
		_impl->setPossibleSet(var, possible);
	}
#ifndef CPPREF_CPO
	
#else
	
#endif
	void setRequiredSet(IloAnySetVar var, IloAnySet required) const {
		assert(_impl != 0);
		_impl->setRequiredSet(var, required);
	}

	
	
	

public:

	
	void add(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_add(a.getImpl(), RestoreAll);
	}

	
	void add(const IloIntervalVarArray a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVarArray: empty handle.");
		_add(a, RestoreAll);
	}

	
	IloBool isPresent(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _isPresent(a);
	}

	
	IloBool isAbsent(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _isAbsent(a);
	}

	
	IloInt getStart(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getStart(a);
	}

	
	IloInt getStartMin(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getStartMin(a);
	}

	
	IloInt getStartMax(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getStartMax(a);
	}

	
	IloInt getEnd(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getEnd(a);
	}

	
	IloInt getEndMin(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getEndMin(a);
	}

	
	IloInt getEndMax(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getEndMax(a);
	}

	
	IloInt getLength(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getLength(a);
	}

	
	IloInt getLengthMin(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getLengthMin(a);
	}

	
	IloInt getLengthMax(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getLengthMax(a);
	}

	
	IloInt getSize(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getSize(a);
	}

	
	IloInt getSizeMin(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getSizeMin(a);
	}

	
	IloInt getSizeMax(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(contains(a), "IloSolution: interval not stored.");
		return _getSizeMax(a);
	}

	
	void setPresent(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setPresent(a);
	}

	
	void setAbsent(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setAbsent(a);
	}

	
	void setOptional(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setOptional(a);
	}

	
	void setStart(const IloIntervalVar a, IloInt v) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(v >= IloIntervalMin, "IloSolution: invalid value.");
		IloAssert(v <= IloIntervalMax, "IloSolution: invalid value.");
		_setStart(a, v);
	}

	
	void setStartMin(const IloIntervalVar a, IloInt min) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(min >= IloIntervalMin, "IloSolution: invalid value.");
		IloAssert(min <= IloIntervalMax, "IloSolution: invalid value.");
		_setStartMin(a, min);
	}

	
	void setStartMax(const IloIntervalVar a, IloInt max) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(max >= IloIntervalMin, "IloSolution: invalid value.");
		IloAssert(max <= IloIntervalMax, "IloSolution: invalid value.");
		_setStartMax(a, max);
	}

	
	void setEnd(const IloIntervalVar a, IloInt v) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(v >= IloIntervalMin, "IloSolution: invalid value.");
		IloAssert(v <= IloIntervalMax, "IloSolution: invalid value.");
		_setEnd(a, v);
	}

	
	void setEndMin(const IloIntervalVar a, IloInt min) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(min >= IloIntervalMin, "IloSolution: invalid value.");
		IloAssert(min <= IloIntervalMax, "IloSolution: invalid value.");
		_setEndMin(a, min);
	}

	
	void setEndMax(const IloIntervalVar a, IloInt max) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(max >= IloIntervalMin, "IloSolution: invalid value.");
		IloAssert(max <= IloIntervalMax, "IloSolution: invalid value.");
		_setEndMax(a, max);
	}

	
	void setLength(const IloIntervalVar a, IloInt v) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setLength(a, v);
	}

	
	void setLengthMin(const IloIntervalVar a, IloInt min) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setLengthMin(a, min);
	}

	
	void setLengthMax(const IloIntervalVar a, IloInt max) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setLengthMax(a, max);
	}

	
	void setSize(const IloIntervalVar a, IloInt v) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setSize(a, v);
	}

	
	void setSizeMin(const IloIntervalVar a, IloInt min) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setSizeMin(a, min);
	}

	
	void setSizeMax(const IloIntervalVar a, IloInt max) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_setSizeMax(a, max);
	}

private:
	friend class IloSavedIntervalI;
	friend class IloSavedSequenceI;
	friend class IloSavedAtomI;

public:
	enum RestoreFields {
		RestoreNothing     = ((IloUInt)0),
		RestorePresence    = ((IloUInt)1) << 0,
		RestoreStart       = ((IloUInt)1) << 1,
		RestoreEnd         = ((IloUInt)1) << 2,
		RestoreLength      = ((IloUInt)1) << 3,
		RestoreSize        = ((IloUInt)1) << 4,
		RestoreBefore      = ((IloUInt)1) << 5,
		RestoreAfter       = ((IloUInt)1) << 6,
		RestorePrevious    = ((IloUInt)1) << 7,
		RestoreNext        = ((IloUInt)1) << 8,
		RestoreAlternative = ((IloUInt)1) << 9,
		RestoreLevel       = ((IloUInt)1) << 10,
		RestoreState       = ((IloUInt)1) << 11,
		RestoreAll         = (((IloUInt)1) << 12) - 1
	};
	void add(const IloIntervalVar a, IloUInt restoreFields) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloIntervalVar.");
		_add(a.getImpl(), restoreFields);
	}
	void add(const IloIntervalVarArray a, IloUInt restoreFields) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVarArray: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloIntervalVar.");
		_add(a, restoreFields);
	}
	void add(const IloAlternative alt) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(alt.getImpl(), "IloAlternative: empty handle.");
		_add(alt.getImpl());
	}
	void add(const IloSpan span) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(span.getImpl(), "IloSpan: empty handle.");
		_add(span.getImpl());
	}
	void add(const IloIntervalSequenceVar s, IloUInt restoreFields = RestoreAll) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(s.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloIntervalSequenceVar.");
		_add(s.getImpl(), restoreFields);
	}
	void add(const IloIntervalSequenceVarArray s, IloUInt restoreFields = RestoreAll) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(s.getImpl(), "IloIntervalSequenceVarArray: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloIntervalSequenceVarArray.");
		_add(s, restoreFields);
	}
	void add(const IloStateFunction f, IloUInt restoreFields = RestoreAll) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloStateFunction.");
		_add(f.getImpl(), restoreFields);
	}
	void setRestorable(const IloIntervalVar a, IloUInt restoreFields) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloIntervalVar.");
		_setRestorable(a, restoreFields);
	}
	void setRestorable(const IloIntervalSequenceVar s, IloUInt restoreFields) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(s.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloIntervalSequenceVar.");
		_setRestorable(s, restoreFields);
	}
	void setRestorable(const IloStateFunction f, IloUInt restoreFields) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloStateFunction.");
		_setRestorable(f, restoreFields);
	}
	void setRestorable(const IloIntervalSequenceVar s, const IloIntervalVar a, IloUInt restoreFields) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(s.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloIntervalSequenceVar.");
		_setRestorable(s, a, restoreFields);
	}
	IloUInt getRestorable(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		return _getRestorable(a);
	}
	IloUInt getRestorable(const IloIntervalSequenceVar s) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(s.getImpl(), "IloIntervalSequenceVar: empty handle.");
		return _getRestorable(s);
	}
	IloUInt getRestorable(const IloIntervalSequenceVar s, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(s.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		return _getRestorable(s, a);
	}
	IloUInt getRestorable(const IloStateFunction f) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
		return _getRestorable(f);
	}
	void mark(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_mark(a);
	}
	void unmark(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_unmark(a);
	}
	IloBool isMarked(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		return _isMarked(a);
        }
	void storePrecedence(const IloIntervalVar a, const IloIntervalVar b) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
		_storePrecedence(a, b);
	}
	void removeIncomingPrecedences(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_removeIncomingPrecedences(a);
	}
	void removeOutgoingPrecedences(const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_removeOutgoingPrecedences(a);
	}
	
	IloIntervalVar getSelected(const IloAlternative alt) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(alt.getImpl(), "IloAlternative: empty handle.");
		for (IloAlternative::IntervalIterator iter(alt); iter.ok(); ++iter) {
			IloAssert(contains(*iter), "IloSolution: missing alternative interval.");
			if (isPresent(*iter))
				return *iter;
		}
		return 0;
	}
	void setSelected(const IloAlternative alt, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(alt.getImpl(), "IloAlternative: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		IloAssert(alt.contains(a), "IloSolution: interval is not member of alternative.");
		IloAssert(contains(alt.getAlternativeInterval()), "IloSolution: alternative is not stored.");
		if (isPresent(alt.getAlternativeInterval())) {
			for (IloAlternative::IntervalIterator iter(alt); iter.ok(); ++iter) {
				IloAssert(contains(*iter), "IloSolution: missing alternative interval.");
				if ((*iter).getImpl() != a.getImpl())
					setAbsent(*iter);
			}
			setPresent(a);
		}
	}
	void unsetSelected(const IloAlternative alt) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(alt.getImpl(), "IloAlternative: empty handle.");
		IloAssert(contains(alt.getAlternativeInterval()), "IloSolution: alternative is not stored.");
		if (isPresent(alt.getAlternativeInterval()))
			for (IloAlternative::IntervalIterator iter(alt); iter.ok(); ++iter) {
				IloAssert(contains(*iter), "IloSolution: missing alternative interval.");
				setOptional(*iter);
			}
	}
	
	IloIntervalVar getFirst(const IloIntervalSequenceVar seq) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		return _getFirst(seq);
	}
	IloIntervalVar getLast(const IloIntervalSequenceVar seq) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		return _getLast(seq);
	}
	IloIntervalVar getNext(const IloIntervalSequenceVar seq, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		return _getNext(seq, a);
	}
	IloIntervalVar getPrevious(const IloIntervalSequenceVar seq, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		return _getPrevious(seq, a);
	}
	void setFirst(const IloIntervalSequenceVar seq, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		return _setFirst(seq, a);
	}
	void setLast(const IloIntervalSequenceVar seq, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		return _setLast(seq, a);
	}
	void storePrecedence(const IloIntervalSequenceVar seq,
		const IloIntervalVar a,
		const IloIntervalVar b,
		IloBool immediate = IloFalse) const {
			IloAssert(getImpl(), "IloSolution: empty handle.");
			IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
			IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
			IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
			_storePrecedence(seq, a, b, immediate);
	}
	void removeIncomingPrecedences(const IloIntervalSequenceVar seq, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_removeIncomingPrecedences(seq, a);
	}
	void removeOutgoingPrecedences(const IloIntervalSequenceVar seq, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_removeOutgoingPrecedences(seq, a);
	}
	void removePrecedences(const IloIntervalSequenceVar seq) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		_removePrecedences(seq);
	}
	void mark(const IloIntervalSequenceVar seq, const IloIntervalVar a) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
		IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
		_mark(seq, a);
	}

	class SequencePrecedenceIterator {
	private:
		IloAny* _curr;
		IloAny* _end;
		IloBool _outgoing;
	public:
		SequencePrecedenceIterator(const IloSolution sol,
			const IloIntervalSequenceVar seq,
			const IloIntervalVar a,
			IloBool outgoingPrecedences = IloTrue) {
				IloAssert(sol.getImpl(), "IloSolution: empty handle.");
				IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
				IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
				_ctor(sol, seq, a, outgoingPrecedences);
		}
		~SequencePrecedenceIterator(){}
		IloBool isPrevious() const;
		IloBool ok() const { return (_curr != _end); }
		IloSolution::SequencePrecedenceIterator& operator++() { _curr++; return *this; }
		IloIntervalVar operator*() const;
	private:
		void _ctor(const IloSolution sol,
			const IloIntervalSequenceVar seq,
			const IloIntervalVar a,
			IloBool outgoingPrecedences);
	};

	
	void add(const IloCumulFunctionExpr d, IloUInt restoreFields = RestoreAll) const;
	void add(const IloCumulFunctionExprArray d, IloUInt restoreFields = RestoreAll) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(d.getImpl(), "IloCumulFunctionExprArray: empty handle.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloCumulFunctionExprArray.");
		_add(d, restoreFields);
	}
	void setRestorable(const IloCumulFunctionExpr d, IloUInt restoreFields) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(d.getImpl(), "IloCumulFunctionExpr: empty handle.");
		IloAssert(d.isElementary(), "IloCumulFunctionExpr: non-elementary cumul function expression.");
		IloAssert(restoreFields <= RestoreAll, "IloSolution: invalid restore fields for IloCumulFunctionExpr.");
		_setRestorable(d, restoreFields);
	}
	IloUInt getRestorable(const IloCumulFunctionExpr d) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(d.getImpl(), "IloCumulFunctionExpr: empty handle.");
		IloAssert(d.isElementary(), "IloCumulFunctionExpr: non-elementary cumul function expression.");
		return _getRestorable(d);
	}
	IloInt getLevelMin(const IloCumulFunctionExpr d) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(d.getImpl(), "IloCumulFunctionExpr: empty handle.");
		IloAssert(d.isElementary(), "IloCumulFunctionExpr: non-elementary cumul function expression.");
		IloAssert(contains(d), "IloSolution: cumul atom not stored.");
		return _getLevelMin(d);
	}
	IloInt getLevelMax(const IloCumulFunctionExpr d) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(d.getImpl(), "IloCumulFunctionExpr: empty handle.");
		IloAssert(d.isElementary(), "IloCumulFunctionExpr: non-elementary cumul function expression.");
		IloAssert(contains(d), "IloSolution: cumul atom not stored.");
		return _getLevelMax(d);
	}
	void setLevelMin(const IloCumulFunctionExpr d, IloInt min) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(d.getImpl(), "IloCumulFunctionExpr: empty handle.");
		IloAssert(d.isElementary(), "IloCumulFunctionExpr: non-elementary cumul function expression.");
		IloAssert(contains(d), "IloSolution: cumul atom not stored.");
		IloAssert(min >= 0, "IloSolution: invalid level value.");
		_setLevelMin(d, min);
	}
	void setLevelMax(const IloCumulFunctionExpr d, IloInt max) const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		IloAssert(d.getImpl(), "IloCumulFunctionExpr: empty handle.");
		IloAssert(d.isElementary(), "IloCumulFunctionExpr: non-elementary cumul function expression.");
		IloAssert(contains(d), "IloSolution: cumul atom not stored.");
		IloAssert(max >= 0, "IloSolution: invalid level value.");
		_setLevelMax(d, max);
	}
	void relink() const {
		IloAssert(getImpl(), "IloSolution: empty handle.");
		_relink();
	}

	
	class IntervalIterator {
	private:
		IloSolutionI *               _sol;
		IloTypedSolutionIteratorBase _iter;
		IloBool                      _onlyPresent;
		void                         _advance();
	public:
		IntervalIterator(IloSolution sol, IloBool onlyPresent = IloTrue);
		IloBool ok() const {
			IloAssert(_sol, "IloSolution: empty handle.");
			return _iter.ok();
		}
		IntervalIterator& operator++() {
			IloAssert(_sol, "IloSolution: empty handle.");
			_advance();
			return *this;
		}
		IloIntervalVar operator*() const;
	};

	
	IloSolutionI* getSafeImpl() const;
	void add(IloSchedGlobalExtractableI* s, IloUInt restoreFields) const;
	void setRestorable(IloSchedGlobalExtractableI* s, const IloIntervalVar a, IloUInt restoreFields);
	void mark(IloSchedGlobalExtractableI* s, const IloIntervalVar x);

private:
	IloSavedIntervalI* _getAndAddInterval(const IloIntervalVar a) const;
	IloBool _isPresent    (const IloIntervalVar a) const;
	IloBool _isAbsent     (const IloIntervalVar a) const;
	IloInt  _getStart     (const IloIntervalVar a) const;
	IloInt  _getStartMin  (const IloIntervalVar a) const;
	IloInt  _getStartMax  (const IloIntervalVar a) const;
	IloInt  _getEnd       (const IloIntervalVar a) const;
	IloInt  _getEndMin    (const IloIntervalVar a) const;
	IloInt  _getEndMax    (const IloIntervalVar a) const;
	IloInt  _getLength    (const IloIntervalVar a) const;
	IloInt  _getLengthMin (const IloIntervalVar a) const;
	IloInt  _getLengthMax (const IloIntervalVar a) const;
	IloInt  _getSize      (const IloIntervalVar a) const;
	IloInt  _getSizeMin   (const IloIntervalVar a) const;
	IloInt  _getSizeMax   (const IloIntervalVar a) const;
	void _setPresent   (const IloIntervalVar a) const;
	void _setAbsent    (const IloIntervalVar a) const;
	void _setOptional  (const IloIntervalVar a) const;
	void _setStart     (const IloIntervalVar a, IloInt v) const;
	void _setStartMin  (const IloIntervalVar a, IloInt min) const;
	void _setStartMax  (const IloIntervalVar a, IloInt max) const;
	void _setEnd       (const IloIntervalVar a, IloInt v) const;
	void _setEndMin    (const IloIntervalVar a, IloInt min) const;
	void _setEndMax    (const IloIntervalVar a, IloInt max) const;
	void _setLength    (const IloIntervalVar a, IloInt v) const;
	void _setLengthMin (const IloIntervalVar a, IloInt min) const;
	void _setLengthMax (const IloIntervalVar a, IloInt min) const;
	void _setSize      (const IloIntervalVar a, IloInt v) const;
	void _setSizeMin   (const IloIntervalVar a, IloInt max) const;
	void _setSizeMax   (const IloIntervalVar a, IloInt max) const;

	void _storePrecedence(const IloIntervalVar a, const IloIntervalVar b) const;
	void _removeIncomingPrecedences(const IloIntervalVar a) const;
	void _removeOutgoingPrecedences(const IloIntervalVar a) const;
	void _add(IloIntervalVarI* ti, IloUInt restoreFields = RestoreAll) const;
	void _add(IloIntervalVarArray ti, IloUInt restoreFields = RestoreAll) const;
	void _add(IloAlternativeI* alt) const;
	void _add(IloSpanI* span) const;
	void _add(IloIntervalSequenceVarI* s, IloUInt restoreFields = RestoreAll) const;
	void _add(IloIntervalSequenceVarArray s, IloUInt restoreFields = RestoreAll) const;
        void _add(IloStateFunctionI* f, IloUInt restoreFields = RestoreAll) const;
	void _setRestorable(const IloIntervalVar a, IloUInt restoreFields) const;
	void _setRestorable(const IloIntervalSequenceVar s, IloUInt restoreFields) const;
	void _setRestorable(const IloIntervalSequenceVar s, const IloIntervalVar a, IloUInt restoreFields) const;
	void _setRestorable(const IloStateFunction s, IloUInt restoreFields) const;
	void _mark(const IloIntervalVar a) const;
	void _unmark(const IloIntervalVar a) const;
	IloBool _isMarked(const IloIntervalVar a) const;
	IloUInt _getRestorable(const IloIntervalVar a) const;
	IloUInt _getRestorable(const IloIntervalSequenceVar s) const;
	IloUInt _getRestorable(const IloIntervalSequenceVar s, const IloIntervalVar a) const;
	IloUInt _getRestorable(const IloStateFunction s) const;
	IloIntervalVar _getFirst(const IloIntervalSequenceVar seq) const;
	IloIntervalVar _getLast(const IloIntervalSequenceVar seq) const;
	IloIntervalVar _getNext(const IloIntervalSequenceVar seq, const IloIntervalVar a) const;
	IloIntervalVar _getPrevious(const IloIntervalSequenceVar seq, const IloIntervalVar a) const;
	void _setFirst(const IloIntervalSequenceVar seq, const IloIntervalVar a) const;
	void _setLast(const IloIntervalSequenceVar seq, const IloIntervalVar a) const;
	void _storePrecedence(const IloIntervalSequenceVar seq,
		const IloIntervalVar a,
		const IloIntervalVar b,
		IloBool immediate) const;
	void _removeIncomingPrecedences(const IloIntervalSequenceVar seq, const IloIntervalVar a) const;
	void _removeOutgoingPrecedences(const IloIntervalSequenceVar seq, const IloIntervalVar a) const;
	void _removePrecedences(const IloIntervalSequenceVar seq) const;
	void _mark(const IloIntervalSequenceVar seq, const IloIntervalVar a) const;
	void _add(IloCumulAtomI* d, IloUInt restoreFields = RestoreAll) const;
	void _add(IloCumulFunctionExprArray d, IloUInt restoreFields = RestoreAll) const;
	void _setRestorable(const IloCumulFunctionExpr d, IloUInt restoreFields) const;
	IloUInt _getRestorable(const IloCumulFunctionExpr d) const;
	IloInt _getLevelMin(const IloCumulFunctionExpr d) const;
	IloInt _getLevelMax(const IloCumulFunctionExpr d) const;
	void _setLevelMin(const IloCumulFunctionExpr d, IloInt min) const;
	void _setLevelMax(const IloCumulFunctionExpr d, IloInt max) const;
	void _relink() const;
	IloSavedIntervalI* _getSavedIntervalI(IloIntervalVarI* a) const;
	IloSavedSequenceI* _getSavedSequenceI(IloIntervalSequenceVarI* s) const;
	IloSavedAtomI* _getSavedAtomI(IloCumulAtomI* d) const;

public:
	
	
	
	void add(IloConstraint con) const {
		assert(_impl != 0);
		_impl->add(con);
	}

	
	
	
	
	class Iterator {
	private:
		IloSolutionI::Iterator _iter;
	public:
		
		Iterator(IloSolution solution);
		
		IloBool ok() const { return _iter.ok(); }
		
		IloExtractable operator*() const { return *_iter; }
		IloSolutionElement getElement() const { return _iter.getElement(); }
		
		Iterator& operator++() { ++_iter; return *this; }
		void remove() { _iter.remove(); }
	};
	ILOEXTENSIONH(IloSolution)
};

template <class E>
class IloSolutionIterator : public IloTypedSolutionIteratorBase {
protected:
	typedef typename E::ImplClass EImplClass;
public:
	
	IloSolutionIterator(IloSolution s)
		: IloTypedSolutionIteratorBase(s.getImpl(), EImplClass::GetTypeInfo()) {
			advance();
	}
	
	E operator * () const { return (EImplClass *)getExtractableI(); }
#ifdef CPPREF_GENERATION
	
	void operator ++();
#endif
};

template <class A, class SE, class OR>
class IloSolutionOverrideOperationsI : public IloSolutionOperationsI {
private:
	const OR * const _opClass;
public:
	IloSolutionOverrideOperationsI(IloMemoryManagerI *env, IloTypeIndex t, const OR *ov) :
	  IloSolutionOperationsI(env, t), _opClass(ov) { }
	  IloSolutionOverrideOperationsI(IloEnvI *env, IloTypeIndex t, const OR *ov) :
	  IloSolutionOperationsI(env->getGeneralAllocator(), t), _opClass(ov) { }
	  void storeAux(IloAlgorithmI *a, IloSolutionElementI *e) const {
		  _opClass->store((A *)a, (SE *)e);
	  }
	  void restoreAux(IloAlgorithmI *a, const IloSolutionElementI *e) const {
		  _opClass->restore((A *)a, (const SE *)e);
	  }
	  IloBool isFeasibleAux(IloAlgorithmI *a, const IloSolutionElementI *e) const {
		  return _opClass->isFeasible((A *)a, (const SE *)e);
	  }
	  IloSolutionOperationsI *makeClone(IloMemoryManagerI* env) const {
		  return new (env)
			  IloSolutionOverrideOperationsI(env, getTypeIndex(), _opClass);
	  }
};

template <class A, class SE, class OR>
IloSolutionOperationsI *IloSolutionOverridesOperationsMatch(
	IloMemoryManagerI *env,
	IloTypeIndex t,
	const OR *ov,
	void (OR::*)(A *, SE *) const,
	void (OR::*)(A *, const SE *) const,
	IloBool (OR::*)(A *, const SE *) const
	) {
		IloSolutionOperationsI *ops = new (env)
			IloSolutionOverrideOperationsI<A, SE, OR>(env, t, ov);
		return ops;
}

template <class E, class SE>
class IloSolutionElementIterator : public IloSolutionIterator<E> {
public:
	IloSolutionElementIterator(IloSolution s) : IloSolutionIterator<E>(s) { }
	SE * getElement() const { return (SE *)this->getElementI(); }
};

#define ILOSOLUTIONITERSPECIAL(BASET, T, SE) \
	template <> class IloSolutionIterator<T> \
	: public IloTypedSolutionIteratorBase { \
	protected: \
	void advance() { \
	while (_curr != 0) { \
	SE * sv = (SE *)_curr->getElement(); \
	if (typeMatch(sv->getExtractableI()) && sv->name2(is, BASET)()) \
	break; \
	_curr = _curr->getNext(); \
	} \
} \
	public: \
	IloSolutionIterator(IloSolution s) \
	: IloTypedSolutionIteratorBase(s.getImpl(), name2(T, I)::GetTypeInfo()) { \
	advance(); \
} \
	T operator * () const { return (name2(T, I) *)getExtractableI(); } \
	void operator ++ () { \
	if (_curr != 0) { \
	_curr = _curr->getNext(); \
	advance(); \
	} \
} \
}; \
	template <> class IloSolutionElementIterator<T, SE> \
	: public IloSolutionIterator<T> { \
	public: \
	IloSolutionElementIterator(IloSolution s) : IloSolutionIterator<T>(s) { } \
	SE * getElement() const { return (SE *) getElementI(); } \
};

ILOSOLUTIONITERSPECIAL(Int, IloIntVar, IloSavedIntVarI)
ILOSOLUTIONITERSPECIAL(Any, IloAnyVar, IloSavedAnyVarI)

typedef IloSimpleArray<IloSolution> IloSolutionArray;

class IloSolutionManip {
private:
	IloSolutionElementI *_element;
public:
	
	IloSolutionManip(IloSolution solution, IloExtractable extr);
	void display(ILOSTD(ostream)& stream) const;
};

ILOSTD(ostream)& operator << (ILOSTD(ostream)& stream,
							  const IloSolution& solution);

ILOSTD(ostream)& operator << (ILOSTD(ostream)& stream,
							  const IloSolutionManip& fragment);

ILOSTD(ostream)& operator << (ILOSTD(ostream)& stream,
							  const IloSolutionElement& element);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
