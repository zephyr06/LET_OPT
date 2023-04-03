// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloextractable.h
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
#ifndef __CONCERT_iloextractableH
#define __CONCERT_iloextractableH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/ilolinkedlist.h>

class IloExtractableVisitor;
class IloParameterExtractableLinkI;

class ILO_EXPORTED IloExtractableI : public IloRttiEnvObjectI {
	ILORTTIDECL

		IloInt   _id;
private:

	friend class IloEnvI;
	friend class IloRecycleBinI;
	friend class IloAlgorithmI;
	friend class IloSolverI;
	friend class IloCPExtractorI;
	friend class IlcAddExtractableChange;
	friend IloAny IloCPExtractIloModelI(IloAlgorithm alg, IloModelI* obj);
	void setEnv(IloEnvI* env)   { _env = env; }
	void setId   (IloInt id)      { _id = id; }

	friend class IloCloneManager;
	friend class IloDeleterI;
	virtual IloExtractableI* makeClone(IloEnvI*) const;

	virtual IloAny extractToCP(const IloAlgorithm alg) const=0;
public:
	virtual void unextractFromCP(const IloAlgorithm alg, IloAny ptr) const=0;
private:
	virtual void modifyCP(const IloAlgorithm alg, IloAny ptr) const=0;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0);

public:
	IloExtractableI(IloEnvI* env, const char* name=0);
	virtual ~IloExtractableI();

	void notify(const IloChange&);
	virtual void visitSubExtractables(IloExtractableVisitor* v)=0;

	IloInt   getId() const { return (_id); }

	IloBool  mustRemove() const {
		return _env->mustRemove(_id);
	}

	void removeFromAll();
	
	virtual IloBool isRefCountedExpr() const;
	virtual void end();
	virtual IloInt expandTo(IloModelI* m, const char* propertyName=0, const char* propertyValue=0) const;

	void markRemove() {
		_env->markRemove(_id);
	}
	void unmarkRemove() {
		_env->unmarkRemove(_id, this);
	}

	void operator delete(void* obj, size_t size);
#if defined (ILODELETEOPERATOR)
	void operator delete(void*, const IloEnvI*) {}
	void operator delete(void*, const IloEnv&) {}
#endif

	virtual void display(ILOSTD(ostream)& out) const;
	virtual void whenAddedToCP(const IloAlgorithm alg, IloAny ptr) const;
	virtual void whenRemovedFromCP(const IloAlgorithm alg, IloAny ptr) const;

	
	virtual void changeParameter(const IloParameterExtractableLinkI* ) {}

	
	
	virtual void endParameter(const IloParameterExtractableLinkI* ) {}

private:
	friend class IloFindNongroundTerm;
        
	virtual IloBool hasNongroundType() const;
public:
	virtual IloBool isGround() const;
	virtual IloBool isDecisionExpr() const;

public:
	ILOEXTENSIONI
public:
	virtual IloBool isAggregate() const { return IloFalse; }
	void setNameNoNotif(const char * name);

	virtual IloBool isGenerator() {
		return IloFalse;
	}
	virtual IloBool isComprehension() {
		return IloFalse;
	}
	virtual IloBool isAnyExpr() {
		return IloFalse;
	}
	virtual IloBool isNumExpr() {
		return IloFalse;
	}
	virtual IloBool isNumVar() {
		return IloFalse;
	}
	virtual IloBool isDExpr() {
		return IloFalse;
	}
	virtual IloBool isConstraint() {
		return IloFalse;
	}
	virtual IloBool isModel() {
		return IloFalse;
	}
	virtual void resetExprUsed() { }
	virtual IloInt getExprUsed() const { return -1; }
};

class IloExtractableVisitor {
public:
	
	IloExtractableVisitor() {}
	virtual ~IloExtractableVisitor();
	
	virtual void beginVisit(IloExtractableI* e);

	
	virtual void visitChildren(IloExtractableI* parent, IloExtractableI* child);

	
	virtual void endVisit(IloExtractableI* e);

	
	virtual void visitChildren(IloExtractableI* parent, IloExtractableArray children);
        virtual IloBool stop() const; 
};

class IloChangeVisitor {
public:
	IloChangeVisitor() {}
	virtual ~IloChangeVisitor();
	virtual void addSubExtractable(IloExtractableI* p, IloExtractableI* c);
	virtual void removeSubExtractable(IloExtractableI* p, IloExtractableI* c);
	void addSubExtractable(IloExtractableI* p, IloExtractableArray a);
	void removeSubExtractable(IloExtractableI* p, IloExtractableArray a);
};

class ILO_EXPORTED IloExtractable {
protected:
	IloExtractableI* _impl;
	friend class IloObjective;
public:
	
	IloExtractable(IloExtractableI* obj=0) : _impl(obj)   {}
	IloExtractable(const IloExtractable& obj) : _impl(obj.getImpl())   {}

	typedef IloExtractableI ImplClass;

	
	IloExtractableI* getImpl() const { return (_impl); }

	
	IloEnv getEnv() const {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		return (getImpl()->getEnv());
	}
	
	IloInt getId() const {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		return (_impl->getId());
	}

	
	void end() {
		if ( _impl ) {
			_impl->end();
			_impl = 0;
		}
	}

        
	void removeFromAll() {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		getImpl()->removeFromAll();
	}

	IloBool mustRemove() const {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		return (getImpl()->mustRemove());
	}

	void replaceByClone(IloEnvI* env) {
		if (_impl) _impl = env->getClone(_impl);
	}

	IloExtractable getClone() const {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		return (getEnv().getImpl())->getClone(getImpl());
	}
	void setClone(IloExtractable clone) const {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		getEnv().getImpl()->setClone(getImpl(), clone.getImpl());
	}
	IloExtractable getClone(const IloEnv env) const {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		return (env.getImpl() ? env.getImpl() : getEnv().getImpl())->getClone(getImpl());
	}
	void setClone(IloExtractable clone, const IloEnv env) const {
		IloAssert (getImpl(), "accessing IloExtractable through 0 handle");
		(env.getImpl() ? env.getImpl() : getEnv().getImpl())->setClone(getImpl(), clone.getImpl());
	}
        IloExtractable& operator=(const IloExtractable& other) {
          _impl = other._impl;
          return *this;
        }

	class NoCloneMechanismException : public IloException {
		const IloExtractableI* _from;
	public:
		NoCloneMechanismException(const IloExtractableI* from);
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
		const IloExtractableI* getExtractable() const { return (_from); }
	};

	
        IloBool isValid() const {
          return (_impl != 0);
        }

	
	IloBool isVariable() const;
	
	IloBool isModel() const;
	
	IloBool isObjective() const;
	
	IloBool isConstraint() const;
	
	IloBool isIntExpr() const;
	
	IloBool isNumExpr() const;

	
	IloNumVar asVariable() const;
	
	IloModel asModel() const;
	
	IloObjective asObjective() const;
	
	IloConstraint asConstraint() const;
	
	IloIntExprArg asIntExpr() const;
	
	IloNumExprArg asNumExpr() const;

	ILOEXTENSIONH(IloExtractable)
};

inline ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloExtractable& ext) {
	if (ext.getImpl()) ext.getImpl()->display(out);
	return (out);
}

inline void IloEndExtractable(IloExtractableI* ext) {
	if (ext->getEnv()->useMemory())
		ext->end();
}

#ifndef ILONOLOCATIONS

#  define ILOSETLOCATION(extractable) \
     extractable.setLocation(__FILE__, __LINE__)
#else
#  define ILOSETLOCATION(extractable)
#endif

typedef IloArray<IloExtractable> IloExtractableArrayBase;

class ILO_EXPORTED IloExtractableArray : public IloExtractableArrayBase {
public:
	
	IloExtractableArray(IloDefaultArrayI* i=0) : IloExtractableArrayBase(i) {}
	
	IloExtractableArray(const IloExtractableArray& r)
		: IloExtractableArrayBase(r) {}
	
	IloExtractableArray(const IloEnv env, IloInt n = 0);
	IloExtractableArray(const IloMemoryManager env, IloInt n = 0);

	IloExtractableArray(const IloEnv env, IloInt n, const IloExtractable v0);
#ifdef ILOALLOWVARARGS
	
	IloExtractableArray(const IloEnv env, IloInt n, const IloExtractable v0,
		const IloExtractable v1 ...);
#else 
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2,
		const IloExtractable v3);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2,
		const IloExtractable v3, const IloExtractable v4);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2,
		const IloExtractable v3, const IloExtractable v4, const IloExtractable v5);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2,
		const IloExtractable v3, const IloExtractable v4, const IloExtractable v5,
		const IloExtractable v6);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2,
		const IloExtractable v3, const IloExtractable v4, const IloExtractable v5,
		const IloExtractable v6, const IloExtractable v7);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2,
		const IloExtractable v3, const IloExtractable v4, const IloExtractable v5,
		const IloExtractable v6, const IloExtractable v7, const IloExtractable v8);
	IloExtractableArray(const IloEnv env, IloInt n,
		const IloExtractable v0, const IloExtractable v1, const IloExtractable v2,
		const IloExtractable v3, const IloExtractable v4, const IloExtractable v5,
		const IloExtractable v6, const IloExtractable v7, const IloExtractable v8,
		const IloExtractable v9);
#endif

	
	void add(const IloExtractableArray x);
	
	void add(const IloExtractable x);
	
	void add(IloInt more, const IloExtractable x);

	IloInt find(const IloExtractable x) const;

        
	void removeFromAll() {
		if (getImpl()) {
			getImpl()->getEnv()->removeFromAll(*this);
		}
	}

	
	void endElements();

	void replaceByClone(IloEnvI* env);
	void removeMarked();
        IloExtractableArray& operator=(const IloExtractableArray& other) {
          setImpl(other.getImpl());
          return *this;
        }

	
	void setNames( const char* name );
};

#define IloROExtractableArray IloExtractableArray

ILOARRAY(IloExtractableArray,IloExtractableArray2)
ILOARRAY(IloExtractableArray2,IloExtractableArray3)
ILOARRAY(IloExtractableArray3,IloExtractableArray4)

typedef IloLinkedList<IloExtractableI> IloExtractableListBase;

class IloExtractableList : public IloLinkedList<IloExtractableI> {
public:
	using IloLinkedList<IloExtractableI>::add;
	using IloLinkedList<IloExtractableI>::remove;
	IloExtractableList() : IloExtractableListBase()  {}

	IloExtractableI* firstExtractable() const {
		return (IloExtractableI*)firstObject();
	}

	void    add   (const IloExtractableArray);
	void    remove(const IloExtractableArray, IloExtractableArray=0);
};

#if !defined(__CONCERT_iloalgH)
#include <ilconcert/iloalg.h>
#endif

typedef IloAny (*IloExtractToCP)(const IloAlgorithm alg,
								 const IloExtractableI* ext);
typedef void (*IloUnextractFromCP)(const IloAlgorithm alg,
								   const IloExtractableI* ext,
								   IloAny ptr);
typedef void (*IloModifyCP)(const IloAlgorithm alg,
							const IloExtractableI* ext,
							IloAny ptr);
typedef void (*IloAddToCP)(const IloAlgorithm alg,
						   const IloExtractableI* ext,
						   IloAny ptr);
typedef void (*IloRemoveFromCP)(const IloAlgorithm alg,
								const IloExtractableI* ext,
								IloAny ptr);

#define ILOEXTRDECL \
	ILORTTIDECL\
	virtual IloAny extractToCP(const IloAlgorithm alg) const ILO_OVERRIDE;\
	virtual void unextractFromCP(const IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE;\
	virtual void modifyCP(const IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE; \
	static IloExtractToCP _extractCP;\
	static IloUnextractFromCP _unextractCP;\
	static IloModifyCP _modifyCP;

#define ILOEXTR(_this,_base)\
	ILORTTI(_this,_base)\
	IloAny _this::extractToCP(const IloAlgorithm alg) const {\
	if (_this::_extractCP == 0) throw IloAlgorithm::ExtractFailure();\
	return (*_this::_extractCP)(alg, this);\
}\
	void _this::unextractFromCP(const IloAlgorithm alg, IloAny ptr) const {\
	if (_this::_unextractCP == 0) throw IloAlgorithm::RemoveFailure();\
	(_this::_unextractCP)(alg, this, ptr);\
}\
	void _this::modifyCP(const IloAlgorithm alg, IloAny ptr) const {\
	if (_this::_modifyCP == 0) throw IloAlgorithm::RemoveFailure();\
	(_this::_modifyCP)(alg, this, ptr);\
}\
	IloExtractToCP _this::_extractCP = 0;\
	IloUnextractFromCP _this::_unextractCP = 0;\
	IloModifyCP _this::_modifyCP = 0;

#define ILOEXTRHANDLE(Hname, RHname)\
public: \
	typedef name2(Hname,I) ImplClass; \
	\
	Hname():RHname(){} \
	\
	Hname(name2(Hname,I)* impl):RHname((RHname::ImplClass*)(void*)impl){} \
	\
	name2(Hname,I)* getImpl() const { \
	return (ImplClass*)(void*)_impl; \
} \
	Hname getClone() const { \
	return (ImplClass*)(void*)getEnv().getImpl()->getClone(_impl); \
} \
	void setClone(Hname clone) const { \
	(getEnv().getImpl())->setClone(_impl, clone._impl); \
} \
	Hname getClone(const IloEnv env) const { \
	return (ImplClass*)(void*)(env.getImpl() ? env.getImpl() : getEnv().getImpl())->getClone(_impl); \
} \
	void setClone(Hname clone, const IloEnv env) const { \
	(env.getImpl() ? env.getImpl() : getEnv().getImpl())->setClone(_impl, clone._impl); \
} \
private:

#define ILOEXTRHANDLEDECL(Hname, RHname)\
public: \
	typedef name2(Hname,I) ImplClass; \
	\
	Hname():RHname(){} \
	\
	Hname(name2(Hname,I)* impl);\
	\
	name2(Hname,I)* getImpl() const { \
	return (ImplClass*)(void*)_impl; \
} \
	Hname getClone() const ; \
	void setClone(Hname clone) const; \
	Hname getClone(const IloEnv env) const; \
	void setClone(Hname clone, const IloEnv env) const; \
private:

#define ILOEXTRHANDLEIMPL(Hname, RHname)\
	Hname::Hname(ImplClass* impl):RHname(impl){} \
	Hname Hname::getClone() const { \
	return (ImplClass*)getEnv().getImpl()->getClone(_impl);\
} \
	void Hname::setClone(Hname clone) const { \
	getEnv().getImpl()->setClone(_impl, clone._impl); \
} \
	Hname Hname::getClone(const IloEnv env) const { \
	return (ImplClass*)env.getImpl()->getClone(_impl); \
} \
	void Hname::setClone(Hname clone, const IloEnv env) const { \
	env.getImpl()->setClone(_impl, clone._impl); \
}

template<class X>
inline X IloGetClone(IloEnvI* env, const X x) {
	IloAssert(env, "Using empty IloEnv handle.");
	X clone = x;
	clone.replaceByClone(env);
	return clone;
}
template<class X>
inline X IloGetClone(const IloEnv env, const X x) {
	return IloGetClone(env.getImpl(), x);
}

inline void IloReplaceByClone(const IloExtractable* ext, IloEnvI* env) {
	*(IloExtractable*)ext = env->getClone(ext->getImpl());
}
inline void IloReplaceByClone(const IloExtractableArray* ext, IloEnvI* env) {
	((IloExtractableArray*)ext)->replaceByClone(env);
}

inline void IloReplaceByClone(const IloNumArray* ext, IloEnvI* env) {
	IloNumArray ar(env);
	ar.add(*ext);
	*((IloNumArray*)ext) = ar;
}
inline void IloReplaceByClone(const IloIntArray* ext, IloEnvI* env) {
	IloIntArray ar(env);
	ar.add(*ext);
	*((IloIntArray*)ext) = ar;
}

void IloReplaceByClone(const IloAnyArray* ext, IloEnvI* env);
void IloReplaceByClone(const IloNumSet* set, IloEnvI* env);

void IloReplaceByClone(const IloAnySet* set, IloEnvI* env);

inline void IloReplaceByClone(const void*, IloEnvI*) {
}

#define ILOEXTRMAKECLONEDECL \
	virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

#define ILOEXTRMAKECLONE0(_this)	\
	IloExtractableI* _this::makeClone(IloEnvI* env) const {\
	IloExtractableI* clone = new (env) _this(env, (const char*)0); \
	return clone;					\
}

#define ILOEXTRMAKECLONE1(_this,type1,arg1)\
	IloExtractableI* _this::makeClone(IloEnvI* env) const {\
	type1 targ1 = arg1;				\
	IloReplaceByClone(&targ1, env);	\
	IloExtractableI* clone = new (env) _this(env, targ1, (const char*)0);\
	return clone;					\
}

#define ILOEXTRMAKECLONE2(_this,type1,arg1,type2,arg2)\
	IloExtractableI* _this::makeClone(IloEnvI* env) const {\
	type1 targ1 = arg1;				\
	IloReplaceByClone(&targ1, env);	\
	type2 targ2 = arg2;				\
	IloReplaceByClone(&targ2, env);	\
	IloExtractableI* clone = new (env) _this(env, targ1, targ2, (const char*)0);\
	return clone;					\
}

#define ILOEXTRMAKECLONE3(_this,type1,arg1,type2,arg2,type3,arg3)\
	IloExtractableI* _this::makeClone(IloEnvI* env) const {\
	type1 targ1 = arg1;				\
	IloReplaceByClone(&targ1, env);	\
	type2 targ2 = arg2;				\
	IloReplaceByClone(&targ2, env);	\
	type3 targ3 = arg3;				\
	IloReplaceByClone(&targ3, env);	\
	IloExtractableI* clone = new (env) _this(env, targ1, targ2, targ3, (const char*)0);\
	return clone;					\
}

#define ILOEXTRMAKECLONE4(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4)\
	IloExtractableI* _this::makeClone(IloEnvI* env) const {\
	type1 targ1 = arg1;\
	IloReplaceByClone(&targ1, env);\
	type2 targ2 = arg2;\
	IloReplaceByClone(&targ2, env);\
	type3 targ3 = arg3;\
	IloReplaceByClone(&targ3, env);\
	type4 targ4 = arg4;\
	IloReplaceByClone(&targ4, env);\
	IloExtractableI* clone = new (env) _this(env, targ1, targ2, targ3, targ4, (const char*)0);\
	return clone;					\
}

#define ILOEXTRMAKECLONE5(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)\
	IloExtractableI* _this::makeClone(IloEnvI* env) const {\
	type1 targ1 = arg1;\
	IloReplaceByClone(&targ1, env);\
	type2 targ2 = arg2;\
	IloReplaceByClone(&targ2, env);\
	type3 targ3 = arg3;\
	IloReplaceByClone(&targ3, env);\
	type4 targ4 = arg4;\
	IloReplaceByClone(&targ4, env);\
	type5 targ5 = arg5;\
	IloReplaceByClone(&targ5, env);\
	IloExtractableI* clone = new (env) _this(env, targ1, targ2, targ3, targ4, targ5, (const char*)0);\
	return clone;					\
}

#define ILOEXTRMAKECLONE6(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6)\
	IloExtractableI* _this::makeClone(IloEnvI* env) const {\
	type1 targ1 = arg1;\
	IloReplaceByClone(&targ1, env);\
	type2 targ2 = arg2;\
	IloReplaceByClone(&targ2, env);\
	type3 targ3 = arg3;\
	IloReplaceByClone(&targ3, env);\
	type4 targ4 = arg4;\
	IloReplaceByClone(&targ4, env);\
	type5 targ5 = arg5;\
	IloReplaceByClone(&targ5, env);\
	type6 targ6 = arg6;\
	IloReplaceByClone(&targ6, env);\
	IloExtractableI* clone = new (env) _this(env, targ1, targ2, targ3, targ4, targ5, targ6, (const char*)0);\
	return clone;					\
}

#define ILOAUXDISPLAYMEMBER(type,arg) \
	out << "  " << #arg << " " << arg << ILOSTD(endl);

#define ILOAUXDISPLAY(_this) \
	out << # _this << " ("; \
	if (getName()) \
	out << getName(); \
  else \
  out << getId(); \
  out << ")" << ILOSTD(endl);

#define ILOEXTRDISPLAYDECL \
	virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

#define ILOEXTRDISPLAY0(_this)  \
	void _this::display(ILOSTD(ostream)& out) const { \
	ILOAUXDISPLAY(_this) \
}

#define ILOEXTRDISPLAY1(_this, \
	type1,arg1) \
	void _this::display(ILOSTD(ostream)& out) const { \
	ILOAUXDISPLAY(_this) \
	ILOAUXDISPLAYMEMBER(type1,arg1) \
}

#define ILOEXTRDISPLAY2(_this, \
	type1,arg1, \
	type2,arg2) \
	void _this::display(ILOSTD(ostream)& out) const { \
	ILOAUXDISPLAY(_this) \
	ILOAUXDISPLAYMEMBER(type1,arg1) \
	ILOAUXDISPLAYMEMBER(type2,arg2) \
}

#define ILOEXTRDISPLAY3(_this, \
	type1,arg1, \
	type2,arg2, \
	type3,arg3) \
	void _this::display(ILOSTD(ostream)& out) const { \
	ILOAUXDISPLAY(_this)\
	ILOAUXDISPLAYMEMBER(type1,arg1)\
	ILOAUXDISPLAYMEMBER(type2,arg2)\
	ILOAUXDISPLAYMEMBER(type3,arg3)\
}

#define ILOEXTRDISPLAY4(_this, \
	type1,arg1, \
	type2,arg2, \
	type3,arg3, \
	type4,arg4) \
	void _this::display(ILOSTD(ostream)& out) const { \
	ILOAUXDISPLAY(_this)\
	ILOAUXDISPLAYMEMBER(type1,arg1)\
	ILOAUXDISPLAYMEMBER(type2,arg2)\
	ILOAUXDISPLAYMEMBER(type3,arg3)\
	ILOAUXDISPLAYMEMBER(type4,arg4)\
}

#define ILOEXTRDISPLAY5(_this, \
	type1,arg1, \
	type2,arg2, \
	type3,arg3, \
	type4,arg4, \
	type5,arg5) \
	void _this::display(ILOSTD(ostream)& out) const { \
	ILOAUXDISPLAY(_this)\
	ILOAUXDISPLAYMEMBER(type1,arg1)\
	ILOAUXDISPLAYMEMBER(type2,arg2)\
	ILOAUXDISPLAYMEMBER(type3,arg3)\
	ILOAUXDISPLAYMEMBER(type4,arg4)\
	ILOAUXDISPLAYMEMBER(type5,arg5)\
}

#define ILOEXTRDISPLAY6(_this, \
	type1,arg1, \
	type2,arg2, \
	type3,arg3, \
	type4,arg4, \
	type5,arg5, \
	type6,arg6) \
	void _this::display(ILOSTD(ostream)& out) const { \
	ILOAUXDISPLAY(_this)\
	ILOAUXDISPLAYMEMBER(type1,arg1)\
	ILOAUXDISPLAYMEMBER(type2,arg2)\
	ILOAUXDISPLAYMEMBER(type3,arg3)\
	ILOAUXDISPLAYMEMBER(type4,arg4)\
	ILOAUXDISPLAYMEMBER(type5,arg5)\
	ILOAUXDISPLAYMEMBER(type6,arg6)\
}

#define ILOEXTRCONSTRUCTOR0DECL(_this,_base) \
	_this(IloEnvI*, const char*);

#define ILOEXTRCONSTRUCTOR0(_this,_base) \
	_this::_this(IloEnvI* env, const char* name) \
	: _base(env, name) {}

#define ILOEXTRCONSTRUCTOR1DECL(_this,_base,type1,arg1) \
	_this(IloEnvI*, type1, const char*);

#define ILOEXTRCONSTRUCTOR1(_this,_base,type1,arg1) \
	_this::_this(IloEnvI* env, \
	type1 name2(T,arg1), \
	const char* name) \
	: _base(env, name), \
	arg1((type1)name2(T,arg1)) {}

#define ILOEXTRCONSTRUCTOR2DECL(_this,_base,type1,arg1,type2,arg2) \
	_this(IloEnvI*, type1, type2, const char*);

#define ILOEXTRCONSTRUCTOR2(_this,_base,type1,arg1,type2,arg2) \
	_this::_this(IloEnvI* env, \
	type1 name2(T,arg1), \
	type2 name2(T,arg2), \
	const char* name) \
	: _base(env, name), \
	arg1((type1)name2(T,arg1)), \
	arg2((type2)name2(T,arg2))  {}

#define ILOEXTRCONSTRUCTOR3DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3) \
	_this(IloEnvI*, type1, type2, type3, const char*);

#define ILOEXTRCONSTRUCTOR3(_this,_base,type1,arg1,type2,arg2,type3,arg3) \
	_this::_this(IloEnvI* env, \
	type1 name2(T,arg1), \
	type2 name2(T,arg2), \
	type3 name2(T,arg3), \
	const char* name) \
	: _base(env, name), \
	arg1((type1)name2(T,arg1)), \
	arg2((type2)name2(T,arg2)), \
	arg3((type3)name2(T,arg3))  {}

#define ILOEXTRCONSTRUCTOR4DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
	_this(IloEnvI*, type1, type2, type3, type4, const char*);

#define ILOEXTRCONSTRUCTOR4(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
	_this::_this(IloEnvI* env, \
	type1 name2(T,arg1), \
	type2 name2(T,arg2), \
	type3 name2(T,arg3), \
	type4 name2(T,arg4), \
	const char* name) \
	: _base(env, name), \
	arg1((type1)name2(T,arg1)), \
	arg2((type2)name2(T,arg2)), \
	arg3((type3)name2(T,arg3)), \
	arg4((type4)name2(T,arg4))  {}

#define ILOEXTRCONSTRUCTOR5DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
	_this(IloEnvI*, type1, type2, type3, type4, type5, const char*);

#define ILOEXTRCONSTRUCTOR5(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
	_this::_this(IloEnvI* env, \
	type1 name2(T,arg1), \
	type2 name2(T,arg2), \
	type3 name2(T,arg3), \
	type4 name2(T,arg4), \
	type5 name2(T,arg5), \
	const char* name) \
	: _base(env, name), \
	arg1((type1)name2(T,arg1)), \
	arg2((type2)name2(T,arg2)), \
	arg3((type3)name2(T,arg3)), \
	arg4((type4)name2(T,arg4)), \
	arg5((type5)name2(T,arg5))  {}

#define ILOEXTRCONSTRUCTOR6DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
	_this(IloEnvI*, type1, type2, type3, type4, type5, type6, const char*);

#define ILOEXTRCONSTRUCTOR6(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
	_this::_this(IloEnvI* env, \
	type1 name2(T,arg1), \
	type2 name2(T,arg2), \
	type3 name2(T,arg3), \
	type4 name2(T,arg4), \
	type5 name2(T,arg5), \
	type6 name2(T,arg6), \
	const char* name) \
	: _base(env, name), \
	arg1((type1)name2(T,arg1)), \
	arg2((type2)name2(T,arg2)), \
	arg3((type3)name2(T,arg3)), \
	arg4((type4)name2(T,arg4)), \
	arg5((type5)name2(T,arg5)), \
	arg6((type6)name2(T,arg6))  {}

#define ILOEXTROTHERDECL \
	ILOEXTRMAKECLONEDECL \
	ILOEXTRDISPLAYDECL

#define ILOEXTRMEMBERS0DECL(_this,_base)\
	ILOEXTRDECL \
public:\
	ILOEXTRCONSTRUCTOR0DECL(_this,_base) \
	ILOEXTROTHERDECL

#define ILOEXTRMEMBERS0(_this,_base)\
	ILOEXTR(_this,_base) \
	ILOEXTRCONSTRUCTOR0(_this,_base) \
	ILOEXTRMAKECLONE0(_this) \
	ILOEXTRDISPLAY0(_this)

#define ILOEXTRMEMBERS1DECL(_this, _base, \
	type1, arg1) \
	ILOEXTRDECL \
	type1 arg1;\
public:\
	ILOEXTRCONSTRUCTOR1DECL(_this,_base,type1,arg1) \
	ILOEXTROTHERDECL

#define ILOEXTRMEMBERS1(_this, _base, \
	type1, arg1) \
	ILOEXTR(_this,_base) \
	ILOEXTRCONSTRUCTOR1(_this,_base,type1,arg1) \
	ILOEXTRMAKECLONE1(_this,type1,arg1) \
	ILOEXTRDISPLAY1(_this,type1,arg1)

#define ILOEXTRMEMBERS2DECL(_this, _base, \
	type1, arg1, \
	type2, arg2) \
	ILOEXTRDECL \
	type1 arg1;\
	type2 arg2;\
public:\
	ILOEXTRCONSTRUCTOR2DECL(_this,_base,type1,arg1,type2,arg2) \
	ILOEXTROTHERDECL

#define ILOEXTRMEMBERS2(_this, _base, \
	type1, arg1, \
	type2, arg2) \
	ILOEXTR(_this, _base) \
	ILOEXTRCONSTRUCTOR2(_this,_base,type1,arg1,type2,arg2) \
	ILOEXTRMAKECLONE2(_this,type1,arg1,type2,arg2) \
	ILOEXTRDISPLAY2(_this,type1,arg1,type2,arg2)

#define ILOEXTRMEMBERS3DECL(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3) \
	ILOEXTRDECL \
	type1 arg1;\
	type2 arg2;\
	type3 arg3;\
public:\
	ILOEXTRCONSTRUCTOR3DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3) \
	ILOEXTROTHERDECL

#define ILOEXTRMEMBERS3(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3) \
	ILOEXTR(_this,_base) \
	ILOEXTRCONSTRUCTOR3(_this,_base,type1,arg1,type2,arg2,type3,arg3) \
	ILOEXTRMAKECLONE3(_this,type1,arg1,type2,arg2,type3,arg3) \
	ILOEXTRDISPLAY3(_this,type1,arg1,type2,arg2,type3,arg3)

#define ILOEXTRMEMBERS4DECL(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3, \
	type4, arg4) \
	ILOEXTRDECL \
	type1 arg1;\
	type2 arg2;\
	type3 arg3;\
	type4 arg4;\
public:\
	ILOEXTRCONSTRUCTOR4DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
	ILOEXTROTHERDECL

#define ILOEXTRMEMBERS4(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3, \
	type4, arg4) \
	ILOEXTR(_this,_base) \
	ILOEXTRCONSTRUCTOR4(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
	ILOEXTRMAKECLONE4(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
	ILOEXTRDISPLAY4(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4)

#define ILOEXTRMEMBERS5DECL(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3, \
	type4, arg4, \
	type5, arg5) \
	ILOEXTRDECL \
	type1 arg1;\
	type2 arg2;\
	type3 arg3;\
	type4 arg4;\
	type5 arg5;\
public:\
	ILOEXTRCONSTRUCTOR5DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
	ILOEXTROTHERDECL

#define ILOEXTRMEMBERS5(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3, \
	type4, arg4, \
	type5, arg5) \
	ILOEXTR(_this,_base) \
	ILOEXTRCONSTRUCTOR5(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
	ILOEXTRMAKECLONE5(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
	ILOEXTRDISPLAY5(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)
#define ILOEXTRMEMBERS6DECL(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3, \
	type4, arg4, \
	type5, arg5, \
	type6, arg6) \
	ILOEXTRDECL \
	type1 arg1;\
	type2 arg2;\
	type3 arg3;\
	type4 arg4;\
	type5 arg5;\
	type6 arg6;\
public:\
	ILOEXTRCONSTRUCTOR6DECL(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
	ILOEXTROTHERDECL

#define ILOEXTRMEMBERS6(_this, _base, \
	type1, arg1, \
	type2, arg2, \
	type3, arg3, \
	type4, arg4, \
	type5, arg5, \
	type6, arg6) \
	ILOEXTR(_this,_base) \
	ILOEXTRCONSTRUCTOR6(_this,_base,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
	ILOEXTRMAKECLONE6(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
	ILOEXTRDISPLAY6(_this,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6)

typedef void (*IloApplyToCP)(IloAlgorithm alg, const IloChange& change, IloAny ptr);

#define ILOCHANGEDECL ILORTTIDECL \
	virtual void visitChange(IloEnvI*, IloExtractableI*, IloChangeVisitor* v) const ILO_OVERRIDE;\
	virtual void applyToCP(IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE;\
	static IloApplyToCP _applyCP;

#define ILOCHANGEBASE(_this,_base) ILORTTI(_this, _base) \
	void _this::applyToCP(IloAlgorithm alg, IloAny ptr) const {\
	if (_applyCP == 0) throw IloAlgorithm::CannotChangeException(alg.getImpl());\
	(*_applyCP)(alg, *this, ptr);\
}\
	IloApplyToCP _this::_applyCP = 0;\

#define ILOCHANGE(_this,_base, visitor) ILOCHANGEBASE(_this, _base) \
	void _this::visitChange(IloEnvI* env, IloExtractableI* extractablei, \
	IloChangeVisitor* visitor) const

#define ILOCHANGEEXTR(_this,_base, visitor) ILOCHANGEBASE(_this, _base) \
	void _this::visitChange(IloEnvI*, IloExtractableI* extractablei, \
	IloChangeVisitor* visitor) const

#define ILOOPENCHANGE(_this,_base, visitor) ILOCHANGEBASE(_this, _base) \
	void _this::visitChange(IloEnvI* , IloExtractableI* , \
	IloChangeVisitor* visitor) const

#define ILOEMPTYCHANGE(_this) ILOCHANGEBASE(_this, IloChange) \
	void _this::visitChange(IloEnvI* , IloExtractableI*, \
	IloChangeVisitor*) const { }

class IloChange : public IloRtti {
	ILORTTIDECL
		virtual void visitChange(IloEnvI*, IloExtractableI*, IloChangeVisitor*) const=0;
	virtual void applyToCP(IloAlgorithm alg, IloAny ptr) const=0;
public:
	virtual ~IloChange();
	virtual IloBool isApplyingOnArray() const { return IloFalse; }
	virtual IloExtractableArray getChangedExtractableArray() const {
		throw IloWrongUsage("IloChange::getChangedExtractableArray can not be called");
		ILOUNREACHABLE(return 0;)
	}
	virtual const IloExtractableI* getChangedExtractable() const {
		throw IloWrongUsage("IloChange::getChangedExtractable can not be called");
		ILOUNREACHABLE(return 0;)
	}
#ifndef ILCBUILDWITHNAMESPACE  
	friend class IloSolverI;
	friend class IloCPExtractorI;
#else
	friend class CPOptimizer::IloSolverI;
	friend class CPOptimizer::IloCPExtractorI;
#endif
};

class IloSetNameArrayChange : public IloChange {
	ILOCHANGEDECL
		IloExtractableArray _ex;
public:
	IloSetNameArrayChange(IloExtractableArray removed);
	virtual ~IloSetNameArrayChange();
	virtual IloBool isApplyingOnArray() const ILO_OVERRIDE { return IloTrue; }
	virtual IloExtractableArray getChangedExtractableArray() const ILO_OVERRIDE {
		return _ex;
	}
};

class IloSetNameChange : public IloChange {
	ILOCHANGEDECL
		IloExtractableI* _ex;
public:
	IloSetNameChange(IloExtractableI* destroyed);
	virtual ~IloSetNameChange();
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return _ex;
	}
};

class IloRemoveFromAllChange : public IloChange {
	ILOCHANGEDECL
		IloExtractableArray _removed;
public:
	IloRemoveFromAllChange(const IloExtractableArray removed);
	virtual ~IloRemoveFromAllChange();
	IloExtractableArray getRemoved() const {return _removed;}
	virtual IloBool isApplyingOnArray() const ILO_OVERRIDE { return IloTrue; }
	virtual IloExtractableArray getChangedExtractableArray() const ILO_OVERRIDE {
		return _removed;
	}
};

class IloDestroyExtractable : public IloChange {
	ILOCHANGEDECL
		IloExtractableI* _destroyed;
public:
	IloDestroyExtractable(IloExtractableI* destroyed);
	virtual ~IloDestroyExtractable();
	IloExtractableI* getDestroyed() const {return _destroyed;}
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getDestroyed();
	}
};

class IloDestroyExtractableArray : public IloChange {
	ILOCHANGEDECL
		IloExtractableArray _destroyed;
public:
	IloDestroyExtractableArray(IloExtractableArray destroyed);
	virtual ~IloDestroyExtractableArray();
	IloExtractableArray getDestroyed() const {return _destroyed;}
	virtual IloExtractableArray getChangedExtractableArray() const ILO_OVERRIDE {
		return getDestroyed();
	}
};

class IloAddExtractable : public IloChange {
	ILOCHANGEDECL
		IloAddExtractable*     _next;
	const IloExtractableI* _obj;

public:
	virtual ~IloAddExtractable();
	IloAddExtractable() : _next(0), _obj(0) {}
	IloAddExtractable(const IloAddExtractable&)
		: IloChange(), _next(0), _obj(0) {}

	virtual IloEnvI* getEnv() const { return (what()->getEnv()); }
	virtual IloAddExtractable* getCopy(IloEnvI* env) const = 0;
	virtual void destroy(IloEnvI* m) = 0;
	virtual const IloExtractableI* what() const = 0;
	virtual void apply() const = 0;

	virtual void setObj(const IloExtractableI* obj);

	const IloExtractableI* getObj()  const      { return (_obj); }
	virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
		return getObj();
	}
	IloAddExtractable*     getNext() const      { return (_next); }

	friend inline const IloAddExtractable& operator+ (const IloAddExtractable& ,
		const IloAddExtractable& );

	void install(IloExtractableI* obj) const;

	class CannotAddExtractableException : public IloException {
		const IloExtractableI* _obj;
		const IloExtractableI* _new;
	protected:
		CannotAddExtractableException(const IloExtractableI* obj,
			const IloExtractableI* newex,
			const char*            msg)
			: IloException(msg), _obj(obj), _new(newex)     {}
	public:
		CannotAddExtractableException(const IloExtractableI* obj,
			const IloExtractableI* newex)
			: IloException("Cannot add new IloExtractable to extractor")
			, _obj(obj), _new(newex)     {}
		const IloExtractableI* getNewExtractable() const   { return (_new); }
		const IloExtractableI* getAddToExtractable() const { return (_obj); }
	};
};

inline const IloAddExtractable& operator+ (const IloAddExtractable& one,
										   const IloAddExtractable& two) {
											   IloAssert (one.getEnv() == two.getEnv(),
												   "column elements must be from the same env");
											   IloAddExtractable* i = (IloAddExtractable*)&two;
											   while ( i->_next ) i = (i->_next);
											   i->_next = (IloAddExtractable*)&one;
											   return (two);
}

#define ILOCOLUMNDECL \
	ILOCHANGEDECL \
public: \
	IloAddExtractable* getCopy(IloEnvI* env) const ILO_OVERRIDE;\
	void destroy(IloEnvI* env) ILO_OVERRIDE;\
private:

#define ILOCOLUMNBASE(_this,_base) ILORTTI(_this,_base)  \
	IloAddExtractable* _this::getCopy(IloEnvI* env) const {\
	return (new (env) _this(*this));\
}\
	void _this::destroy(IloEnvI* env) {\
	this->~_this();\
	env->free((void*)this, sizeof(_this));\
}\
	void _this::applyToCP(IloAlgorithm alg, IloAny ptr) const {\
	if (_applyCP == 0) throw IloAlgorithm::CannotChangeException(alg.getImpl());\
	(*_applyCP)(alg, *this, ptr);\
}\
	IloApplyToCP _this::_applyCP = 0;

#define ILOCOLUMN(_this,_base, visitor) \
	ILOCOLUMNBASE(_this, _base) \
	void _this::visitChange(IloEnvI* env, IloExtractableI* extractablei, IloChangeVisitor* visitor) const

#define ILOOPENCOLUMN(_this,_base, visitor)\
	ILOCOLUMNBASE(_this, _base) \
	void _this::visitChange(IloEnvI*, IloExtractableI*, IloChangeVisitor* visitor) const

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

