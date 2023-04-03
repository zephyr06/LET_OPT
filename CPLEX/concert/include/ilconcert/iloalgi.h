// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloalgi.h
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

#ifndef __CONCERT_iloalgiH
#define __CONCERT_iloalgiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilomodeli.h>
#include <ilconcert/ilolinear.h>

#define ILO_RTC_36102

class IloSolutionOperationsI;
class IloModelEvaluatorI;
class IloEvalOpenIntIterator;
class IloIntSetVarI;
class IloAlgorithmProfilerCallbackI;

class IloAlgorithmExtractCallbackI : public IloRttiEnvObjectI {
public:
	IloAlgorithmExtractCallbackI(IloEnvI* env) : IloRttiEnvObjectI(env) {}
	virtual void prepareExtraction() = 0;
	virtual void endExtraction() = 0;
};

class IloSolutionManagerI : public IloPlugIn {
	ILORTTIDECL
private:
	IloAlgorithmI           *_algorithm;
	IloSolutionOperationsI **_opTable;
	IloInt                   _opTableSize;
	IloSolutionOperationsI  *_defaults;

	void sizeTo(IloTypeIndex);
	void fillClone(IloSolutionManagerI *) const;
public:
	IloSolutionManagerI(IloAlgorithmI *algorithm);
	IloSolutionManagerI(IloEnvI *env);
	~IloSolutionManagerI();
	IloAlgorithmI *getAlgorithm() const { return _algorithm; }
	IloEnvI *getEnv() const { return _env; }
	void setSolutionOps(IloSolutionOperationsI *);
	void removeSolutionOps(IloTypeIndex);
	IloSolutionOperationsI *getSolutionOps(const IloExtractableI *ex) const {
#ifdef ILOSOLUTIONNULLOBJECTS
		if (ex == 0) return _defaults;
#endif
		IloTypeIndex tIndex = ex->getTypeIndex();
		return (tIndex < _opTableSize) ? _opTable[tIndex] : _defaults;
	}

	void setForceCallStore(IloTypeIndex t);
	void setForceCallRestore(IloTypeIndex t);
	void setForceCallIsFeasible(IloTypeIndex t);
	void clearForceCallStore(IloTypeIndex t);
	void clearForceCallRestore(IloTypeIndex t);
	void clearForceCallIsFeasible(IloTypeIndex t);
	IloBool forceCallStore(IloTypeIndex t) const;
	IloBool forceCallRestore(IloTypeIndex t) const;
	IloBool forceCallIsFeasible(IloTypeIndex t) const;

	IloSolutionOperationsI *getDefaults() const { return _defaults; }
	IloSolutionManagerI *makeClone(IloEnvI *env) const;
	IloSolutionManagerI *makeClone(IloAlgorithmI *alg) const;
};

class IloAlgorithmDataVectorI {
public:
	virtual ~IloAlgorithmDataVectorI(){};
	virtual void    clear()=0;
	virtual IloBool hasData(IloInt id) const=0;
	virtual void*   getData(IloInt id) const=0;
	virtual void    setData(IloInt id, void* data)=0;
	virtual void    saveData(IloAlgorithmI* a, const IloExtractableI* o)=0;
};

class IloAlgorithmI {

	friend class IloEnvI;
	friend class IloModelI;
	friend class IloAlgorithm;
	friend class IloSolverForallOp;
	friend class IloFastUseVisitor;

	IloEnvI*       _env;
protected:
	IloModelI*     _model;
public:
	IloAlgorithmI* _next;
#if defined(ILOTOMOVE)
	IloTimer       _time;
#endif
	ILOSTD(ostream)* _out;
	ILOSTD(ostream)* _war;
	ILOSTD(ostream)* _err;
#if defined(ILOTOMOVE)
protected:
	void** _datadata;
	IloInt* _datausecnt;
	IloInt _datasize;
	void setMaxId();
	void setMaxId(IloInt id);
	IloAlgorithmDataVectorI* _datavector;
private:
	IloExtractableArray _remFailed;
	IloExtractableArray _extFailed;
public:
	void addExtFailed(IloExtractable obj) {_extFailed.add(obj);}
	void addRemFailed(IloExtractable obj) {_remFailed.add(obj);}
#ifdef ILO_RTC_36102
  IloExtractableArray getExtFailed() const { return _extFailed; }
#endif
#endif

protected:
	IloBool _isInSearch;
	IloSolutionManagerI* _solutionManager;
	IloModelEvaluatorI* _evaluator;
	IloAlgorithmExtractCallbackI* _extractCallback;
public:
	virtual IloEvalOpenIntIterator* makeOpenIterator(IloGenAlloc* heap,
		IloIntSetVarI* var);
public:
#if defined(ILOTOMOVE)
	void  setData(IloInt id, void* data)  {
		if (_datavector == 0)
			_datadata[id] = data;
		else
			_datavector->setData(id, data);
	}
	void* getData(IloInt id) const        {
		return ((_datavector == 0)
			? (_datadata[id])
			: _datavector->getData(id));
	}
	IloInt getDataSize() const            { return (_datasize); }
	IloInt  fastuse    (IloInt id)           { return (++_datausecnt[id]); }
	IloInt  fastrelease(IloInt id)           { return (--_datausecnt[id]); }
	IloBool fastuses(IloInt id) const     { return (_datausecnt[id] > 0); }
	IloBool uses    (IloInt id) const     {
		return (_datavector == 0) ? (id < _datasize && _datausecnt[id] > 0) : _datavector->hasData(id);
	}
	IloBool usesAtLeastOne(IloExtractableArray) const;
	void unuse(IloInt id) {
		_datausecnt[id] = 0;
	}

	IloBool isPushed(IloAlgorithmDataVectorI* dv) const {
		return (_datavector == dv);
	}
	void pushDataVector(IloAlgorithmDataVectorI* dv) {
		_datavector = dv;
	}  
	IloAlgorithmDataVectorI* popDataVector() {
		IloAlgorithmDataVectorI* dv = _datavector;
		_datavector = 0;
		return dv;
	}

#endif
	IloAlgorithmI(IloEnvI*);
	virtual ~IloAlgorithmI();

	IloEnvI*   getEnv() const     { return _env; }
	IloModelI* getModel() const   { return _model; }
	IloModelEvaluatorI* getModelEvaluator();

	class IloExpanderI* getExpander() const { return _env->getExpander(); }
	void unlinkEvaluator();

	ILOSTD(ostream)& out() const       { return *(((IloAlgorithmI*)this)->_out); }
	void setOut(ILOSTD(ostream)& s);
	ILOSTD(ostream)& warning() const   {
		return *(((IloAlgorithmI*)this)->_war);
	}
	void setWarning(ILOSTD(ostream)& s){ _war = &s; }
	ILOSTD(ostream)& error() const     { return *(((IloAlgorithmI*)this)->_err); }
	void setError(ILOSTD(ostream)& s)  { _err = &s; }
	virtual void setOutCallback();

	virtual void initTime();
	virtual IloNum getTime() const;
	virtual void   printTime() const;
	virtual void   resetTime();

	virtual void extract(const IloExtractableI* obj);
	void extract(const IloExtractableArray&);

	IloSolutionManagerI *getSolutionManager() const { return _solutionManager; }
	IloSolutionOperationsI *getSolutionOperations(const IloExtractableI *obj) const {
		return _solutionManager->getSolutionOps(obj);
	}
	void createSolutionManager(const char *name);
	void destroySolutionManager(const char *name);

	virtual void apply    (const IloChange& chg, const IloExtractableI* obj);
	virtual void apply    (const IloChange& chg);
	virtual void remove   (const IloExtractableI* obj);
	void remove(const IloExtractableArray&);

	virtual IloBool doExtract(const IloExtractableI* obj, void**) = 0;
	virtual IloBool doRemove(const IloExtractableI*, void*) = 0;
	virtual IloBool doApply(const IloChange* chg, void** data,
		const IloExtractableI* obj) = 0;

	virtual void handleForAllElement(void *) { }
	IloExtractableI * addTemporaryExtractable(IloExtractableI * ex) { return ex; }

	virtual void decreaseUsage (const IloExtractableI* );

	virtual void load(const IloModelI* con);
	virtual void loadWithoutClear(const IloModelI* con);

	virtual IloBool solve();
	virtual IloAlgorithm::Status getStatus() const;
	virtual void clear();
	virtual IloNum getObjValue() const = 0;
	virtual IloNum getValue(const IloNumVarI*) const = 0;
	virtual IloInt getIntValue(const IloNumVarI*) const;
	virtual IloNum getValue(const IloObjectiveI*) const = 0;
	virtual IloNum getValue(const IloConstraintI*) const;
	virtual void getValues(const IloNumVarArray, IloNumArray) const;
	virtual void getValues(const IloIntVarArray, IloNumArray) const;
	virtual void getIntValues(const IloIntVarArray, IloIntArray) const;
	virtual IloNum getExprValue(const IloNumExprI* expr) const;

	IloMemoryManagerI* getMemoryManager() const { return getEnv()->getGeneralAllocator(); }
	ILOEXTENSIONI
public:
	void setModel(IloModelI*);
	void setExtractCallback(IloAlgorithmExtractCallbackI* extractCallback){ _extractCallback = extractCallback; }
	IloBool hasExtractCallback() const { return _extractCallback != 0; }
  IloBool callSolve();

};

class IloAlgorithmProfilerCallbackI : public IloRttiEnvObjectI {
public:
	IloAlgorithmProfilerCallbackI(IloEnvI* env) : IloRttiEnvObjectI(env) {}
	static const char* PROPERTY_NAME; 
	virtual void enterExtraction(const IloExtractableI*) = 0;
	virtual void exitExtraction(const IloExtractableI*) = 0;

};

class IloIntervalVarI;
class IloIntervalSequenceVarI;
class IloCumulFunctionExprI;
class IloStateFunctionExprI;

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

