// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloexpression.h
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

#ifndef __CONCERT_iloexpressionH
#define __CONCERT_iloexpressionH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloextractable.h>
#include <ilconcert/iloalg.h>

#undef ILOEXTRAINTVARCTOR

class IloNumLinExprTerm;
class IloIntLinExprTerm;
class IloNumLinExprTermI;
class IloIntLinExprTermI;
class IloNumQuadExprTermI;

class IloIntVar;
class IloNumVarI;
class IloIntVarI;
class IloNumLinTermI;
class IloIntLinTermI;
class IloIntExpr;
class IloNumExpr;
class IloIntExprArg;
class IloIntExprArray;
class IloNumExprArray;
class IloSemiContVarI;
class IloAddNumVar;
class IloNumColumnArray;
class IloSemiContI;
class IloIntIndex;

class IloVarDomainI;

class IloIntRangeI;
class IloNumRangeI;
class IloIntRange;
class IloNumRange;
class IloIntSet;
class IloNumSet;
class IloNumToNumSegmentFunction;
class IloNumToNumStepFunction;

#define IloExprBase IloNumExprArg
#define IloExprNode IloNumExprArg

#define IloFloatVarArray IloNumVarArray

#define IloFloatVar IloNumVar

class IloNumExprI : public IloExtractableI {
	ILOEXTRDECL
protected:
	IloInt _exprUsed;
	
	virtual IloNumExprI* copy();
public:
	virtual void resetExprUsed() ILO_OVERRIDE { _exprUsed =0; }
	IloNumExprI(IloEnvI* env, const char* name=0) : IloExtractableI(env, name), _exprUsed(0) {}
	
	IloNumExprI* lockExpr() { _exprUsed++; return this; }
	
	IloNumExprI* unlockExpr() {_exprUsed--; return this; }
	
	virtual void end() ILO_OVERRIDE;
	

	virtual void release();
	

	virtual IloInt getExprUsed() const ILO_OVERRIDE { return _exprUsed; }
	

	IloNumExprI* getFreeCopy() {
		if (_exprUsed > 0) return copy() ;
		else return this;
	}
	
	virtual IloNumLinTermI* toNumLinTermI();
	
	virtual IloBool isInteger() const;
	
	virtual IloNum eval(const IloAlgorithm alg) const=0;
	
	virtual IloNum getNumConstant() const;
	
	virtual IloBool isRefCountedExpr() const ILO_OVERRIDE;
	friend class IloExpr;
	virtual IloBool isNumExpr() ILO_OVERRIDE {
		return IloTrue;
	}
};

#define IloNumExprArgI IloNumExprI

class IloNumCollectionExprI : public IloNumExprI {
	ILOEXTRDECL
public:
	IloNumCollectionExprI(IloEnvI* env, const char* name = 0) : IloNumExprI(env, name) {}
};
#define IloNumCollectionExprArgI IloNumCollectionExprI

class ILO_EXPORTED IloNumExprArg : public IloExtractable {
	ILOEXTRHANDLE(IloNumExprArg, IloExtractable)
public:
	
	IloNum getNumConstant() const;
	
	IloBool isConstant() const;
	IloBool isIndex() const;
	IloNumExprArg(const IloNumExprArg& ex) : IloExtractable(ex){}
        IloNumExprArg& operator=(const IloNumExprArg& other) {
          _impl = other._impl;
          return *this;
        }
};

#define IloExprArg IloNumExprArg

class ILO_EXPORTED IloNumExpr : public IloNumExprArg {
	ILOEXTRHANDLEDECL(IloNumExpr, IloNumExprArg)
public:
	
	IloNumExpr(const IloNumExprArg expr);
	
	IloNumExpr(const IloEnv env, IloNum = 0);
	
	IloNumExpr(const IloNumLinExprTerm term);
	
	IloNumExpr(const IloIntLinExprTerm term);
	
	IloNumExpr(const IloNumQuadExprTerm term);
	
	IloNumExpr(const IloIntQuadExprTerm term);
	
	IloNumExpr(const IloExpr& expr);
	
	IloNumExpr& operator +=(IloNum val);
	
	IloNumExpr& operator +=(const IloNumExprArg expr);
	
	IloNumExpr& operator -=(IloNum val);
	
	IloNumExpr& operator -=(const IloNumExprArg expr);
	
	IloNumExpr& operator *=(IloNum val);
	
	IloNumExpr& operator /=(IloNum val);

	
	class NonLinearExpression : public IloException {
		IloNumExprI*  _expr;
	public:
		
		NonLinearExpression(IloNumExprI* expr);
		
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
		
		const IloNumExprArg getExpression() const       { return (_expr); }
	};
};

inline ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloNumExpr& ext) {
	if (ext.getImpl()) ext.getImpl()->display(out);
	return (out);
}

class ILO_EXPORTED IloNumExprArray : public IloExtractableArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloNumExprArray(IloDefaultArrayI* i=0) : IloExtractableArray(i) {}
	
	IloNumExprArray(const IloEnv env, IloInt n = 0) :
	IloExtractableArray(env, n) {}
	IloNumExprArray(const IloMemoryManager env, IloInt n = 0) :
	IloExtractableArray(env, n) {}

	
	IloNumExprArg operator[] (IloIntExprArg anIntegerExpr) const;

	
	void lockExprs();
	
	void releaseExprs();

	void replaceByClone(IloEnvI* env);
	
	IloNumExpr& operator[] (IloInt i) {
		return (*(IloNumExpr*)&(IloExtractableArray::operator[](i)));
	}
	
	IloNumExpr  operator[] (IloInt i) const {
		return ((IloNumExprI*)((IloExtractableArray::operator[](i)).getImpl()));
	}
	
	void add(const IloNumExprArray array) { IloExtractableArray::add (array); }
	
	void add(const IloNumExpr x) { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloNumExpr x) {
		IloExtractableArray::add (more, x);
	}
	IloNumExprArray getClone(IloEnvI*) const;
};

typedef IloDefaultArrayI IloNumExprArrayI;

#define ILOINT   IloNumVar::Int
#define ILOFLOAT IloNumVar::Float
#define ILOBOOL  IloNumVar::Bool

class ILO_EXPORTED IloNumVar : public IloNumExprArg {
	ILOEXTRHANDLE(IloNumVar, IloNumExprArg)
public:
	
	enum Type { Int=1, Float=2, Bool=3 };
	
	IloNumVar(const IloEnv env, IloNum lb=0, IloNum ub=IloInfinity, Type type=Float, const char* name=0);
	
	IloNumVar(const IloEnv env,
		IloNum       lowerBound,
		IloNum       upperBound,
		const char*  name);
	
	IloNumVar(const IloAddNumVar& var,
		IloNum              lowerBound   = 0.0,
		IloNum              upperBound   = IloInfinity,
		Type                type = Float,
		const char*         name = 0);
	
	IloNumVar(const IloEnv env,
		const IloNumArray values,
		Type         type = Float,
		const char*  name = 0);
	
	IloNumVar(const IloAddNumVar& var,
		const IloNumArray values,
		Type         type = Float,
		const char*  name = 0);
	
	IloNumVar(const IloConstraint constraint);

	
	

	
	IloNumVar(const IloNumRange coll, const char* name = 0);

	
	Type getType() const;
	
	IloNum getUB() const;
	
	IloNum getLB() const;
	IloNum getUb() const { return getUB(); } 
	IloNum getLb() const { return getLB(); } 
	
	void setUB(IloNum num) const;
	
	void setLB(IloNum num) const;
	void setUb(IloNum num) const { setUB(num); } 
	void setLb(IloNum num) const { setLB(num); } 
	
	void setBounds(IloNum lb, IloNum ub) const;
	
	void getPossibleValues(IloNumArray values) const;
	
	void setPossibleValues(const IloNumArray values) const;
};

#define IloDiscreteNumVar IloNumVar
#define IloDiscreteNumVarI IloNumVarI

class ILO_EXPORTED IloNumVarArray : public IloNumExprArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloNumVarArray(IloDefaultArrayI* i=0) : IloNumExprArray(i) {}
	
	IloNumVarArray (const IloEnv env, IloInt n = 0) : IloNumExprArray(env, n) {}
	
	IloNumVarArray (const IloEnv      env,
		const IloNumArray lb,
		const IloNumArray ub,
		IloNumVar::Type   type = ILOFLOAT);
	
	IloNumVarArray (const IloEnv      env,
		IloNum            lb,
		const IloNumArray ub,
		IloNumVar::Type   type = ILOFLOAT);
	
	IloNumVarArray (const IloEnv      env,
		const IloNumArray lb,
		IloNum            ub,
		IloNumVar::Type   type = ILOFLOAT);
	
	IloNumVarArray (const IloEnv    env,
		IloInt          n,
		IloNum          lb,
		IloNum          ub,
		IloNumVar::Type type = ILOFLOAT);
	
	IloNumVarArray (const IloEnv env,
		const IloNumColumnArray columnarray,
		IloNumVar::Type         type = ILOFLOAT);
	
	IloNumVarArray (const IloEnv env,
		const IloNumColumnArray columnarray,
		const IloNumArray       lb,
		const IloNumArray       ub,
		IloNumVar::Type         type = ILOFLOAT);

	IloNumVarArray(const IloEnv env, IloInt n, const IloNumVar v0);

	IloNumVarArray(const IloMemoryManager env, IloInt n = 0) : IloNumExprArray(env, n) {}

#ifdef ILOALLOWVARARGS
	
	IloNumVarArray(const IloEnv env, IloInt n, const IloNumVar v0, const IloNumVar v1 ...);
#else 
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2,
		const IloNumVar v3);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2,
		const IloNumVar v3, const IloNumVar v4);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2,
		const IloNumVar v3, const IloNumVar v4, const IloNumVar v5);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2,
		const IloNumVar v3, const IloNumVar v4, const IloNumVar v5,
		const IloNumVar v6);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2,
		const IloNumVar v3, const IloNumVar v4, const IloNumVar v5,
		const IloNumVar v6, const IloNumVar v7);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2,
		const IloNumVar v3, const IloNumVar v4, const IloNumVar v5,
		const IloNumVar v6, const IloNumVar v7, const IloNumVar v8);
	IloNumVarArray(const IloEnv env, IloInt n,
		const IloNumVar v0, const IloNumVar v1, const IloNumVar v2,
		const IloNumVar v3, const IloNumVar v4, const IloNumVar v5,
		const IloNumVar v6, const IloNumVar v7, const IloNumVar v8,
		const IloNumVar v9);
#endif

	IloNumVar& operator[] (IloInt i) {
		return (*(IloNumVar*)(void*)&(IloNumExprArray::operator[](i)));
	}
	IloNumVar  operator[] (IloInt i) const {
		return ((IloNumVarI*)(void*)((IloNumExprArray::operator[](i)).getImpl()));
	}
	
	IloNumExprArg operator[] (IloIntExprArg anIntegerExpr) const;

	
	void add(const IloNumVarArray array)         { IloNumExprArray::add (array); }
	
	void add(const IloNumVar x)              { IloNumExprArray::add (x); }
	
	void add(IloInt more, const IloNumVar x) { IloNumExprArray::add (more, x); }

	IloNumVarArray getClone(IloEnvI*) const;

	IloBool isElementsType(IloNumVar::Type) const;
	void checkTypes(IloNumVar::Type) const;

	
	
	void setBounds(const IloNumArray lb, const IloNumArray ub);
	
	IloNumExprArray toNumExprArray() const;
	
	IloIntExprArray toIntExprArray() const;
	
	IloIntVarArray toIntVarArray() const;
};

class ILO_EXPORTED IloSemiContVar : public IloNumVar {
	ILOEXTRHANDLEDECL(IloSemiContVar, IloNumVar)
public:
	
	IloSemiContVar (const IloEnv    env,
		IloNum          sclb,
		IloNum          ub,
		IloNumVar::Type type = ILOFLOAT,
		const char*     name = 0);
	
	IloSemiContVar (const IloAddNumVar& var,
		IloNum              sclb,
		IloNum              ub,
		IloNumVar::Type     type = ILOFLOAT,
		const char*         name = 0);

	IloNum getSemiContinuousLb() const;
	
	IloNum getSemiContLB() const { return getSemiContinuousLb(); }
	void setSemiContinuousLb(IloNum sclb) const;
	
	void setSemiContLB(IloNum sclb) const { setSemiContinuousLb(sclb); }
};

typedef IloDefaultArrayI IloNumVarArrayI;

class ILO_EXPORTED IloSemiContVarArray : public IloNumVarArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloSemiContVarArray(IloDefaultArrayI* i=0) : IloNumVarArray(i) {}
	
	IloSemiContVarArray (const IloEnv env) : IloNumVarArray(env) {}
	
	IloSemiContVarArray (const IloEnv env, IloInt n)
		: IloNumVarArray(env, n)        {}

	IloSemiContVarArray (const IloEnv      env,
		const IloNumArray sclb,
		const IloNumArray ub,
		IloNumVar::Type   type = ILOFLOAT);
	
	IloSemiContVarArray (const IloEnv    env,
		IloInt          n,
		IloNum          sclb,
		IloNum          ub,
		IloNumVar::Type type = ILOFLOAT);

	
	IloSemiContVarArray (const IloEnv            env,
		const IloNumColumnArray columnarray,
		const IloNumArray       sclb,
		const IloNumArray       ub,
		IloNumVar::Type         type = ILOFLOAT);

	IloSemiContVarArray(const IloEnv env, IloInt n, const IloSemiContVar v0);
	IloSemiContVarArray(const IloMemoryManager env, IloInt n = 0) : IloNumVarArray(env, n) {}

#ifdef ILOALLOWVARARGS
	
	IloSemiContVarArray(const IloEnv env, IloInt n, const IloSemiContVar v0,
		const IloSemiContVar v1 ...);
#else 
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2,
		const IloSemiContVar v3);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2,
		const IloSemiContVar v3, const IloSemiContVar v4);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2,
		const IloSemiContVar v3, const IloSemiContVar v4, const IloSemiContVar v5);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2,
		const IloSemiContVar v3, const IloSemiContVar v4, const IloSemiContVar v5,
		const IloSemiContVar v6);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2,
		const IloSemiContVar v3, const IloSemiContVar v4, const IloSemiContVar v5,
		const IloSemiContVar v6, const IloSemiContVar v7);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2,
		const IloSemiContVar v3, const IloSemiContVar v4, const IloSemiContVar v5,
		const IloSemiContVar v6, const IloSemiContVar v7, const IloSemiContVar v8);
	IloSemiContVarArray(const IloEnv env, IloInt n,
		const IloSemiContVar v0, const IloSemiContVar v1, const IloSemiContVar v2,
		const IloSemiContVar v3, const IloSemiContVar v4, const IloSemiContVar v5,
		const IloSemiContVar v6, const IloSemiContVar v7, const IloSemiContVar v8,
		const IloSemiContVar v9);
#endif

	IloSemiContVar& operator[] (IloInt i) {
		return (*(IloSemiContVar*)&(IloNumVarArray::operator[](i)));
	}
	IloSemiContVar  operator[] (IloInt i) const {
		return ((IloSemiContVarI*)(void*)((IloNumVarArray::operator[](i)).getImpl()));
	}

	
	void add(const IloSemiContVarArray array)         { IloNumVarArray::add (array); }
	
	void add(const IloSemiContVar x)              { IloNumVarArray::add (x); }
	
	void add(IloInt more, const IloSemiContVar x) { IloNumVarArray::add (more, x); }
};

typedef IloDefaultArrayI IloSemiContVarArrayI;

class IloNumLinExprTerm {
	IloNumLinExprTermI* _impl;
public:
	IloNumLinExprTerm(IloNumLinExprTermI* impl=0) : _impl(impl) {}
	IloNumLinExprTermI* getImpl() const { return _impl; }
	operator IloNumExprArg() const;
};

class IloNumQuadExprTerm {
   IloNumQuadExprTermI* _impl;
public:
   IloNumQuadExprTerm(IloNumQuadExprTermI* impl=0) : _impl(impl) {}
   IloNumQuadExprTermI* getImpl() const { return _impl; }
   operator IloNumExprArg() const;
};

class ILO_EXPORTED IloExpr : public IloNumExpr {
public:
	
	IloExpr() : IloNumExpr() {}
	
	IloExpr(IloNumExprI* expr) : IloNumExpr(expr) {}
	
	IloExpr(const IloNumLinExprTerm term);
	
	IloExpr(const IloIntLinExprTerm term);
	
	IloExpr(const IloNumQuadExprTerm term);
	
	IloExpr(const IloIntQuadExprTerm term);
	
	IloExpr(IloNumExprArg);
	
	IloExpr(const IloEnv env, IloNum val = 0);

	
	IloNumLinTermI* getImpl() const { return (IloNumLinTermI*)(void*)_impl; }
	
	IloInt normalize() const;
	
	IloBool isNormalized() const;
	
	void remove(const IloNumVarArray vars);
	void remove(const IloExtractableArray vars);

	
	void setQuadCoef(const IloNumVar var1, const IloNumVar var2, IloNum value);
	
	
	void setLinearCoef(const IloNumVar var, IloNum value);
	
	void setLinearCoefs(const IloNumVarArray vars, IloNumArray values);
	
	void setNumConstant(IloNum constant);
	
	void setConstant(IloNum cst) {
		setNumConstant(cst);
	}
	
	IloNum getConstant() const {
		return getNumConstant();
	}

	IloExpr copy() const;
        
	void clear();
	
	IloExpr& operator +=(IloNum val);
	
	IloExpr& operator +=(const IloNumExprArg expr);
	
	IloExpr& operator +=(const IloNumVar var);
	
	IloExpr& operator +=(const IloIntVar var);

	
	IloExpr& operator +=(const IloNumLinExprTerm term);
	
	IloExpr& operator +=(const IloIntLinExprTerm term);

	
	IloExpr& operator +=(const IloNumQuadExprTerm term);
	
	IloExpr& operator +=(const IloIntQuadExprTerm term);

	
	IloExpr& operator -=(IloNum val);
	
	IloExpr& operator -=(const IloNumExprArg expr);
	
	IloExpr& operator -=(const IloNumVar var);
	
	IloExpr& operator -=(const IloIntVar var);

	
	IloExpr& operator -=(const IloNumLinExprTerm term);
	
	IloExpr& operator -=(const IloIntLinExprTerm term);

	
	IloExpr& operator -=(const IloNumQuadExprTerm term);
	
	IloExpr& operator -=(const IloIntQuadExprTerm term);

	
	IloExpr& operator *=(IloNum val);
	

	IloExpr& operator /=(IloNum val);

	
	
	
	

	

	class LinearIterator {
		IloNumLinExprTermI* _impl;
	public:
		LinearIterator(IloNumLinExprTermI* term=0) : _impl (term) {}
		

		void operator++ ();
		

		IloBool  ok () const { return (_impl != 0); }
		

		IloNum getCoef() const;
		
		IloNumVar getVar() const;
		IloNumLinExprTermI* getImpl() const  { return (_impl); }
	};

	
	IloExpr::LinearIterator getLinearIterator() const;

         
	class QuadIterator {
            IloNumQuadExprTermI* _impl;
        public:
            QuadIterator(IloNumQuadExprTermI* term=0) : _impl (term) {
			}

            
            void operator++ ();

            
            IloBool  ok () const { return (_impl != 0); }

            
            IloNum getCoef() const;

            
            IloNumVar getVar1() const;

            
            IloNumVar getVar2() const;

            IloNumQuadExprTermI* getImpl() const  { return (_impl); }
	};

	
	IloExpr::QuadIterator getQuadIterator() const;
};

class IloExprArray : public IloNumExprArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloExprArray(IloDefaultArrayI* i=0) : IloNumExprArray(i) {}
	
	IloExprArray(const IloEnv env, IloInt n = 0) : IloNumExprArray(env, n) {}
	IloExprArray(const IloMemoryManager env, IloInt n = 0) : IloNumExprArray(env, n) {}

	
	IloNumExpr& operator[] (IloInt i) {
		return (*(IloNumExpr*)&(IloExtractableArray::operator[](i)));
	}
	IloNumExprArg operator[] (IloIntExprArg anIntegerExpr) const;

	
	IloNumExpr  operator[] (IloInt i) const {
		return ((IloNumExprI*)((IloExtractableArray::operator[](i)).getImpl()));
	}
};

typedef IloDefaultArrayI IloExprArrayI;

class IloIntExprI : public IloNumExprI {
	ILOEXTRDECL
		virtual IloNumExprI* copy() ILO_OVERRIDE;
	virtual IloIntExprI* intCopy();
public:
	IloIntExprI* intLockExpr() { return (IloIntExprI*)lockExpr(); }
	IloIntExprI(IloEnvI* env, const char* name=0) : IloNumExprI(env, name) {}
	IloIntExprI* getFreeIntCopy() { return (IloIntExprI*)getFreeCopy(); }
	virtual IloBool isInteger() const ILO_OVERRIDE;
	
	virtual IloIntLinTermI* toIntLinTermI();
};

class IloIntCollectionExprI : public IloIntExprI {
	ILOEXTRDECL
public:
	IloIntCollectionExprI(IloEnvI* env, const char* name = 0) : IloIntExprI(env, name) {}
};
#define IloIntCollectionExprArgI IloIntCollectionExprI

class IloNumVarI : public IloIntExprI {
	ILOEXTRDECL
		virtual ~IloNumVarI();
	IloVarDomainI* _domain;
	void* _work;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloNumVarI(const IloIntRangeI* coll, const char* name=0);
	IloNumVarI(const IloNumRangeI* coll, const char* name=0);

	IloNumVarI(IloEnvI* env, IloVarDomainI* domain, const char* name=0);
	IloNumVarI(IloEnvI* env, IloNum lb=0, IloNum ub=IloInfinity, IloNumVar::Type type=ILOFLOAT, const char* name=0);
	IloNumVarI(IloEnvI* env, IloNumVar::Type type, IloNum lb, IloNum ub, const char* name=0);
	IloNumVarI(IloEnvI* env, const char* name);
	IloNumVarI(IloEnvI* env, IloNumArray values, IloNumVar::Type type = ILOFLOAT, const char* name = 0);
	IloNumVarI(IloEnvI* env, IloIntArray values, IloNumVar::Type type = ILOINT, const char* name = 0);
	IloNumVarI(IloEnvI* env, IloAnyArray values, const char * name = 0);

	IloNumVar::Type getType() const;
	virtual void setUB(IloNum ub);
	IloNum getUB() const;
	virtual void setLB(IloNum lb);
	IloNum getLB() const;
	virtual void setBounds(IloNum lb, IloNum ub);
	IloNumArray getNumValues() const;
	IloIntArray getIntValues() const;
	void setPossibleValues(const IloNumArray values);
	void setPossibleValues(const IloIntArray values);
	IloBool isEnumerated() const;
	IloVarDomainI* getDomain() const { return _domain; }
	void makeDomain(IloNum lb, IloNum ub);

	virtual IloNumLinTermI* toNumLinTermI() ILO_OVERRIDE;
	virtual IloIntLinTermI* toIntLinTermI() ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream)&) const ILO_OVERRIDE;
	virtual IloBool isInteger() const ILO_OVERRIDE;
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual void end() ILO_OVERRIDE;
	virtual void release() ILO_OVERRIDE;

	friend class IloRangeI;
	friend class IloNumLinTermI;

	virtual IloBool isRefCountedExpr() const ILO_OVERRIDE;
	virtual IloBool isDecisionExpr() const ILO_OVERRIDE { return IloTrue; }
	virtual IloBool isGround() const ILO_OVERRIDE { return IloTrue; }
	void setBounds(IloNum lb, IloNum ub, IloBool withNotif);
	virtual IloBool isNumVar() ILO_OVERRIDE {
		return IloTrue;
	}
};

#define IloBoolExpr IloIntExpr
#define IloIntVarI IloNumVarI
#define IloBoolVarI IloNumVarI
#define IloIntExprArgI IloIntExprI

class ILO_EXPORTED IloIntExprArg : public IloNumExprArg {
	ILOEXTRHANDLE(IloIntExprArg, IloNumExprArg)
public:
	IloIntExprArg(const IloIntExprArg& ex) : IloNumExprArg(ex){}
        IloIntExprArg& operator=(const IloIntExprArg& other) {
          _impl = other._impl;
          return *this;
        }
};

class IloIntExpr : public IloIntExprArg {
	ILOEXTRHANDLE(IloIntExpr, IloIntExprArg)
public:
	
	IloIntExpr(const IloIntExprArg arg);
	IloIntExpr(const IloIntQuadExprTerm arg);
	
	IloIntExpr(const IloIntLinExprTerm term);
	
	IloIntExpr(const IloEnv env, IloInt constant=0);
	
	IloIntExpr& operator +=(IloInt val);
	
	IloIntExpr& operator +=(const IloIntExprArg expr);
	
	IloIntExpr& operator -=(IloInt val);
	
	IloIntExpr& operator -=(const IloIntExprArg expr);
	
	IloIntExpr& operator *=(IloInt val);

	class IloOverflowOccurred : public IloException {
	public:
		IloOverflowOccurred(const char * message) : IloException(message) {}
		virtual ~IloOverflowOccurred(){}
	};
};

class ILO_EXPORTED IloIntExprArray : public IloExtractableArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloIntExprArray(IloDefaultArrayI* i=0) : IloExtractableArray(i) {}
	
	IloIntExprArray(const IloEnv env, IloInt n = 0) :
	IloExtractableArray(env, n) {}
	IloIntExprArray(const IloMemoryManager env, IloInt n = 0) :
	IloExtractableArray(env, n) {}

	
	void lockExprs();
	
	void releaseExprs();

	void replaceByClone(IloEnvI* env);

	
	IloIntExpr& operator[] (IloInt i) {
		return (*(IloIntExpr*)&(IloExtractableArray::operator[](i)));
	}
	
	IloIntExpr  operator[] (IloInt i) const {
		return ((IloIntExprI*)((IloExtractableArray::operator[](i)).getImpl()));
	}

	
	IloIntExprArg operator[] (IloIntExprArg anIntegerExpr) const;

	
	void add(const IloIntExprArray array) { IloExtractableArray::add (array); }
	
	void add(const IloIntExpr x) { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloIntExpr x) {
		IloExtractableArray::add (more, x);
	}
	IloIntExprArray getClone(IloEnvI*) const;
};

typedef IloDefaultArrayI IloIntExprArrayI;

class ILO_EXPORTED IloIntVar : public IloIntExprArg {
	ILOEXTRHANDLE(IloIntVar, IloIntExprArg)
public:
	IloIntVar(const IloEnv coll, const char*  name);
#if defined(ILOEXTRAINTVARCTOR)
	
	IloIntVar(IloEnv env,
		int vmin,
		int vmax,
		const char* name=0);
	
	IloIntVar(IloEnv env,
		IloNum vmin,
		IloNum vmax,
		const char* name=0);
#endif
	
	IloIntVar(IloEnv env,
		IloInt vmin=0,
		IloInt vmax=IloIntMax,
		const char* name=0);
	
	IloIntVar(const IloAddNumVar& var,
		IloInt              lowerBound   = 0,
		IloInt              upperBound   = IloIntMax,
		const char*         name = 0);
	
	IloIntVar(const IloEnv env,
		const IloIntArray values,
		const char*  name = 0);
	
	IloIntVar(const IloAddNumVar& var,
		const IloIntArray values,
		const char*  name = 0);

	
	IloIntVar(const IloNumVar var);

	
	

	
	IloIntVar(const IloIntRange coll, const char*  name = 0);

	operator IloNumVar() const;

	
	IloInt getMin() const;
	
	IloInt getMax() const;
	
	IloNum getUB() const { return (IloNum)getMax(); }
	
	IloNum getLB() const { return (IloNum)getMin(); }
	IloNum getUb() const { return (IloNum)getMax(); } 
	IloNum getLb() const { return (IloNum)getMin(); } 

	void getPossibleValues(IloIntArray values) const;
	
	void setPossibleValues(const IloIntArray values) const;
	
	void setMin(IloInt min) const;
	
	void setMax(IloInt max) const;
	
	void setUB(IloNum max) const { setMax((IloInt)IloTrunc(IloFloor(max))); }
	void setUb(IloNum max) const { setMax((IloInt)IloTrunc(IloFloor(max))); }
	
	void setLB(IloNum min) const { setMin((IloInt)IloTrunc(IloCeil(min))); }
	void setLb(IloNum min) const { setMin((IloInt)IloTrunc(IloCeil(min))); }

	IloNumVar::Type getType() const;
	
	void setBounds(IloInt lb, IloInt ub) const;

	IloIntVar( const IloEnv env, const IloAnyArray values, const char* name = 0 );

	IloAnyArray getAnyDomain() const;

#if defined(ILOEXTRAINTVARCTOR)
	
	void setBounds(IloNum lb, IloNum ub) const;
	
	void setBounds(int lb, int ub) const;
#endif

	static const char* AnyDomainProperty;
};

class ILO_EXPORTED IloIntVarArray : public IloIntExprArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloIntVarArray(IloDefaultArrayI* i=0) : IloIntExprArray(i) {}
	
	IloIntVarArray(const IloEnv env, IloInt n = 0) : IloIntExprArray(env, n) {}
	IloIntVarArray(const IloMemoryManager env, IloInt n = 0) : IloIntExprArray(env, n) {}

	
	IloIntVarArray(const IloEnv      env,
		const IloIntArray lb,
		const IloIntArray ub);
	
	IloIntVarArray(const IloEnv      env,
		IloInt            lb,
		const IloIntArray ub);
	
	IloIntVarArray(const IloEnv      env,
		const IloIntArray lb,
		IloInt            ub);
	
	IloIntVarArray(const IloEnv env,
		IloInt       n,
		IloInt       lb,
		IloInt       ub);
	
	IloIntVarArray(const IloEnv env,
		const IloNumColumnArray columnarray);
	
	IloIntVarArray(const IloEnv env,
		const IloNumColumnArray columnarray,
		const IloNumArray       lb,
		const IloNumArray       ub);

	IloIntVarArray(const IloEnv env, IloInt n, const IloIntVar v0);

	IloIntVarArray( const IloEnv env,
		const IloInt n,
		const IloAnyArray values,
		const char* name = 0 );

#ifdef ILOALLOWVARARGS
	
	IloIntVarArray(const IloEnv env, IloInt n, const IloIntVar v0, const IloIntVar v1, ...);
#else 
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2,
		const IloIntVar v3);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2,
		const IloIntVar v3, const IloIntVar v4);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2,
		const IloIntVar v3, const IloIntVar v4, const IloIntVar v5);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2,
		const IloIntVar v3, const IloIntVar v4, const IloIntVar v5,
		const IloIntVar v6);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2,
		const IloIntVar v3, const IloIntVar v4, const IloIntVar v5,
		const IloIntVar v6, const IloIntVar v7);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2,
		const IloIntVar v3, const IloIntVar v4, const IloIntVar v5,
		const IloIntVar v6, const IloIntVar v7, const IloIntVar v8);
	IloIntVarArray(const IloEnv env, IloInt n,
		const IloIntVar v0, const IloIntVar v1, const IloIntVar v2,
		const IloIntVar v3, const IloIntVar v4, const IloIntVar v5,
		const IloIntVar v6, const IloIntVar v7, const IloIntVar v8,
		const IloIntVar v9);
#endif

	
	IloIntExprArg operator[] (IloIntExprArg anIntegerExpr) const;

	IloIntExprArray toIntExprArray() const;

	IloIntVar& operator[] (IloInt i) {
		return (*(IloIntVar*)(void*)&(IloIntExprArray::operator[](i)));
	}

	
	IloIntVar  operator[] (IloInt i) const {
		return ((IloIntVarI*)((IloIntExprArray::operator[](i)).getImpl()));
	}

	
	void add(const IloIntVarArray x)         { IloIntExprArray::add (x); }
	
	void add(const IloIntVar x)              { IloIntExprArray::add (x); }
	
	void add(IloInt more, const IloIntVar x) {
		IloIntExprArray::add (more, x);
	}
	IloIntVarArray getClone(IloEnvI*) const;
	
	IloNumVarArray toNumVarArray() const;
};

IloIntExprArg IloElement(IloIntArray a, IloIntExprArg x);

IloIntExprArg IloElement(IloIntVarArray a, IloIntExprArg x);

IloNumExprArg IloElement(IloNumArray a, IloIntExprArg x);

IloNumExprArg IloElement(IloNumVarArray a, IloIntExprArg x);

typedef IloDefaultArrayI IloIntVarArrayI;

class ILO_EXPORTED IloBoolVar : public IloIntVar {
public:
	IloBoolVar(IloBoolVarI* var=0) : IloIntVar(var) {}
	
	IloBoolVar(IloEnv env, IloInt min = 0, IloInt max = 1, const char* name=0);
	
	IloBoolVar(IloEnv env, const char* name);
	IloBoolVar(const IloNumVar);
	IloBoolVar(const IloIntVar);
	
	IloBoolVar(const IloAddNumVar& column, const char* name = 0);
	IloNumVar::Type getType() const { return ILOBOOL; }
};

class IloIntLinExprTerm {
	IloNumLinExprTermI* _impl;
public:
	IloIntLinExprTerm(IloNumLinExprTermI* impl=0):  _impl(impl) {}
	IloNumLinExprTermI* getImpl() const { return _impl; }
	operator IloIntExprArg() const;
};

class IloIntQuadExprTerm {
   IloNumQuadExprTermI* _impl;
public:
   IloIntQuadExprTerm(IloNumQuadExprTermI* impl=0) : _impl(impl) {}
   IloNumQuadExprTermI* getImpl() const { return _impl; }
   operator IloIntExprArg() const;
};

class IloBoolVarArray : public IloIntVarArray {
public:
	IloIntExprArg operator[](IloIntExprArg anIntegerExpr) const{
		return IloIntVarArray::operator[](anIntegerExpr);
	}
	typedef IloDefaultArrayI ImplClass;
	
	IloBoolVarArray(IloDefaultArrayI* i=0) : IloIntVarArray(i) {}
	
	IloBoolVarArray(const IloEnv env, IloInt n) : IloIntVarArray(env, n, 0, 1)  {}
	IloBoolVarArray(const IloEnv env) : IloIntVarArray(env)  {}
	IloBoolVarArray(const IloMemoryManager env, IloInt n = 0) : IloIntVarArray(env, n)  {}

	IloBoolVarArray(const IloIntVarArray& array);
	
	IloBoolVarArray(const IloEnv env, const IloNumColumnArray columnarray);
	IloBoolVarArray(const IloEnv env, IloInt n, const IloBoolVar v0);
#ifdef ILOALLOWVARARGS
	
	IloBoolVarArray(const IloEnv env, IloInt n, const IloBoolVar v0,
		const IloBoolVar v1, ...);
#else 
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2,
		const IloBoolVar v3);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2,
		const IloBoolVar v3, const IloBoolVar v4);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2,
		const IloBoolVar v3, const IloBoolVar v4, const IloBoolVar v5);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2,
		const IloBoolVar v3, const IloBoolVar v4, const IloBoolVar v5,
		const IloBoolVar v6);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2,
		const IloBoolVar v3, const IloBoolVar v4, const IloBoolVar v5,
		const IloBoolVar v6, const IloBoolVar v7);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2,
		const IloBoolVar v3, const IloBoolVar v4, const IloBoolVar v5,
		const IloBoolVar v6, const IloBoolVar v7, const IloBoolVar v8);
	IloBoolVarArray(const IloEnv env, IloInt n,
		const IloBoolVar v0, const IloBoolVar v1, const IloBoolVar v2,
		const IloBoolVar v3, const IloBoolVar v4, const IloBoolVar v5,
		const IloBoolVar v6, const IloBoolVar v7, const IloBoolVar v8,
		const IloBoolVar v9);
#endif

	IloBoolVar& operator[] (IloInt i) {
		return (*(IloBoolVar*)&(IloIntVarArray::operator[](i)));
	}
	
	IloBoolVar  operator[] (IloInt i) const {
		return ((IloNumVarI*)((IloIntVarArray::operator[](i)).getImpl()));
	}

	
	void add(const IloBoolVarArray x)         { IloIntVarArray::add (x); }
	
	void add(const IloBoolVar x)              { IloIntVarArray::add (x); }
	
	void add(IloInt more, const IloBoolVar x) {
		IloIntVarArray::add (more, x);
	}
	IloBoolVarArray getClone(IloEnvI*) const;
};

typedef IloDefaultArrayI IloBoolVarArrayI;

inline IloNumLinExprTerm operator * (const IloNumVar x, IloNum num) {
	return x.getEnv().getImpl()->newTerm(x.getImpl(), num);
}
inline IloNumLinExprTerm operator * (IloNum num, const IloNumVar x)  {
	return x.getEnv().getImpl()->newTerm(x.getImpl(), num);
}

ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloNumVar x, IloInt num);

ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (IloInt num, const IloNumVar x);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloNumVar x, int num);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (int num, const IloNumVar x);

ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloIntVar x, IloNum num);

ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (IloNum num, const IloIntVar x);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloNumVar x, int num);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (int num, const IloNumVar x);

ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (const IloIntVar x, IloInt num);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (IloInt num, const IloIntVar x);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (const IloIntVar x, int num);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (int num, const IloIntVar x);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (int num, const IloIntLinExprTerm x);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (const IloIntLinExprTerm x, int num);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (IloInt num, const IloIntLinExprTerm x);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator * (const IloIntLinExprTerm x, IloInt num);

ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (IloNum num, const IloIntLinExprTerm x);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloIntLinExprTerm x, IloNum num);

ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumVar x1, const IloNumVar x2);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloIntVar x1, const IloNumVar x2);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumVar x1, const IloIntVar x2);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (const IloIntVar x1, const IloIntVar x2);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumLinExprTerm e, const IloNumVar x);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumLinExprTerm e, const IloIntVar x);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloIntLinExprTerm e, const IloNumVar x);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (const IloIntLinExprTerm e, const IloIntVar x);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumLinExprTerm e1, const IloNumLinExprTerm e2);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumLinExprTerm e1, const IloIntLinExprTerm e2);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (const IloIntLinExprTerm e1, const IloIntLinExprTerm e2);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumQuadExprTerm e, IloNum c);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (const IloIntQuadExprTerm e, IloInt c);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumQuadExprTerm e, IloInt c);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (IloInt c, const IloNumQuadExprTerm e);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloConstraint& c, const IloNumLinExprTerm e);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumLinExprTerm e, const IloConstraint& c);

ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (IloNum c, const IloNumQuadExprTerm e);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (IloInt c, const IloIntQuadExprTerm e);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (int c, const IloIntQuadExprTerm e);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (const IloIntQuadExprTerm e, int c);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloIntLinExprTerm e1, const IloNumLinExprTerm e2) ;
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumLinExprTerm e1, const IloIntLinExprTerm e2);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumQuadExprTerm q1, const IloNumQuadExprTerm q2);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloIntQuadExprTerm q1, const IloNumQuadExprTerm q2);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumQuadExprTerm q1, const IloIntQuadExprTerm q2);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntQuadExprTerm q1, const IloIntQuadExprTerm q2);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumQuadExprTerm q1, const IloNumQuadExprTerm q2);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloIntQuadExprTerm q1, const IloNumQuadExprTerm q2);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumQuadExprTerm q1, const IloIntQuadExprTerm q2);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntQuadExprTerm q1, const IloIntQuadExprTerm q2);

ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator-(const IloNumQuadExprTerm term);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator-(const IloIntQuadExprTerm term);

ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloNumLinExprTerm x, IloInt num);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (IloInt num, const IloNumLinExprTerm x);

inline IloNumQuadExprTerm IloSquare(const IloNumVar var) {
   return (var * var);
}

inline IloIntQuadExprTerm IloSquare(const IloIntVar var) {
   return (var * var).getImpl();
}

inline IloNumQuadExprTerm IloSquare(const IloNumLinExprTerm term) {
   return (term * term);
}

inline IloIntQuadExprTerm IloSquare(const IloIntLinExprTerm term) {
   return (term * term);
}

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator * (const IloNumExprArg x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator * (const IloNumExprArg x, IloNum y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator * (IloNum x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator * (const IloIntExprArg x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator * (const IloIntExprArg x, IloInt y);
inline IloNumExprArg operator * (const IloIntExprArg x, IloNum y) {
	return IloNumExprArg(x) * y;
}
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator * (const IloIntExprArg x, int y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator * (IloInt x, const IloIntExprArg y);
inline IloNumExprArg operator * (IloNum x, const IloIntExprArg y) {
	return x * (IloNumExprArg(y));
}
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator * (int x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (int x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumQuadExprTerm) operator * (const IloNumQuadExprTerm y, int x);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (IloNum x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (int x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloNumLinExprTerm y, int x);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator * (const IloNumLinExprTerm y, IloNum x);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator / (const IloNumExprArg x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator / (const IloNumExprArg x, IloNum y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator / (IloNum x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumExprArg x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumExprArg x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumExprArg x, const IloIntLinExprTerm y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumExprArg x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumLinExprTerm x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloIntLinExprTerm x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumLinExprTerm x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloIntLinExprTerm x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumLinExprTerm x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumLinExprTerm x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloIntLinExprTerm x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (int x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumQuadExprTerm x, int y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumQuadExprTerm x, IloNum y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (IloInt x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumQuadExprTerm x, IloInt y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloIntQuadExprTerm x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (IloNum y, const IloNumQuadExprTerm x);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (IloNum y, const IloIntQuadExprTerm x);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumLinExprTerm x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (const IloNumQuadExprTerm x, const IloNumLinExprTerm y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (IloNum x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (IloNum x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator + (IloNum x, const IloIntLinExprTerm y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntExprArg x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntExprArg x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntQuadExprTerm x, IloInt y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (IloInt y, const IloIntQuadExprTerm x);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntQuadExprTerm x, int y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (int y, const IloIntQuadExprTerm x);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntExprArg x, IloInt y);
inline IloNumExprArg operator + (const IloIntExprArg x, IloNum y) {
	return IloNumExprArg(x) + y;
}
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntExprArg x, int y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntLinExprTerm x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntLinExprTerm x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntLinExprTerm x, IloInt y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntLinExprTerm x, int y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (IloInt x, const IloIntExprArg y);
inline IloNumExprArg operator + (IloNum x, const IloIntExprArg y) {
	return x + (IloNumExprArg(y));
}
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (int x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (IloInt x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (int x, const IloIntLinExprTerm y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumExprArg x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumExprArg x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumExprArg x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumExprArg x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumExprArg x, const IloIntQuadExprTerm y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumExprArg x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumLinExprTerm x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloIntLinExprTerm x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumLinExprTerm x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloIntLinExprTerm x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumLinExprTerm x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumLinExprTerm x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloIntLinExprTerm x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (int x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumQuadExprTerm x, int y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (IloNum x, const IloNumExprArg y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (IloNum x, const IloNumLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (IloNum x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumQuadExprTerm x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloIntQuadExprTerm x, IloNum y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (IloNum y, const IloNumQuadExprTerm x);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (IloNum y, const IloIntQuadExprTerm x);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumLinExprTerm x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumQuadExprTerm x, const IloNumLinExprTerm y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (IloInt x, const IloNumQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator - (const IloNumQuadExprTerm x, IloInt y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntExprArg x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntExprArg x, const IloIntLinExprTerm y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntExprArg x, IloInt y);
inline IloNumExprArg operator - (const IloIntExprArg x, IloNum y) {
	return IloNumExprArg(x) - y;
}
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntExprArg x, int y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntLinExprTerm x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntLinExprTerm x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntLinExprTerm x, IloInt y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntLinExprTerm x, int y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntQuadExprTerm x, int y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (int y, const IloIntQuadExprTerm x);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntQuadExprTerm x, IloInt y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (IloInt y, const IloIntQuadExprTerm x);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntQuadExprTerm x, const IloIntQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntQuadExprTerm x, const IloIntQuadExprTerm y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntQuadExprTerm x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntExprArg y, const IloIntQuadExprTerm x);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntQuadExprTerm x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntExprArg y, const IloIntQuadExprTerm x);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (IloInt x, const IloIntExprArg y);
inline IloNumExprArg operator - (IloNum x, const IloIntExprArg y) {
	return x - IloNumExprArg(y);
}
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (int x, const IloIntExprArg y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (IloInt x, const IloIntLinExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (int x, const IloIntLinExprTerm y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator -(const IloIntExprArg arg);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator-(const IloIntVar var);
ILO_EXPORTEDFUNCTION(IloNumExprArg) operator -(const IloNumExprArg arg);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator-(const IloNumVar var);
ILO_EXPORTEDFUNCTION(IloNumLinExprTerm) operator-(const IloNumLinExprTerm term);
ILO_EXPORTEDFUNCTION(IloIntLinExprTerm) operator-(const IloIntLinExprTerm term);

ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (IloNum x, const IloIntQuadExprTerm  y);
ILO_EXPORTEDFUNCTION(IloIntQuadExprTerm) operator * (const IloIntQuadExprTerm  y, IloNum x);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntLinExprTerm x, const  IloIntQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator + (const IloIntQuadExprTerm x, const IloIntLinExprTerm y); 
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntLinExprTerm x, const  IloIntQuadExprTerm y);
ILO_EXPORTEDFUNCTION(IloIntExprArg) operator - (const IloIntQuadExprTerm x, const IloIntLinExprTerm y); 

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator% (const IloIntExprArg x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator% (const IloIntExprArg x, IloInt y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) operator% (IloInt x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloDiv (const IloIntExprArg x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloDiv (const IloIntExprArg x, IloInt y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloDiv (IloInt x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloNumArray values,
												const IloNumVarArray vars);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloNumVarArray vars,
												const IloNumArray values);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloNumArray values, const IloIntVarArray vars);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloIntVarArray vars, const IloNumArray values);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloIntArray values, const IloNumVarArray vars);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloNumVarArray vars, const IloIntArray values);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloNumExprArray leftExprs, const IloNumExprArray rightExprs);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloNumVarArray leftExprs, const IloNumVarArray rightExprs);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloIntVarArray leftExprs, const IloNumVarArray rightExprs);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloScalProd(const IloNumVarArray leftExprs, const IloIntVarArray rightExprs);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloScalProd(const IloIntArray values, const IloIntVarArray vars);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloScalProd(const IloIntVarArray vars, const IloIntArray values);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloScalProd(const IloIntExprArray leftExprs, const IloIntExprArray rightExprs);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloScalProd(const IloIntVarArray leftExprs, const IloIntVarArray rightExprs);

IloIntExprArg IloQuadProd(const IloIntVarArray leftExprs, const IloIntVarArray rightExprs, const IloIntArray coefs);

IloNumExprArg IloQuadProd(const IloNumVarArray leftExprs, const IloNumVarArray rightExprs, const IloNumArray coefs);

ILO_EXPORTEDFUNCTION(IloNum) IloScalProd(const IloNumArray vals1, const IloNumArray vals2);

ILO_EXPORTEDFUNCTION(IloNum) IloScalProd(const IloIntArray vals1, const IloNumArray vals2);

ILO_EXPORTEDFUNCTION(IloNum) IloScalProd(const IloNumArray vals1, const IloIntArray vals2);

ILO_EXPORTEDFUNCTION(IloInt) IloScalProd(const IloIntArray vals1, const IloIntArray vals2);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloSum(const IloNumExprArray exprs);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloSum(const IloIntExprArray exprs);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloSum(const IloNumVarArray exprs); 

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloSum(const IloIntVarArray exprs); 

ILO_EXPORTEDFUNCTION(IloNum) IloSum(const IloNumArray values);

ILO_EXPORTEDFUNCTION(IloInt) IloSum(const IloIntArray values);

ILO_EXPORTEDFUNCTION(IloNum) IloMax(const IloNumArray vals);

ILO_EXPORTEDFUNCTION(IloInt) IloMax(const IloIntArray vals);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMax(const IloNumExprArray exprs);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMax(const IloIntExprArray exprs);
ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMax(const IloNumVarArray exprs); 
ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMax(const IloIntVarArray exprs); 

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMax(const IloNumExprArg x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMax(const IloNumExprArg x, IloNum y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMax(IloNum x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMax(const IloIntExprArg x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMax(const IloIntExprArg x, IloInt y);

inline IloNumExprArg IloMax(const IloIntExprArg x, IloNum y) {
	return IloMax( IloNumExprArg(x), y);
}

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMax(const IloIntExprArg x, int y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMax(IloInt x, const IloIntExprArg y);

inline IloNumExprArg IloMax(IloNum x, const IloIntExprArg y) {
	return IloMax(x, IloNumExprArg(y));
}

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMax(int x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloNum) IloMin(const IloNumArray vals);

ILO_EXPORTEDFUNCTION(IloInt) IloMin(const IloIntArray vals);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMin(const IloNumExprArray exprs);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMin(const IloIntExprArray exprs);
ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMin(const IloNumVarArray exprs); 
ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMin(const IloIntVarArray exprs); 

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMin(const IloNumExprArg x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMin(const IloNumExprArg x, IloNum y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMin(IloNum x, const IloNumExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMin(const IloIntExprArg x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMin(const IloIntExprArg x, IloInt y);

inline IloNumExprArg IloMin(const IloIntExprArg x, IloNum y) {
	return IloMin( IloNumExprArg(x), y);
}

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMin(const IloIntExprArg x, int y);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMin(IloInt x, const IloIntExprArg y);

inline IloNumExprArg IloMin(IloNum x, const IloIntExprArg y) {
	return IloMin(x, IloNumExprArg(y));
}

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloMin(int x, const IloIntExprArg y);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloSin(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloCos(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloTan(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloArcSin(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloArcCos(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloArcTan(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloSquare(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloSquare(const IloIntExprArg arg);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloSgn(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloAbs(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloAbs(const IloIntExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloExponent(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloLog(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloPower(const IloNumExprArg base, const IloNumExprArg exponent);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloPower(const IloNumExprArg base, IloNum exponent);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloPower(IloNum base, const IloNumExprArg exponent);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloPiecewiseLinear(const IloNumExprArg node,
													   const IloNumArray point,
													   const IloNumArray slope,
													   IloNum a,
													   IloNum fa);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloPiecewiseLinear(const IloNumExprArg node,
													   IloNum firstSlope,
													   const IloNumArray point,
													   const IloNumArray value,
													   IloNum lastSlope);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloPiecewiseLinear(const IloNumExprArg node,
						       IloNumToNumSegmentFunction f);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloPiecewiseLinear(const IloNumExprArg node,
						       IloNumToNumStepFunction f);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMonotonicDecreasingNumExpr(IloNumExprArg node,
																  IloNumFunction f,
																  IloNumFunction invf);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloMonotonicIncreasingNumExpr(IloNumExprArg node,
																  IloNumFunction f,
																  IloNumFunction invf);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloCeil(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloFloor(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloTrunc(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloDistToInt(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloNumToInt(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloFract(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloOplRound(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloLog10(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloLog2(const IloNumExprArg arg);

ILO_EXPORTEDFUNCTION(int) convertIloNum(char *outbuf, const size_t outbufSize, double r, int prec, int *status_p);

class IloConversionI : public IloExtractableI {
	ILOEXTRMEMBERS2DECL(IloConversionI, IloExtractableI,
		IloNumVarArray, vars,
		IloNumVar::Type, type)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	void removeVar(IloNumVar);

	IloConversionI(IloEnvI* e,
		const IloNumVar var,
		IloNumVar::Type type,
		const char* name);
	virtual ~IloConversionI();

	IloNumVarArray getVars() const {return vars;}
	IloNumVar::Type getType() const {return type;}
};

class IloConversionRemoveVar : public IloChange {
	ILOCHANGEDECL

		IloNumVarI* _var;
	IloConversionI* _convert;
public:
	IloConversionRemoveVar(IloConversionI* conv, IloNumVarI* var) : IloChange(), _var(var), _convert(conv) {}
	IloNumVarI* getVar() const     { return (_var); }
	IloConversionI*  getConversion() const { return (_convert); }
};

class ILO_EXPORTED IloConversion : public IloExtractable {
	ILOEXTRHANDLE(IloConversion,IloExtractable)
public:
	
	IloConversion(const IloEnv env,
		const IloNumVar var,
		IloNumVar::Type t,
		const char* name = 0);
	
	IloConversion(const IloEnv env,
		const IloNumVarArray vars,
		IloNumVar::Type t,
		const char* name = 0);
	
	IloConversion(const IloEnv env,
		const IloIntVarArray vars,
		IloNumVar::Type t,
		const char* name = 0);
};

#define IloRONumVarArray IloNumVarArray
#define IloROIntVarArray IloIntVarArray

class IloConstraintI : public IloIntExprI {
  ILOEXTRDECL
  virtual void whenAddedToCP(const IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE;
  virtual void whenRemovedFromCP(const IloAlgorithm alg, IloAny ptr) const ILO_OVERRIDE;
  static IloAddToCP _addToCP;
  static IloRemoveFromCP _removeFromCP;
public:
  IloConstraintI(IloEnvI* env, const char* name);
  virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
  virtual ~IloConstraintI();
  virtual void end() ILO_OVERRIDE;
  virtual void release() ILO_OVERRIDE;
  
  virtual IloBool isRefCountedExpr() const ILO_OVERRIDE;
  virtual IloConstraintI* getInnerConstraint() const { return (IloConstraintI*)this; }
  virtual IloBool isConstraint() ILO_OVERRIDE {
    return IloTrue;
  }
  
  virtual IloBool hasEditableBounds() const { return IloFalse;}
};

class IloConstConstraintI : public IloConstraintI {
	ILOEXTRDECL
		IloInt _val;
public:
	IloConstConstraintI(IloEnvI* env, const char* name=0)
		:IloConstraintI(env, name), _val(0) {}
	IloConstConstraintI(IloEnvI* env, IloInt val, const char* name=0)
		:IloConstraintI(env, name), _val(val) {}
	virtual IloNum eval(const IloAlgorithm alg) const ILO_OVERRIDE;
	virtual IloBool isTrue() const;
	IloInt getValue() const { return _val;}
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	virtual void display(ILOSTD(ostream&) out) const ILO_OVERRIDE;
	virtual IloExtractableI* makeClone(IloEnvI* env) const ILO_OVERRIDE;
};

class ILO_EXPORTED IloConstraint : public IloIntExprArg {
	ILOEXTRHANDLE(IloConstraint,IloIntExprArg)
public:
	IloConstraint(const IloConstraint& ct) : IloIntExprArg(ct){}
        IloConstraint& operator=(const IloConstraint& other) {
          _impl = other.getImpl();
          return *this;
        }
};

class ILO_EXPORTED IloConstraintArray : public IloExtractableArray {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloConstraintArray(IloDefaultArrayI* i=0) : IloExtractableArray(i) {}
	
	IloConstraintArray(const IloConstraintArray& copy) : IloExtractableArray(copy)  {}
	
	IloConstraintArray(const IloEnv env, IloInt n = 0) : IloExtractableArray(env, n)    {}
	IloConstraintArray(const IloMemoryManager env, IloInt n = 0) : IloExtractableArray(env, n)    {}

	IloConstraintArray(const IloEnv env, IloInt n, const IloConstraint v0);
#ifdef ILOALLOWVARARGS
	
	IloConstraintArray(const IloEnv env, IloInt n, const IloConstraint v0, const IloConstraint v1 ...);
#else 
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2,
		const IloConstraint v3);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2,
		const IloConstraint v3, const IloConstraint v4);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2,
		const IloConstraint v3, const IloConstraint v4, const IloConstraint v5);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2,
		const IloConstraint v3, const IloConstraint v4, const IloConstraint v5,
		const IloConstraint v6);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2,
		const IloConstraint v3, const IloConstraint v4, const IloConstraint v5,
		const IloConstraint v6, const IloConstraint v7);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2,
		const IloConstraint v3, const IloConstraint v4, const IloConstraint v5,
		const IloConstraint v6, const IloConstraint v7, const IloConstraint v8);
	IloConstraintArray(const IloEnv env, IloInt n,
		const IloConstraint v0, const IloConstraint v1, const IloConstraint v2,
		const IloConstraint v3, const IloConstraint v4, const IloConstraint v5,
		const IloConstraint v6, const IloConstraint v7, const IloConstraint v8,
		const IloConstraint v9);
#endif

	
	void add(const IloConstraintArray x) { IloExtractableArray::add (x); }
	
	void add(const IloConstraint x) { IloExtractableArray::add (x); }
	
	void add(IloInt more, const IloConstraint x) { IloExtractableArray::add (more, x); }

	

	IloConstraint& operator[] (IloInt i) {
		return (*(IloConstraint*)&(IloExtractableArray::operator[](i)));
	}
	
	IloConstraint  operator[] (IloInt i) const {
		return ((IloConstraintI*)((IloExtractableArray::operator[](i)).getImpl()));
	}
};

typedef IloDefaultArrayI IloConstraintArrayI;

class IloAddNumVar : public IloAddExtractable {
	ILOCHANGEDECL
public:
	virtual ~IloAddNumVar();
	const IloNumVarI* getVar() const    { return ((const IloNumVarI*)(void*)getObj());}
};

class IloAddNumColumn : public IloAddNumVar {
	ILOCOLUMNDECL
		IloEnvI*            _env;
	IloAddNumVar*       _first;
	IloAddValueToRange* _firstRange;      
	

	IloEnvI* getEnv() const ILO_OVERRIDE { return _env; }
	const IloExtractableI* what() const ILO_OVERRIDE;
	void apply() const ILO_OVERRIDE;

	virtual void setObj(const IloExtractableI* obj) ILO_OVERRIDE;

	friend class IloNumColumnI;

	void add(const IloAddNumVar& rhs);
	void add(const IloAddValueToRange& rhs);
	void addCopy (const IloAddNumColumn* cpy);

public:
	virtual ~IloAddNumColumn();
	IloAddNumColumn(IloEnvI* env)
		:_env(env),_first(0),_firstRange(0) {}
	void clear();

	const IloAddNumVar*       getFirst() const      { return (_first); }
	const IloAddValueToRange* getFirstRange() const { return (_firstRange); }
};

class IloNumColumnI : public IloEnvObjectI {
	friend class IloNumColumn;
	IloAddNumColumn*    _addNumColumn;

	IloNumColumnI(const IloNumColumnI&);

	void add(const IloAddNumVar& rhs) { _addNumColumn->add(rhs); }
	void add(const IloAddValueToRange& rhs) { _addNumColumn->add(rhs); }
	void addCopy (const IloNumColumnI* cpy) {
		_addNumColumn->addCopy(cpy->_addNumColumn);
	}

public:
	virtual ~IloNumColumnI();
	IloNumColumnI (IloEnvI* env);
	const IloAddNumVar& getAddNumColumn() const {
		return *_addNumColumn;
	}
	void clear() {
		IloAssert(_addNumColumn, "Invalid IloNumColumnI");
		_addNumColumn->clear();
	}
};

class ILO_EXPORTED IloNumColumn {
	IloNumColumnI* _impl;
public:
	
	IloNumColumn(const IloEnv env);
	IloNumColumn(IloNumColumnI* impl = 0) : _impl(impl) {}

	
	IloNumColumn(const IloAddNumVar& var);
	IloNumColumn(const IloAddValueToRange& var);

	IloNumColumnI* getImpl() const { return _impl; }

	
	operator const IloAddNumVar&() const  {
		IloAssert(getImpl(), "Using empty IloNumColumn handle.");
		return (getImpl()->getAddNumColumn());
	}

	
	void clear() const {
		IloAssert(getImpl(), "Using empty IloNumColumn handle.");
		getImpl()->clear();
	}

	void end();

	
	IloNumColumn& operator+= (const IloNumColumn& rhs) {
		IloAssert(getImpl(), "Using empty IloNumColumn handle.");
		IloAssert(rhs.getImpl(), "adding empty handle column");
		IloAssert(getImpl()->getEnv() == rhs.getImpl()->getEnv(),
			"column elements must be from the same model");
		getImpl()->addCopy(rhs.getImpl());
		return (*this);
	}

	
	IloNumColumn& operator+= (const IloAddNumVar& rhs) {
		IloAssert(getImpl(), "Using empty IloNumColumn handle.");
		IloAssert(getImpl()->getEnv() == rhs.getEnv(),
			"column elements must be from the same model");
		getImpl()->add (rhs);
		return (*this);
	}

	
	IloNumColumn& operator+= (const IloAddValueToRange& rhs);
};

inline const IloAddNumVar& operator+ (const IloAddNumVar& one,const IloAddNumVar& two) {
	return ((const IloAddNumVar&)( (const IloAddExtractable&)(one)
		+ (const IloAddExtractable&)(two)));
}

inline const IloAddNumVar& operator+ (const IloAddNumVar& one, const IloNumColumn& two) {
	IloAssert(two.getImpl(), "Using empty handle IloNumColumn");
	return (one + two.getImpl()->getAddNumColumn());
}

inline const IloAddNumVar& operator+ (const IloNumColumn& one, const IloAddNumVar& two) {
	IloAssert(one.getImpl(), "Using empty handle IloNumColumn");
	return (one.getImpl()->getAddNumColumn() + two);
}

inline const IloAddNumVar& operator+ (const IloNumColumn& one, const IloNumColumn& two) {
	IloAssert(one.getImpl(), "Using empty handle IloNumColumn");
	IloAssert(two.getImpl(), "Using empty handle IloNumColumn");
	return (one.getImpl()->getAddNumColumn() + two.getImpl()->getAddNumColumn());
}

typedef IloArray<IloNumColumn> IloNumColumnArrayBase;

class IloNumColumnArray : public IloNumColumnArrayBase {
public:
	typedef IloDefaultArrayI ImplClass;
	
	IloNumColumnArray(IloDefaultArrayI* i=0) : IloNumColumnArrayBase(i) {}
	
	IloNumColumnArray(const IloEnv env, IloInt n = 0) : IloNumColumnArrayBase(env, n) {}
	IloNumColumnArray(const IloMemoryManager env, IloInt n = 0) : IloNumColumnArrayBase(env, n) {}

	
	IloNumColumnArray(const IloNumColumnArray& h) : IloNumColumnArrayBase(h) {}

	
};

typedef IloDefaultArrayI IloNumColumnArrayI;

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloCount(const IloIntExprArray x,
											 IloInt value);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloStandardDeviation(const IloIntExprArray x);

ILO_EXPORTEDFUNCTION(IloNumExprArg) IloStandardDeviation(const IloIntExprArray x, IloNum meanLB, IloNum meanUB);

ILO_EXPORTEDFUNCTION(IloIntExprArg) IloCountDifferent(const IloIntExprArray x);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
