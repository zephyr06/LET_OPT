// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilsched/ilossequencei.h
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

#ifndef __CONCERT_ilossequenceiH
#define __CONCERT_ilossequenceiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/iloexpression.h>
#include <ilconcert/ilsched/ilosbasic.h>
#include <ilconcert/ilsched/ilostimei.h>
#include <ilconcert/iloparami.h>

#define ILO_SAME_SEQUENCE

class IloSequenceNextConstraintI: public IloConstraintI {
  ILOS_CPEXTR_DECL(IloSequenceNextConstraintI, IloConstraintI)
    public:
  IloSequenceNextConstraintI(IloEnvI* env,
                             IloIntervalSequenceVarI* seq, 
                             IloIntervalVarI* prev, 
                             IloIntervalVarI* next);
  virtual ~IloSequenceNextConstraintI(){}
  virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
  IloIntervalSequenceVarI* getSequenceVar() const { return _seq; }
  IloIntervalVarI* getPrev() const { return _prev; } 
  IloIntervalVarI* getNext() const { return _next; }

protected:
  virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;

private:
  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

private:
  IloIntervalSequenceVarI* _seq;
  IloIntervalVarI*         _prev; 
  IloIntervalVarI*         _next;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloSequenceBeforeConstraintI: public IloConstraintI {
  ILOS_CPEXTR_DECL(IloSequenceBeforeConstraintI, IloConstraintI)
    public:
  IloSequenceBeforeConstraintI(IloEnvI* env,
                               IloIntervalSequenceVarI* seq, 
                               IloIntervalVarI* pred, 
                               IloIntervalVarI* succ);
  virtual ~IloSequenceBeforeConstraintI(){}
  virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
  IloIntervalSequenceVarI* getSequenceVar() const { return _seq; }
  IloIntervalVarI* getPredecessor() const { return _pred; } 
  IloIntervalVarI* getSuccessor()   const { return _succ; }
protected:
  virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;

private:
  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

private:
  IloIntervalSequenceVarI* _seq;
  IloIntervalVarI*         _pred; 
  IloIntervalVarI*         _succ;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloSequenceExprI: public IloIntExprI {
  ILOS_CPEXTR_DECL(IloSequenceExprI, IloIntExprI)
  public:
  enum Type { 
    TypeOfNext, 
    StartOfNext, 
    EndOfNext, 
    SizeOfNext, 
    LengthOfNext,
    TypeOfPrev, 
    StartOfPrev, 
    EndOfPrev, 
    SizeOfPrev, 
    LengthOfPrev
  };
  IloSequenceExprI(Type type, 
                   IloIntervalSequenceVarI* seq, 
                   IloIntervalVarI* a,
                   IloInt posVal,
                   IloInt absVal);
  virtual ~IloSequenceExprI(){}
  IloSequenceExprI::Type   getType()        const { return _type; }
  IloIntervalSequenceVarI* getSequenceVar() const { return _seq; }
  IloIntervalVarI*         getIntervalVar() const { return _a; }
  IloInt getExtremePositionValue() const { return _posVal; }
  IloInt getAbsenceValue()         const { return _absVal; }

public:
  virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
  virtual IloNum eval(const IloAlgorithm algo) const ILO_OVERRIDE;
  static IloEvalCP _evalCP;
protected:
  virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;
private:
  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

private:
  Type                      _type;
  IloIntervalSequenceVarI*  _seq;
  IloIntervalVarI*          _a;
  IloInt                    _posVal;
  IloInt                    _absVal;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

class IloNoOverlapI: public IloConstraintI {
  ILOS_CPEXTR_DECL(IloNoOverlapI, IloConstraintI)
    public:
  enum ParamId {
    TransitionDistance =0
  };
public:
  IloNoOverlapI(IloEnvI* env,
		IloIntervalSequenceVarI* seq, 
		IloTransitionDistanceI* tdist,
                IloBool direct,
		const char* name);
  virtual ~IloNoOverlapI(){}
  virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
  virtual void changeParameter(const IloParameterExtractableLinkI* param) ILO_OVERRIDE;
  virtual void endParameter(const IloParameterExtractableLinkI* link) ILO_OVERRIDE;
  IloIntervalSequenceVarI* getSequenceVar() const { return _seq; }
  IloTransitionDistanceI* getTransitionDistanceParam() const { 
    if (0 != _tdistParamLink)
      return (IloTransitionDistanceI*)_tdistParamLink->getParameter();
    return NULL;
  }
  IloBool transitionDistanceOnDirectSuccessors() const { 
    return _direct; 
  }
protected:
  virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;

private:
  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

private:
  IloIntervalSequenceVarI*  _seq;
  IloParameterExtractableLinkI* _tdistParamLink;
  IloBool _direct;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

#if defined(ILO_SAME_SEQUENCE)

class IloSameSequenceI: public IloConstraintI {
  ILOS_CPEXTR_DECL(IloSameSequenceI, IloConstraintI)
public:
  IloSameSequenceI(IloEnvI* env,
		               IloIntervalSequenceVarI* seq1, 
		               IloIntervalSequenceVarI* seq2, 
                   IloExtractableArray      array1,
                   IloExtractableArray      array2,
                   IloBool                  complete,
		               const char*              name);
  virtual ~IloSameSequenceI();
  virtual void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
  
  IloIntervalSequenceVarI* getSequenceVar1() const { return _seq1; }
  IloIntervalSequenceVarI* getSequenceVar2() const { return _seq2; }
  IloExtractableArray      getArray1()       const { return _array1; }
  IloExtractableArray      getArray2()       const { return _array2; }
  IloBool isComplete() const {  return _complete; }
  
protected:
  virtual void atRemove(IloExtractableI* sub = 0, IloAny info = 0) ILO_OVERRIDE;

private:
  virtual IloExtractableI* makeClone(IloEnvI*) const ILO_OVERRIDE;

private:
  IloIntervalSequenceVarI*  _seq1;
  IloIntervalSequenceVarI*  _seq2;
  IloExtractableArray       _array1;
  IloExtractableArray       _array2;
  IloBool                   _complete;
	  virtual IloBool isDecisionExpr() const ILO_OVERRIDE {return IloTrue;}
};

#endif

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
