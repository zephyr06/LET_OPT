// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilsched/iloserror.h
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

#ifndef __CONCERT_iloserrorH
#define __CONCERT_iloserrorH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloalg.h>

#define ILOSEXCEPTION				\
  public:					\
  virtual void raise() ILO_OVERRIDE { throw *this; }

class IloSchedException :public IloException {
private:
	const char* _file;
	const char* _func;
	IloInt      _line;
        const char* _tmpMessage;
public:
	IloSchedException(const IloSchedException& exc)
		:IloException(exc)
		,_file(exc._file)
		,_func(exc._func)
		,_line(exc._line)
		,_tmpMessage(0)
        {}
	virtual ~IloSchedException();
	void setFile     (const char* file) { _file = file; }
	void setFunction (const char* func) { _func = func; }
	void setLine     (IloInt line)      { _line = line; }
	const char* getFile()     const { return _file; }
	const char* getFunction() const { return _func; }
	IloInt      getLine()     const { return _line; }
	void fullPrint(ILOSTD(ostream)& out) const;
	virtual void raise() { throw *this; }
	virtual const char* getMessage() const ILO_OVERRIDE;
protected:
	IloSchedException(const char* message   = 0, 
		IloBool deleteMessage = IloFalse):
          IloException(message, deleteMessage),
          _file(0),
          _func(0),
          _line(0),
          _tmpMessage(0)
        {}
};

#if defined(ILO_SUN55) || defined(ILO_SUNAMD64) || defined(__sun)

#define ILOS_THROW_INIT(_exc)	    \
	_exc.setLine    (__LINE__);	    \
	_exc.setFile    (__FILE__);
#else
#define ILOS_THROW_INIT(_exc)	    \
	_exc.setLine    (__LINE__);	    \
	_exc.setFile    (__FILE__);	    \
	_exc.setFunction(__FUNCTION__);
#endif

#if defined(ILOS_VERBOSE_EXCEPTION) 
#define ILOS_THROW(_exception)			\
{						\
	const IloSchedException& excc = _exception;	\
	IloSchedException& exc = (IloSchedException&)excc;	\
	ILOS_THROW_INIT(exc);			\
	exc.fullPrint(ILOSTD(cerr));		\
	exc.raise();				\
}
#else
#define ILOS_THROW(_exception)			\
{						\
	const IloSchedException& excc = _exception;	\
	IloSchedException& exc = (IloSchedException&)excc;	\
	ILOS_THROW_INIT(exc);			\
	exc.raise();				\
}
#endif 

#define ILOS_EXC0(Name, RName, MSG)		\
class Name :public RName {			\
public:						\
	Name () :RName ( MSG ) {}			\
	Name (const char* m, IloBool del = IloFalse)	\
	: RName (m, del) {}				\
	virtual ~ Name (){}				\
	ILOSEXCEPTION					\
}

#define ILOS_EXC1(Name, RName, MSG, Obj)				\
class Obj;								\
class Name :public RName {						\
private:								\
	const char* _msg;							\
	Obj* _obj1;								\
public:				       			\
	Name (const Obj* obj1 =0)						\
	:RName(), _msg( MSG ), _obj1( (Obj*)obj1) {}			\
	virtual ~ Name (){}							\
	const char* getMsg() const { return _msg; }				\
	Obj* getObj1() const { return _obj1; }				\
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;			\
	ILOSEXCEPTION							\
}

#define ILOS_EXC2(Name, RName, MSG, Obj1, Obj2)			\
class Obj1;							\
class Obj2;							\
class Name :public RName {					\
private:							\
	const char* _msg;						\
	Obj1* _obj1;\
	Obj2* _obj2;								\
public:									\
	Name (const Obj1* obj1 =0, const Obj2* obj2 =0)			\
	:RName(), _msg( MSG ), _obj1( (Obj1*)obj1) , _obj2( (Obj2*)obj2) {}	\
	virtual ~ Name (){}							\
	const char* getMsg() const { return _msg; }				\
	Obj1* getObj1() const { return _obj1; }				\
	Obj2* getObj2() const { return _obj2; }				\
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;			\
	ILOSEXCEPTION								\
}

#define ILOS_EXCS(Name, RName, MSG)			\
class Obj;						\
class Name :public RName {				\
private:						\
	const char* _msg;					\
	const char* _msg1;					\
public:							\
	Name (const char* msg1 =0)				\
	:RName(), _msg( MSG ), _msg1( msg1) {}		\
	virtual ~ Name (){}					\
	const char* getMsg() const { return _msg; }	      	\
	const char* getMsg1() const { return _msg1; }	      	\
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE {	\
	out << _msg;					\
	if (0 != _msg1)					\
	out << ": " << _msg1;				\
}							\
	ILOSEXCEPTION						\
}

#define ILOS_EXCINTARGUMENT(Name, RName, MSG, Obj)				\
class Obj;								\
class Name :public RName {						\
private:								\
	const char* _msg;							\
	Obj* _obj1;								\
	IloInt _argument;								\
public:									\
	Name (IloInt argument, const Obj* obj1 =0)			\
	:RName(),_msg(MSG),_obj1((Obj*)obj1),_argument(argument) {}		\
	virtual ~ Name (){}							\
	const char* getMsg() const { return _msg; }	      	\
	IloInt getArgument() const { return _argument;}        \
	Obj* getObj1() const { return _obj1; }				\
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;			\
	ILOSEXCEPTION								\
}

#define ILOS_EXCRANGE(Name, RName, MSG, Obj)				\
class Obj;								\
class Name :public RName {						\
private:								\
	const char* _msg;							\
	Obj* _obj1;								\
	IloInt _min;								\
	IloInt _max;								\
public:									\
	Name (IloInt min, IloInt max, const Obj* obj1 =0)			\
	:RName(),_msg(MSG),_obj1((Obj*)obj1),_min(min),_max(max) {}		\
	virtual ~ Name (){}							\
	const char* getMsg() const { return _msg; }	      	\
	Obj* getObj1() const { return _obj1; }				\
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;			\
	ILOSEXCEPTION								\
}

#define ILOS_IMPL_EXC1(Name)		\
	void Name ::print(ILOSTD(ostream)& out) const {	\
	out << _msg;					\
	if (0 != _obj1) {				\
	out << ": ";				\
	_obj1->display(out);			\
	}						\
}

#define ILOS_IMPL_EXC2(Name)		\
	void Name ::print(ILOSTD(ostream)& out) const {	\
	out << _msg;					\
	if (0 != _obj1) {				\
	out << ": ";				\
	_obj1->display(out);			\
	}	                                        \
	if (0 != _obj2) {				\
	out << ": ";				\
	_obj2->display(out);			\
	}	                                        \
}

#define ILOS_IMPL_EXCINTARGUMENT(Name)		\
	void Name ::print(ILOSTD(ostream)& out) const {	\
	out << _msg;					\
	if (0 != _obj1) {				\
	out << ": ";				\
	_obj1->display(out);			\
	}						\
	out << ": " << _argument;          \
}

#define ILOS_IMPL_EXCRANGE(Name)		\
	void Name ::print(ILOSTD(ostream)& out) const {	\
	out << _msg;					\
	if (0 != _obj1) {				\
	out << ": ";				\
	_obj1->display(out);			\
	}						\
	out << ": [" << _min << ", " << _max << "]"; \
}

ILOS_EXC0(IloNotYetImplementedException, IloSchedException,   "Not yet implemented");
ILOS_EXC0(IloModelException,             IloSchedException,   "Model exception");
ILOS_EXC0(IloExtractionException,        IloSchedException,   "Extraction exception");
ILOS_EXC0(IloEngineException,            IloSchedException,   "Engine exception");
ILOS_EXC0(IloSolutionException,          IloSchedException,   "Solution exception");
ILOS_EXC0(IloPropagationException,       IloEngineException,  "Propagation exception");
ILOS_EXC0(IloSearchException,            IloEngineException,  "Search exception");
ILOS_EXC0(IloMultipleObjectiveException, IloEngineException,  "Cannot handle multiple objectives");
ILOS_EXC0(IloLockedScopeException,       IloEngineException,  "Scope already locked");
ILOS_EXC0(IloTerminalScopeException,     IloEngineException,  "Scope cannot be used as base scope");
ILOS_EXC0(IloNoCPLEXException,           IloSearchException,  "No CPLEX, please compile with flag ILO_CP_USE_CPLEX");
ILOS_EXC1(IloSolutionAddSearchException, IloSearchException,  "Cannot add solution elements during search", IloSolutionI);
ILOS_EXC1(IloAddCtSolutionException,     IloSolutionException,"Cannot add an IloConstraint to IloSolution", IloConstraintI);
ILOS_EXC1(IloCopySolutionException,      IloSolutionException,"Extractable differ in copy of IloSolution", IloExtractableI);
ILOS_EXC0(IloSegmentedFunctionException, IloSchedException,   "Invalid segmented function");
ILOS_EXC1(IloObsoleteIntervalVarException, IloModelException, "Cannot access values of obsolete variable", IloIntervalVarI);
ILOS_EXC1(IloNotSupportedExpressionException, IloModelException, "Expression is not supported", IloNumExprI);
ILOS_EXCS(IloNoPGException,              IloEngineException,  "Function only available on precedence graph during search");
ILOS_EXC0(IloStateException,             IloEngineException,  "Bad state setting");
ILOS_EXC0(IloLockedTimetableException,   IloEngineException,  "Timetable modification on a locked timetable");
ILOS_EXC0(IloInvalidParameterException,  IloSchedException,   "Invalid parameter");
ILOS_EXC2(IlosAtomFunctionNotMatching,   IloModelException,   "Cannot add solution elements during search", IloCumulFunctionExprI, IloCumulAtomI);
ILOS_EXCINTARGUMENT(IloInvalidTransitionIndex, IloModelException, "Invalid index in transition distance", IloTransitionDistanceI);

class IloAlgorithmI;
class IloSegmentedFunctionI;

class IloIntervalBoundsException :public IloAlgorithm::CannotExtractException {
public:
	IloIntervalBoundsException(IloAlgorithmI* alg, 
		IloExtractableI* expr, 
		IloInt bound);
	virtual ~IloIntervalBoundsException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
	IloInt _bound;
};

class IloNotSemiConvexException :public IloAlgorithm::CannotExtractException {
public:
	IloNotSemiConvexException(IloAlgorithmI* alg, 
		IloExtractableI* expr, 
		IloSegmentedFunctionI* func);
	virtual ~IloNotSemiConvexException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
	IloSegmentedFunctionI* _func;
};

class IloNotIntegerIntensityException :public IloAlgorithm::CannotExtractException {
public:
	IloNotIntegerIntensityException(IloAlgorithmI* alg, 
		IloExtractableI* expr, 
		IloSegmentedFunctionI* func);
	virtual ~IloNotIntegerIntensityException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
	IloSegmentedFunctionI* _func;
};

class IloNegativeIntensityException :public IloAlgorithm::CannotExtractException {
public:
	IloNegativeIntensityException(IloAlgorithmI* alg, 
		IloExtractableI* expr, 
		IloSegmentedFunctionI* func);
	virtual ~IloNegativeIntensityException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
	IloSegmentedFunctionI* _func;
};

class IloIntensityExceedsGranularityException :public IloAlgorithm::CannotExtractException {
public:
	IloIntensityExceedsGranularityException(IloAlgorithmI* alg, 
		IloExtractableI* expr, 
		IloSegmentedFunctionI* func);
	virtual ~IloIntensityExceedsGranularityException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
private:
	IloSegmentedFunctionI* _func;
};

class IloSequenceWrongTypesException: public IloAlgorithm::CannotExtractException {
protected:
	IloInt _nbIntervals;
	IloInt _nbTypes;
public:
	IloSequenceWrongTypesException(IloAlgorithmI* alg, IloExtractableI* sequence, 
		IloInt nbIntervals, IloInt nbTypes);
	virtual ~IloSequenceWrongTypesException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
};

class IloTransitionDistanceTupleException: public IloAlgorithm::CannotExtractException {
protected:
	IloInt _id1;
	IloInt _id2;
	IloInt _dist;
public:
	IloTransitionDistanceTupleException(IloAlgorithmI* alg, IloExtractableI* extr, 
		IloInt id1, IloInt id2, IloInt dist);
	virtual ~IloTransitionDistanceTupleException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	IloInt getIndex1()   const { return _id1; }
	IloInt getIndex2()   const { return _id2; }
	IloInt getDistance() const { return _dist; }
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
};

class IloNegativeStateException: public IloAlgorithm::CannotExtractException {
protected:
	IloInt _state;
public:
	IloNegativeStateException(IloAlgorithmI* alg, IloExtractableI* extr, 
		IloInt state);
	virtual ~IloNegativeStateException(){}
	virtual const char* getMessage() const ILO_OVERRIDE;
	IloInt getState()   const { return _state; }
	virtual void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
