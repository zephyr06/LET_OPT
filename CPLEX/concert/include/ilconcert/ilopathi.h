// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilopathi.h
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

#ifndef __CONCERT_ilopathiH
#define __CONCERT_ilopathiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilomodel.h>

class IloPathTransitEvalI : public IloPathTransitI{
	IloPathTransitFunction  _transit;
public:
	IloPathTransitEvalI(IloPathTransitFunction function) 
		: _transit(function) {};
	virtual ~IloPathTransitEvalI();
	virtual IloNum transit(IloInt i, IloInt j) ILO_OVERRIDE;
	virtual IloPathTransitI* makeClone(IloEnvI*) const ILO_OVERRIDE;
};

class IloPathLengthI : public IloConstraintI {
	ILOEXTRDECL

		IloIntVarArray   _nextVarArray;
	IloNumVarArray   _cumulVarArray;
	IloPathTransitI* _pathTransit;
	IloInt           _nbPaths;
	IloBool          _deleteTransit;

	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloPathLengthI(IloEnvI*               env,
		const IloIntVarArray   nexts,
		const IloNumVarArray   cumul,
		IloPathTransitFunction transit,
		IloInt                 nbPaths,
		const char*            name=0);
	IloPathLengthI(IloEnvI*         env,
		const IloIntVarArray  nexts,
		const IloNumVarArray  cumul,
		IloPathTransitI* pathTransit,
		IloInt           nbPaths,
		const char*      name=0);

	virtual ~IloPathLengthI();

	ILOEXTROTHERDECL

		IloNumVarArray getCumulVarArray() const {return _cumulVarArray;}
	IloIntVarArray getNextVarArray() const {return _nextVarArray;}
	IloPathTransitI* getPathTransit() const {return _pathTransit;}
	IloInt getNbPaths() const {return _nbPaths;}
};

class IloDistributeI : public IloConstraintI {
	ILOEXTRDECL

	IloIntExprArray  _varArray;
	IloIntExprArray  _cardVarArray;
	IloIntArray     _valueArray;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:

	IloDistributeI(IloEnvI*       env,
		const IloIntExprArray cards,
		const IloIntExprArray vars,
		const IloIntArray    values,
		const char*    name);

	IloDistributeI(IloEnvI*       env,
		const IloIntExprArray cards,
		const IloIntExprArray vars,
		const char* name);

	virtual ~IloDistributeI();

	ILOEXTROTHERDECL

	IloIntExprArray getCardVarArray() const {return _cardVarArray;}
	IloIntExprArray getVarArray() const {return _varArray;}
	IloIntArray getValueArray() const {return _valueArray;}
};

class IloIntAbstractionI : public IloConstraintI {
	ILOEXTRDECL

		IloIntVarArray  _avarArray;
	IloIntVarArray  _varArray;
	IloIntArray     _valueArray;
	IloInt          _abstractValue;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloIntAbstractionI(IloEnvI*       env,
		const IloIntVarArray avars,
		const IloIntVarArray vars,
		const IloIntArray    values,
		IloInt abstractValue,
		const char*    name);

	virtual ~IloIntAbstractionI();

	ILOEXTROTHERDECL

		IloIntVarArray getAVarArray() const {return _avarArray;}
	IloIntVarArray getVarArray() const {return _varArray;}
	IloIntArray getValueArray() const {return _valueArray;}
	IloInt getAbstractValue() const {return _abstractValue;}
};

class IloIntAbstraction : public IloConstraint {
	ILOEXTRHANDLE(IloIntAbstraction,IloConstraint)
public:
	IloIntAbstraction(const IloEnv            env,
		const IloIntVarArray avars,
		const IloIntVarArray vars,
		const IloIntArray    values,
		IloInt    abstractValue,
		const char*    name = 0);
	class InvalidArraysException : public IloException {
		
	public:
		InvalidArraysException(IloIntAbstractionI*)
			: IloException("IloIntAbstraction: invalid array sizes")
			
      {}
	};
};

class IloBoolIntAbstractionI : public IloConstraintI {
	ILOEXTRDECL

		IloBoolVarArray  _avarArray;
	IloIntVarArray  _varArray;
	IloIntArray     _valueArray;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	IloBoolIntAbstractionI(IloEnvI*       env,
		const IloBoolVarArray avars,
		const IloIntVarArray vars,
		const IloIntArray    values,
		const char*    name);

	virtual ~IloBoolIntAbstractionI();

	ILOEXTROTHERDECL

		IloBoolVarArray getAVarArray() const {return _avarArray;}
	IloIntVarArray getVarArray() const {return _varArray;}
	IloIntArray getValueArray() const {return _valueArray;}
};

class IloBoolIntAbstraction : public IloConstraint {
	ILOEXTRHANDLE(IloBoolIntAbstraction,IloConstraint)
public:
	IloBoolIntAbstraction(const IloEnv            env,
		const IloBoolVarArray avars,
		const IloIntVarArray vars,
		const IloIntArray    values,
		const char*    name = 0);
	class InvalidArraysException : public IloException {
		
	public:
		InvalidArraysException(IloBoolIntAbstractionI*)
			: IloException("IloBoolIntAbstraction: invalid array sizes")
			
      {}
	};
};

class IloSequenceI : public IloConstraintI {
	ILOEXTRDECL

		IloInt          _nbMin;
	IloInt          _nbMax;
	IloInt          _seqWidth;
	IloIntVarArray  _varArray;
	IloIntVarArray  _cardVarArray;
	IloIntArray     _valueArray;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:

	IloSequenceI(IloEnvI*         env,
		IloInt          nbMin,
		IloInt          nbMax,
		IloInt          seqWidth,
		const IloIntVarArray vars,
		const IloIntArray    values,
		const IloIntVarArray cards,
		const char*     name);

	virtual ~IloSequenceI();

	ILOEXTROTHERDECL

		IloIntVarArray getCardVarArray() const {return _cardVarArray;}
	IloIntVarArray getVarArray() const {return _varArray;}
	IloIntArray getValueArray() const {return _valueArray;}
	IloInt getNbMin() const {return _nbMin;}
	IloInt getNbMax() const {return _nbMax;}
	IloInt getSeqWidth() const {return _seqWidth;}
};

class IloInverseI : public IloConstraintI {
	ILOEXTRDECL

	IloIntExprArray  _f;
	IloIntExprArray  _invf;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:

	IloInverseI(IloEnvI*       env,
		const IloIntExprArray f,
		const IloIntExprArray invf,
		const char*    name);

	virtual ~IloInverseI();

	ILOEXTROTHERDECL

	IloIntExprArray getVarArray1() const {return _f;}
	IloIntExprArray getVarArray2() const {return _invf;}
};

class IloSetInverseI : public IloConstraintI {
	ILOEXTRDECL

		IloIntSetVarArray  _f;
	IloIntVarArray  _invf;
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:

	IloSetInverseI(IloEnvI*       env,
		const IloIntSetVarArray f,
		const IloIntVarArray invf,
		const char*    name);

	virtual ~IloSetInverseI();

	ILOEXTROTHERDECL

		IloIntSetVarArray getVarArray1() const {return _f;}
	IloIntVarArray getVarArray2() const {return _invf;}
};

class IloStrongI : public IloConstraintI {
  ILOEXTRDECL
  IloIntVarArray  _vars;
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
  IloStrongI(IloEnvI* env, const IloIntVarArray vars, const char* name);
  virtual ~IloStrongI();
  ILOEXTROTHERDECL
  IloIntVarArray getVarArray() const { return _vars; }
}; 

class IloNumArrayMemberI : public IloConstraintI {
	ILOEXTRMEMBERS2DECL(IloNumArrayMemberI,IloConstraintI,
		IloNumExprI*, _expr,
		IloNumArray, _elements)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	virtual ~IloNumArrayMemberI();
	IloNumExprI* getNumExpr() const {return _expr;}
	IloNumArray getElements() const {return _elements;}
};

class IloNumArrayMember : public IloConstraint {
	ILOEXTRHANDLE(IloNumArrayMember,IloConstraint)
public:
	IloNumArrayMember(const IloNumExprArg expr,
		const IloNumArray elements,
		const char* name=0);
};

class IloIntArrayMemberI : public IloConstraintI {
	ILOEXTRMEMBERS2DECL(IloIntArrayMemberI,IloConstraintI,
		IloIntExprI*, _expr,
		IloIntArray, _elements)
		virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
	virtual ~IloIntArrayMemberI();
	IloIntExprI* getIntExpr() const {return _expr;}
	IloIntArray getElements() const {return _elements;}
};

class IloIntArrayMember : public IloConstraint {
	ILOEXTRHANDLE(IloIntArrayMember,IloConstraint)
public:
	IloIntArrayMember(const IloIntExprArg expr,
		const IloIntArray elements,
		const char* name=0);
};

class IloLexicographicI : public IloConstraintI {
	ILOEXTRDECL
private:
	IloIntExprArray _first;
	IloIntExprArray _second;
	IloBool         _strict;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
	IloLexicographicI(IloEnvI * env,
		IloIntExprArray first,
		IloIntExprArray second,
		IloBool strict,
		const char * name);
	~IloLexicographicI();
	IloIntExprArray getFirstArray() const { return _first; }
	IloIntExprArray getSecondArray() const { return _second; }
	IloBool isStrict() const { return _strict; }
	IloExtractableI* makeClone(IloEnvI *) const ILO_OVERRIDE;
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
};

class IloSpreadI : public IloConstraintI {
	ILOEXTRDECL
private:
	IloIntExprArray  _exprArray;
	IloNumExprI*       _mean;
	IloNumExprI*       _standardDeviation;

	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
	virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;

public:
	IloSpreadI(IloEnvI* env,
		IloIntExprArray array,
		IloNumExprI* mean,
		IloNumExprI* standardDeviation,
		const char* name = 0);
	virtual ~IloSpreadI();
	IloIntExprArray getExprArray() const {return _exprArray;}
	IloNumExprI* getMean() const {return _mean;}
	IloNumExprI* getStandardDeviation() const {return _standardDeviation;}
	ILOEXTROTHERDECL
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
