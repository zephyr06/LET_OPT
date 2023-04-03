// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilosmodel.h
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

#ifndef __CONCERT_ilosmodelH
#define __CONCERT_ilosmodelH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#define ILO_SAME_SEQUENCE

#include <ilconcert/iloexpression.h>
#include <ilconcert/iloexpressioni.h>
#include <ilconcert/ilointervals.h>
#include <ilconcert/ilonumfunc.h>
#include <ilconcert/ilosegfunc.h>

class IloIntervalVarI;
class IloSpanI;
class IloAlternativeI;
class IloSynchronizeI;
class IloIntervalMapConstraintI;
class IloIntervalSequenceVarI;
class IloTransitionDistanceI;
class IloNoOverlapI;
class IloSameSequenceI;
class IloCumulAtomI;
class IloCumulFunctionExprI;
class IloStateFunctionI;
class IloStateAlwaysI;

#ifndef IloIntervalMin
#define IloIntervalMin (IloIntMin/2 + 1)
#endif

#ifndef IloIntervalMax
#define IloIntervalMax (IloIntMax/2 - 1)
#endif

#if !defined(ILOSHANDLENOIMPL)
#define ILOSHANDLENOIMPL(Hname, Iname, RHname, RIname)  \
  public:                                               \
  Hname():RHname(){}                                    \
  Hname(Iname* impl):RHname((RIname*)(void*)impl){}     \
  Iname* getImpl() const {return (Iname*)(void*)_impl;}
#endif

class IloIntervalVar: public IloExtractable {
  ILOEXTRHANDLE(IloIntervalVar, IloExtractable)
    public:
  
  IloIntervalVar(const IloEnv env, const char* name=0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    _ctor(env, name);
  }

  
  IloIntervalVar(const IloEnv env,
                 IloInt       sz,
                 const char*  name = 0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    _ctor(env, sz, name);
  }

  
  IloIntervalVar(const IloEnv                  env,
                 IloInt                        szmin,
                 IloInt                        szmax,
                 IloBool                       opt = IloFalse,
                 const IloNumToNumStepFunction f = 0,
                 const char*                   name = 0)
  {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    _ctor(env, szmin, szmax, name, f, opt);
  }

  
  IloIntervalVar(const IloEnv                  env,
                 IloInt                        szmin,
                 IloInt                        szmax,
                 IloBool                       opt,
                 const IloNumToNumStepFunction f,
                 IloInt                        granularity,
                 const char*                   name = 0)
  {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(granularity > 0, "IloIntervalVar: Invalid granularity");
    _ctor(env, szmin, szmax, name, f, granularity, opt);
  }

  
  IloInt getStartMin() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getStartMin();
  }

  
  IloInt  getStartMax() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getStartMax();
  }

  
  IloInt  getEndMin() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getEndMin();
  }

  
  IloInt  getEndMax() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getEndMax();
  }

  
  IloInt  getSizeMin() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getSizeMin();
  }

  
  IloInt  getSizeMax() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getSizeMax();
  }

  
  IloInt  getLengthMin() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getLengthMin();
  }

  
  IloInt  getLengthMax() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getLengthMax();
  }

  
  IloBool isPresent() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _isPresent();
  }

  
  IloBool isAbsent() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _isAbsent();
  }

  
  IloNumToNumStepFunction getIntensity() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getIntensity();
  }

  
  IloInt getGranularity() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    return _getGranularity();
  }

  
  void setStartMin(IloInt min) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(min >= IloIntervalMin, "IloIntervalVar: invalid value.");
    IloAssert(min <= IloIntervalMax, "IloIntervalVar: invalid value.");
    _setStartMin(min);
  }

  
  void setStartMax(IloInt max) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(max >= IloIntervalMin, "IloIntervalVar: invalid value.");
    IloAssert(max <= IloIntervalMax, "IloIntervalVar: invalid value.");
    _setStartMax(max);
  }

  
  void setEndMin(IloInt min) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(min >= IloIntervalMin, "IloIntervalVar: invalid value.");
    IloAssert(min <= IloIntervalMax, "IloIntervalVar: invalid value.");
    _setEndMin(min);
  }

  
  void setEndMax(IloInt max) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(max >= IloIntervalMin, "IloIntervalVar: invalid value.");
    IloAssert(max <= IloIntervalMax, "IloIntervalVar: invalid value.");
    _setEndMax(max);
  }

  
  void setSizeMin(IloInt min) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    _setSizeMin(min);
  }

  
  void setSizeMax(IloInt max) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    _setSizeMax(max);
  }

  
  void setLengthMin(IloInt min) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    _setLengthMin(min);
  }

  
  void setLengthMax(IloInt max) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    _setLengthMax(max);
  }

  
  void setPresent() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    _setPresent();
  }

  
  void setAbsent() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    _setAbsent();
  }

  
  void setOptional() const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    _setOptional();
  }

  
  void setIntensity(IloNumToNumStepFunction f, IloInt g = 100) const {
    IloAssert(getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(f.getImpl(), "IloNumToNumStepFunction: empty handle");
    IloAssert(g > 0, "IloIntervalVar: Invalid granularity");
    _setIntensity(f, g);
  }

  
  IloIntervalVarI* getSafeImpl() const;

  
  static IloBool MatchTypeInfo(IloTypeInfo);

private:
  void _ctor(const IloEnv env, const char* name);
  void _ctor(const IloEnv env, IloInt nd, const char* name);
  void _ctor(const IloEnv                   env,
             IloInt                         szmin,
             IloInt                         szmax,
             const char*                    name,
             const IloNumToNumStepFunction  eff,
             IloBool                        opt);
  void _ctor(const IloEnv                   env,
             IloInt                         ndmin,
             IloInt                         ndmax,
             const char*                    name,
             const IloNumToNumStepFunction  eff,
             IloInt                         granularity,
             IloBool                        opt);

  IloInt  _getStartMin()  const;
  IloInt  _getStartMax()  const;
  IloInt  _getEndMin()    const;
  IloInt  _getEndMax()    const;
  IloInt  _getSizeMin()   const;
  IloInt  _getSizeMax()   const;
  IloInt  _getLengthMin() const;
  IloInt  _getLengthMax() const;
  IloBool _isPresent()    const;
  IloBool _isAbsent()     const;

  void _setStartMin (IloInt min) const;
  void _setStartMax (IloInt max) const;
  void _setEndMin   (IloInt min) const;
  void _setEndMax   (IloInt max) const;
  void _setSizeMin  (IloInt min) const;
  void _setSizeMax  (IloInt max) const;
  void _setLengthMin(IloInt min) const;
  void _setLengthMax(IloInt max) const;
  void _setPresent  () const;
  void _setAbsent   () const;
  void _setOptional () const;

  IloNumToNumStepFunction _getIntensity() const;
  IloInt _getGranularity() const;
  void _setIntensity(IloNumToNumStepFunction f, IloInt g) const;

  friend IloConstraint IloForbidStart (const IloEnv env, const IloIntervalVar a, const IloNumToNumStepFunction f);
  friend IloConstraint IloForbidEnd   (const IloEnv env, const IloIntervalVar a, const IloNumToNumStepFunction f);
  friend IloConstraint IloForbidExtent(const IloEnv env, const IloIntervalVar a, const IloNumToNumStepFunction f);

  IloConstraintI* _forbidStart (const IloEnv env, const IloNumToNumStepFunction f) const;
  IloConstraintI* _forbidEnd   (const IloEnv env, const IloNumToNumStepFunction f) const;
  IloConstraintI* _forbidExtent(const IloEnv env, const IloNumToNumStepFunction f) const;

  friend IloConstraint IloEndBeforeEnd    (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);
  friend IloConstraint IloEndBeforeStart  (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);
  friend IloConstraint IloEndAtEnd        (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);
  friend IloConstraint IloEndAtStart      (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);
  friend IloConstraint IloStartBeforeEnd  (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);
  friend IloConstraint IloStartBeforeStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);
  friend IloConstraint IloStartAtEnd      (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);
  friend IloConstraint IloStartAtStart    (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z);

  friend IloConstraint IloEndBeforeEnd    (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);
  friend IloConstraint IloEndBeforeStart  (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);
  friend IloConstraint IloEndAtEnd        (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);
  friend IloConstraint IloEndAtStart      (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);
  friend IloConstraint IloStartBeforeEnd  (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);
  friend IloConstraint IloStartBeforeStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);
  friend IloConstraint IloStartAtEnd      (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);
  friend IloConstraint IloStartAtStart    (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z);

  IloConstraintI* _endBeforeEnd    (const IloEnv env, const IloIntervalVar a, IloInt d) const;
  IloConstraintI* _endBeforeStart  (const IloEnv env, const IloIntervalVar a, IloInt d) const;
  IloConstraintI* _endAtEnd        (const IloEnv env, const IloIntervalVar a, IloInt d) const;
  IloConstraintI* _endAtStart      (const IloEnv env, const IloIntervalVar a, IloInt d) const;
  IloConstraintI* _startBeforeEnd  (const IloEnv env, const IloIntervalVar a, IloInt d) const;
  IloConstraintI* _startBeforeStart(const IloEnv env, const IloIntervalVar a, IloInt d) const;
  IloConstraintI* _startAtEnd      (const IloEnv env, const IloIntervalVar a, IloInt d) const;
  IloConstraintI* _startAtStart    (const IloEnv env, const IloIntervalVar a, IloInt d) const;

  IloConstraintI* _endBeforeEnd    (const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;
  IloConstraintI* _endBeforeStart  (const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;
  IloConstraintI* _endAtEnd        (const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;
  IloConstraintI* _endAtStart      (const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;
  IloConstraintI* _startBeforeEnd  (const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;
  IloConstraintI* _startBeforeStart(const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;
  IloConstraintI* _startAtEnd      (const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;
  IloConstraintI* _startAtStart    (const IloEnv env, const IloIntervalVar a, const IloIntExprArg d) const;

  friend IloConstraint IloImplies    (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b);
  friend IloConstraint IloImpliesNot (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b);
  friend IloConstraint IloOrWith     (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b);
  friend IloConstraint IloEquivalent (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b);
  friend IloConstraint IloOpposite   (const IloEnv env, const IloIntervalVar a, const IloIntervalVar b);

  IloConstraintI* _implies      (const IloEnv env, const IloIntervalVar a) const;
  IloConstraintI* _impliesNot   (const IloEnv env, const IloIntervalVar a) const;
  IloConstraintI* _orWith       (const IloEnv env, const IloIntervalVar a) const;
  IloConstraintI* _equivalentTo (const IloEnv env, const IloIntervalVar a) const;
  IloConstraintI* _oppositeOf   (const IloEnv env, const IloIntervalVar a) const;

  friend IloIntExprArg IloStartOf (const IloIntervalVar a, IloInt absVal);
  friend IloIntExprArg IloEndOf   (const IloIntervalVar a, IloInt absVal);
  friend IloIntExprArg IloLengthOf(const IloIntervalVar a, IloInt absVal);
  friend IloIntExprArg IloSizeOf  (const IloIntervalVar a, IloInt absVal);

  IloIntExprArgI* _makeStartOf   (const IloEnv env, IloInt absVal) const;
  IloIntExprArgI* _makeEndOf     (const IloEnv env, IloInt absVal) const;
  IloIntExprArgI* _makeLengthExpr(const IloEnv env, IloInt absVal) const;
  IloIntExprArgI* _makeSizeExpr  (const IloEnv env, IloInt absVal) const;

  friend IloConstraint IloPresenceOf(const IloEnv env, const IloIntervalVar a);

  IloConstraintI* _getPresenceConstraint() const;

  friend IloNumExprArg IloStartEval(const IloIntervalVar a,
                                    const IloNumToNumSegmentFunction f,
                                    IloNum absVal);
  friend IloNumExprArg IloEndEval(const IloIntervalVar a,
                                  const IloNumToNumSegmentFunction f,
                                  IloNum absVal);
  friend IloNumExprArg IloLengthEval(const IloIntervalVar a,
                                     const IloNumToNumSegmentFunction f,
                                     IloNum absVal);
  friend IloNumExprArg IloSizeEval(const IloIntervalVar a,
                                   const IloNumToNumSegmentFunction f,
                                   IloNum absVal);

  IloNumExprArgI* _makeStartEval (const IloEnv env, const IloNumToNumSegmentFunction f, IloNum absVal) const;
  IloNumExprArgI* _makeEndEval   (const IloEnv env, const IloNumToNumSegmentFunction f, IloNum absVal) const;
  IloNumExprArgI* _makeLengthEval(const IloEnv env, const IloNumToNumSegmentFunction f, IloNum absVal) const;
  IloNumExprArgI* _makeSizeEval  (const IloEnv env, const IloNumToNumSegmentFunction f, IloNum absVal) const;

  friend IloIntExprArg IloOverlapLength(const IloEnv env,
                                        const IloIntervalVar a1,
                                        const IloIntervalVar a2,
                                        IloInt absVal);
  friend IloIntExprArg IloOverlapLength(const IloEnv env,
                                        const IloIntervalVar a,
                                        IloInt start, IloInt end,
                                        IloInt absVal);

  IloIntExprArgI* _makeOverlapLength(const IloEnv env, const IloIntervalVar a, IloInt absVal) const;
  IloIntExprArgI* _makeOverlapLength(const IloEnv env, IloInt start, IloInt end, IloInt absVal) const;
};

typedef IloArray<IloIntervalVar> IloIntervalVarArray;

typedef IloArray<IloIntervalVarArray> IloIntervalVarArray2;

IloBool IloCheckNoDuplicates(IloEnv env, IloIntervalVarArray intervals);
IloBool IloCheckEqual(IloEnv env, IloIntervalVarArray intervals1, IloIntervalVarArray intervals2);
IloBool IloCheckIncluded(IloEnv env, IloIntervalVarArray intervals1, IloIntervalVarArray intervals2);

inline IloConstraint IloStartBeforeStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._startBeforeStart(env, b, z);
}

inline IloConstraint IloStartBeforeEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._startBeforeEnd(env, b, z);
}

inline IloConstraint IloEndBeforeStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._endBeforeStart(env, b, z);
}

inline IloConstraint IloEndBeforeEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._endBeforeEnd(env, b, z);
}

inline IloConstraint IloStartAtStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._startAtStart(env, b, z);
}

inline IloConstraint IloStartAtEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._startAtEnd(env, b, z);
}

inline IloConstraint IloEndAtStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._endAtStart(env, b, z);
}

inline IloConstraint IloEndAtEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, IloInt z = 0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._endAtEnd(env, b, z);
}

inline IloConstraint IloStartBeforeStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._startBeforeStart(env, b, z);
}

inline IloConstraint IloStartBeforeEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._startBeforeEnd(env, b, z);
}

inline IloConstraint IloEndBeforeStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._endBeforeStart(env, b, z);
}

inline IloConstraint IloEndBeforeEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._endBeforeEnd(env, b, z);
}

inline IloConstraint IloStartAtStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._startAtStart(env, b, z);
}

inline IloConstraint IloStartAtEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._startAtEnd(env, b, z);
}

inline IloConstraint IloEndAtStart(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._endAtStart(env, b, z);
}

inline IloConstraint IloEndAtEnd(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b, const IloIntExprArg z) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(z.getImpl(), "IloIntExprArg: empty handle.");
  return a._endAtEnd(env, b, z);
}

#ifdef NDEBUG
inline IloConstraint IloPresenceOf(const IloEnv, const IloIntervalVar a) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return a._getPresenceConstraint();
}
#else
inline IloConstraint IloPresenceOf(const IloEnv env, const IloIntervalVar a) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return a._getPresenceConstraint();
}
#endif

inline IloConstraint IloImplies(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._implies(env, b);
}

inline IloConstraint IloImpliesNot(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._impliesNot(env, b);
}

inline IloConstraint IloOrWith(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._orWith(env, b);
}

inline IloConstraint IloEquivalent(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._equivalentTo(env, b);
}

inline IloConstraint IloOpposite(const IloEnv env, const IloIntervalVar a, const IloIntervalVar b) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(b.getImpl(), "IloIntervalVar: empty handle.");
  return a._oppositeOf(env, b);
}

inline IloIntExprArg IloStartOf(const IloIntervalVar a, IloInt absVal =0){
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return a._makeStartOf(a.getEnv(),absVal);
}

inline IloIntExprArg IloEndOf(const IloIntervalVar a, IloInt absVal =0){
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return a._makeEndOf(a.getEnv(),absVal);
}

inline IloIntExprArg IloLengthOf(const IloIntervalVar a, IloInt absVal =0){
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return a._makeLengthExpr(a.getEnv(),absVal);
}

inline IloIntExprArg IloSizeOf(const IloIntervalVar a, IloInt absVal =0){
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return a._makeSizeExpr(a.getEnv(),absVal);
}

inline IloIntExprArg IloOverlapLength (const IloEnv env,
                                       const IloIntervalVar a1,
                                       const IloIntervalVar a2,
                                       IloInt absVal =0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a1.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(a2.getImpl(), "IloIntervalVar: empty handle.");
  return a1._makeOverlapLength(env, a2, absVal);
}

inline IloIntExprArg IloOverlapLength (const IloEnv env,
                                       const IloIntervalVar a,
                                       IloInt start, IloInt end,
                                       IloInt absVal =0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(start <= end, "IloOverlapLength: empty interval.");
  return a._makeOverlapLength(env, start, end, absVal);
}

inline IloNumExprArg IloStartEval(const IloIntervalVar a,
                                  const IloNumToNumSegmentFunction f,
                                  IloNum absVal =0) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloNumToNumSegmentFunction: empty handle.");
  return a._makeStartEval(a.getEnv(),f, absVal);
}

inline IloNumExprArg IloEndEval(const IloIntervalVar a,
                                const IloNumToNumSegmentFunction f,
                                IloNum absVal =0) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloNumToNumSegmentFunction: empty handle.");
  return a._makeEndEval(a.getEnv(),f, absVal);
}

inline IloNumExprArg IloLengthEval(const IloIntervalVar a,
                                   const IloNumToNumSegmentFunction f,
                                   IloNum absVal =0) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloNumToNumSegmentFunction: empty handle.");
  return a._makeLengthEval(a.getEnv(),f, absVal);
}

inline IloNumExprArg IloSizeEval(const IloIntervalVar a,
                                 const IloNumToNumSegmentFunction f,
                                 IloNum absVal =0) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloNumToNumSegmentFunction: empty handle.");
  return a._makeSizeEval(a.getEnv(), f, absVal);
}

inline IloConstraint IloForbidStart(const IloEnv env,
                                    const IloIntervalVar a,
                                    const IloNumToNumStepFunction f) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloNumToNumStepFunction: empty handle.");
  return a._forbidStart(env, f);
}

inline IloConstraint IloForbidEnd(const IloEnv env,
                                  const IloIntervalVar a,
                                  const IloNumToNumStepFunction f) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloNumToNumStepFunction: empty handle.");
  return a._forbidEnd(env, f);
}

inline IloConstraint IloForbidExtent(const IloEnv env,
                                     const IloIntervalVar a,
                                     const IloNumToNumStepFunction f) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloNumToNumStepFunction: empty handle.");
  return a._forbidExtent(env, f);
}

class IloSpan: public IloConstraint {
  ILOEXTRHANDLE(IloSpan, IloConstraint)
    public:
  
  IloSpan(const IloEnv env, const IloIntervalVar a, const IloIntervalVarArray bs, const char *name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(bs.getImpl(), "IloIntervalVarArray: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < bs.getSize(); i++)
      IloAssert(bs[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, bs, name);
  }

  IloSpan(const IloEnv env, const IloIntervalVar a, const char *name = 0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    _ctor(env, a, name);
  }
  void add(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloSpan: empty handle.");
    IloAssert(b.getImpl(), "IloSpan: empty handle.");
    _add(b);
  }
  void remove(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloSpan: empty handle.");
    IloAssert(b.getImpl(), "IloSpan: empty handle.");
    _remove(b);
  }

  IloBool contains(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloSpan: empty handle.");
    return _contains(b);
  }
  IloIntervalVar getSpanningInterval() const {
    IloAssert(getImpl(), "IloSpan: empty handle.");
    return _getSpanningInterval();
  }

  class IntervalIterator {
    IloInt              _pos;
    IloExtractableArray _array;
  public:
    IntervalIterator(const IloSpan span);
    IloBool ok() const {
      IloAssert(_array.getImpl(), "IloSpan::IntervalIterator: empty handle.");
      return _pos < _array.getSize();
    }
    IloIntervalVar operator*() {
      IloAssert(ok(), "IloSpan::IntervalIterator: out of range iterator.");
      return (IloIntervalVarI*)(void*)(_array[_pos].getImpl());
    }
    IntervalIterator& operator++() {
      _pos++;
      return *this;
    }
  };

private:
  void _ctor(const IloEnv env, const IloIntervalVar a, const char *name);
  void _ctor(const IloEnv env, const IloIntervalVar a, const IloIntervalVarArray bs, const char *name);
  void _add(const IloIntervalVar b) const;
  void _remove(const IloIntervalVar b) const;
  IloBool _contains(const IloIntervalVar b) const;
  IloIntervalVar _getSpanningInterval() const;
};

typedef IloArray<IloSpan> IloSpanArray;

class IloAlternative: public IloConstraint {
  ILOEXTRHANDLE(IloAlternative, IloConstraint)
    public:
  
  IloAlternative(const IloEnv env, const IloIntervalVar a,
                 const IloIntervalVarArray bs, const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(bs.getImpl(), "IloIntervalVarArray: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < bs.getSize(); i++)
      IloAssert(bs[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, bs, name);
  }

  
  IloAlternative(const IloEnv env, const IloIntervalVar a,
                 const IloIntervalVarArray bs,
                 IloInt c,
                 const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(bs.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(0<=c, "IloAlternative: cardinality should non-negative.");
#ifndef NDEBUG
    for (IloInt i = 0; i < bs.getSize(); i++)
      IloAssert(bs[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, bs, c, name);
  }

  
  IloAlternative(const IloEnv env, const IloIntervalVar a,
                 const IloIntervalVarArray bs,
                 const IloIntExprArg c,
                 const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(bs.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(c.getImpl(), "IloIntExprArg: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < bs.getSize(); i++)
      IloAssert(bs[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, bs, c, name);
  }

  IloAlternative(const IloEnv env, const IloIntervalVar a, const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    _ctor(env, a, name);
  }
  void add(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloAlternative: empty handle.");
    IloAssert(b.getImpl(), "IloAlternative: empty handle.");
    _add(b);
  }
  void remove(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloAlternative: empty handle.");
    IloAssert(b.getImpl(), "IloAlternative: empty handle.");
    _remove(b);
  }

  IloBool contains(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloAlternative: empty handle.");
    return _contains(b);
  }
  IloIntervalVar getAlternativeInterval() const {
    IloAssert(getImpl(), "IloAlternative: empty handle.");
    return _getAlternativeInterval();
  }

  class IntervalIterator {
    IloInt              _pos;
    IloExtractableArray _array;
  public:
    IntervalIterator(const IloAlternative span);
    IloBool ok() const {
      IloAssert(_array.getImpl(), "IloAlternative::IntervalIterator: empty handle.");
      return _pos < _array.getSize();
    }
    IloIntervalVar operator*() {
      IloAssert(ok(), "IloAlternative::IntervalIterator: out of range iterator.");
      return (IloIntervalVarI*)(void*)(_array[_pos].getImpl());
    }
    IntervalIterator& operator++() {
      _pos++;
      return *this;
    }
  };

private:
  void _ctor(const IloEnv env, const IloIntervalVar a, const char *name);
  void _ctor(const IloEnv env, const IloIntervalVar a, const IloIntervalVarArray bs, const char *name);
  void _ctor(const IloEnv env, const IloIntervalVar a, const IloIntervalVarArray bs, IloInt n, const char *name);
  void _ctor(const IloEnv env, const IloIntervalVar a, const IloIntervalVarArray bs, const IloIntExprArg n, const char *name);
  void _add(const IloIntervalVar b) const;
  void _remove(const IloIntervalVar b) const;
  IloBool _contains(const IloIntervalVar b) const;
  IloIntervalVar _getAlternativeInterval() const;
};

typedef IloArray<IloAlternative> IloAlternativeArray;

class IloSynchronize: public IloConstraint {
  ILOEXTRHANDLE(IloSynchronize, IloConstraint)
    public:
  
  IloSynchronize(const IloEnv env, const IloIntervalVar a,
                 const IloIntervalVarArray bs, const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    IloAssert(bs.getImpl(), "IloIntervalVarArray: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < bs.getSize(); i++)
      IloAssert(bs[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, bs, name);
  }

  IloSynchronize(const IloEnv env, const IloIntervalVar a, const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
    _ctor(env, a, name);
  }
  void add(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloSynchronize: empty handle.");
    IloAssert(b.getImpl(), "IloSynchronize: empty handle.");
    _add(b);
  }
  void remove(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloSynchronize: empty handle.");
    IloAssert(b.getImpl(), "IloSynchronize: empty handle.");
    _remove(b);
  }

  IloBool contains(const IloIntervalVar b) const {
    IloAssert(getImpl(), "IloSynchronize: empty handle.");
    return _contains(b);
  }
  IloIntervalVar getSynchronizationInterval() const {
    IloAssert(getImpl(), "IloSynchronize: empty handle.");
    return _getSynchronizationInterval();
  }

  class IntervalIterator {
    IloInt              _pos;
    IloExtractableArray _array;
  public:
    IntervalIterator(const IloSynchronize span);
    IloBool ok() const {
      IloAssert(_array.getImpl(), "IloSynchronize::IntervalIterator: empty handle.");
      return _pos < _array.getSize();
    }
    IloIntervalVar operator*() {
      IloAssert(ok(), "IloSynchronize::IntervalIterator: out of range iterator.");
      return (IloIntervalVarI*)(void*)(_array[_pos].getImpl());
    }
    IntervalIterator& operator++() {
      _pos++;
      return *this;
    }
  };

private:
  void _ctor(const IloEnv env, const IloIntervalVar a, const char *name);
  void _ctor(const IloEnv env, const IloIntervalVar a, const IloIntervalVarArray bs, const char *name);
  void _add(const IloIntervalVar b) const;
  void _remove(const IloIntervalVar b) const;
  IloBool _contains(const IloIntervalVar b) const;
  IloIntervalVar _getSynchronizationInterval() const;
};

typedef IloArray<IloSynchronize> IloSynchronizeArray;

class IloIsomorphism : public IloConstraint {
  
 public:

  
  IloIsomorphism(const IloEnv env,
		 const IloIntervalVarArray domain,
		 const IloIntervalVarArray intervals,
		 const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(domain.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(intervals.getImpl(), "IloIntervalVarArray: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < domain.getSize(); i++)
      IloAssert(domain[i].getImpl(), "IloIntervalVarArray: contains empty handle.");
    for (IloInt j = 0; j < intervals.getSize(); j++)
      IloAssert(intervals[j].getImpl(), "IloIntervalVarArray: contains empty handle.");
#endif
    IloAssert(IloCheckNoDuplicates(env, intervals), "IloIntervalVarArray: forbidden duplicate element.");
    IloAssert(IloCheckNoDuplicates(env, domain), "IloIntervalVarArray: forbidden duplicate element.");
    _ctor(env, domain, intervals, 0, domain.getSize(), name);
  }

  
  IloIsomorphism(const IloEnv env,
		 const IloIntervalVarArray domain,
		 const IloIntervalVarArray intervals,
		 const IloIntExprArray map,
		 const IloInt absVal,
		 const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(domain.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(intervals.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(map.getImpl(), "IloIntExprArray: empty handle.");
    IloAssert(intervals.getSize() == map.getSize(), "IloIntExprArray: bad mapping size.");
#ifndef NDEBUG
    for (IloInt i = 0; i < domain.getSize(); i++) {
      IloAssert(domain[i].getImpl(), "IloIntervalVarArray: contains empty handle.");
    }
    for (IloInt j = 0; j < intervals.getSize(); j++) {
      IloAssert(intervals[j].getImpl(), "IloIntervalVarArray: contains empty handle.");
      IloAssert(map[j].getImpl(), "IloIntExprArray: contains empty handle.");
    }
#endif
    IloAssert(IloCheckNoDuplicates(env, intervals), "IloIntervalVarArray: forbidden duplicate element.");
    IloAssert(IloCheckNoDuplicates(env, domain), "IloIntervalVarArray: forbidden duplicate element.");
    _ctor(env, domain, intervals, map, absVal, name);
  }
private:
  void _ctor(const IloEnv env,
             const IloIntervalVarArray domain,
             const IloIntervalVarArray intervals,
             const IloIntExprArray map,
	     const IloInt absVal,
             const char* name);
};

typedef IloArray<IloIsomorphism> IloIsomorphismArray;

class IloMultipleSpan: public IloConstraint {
  
public:

  
  IloMultipleSpan(const IloEnv env,
		  const IloIntervalVarArray domain,
		  const IloIntervalVarArray intervals,
		  const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(domain.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(intervals.getImpl(), "IloIntervalVarArray: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < domain.getSize(); i++)
      IloAssert(domain[i].getImpl(), "IloIntervalVarArray: contains empty handle.");
    for (IloInt j = 0; j < intervals.getSize(); j++)
      IloAssert(intervals[j].getImpl(), "IloIntervalVarArray: contains empty handle.");
#endif
    IloAssert(IloCheckNoDuplicates(env, intervals), "IloIntervalVarArray: forbidden duplicate element.");
    IloAssert(IloCheckNoDuplicates(env, domain), "IloIntervalVarArray: forbidden duplicate element.");
    _ctor(env, domain, intervals, 0, intervals.getSize(), name);
  }

  
  IloMultipleSpan(const IloEnv env,
		  const IloIntervalVarArray domain,
		  const IloIntervalVarArray intervals,
		  const IloIntExprArray map,
		  const IloInt absVal,
		  const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(domain.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(intervals.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(map.getImpl(), "IloIntExprArray: empty handle.");
    IloAssert(intervals.getSize() == map.getSize(), "IloIntExprArray: bad mapping size.");
#ifndef NDEBUG
    for (IloInt i = 0; i < domain.getSize(); i++) {
      IloAssert(domain[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
    }
    for (IloInt j = 0; j < intervals.getSize(); j++) {
      IloAssert(intervals[j].getImpl(), "IloIntervaVarArray: contains empty handle.");
      IloAssert(map[j].getImpl(), "IloIntExprArray: contains empty handle.");
    }
#endif
    IloAssert(IloCheckNoDuplicates(env, intervals), "IloIntervalVarArray: forbidden duplicate element.");
    IloAssert(IloCheckNoDuplicates(env, domain), "IloIntervalVarArray: forbidden duplicate element.");
    _ctor(env, domain, intervals, map, absVal, name);
  }
private:
  void _ctor(const IloEnv env,
             const IloIntervalVarArray domain,
             const IloIntervalVarArray intervals,
             const IloIntExprArray map,
	     const IloInt absVal,
             const char* name);
};

typedef IloArray<IloMultipleSpan> IloMultipleSpanArray;

class IloTransitionDistance : public IloParameter {
  ILOSHANDLENOIMPL(IloTransitionDistance, IloTransitionDistanceI,
                   IloParameter, IloParameterI);
public:
  
  IloTransitionDistance(IloIntArray2 dtable, const char* name = 0) {
    IloAssert(dtable.getEnv().getImpl(), "IloEnv: empty handle.");
    IloAssert(0 < dtable.getSize(), "IloTransitionDistance: zero or negative size.");
#ifndef NDEBUG
    for (IloInt i = 0; i < dtable.getSize(); i++)
      IloAssert(dtable[i].getSize() == dtable.getSize(), "IloTransitionDistance: distance table must be square matrix.");
#endif
    _ctor(dtable.getEnv(), dtable, name);
  }

  

  IloTransitionDistance(IloIntArrayArray dtable, const char* name = 0) {
    IloAssert(dtable.getEnv().getImpl(), "IloEnv: empty handle.");
    IloAssert(0 < dtable.getSize(), "IloTransitionDistance: zero or negative size.");
#ifndef NDEBUG
    for (IloInt i = 0; i < dtable.getSize(); i++)
      IloAssert(dtable[i].getSize() == dtable.getSize(), "IloTransitionDistance: distance table must be square matrix.");
#endif
    _ctor(dtable.getEnv(), (IloIntArray2)dtable, name);
  }

  
  IloTransitionDistance(const IloEnv env, IloInt size, const char* name = 0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(0 < size, "IloTransitionDistance: zero or negative size.");
    _ctor(env, size, name);
  }

  
  IloInt getSize() const {
    IloAssert(getImpl(), "IloTransitionDistance: empty handle.");
    return _getSize();
  }

  
  void setValue(IloInt from, IloInt to, IloInt val) const {
    IloAssert(getImpl(), "IloTransitionDistance: empty handle.");
    IloAssert(from >= 0 && from < getSize(), "IloTransitionDistance: invalid index.");
    IloAssert(to >= 0 && to < getSize(), "IloTransitionDistance: invalid index.");
    IloAssert(val >= 0, "IloTransitionDistance: transition distance cannot be negative.");
    _setValue(from, to, val);
  }

  
  IloInt getValue(IloInt from, IloInt to) const {
    IloAssert(getImpl(), "IloTransitionDistance: empty handle.");
    IloAssert(from >= 0 && from < getSize(), "IloTransitionDistance: invalid index.");
    IloAssert(to >= 0 && to < getSize(), "IloTransitionDistance: invalid index.");
    return _getValue(from, to);
  }

  IloBool checkTriangularInequality() const {
    IloAssert(getImpl(), "IloTransitionDistance: empty handle.");
    return _checkTriangularInequality();
  }
private:
  void _ctor(const IloEnv env, IloInt size, const char* name);
  void _ctor(const IloEnv env, IloIntArray2 dtable, const char* name);
  IloInt  _getSize() const;
  IloBool _checkTriangularInequality() const;
  void   _setValue(IloInt from, IloInt to, IloInt val) const;
  IloInt _getValue(IloInt from, IloInt to) const;
  void _display(ILOSTD(ostream)& str) const;
  friend ILOSTD(ostream&) operator<<(ILOSTD(ostream)& str, const IloTransitionDistance& tdist);
};

inline ILOSTD(ostream&) operator<<(ILOSTD(ostream)& str, const IloTransitionDistance& tdist) {
  IloAssert(tdist.getImpl(), "IloTransitionDistance: empty handle.");
  tdist._display(str);
  return str;
}

class IloIntervalSequenceVar :public IloExtractable {
  ILOEXTRHANDLE(IloIntervalSequenceVar, IloExtractable)
    public:

  
  IloIntervalSequenceVar(const IloEnv env,
                         const IloIntervalVarArray a,
                         const char* name=0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVarArray: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < a.getSize(); i++)
      IloAssert(a[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, 0, name);
  }

  
  IloIntervalSequenceVar(const IloEnv env,
                         const IloIntervalVarArray a,
                         const IloIntArray types,
                         const char* name=0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVarArray: empty handle.");
    IloAssert(types.getImpl(), "IloIntArray: empty handle.");
    IloAssert(types.getSize()==a.getSize(), "IloIntervalSequenceVar: array size mismatch.");
#ifndef NDEBUG
    for (IloInt i = 0; i < a.getSize(); i++)
      IloAssert(a[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, types, name);
  }
  IloInt getSize() const {
    IloAssert(getImpl(), "IloIntervalSequenceVar: empty handle.");
    return _getSize();
  }
  IloIntArray getTypes() const;
  IloIntervalVarArray getIntervals() const;

private:
  void _ctor(const IloEnv env, const IloIntervalVarArray a, const IloIntArray types, const char* name);
  IloInt _getSize() const;
};

typedef IloArray<IloIntervalSequenceVar> IloIntervalSequenceVarArray;

typedef IloArray<IloIntervalSequenceVarArray> IloIntervalSequenceVarArray2;

IloConstraint _IloPrevious(const IloEnv env,
                           const IloIntervalSequenceVar seq,
                           const IloIntervalVar prev,
                           const IloIntervalVar next);

IloConstraint _IloBefore(const IloEnv env,
                         const IloIntervalSequenceVar seq,
                         const IloIntervalVar pred,
                         const IloIntervalVar succ);

inline IloConstraint IloPrevious(const IloEnv env,
                                 const IloIntervalSequenceVar seq,
                                 const IloIntervalVar prev,
                                 const IloIntervalVar next) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(prev.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(next.getImpl(), "IloIntExprArg: empty handle.");
  return _IloPrevious(env, seq, prev, next);
}

inline IloConstraint IloFirst(const IloEnv env,
                              const IloIntervalSequenceVar seq,
                              const IloIntervalVar a) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloPrevious(env, seq, 0, a);
}

inline IloConstraint IloLast(const IloEnv env,
                             const IloIntervalSequenceVar seq,
                             const IloIntervalVar a) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloPrevious(env, seq, a, 0);
}

inline IloConstraint IloBefore(const IloEnv env,
                               const IloIntervalSequenceVar seq,
                               const IloIntervalVar pred,
                               const IloIntervalVar succ) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(pred.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(succ.getImpl(), "IloIntervalVar: empty handle.");
  return _IloBefore(env, seq, pred, succ);
}

IloIntExprArg _IloTypeOfNext(const IloIntervalSequenceVar seq,
                             const IloIntervalVar a,
                             IloInt lastVal);
IloIntExprArg _IloTypeOfNext(const IloIntervalSequenceVar seq,
                             const IloIntervalVar a,
                             IloInt lastVal, IloInt absVal);
IloIntExprArg _IloTypeOfPrevious(const IloIntervalSequenceVar seq,
                                 const IloIntervalVar a,
                                 IloInt firstVal);
IloIntExprArg _IloTypeOfPrevious(const IloIntervalSequenceVar seq,
                                 const IloIntervalVar a,
                                 IloInt firstVal, IloInt absVal);
IloIntExprArg _IloStartOfNext(const IloIntervalSequenceVar seq,
                              const IloIntervalVar a,
                              IloInt lastVal);
IloIntExprArg _IloStartOfNext(const IloIntervalSequenceVar seq,
                              const IloIntervalVar a,
                              IloInt lastVal, IloInt absVal);
IloIntExprArg _IloStartOfPrevious(const IloIntervalSequenceVar seq,
                                  const IloIntervalVar a,
                                  IloInt firstVal);
IloIntExprArg _IloStartOfPrevious(const IloIntervalSequenceVar seq,
                                  const IloIntervalVar a,
                                  IloInt firstVal, IloInt absVal);
IloIntExprArg _IloEndOfNext(const IloIntervalSequenceVar seq,
                            const IloIntervalVar a,
                            IloInt lastVal);
IloIntExprArg _IloEndOfNext(const IloIntervalSequenceVar seq,
                            const IloIntervalVar a,
                            IloInt lastVal, IloInt absVal);
IloIntExprArg _IloEndOfPrevious(const IloIntervalSequenceVar seq,
                                const IloIntervalVar a,
                                IloInt firstVal);
IloIntExprArg _IloEndOfPrevious(const IloIntervalSequenceVar seq,
                                const IloIntervalVar a,
                                IloInt firstVal, IloInt absVal);
IloIntExprArg _IloSizeOfNext(const IloIntervalSequenceVar seq,
                             const IloIntervalVar a,
                             IloInt lastVal);
IloIntExprArg _IloSizeOfNext(const IloIntervalSequenceVar seq,
                             const IloIntervalVar a,
                             IloInt lastVal, IloInt absVal);
IloIntExprArg _IloSizeOfPrevious(const IloIntervalSequenceVar seq,
                                 const IloIntervalVar a,
                                 IloInt firstVal);
IloIntExprArg _IloSizeOfPrevious(const IloIntervalSequenceVar seq,
                                 const IloIntervalVar a,
                                 IloInt firstVal, IloInt absVal);
IloIntExprArg _IloLengthOfNext(const IloIntervalSequenceVar seq,
                               const IloIntervalVar a,
                               IloInt lastVal);
IloIntExprArg _IloLengthOfNext(const IloIntervalSequenceVar seq,
                               const IloIntervalVar a,
                               IloInt lastVal, IloInt absVal);
IloIntExprArg _IloLengthOfPrevious(const IloIntervalSequenceVar seq,
                                   const IloIntervalVar a,
                                   IloInt firstVal);
IloIntExprArg _IloLengthOfPrevious(const IloIntervalSequenceVar seq,
                                   const IloIntervalVar a,
                                   IloInt firstVal, IloInt absVal);

inline IloIntExprArg IloTypeOfNext(const IloIntervalSequenceVar seq,
                                   const IloIntervalVar a,
                                   IloInt lastVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloTypeOfNext(seq, a, lastVal);
}

inline IloIntExprArg IloTypeOfNext(const IloIntervalSequenceVar seq,
                                   const IloIntervalVar a,
                                   IloInt lastVal,
                                   IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloTypeOfNext(seq, a, lastVal, absVal);
}

inline IloIntExprArg IloTypeOfPrevious(const IloIntervalSequenceVar seq,
                                       const IloIntervalVar a,
                                       IloInt firstVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloTypeOfPrevious(seq, a, firstVal);
}

inline IloIntExprArg IloTypeOfPrevious(const IloIntervalSequenceVar seq,
                                       const IloIntervalVar a,
                                       IloInt firstVal,
                                       IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloTypeOfPrevious(seq, a, firstVal, absVal);
}

inline IloIntExprArg IloStartOfNext(const IloIntervalSequenceVar seq,
                                    const IloIntervalVar a,
                                    IloInt lastVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloStartOfNext(seq, a, lastVal);
}

inline IloIntExprArg IloStartOfNext(const IloIntervalSequenceVar seq,
                                    const IloIntervalVar a,
                                    IloInt lastVal,
                                    IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloStartOfNext(seq, a, lastVal, absVal);
}

inline IloIntExprArg IloStartOfPrevious(const IloIntervalSequenceVar seq,
                                        const IloIntervalVar a,
                                        IloInt firstVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloStartOfPrevious(seq, a, firstVal);
}

inline IloIntExprArg IloStartOfPrevious(const IloIntervalSequenceVar seq,
                                        const IloIntervalVar a,
                                        IloInt firstVal,
                                        IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloStartOfPrevious(seq, a, firstVal, absVal);
}

inline IloIntExprArg IloEndOfNext(const IloIntervalSequenceVar seq,
                                  const IloIntervalVar a,
                                  IloInt lastVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloEndOfNext(seq, a, lastVal);
}

inline IloIntExprArg IloEndOfNext(const IloIntervalSequenceVar seq,
                                  const IloIntervalVar a,
                                  IloInt lastVal,
                                  IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloEndOfNext(seq, a, lastVal, absVal);
}

inline IloIntExprArg IloEndOfPrevious(const IloIntervalSequenceVar seq,
                                      const IloIntervalVar a,
                                      IloInt firstVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloEndOfPrevious(seq, a, firstVal);
}

inline IloIntExprArg IloEndOfPrevious(const IloIntervalSequenceVar seq,
                                      const IloIntervalVar a,
                                      IloInt firstVal,
                                      IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloEndOfPrevious(seq, a, firstVal, absVal);
}

inline IloIntExprArg IloSizeOfNext(const IloIntervalSequenceVar seq,
                                   const IloIntervalVar a,
                                   IloInt lastVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloSizeOfNext(seq, a, lastVal);
}

inline IloIntExprArg IloSizeOfNext(const IloIntervalSequenceVar seq,
                                   const IloIntervalVar a,
                                   IloInt lastVal,
                                   IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloSizeOfNext(seq, a, lastVal, absVal);
}

inline IloIntExprArg IloSizeOfPrevious(const IloIntervalSequenceVar seq,
                                       const IloIntervalVar a,
                                       IloInt firstVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloSizeOfPrevious(seq, a, firstVal);
}

inline IloIntExprArg IloSizeOfPrevious(const IloIntervalSequenceVar seq,
                                       const IloIntervalVar a,
                                       IloInt firstVal,
                                       IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloSizeOfPrevious(seq, a, firstVal, absVal);
}

inline IloIntExprArg IloLengthOfNext(const IloIntervalSequenceVar seq,
                                     const IloIntervalVar a,
                                     IloInt lastVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloLengthOfNext(seq, a, lastVal);
}

inline IloIntExprArg IloLengthOfNext(const IloIntervalSequenceVar seq,
                                     const IloIntervalVar a,
                                     IloInt lastVal,
                                     IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloLengthOfNext(seq, a, lastVal, absVal);
}

inline IloIntExprArg IloLengthOfPrevious(const IloIntervalSequenceVar seq,
                                         const IloIntervalVar a,
                                         IloInt firstVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloLengthOfPrevious(seq, a, firstVal);
}

inline IloIntExprArg IloLengthOfPrevious(const IloIntervalSequenceVar seq,
                                         const IloIntervalVar a,
                                         IloInt firstVal,
                                         IloInt absVal){
  IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return _IloLengthOfPrevious(seq, a, firstVal, absVal);
}

class IloNoOverlap :public IloConstraint {
  ILOEXTRHANDLE(IloNoOverlap,IloConstraint)
    public:
  
  IloNoOverlap(const IloEnv env,
               const IloIntervalVarArray a,
               const char* name=0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(a.getImpl(), "IloIntervalVarArray: empty handle.");
#ifndef NDEBUG
    for (IloInt i = 0; i < a.getSize(); i++)
      IloAssert(a[i].getImpl(), "IloIntervaVarArray: contains empty handle.");
#endif
    _ctor(env, a, 0, name);
  }

  
  IloNoOverlap(const IloEnv env,
               const IloIntervalSequenceVar seq,
               const IloTransitionDistance tdist =0,
               const char* name=0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
#ifndef NDEBUG
    _checkTransParam(seq.getSize(), tdist, seq.getTypes());
#endif
    _ctor(env, seq, tdist, IloFalse, name); 
  }

  
  IloNoOverlap(const IloEnv env,
               const IloIntervalSequenceVar seq,
               const IloTransitionDistance tdist,
               IloBool direct,
               const char* name=0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(seq.getImpl(), "IloIntervalSequenceVar: empty handle.");
#ifndef NDEBUG
    _checkTransParam(seq.getSize(), tdist, seq.getTypes());
#endif
    _ctor(env, seq, tdist, direct, name);
  }

private:
  void _ctor(const IloEnv env,
             const IloIntervalVarArray a,
             const IloTransitionDistance tdist,
             const char* name);
  void _ctor(const IloEnv env,
             const IloIntervalSequenceVar seq,
             const IloTransitionDistance tdist,
             IloBool direct,
             const char* name);
#ifndef NDEBUG
  void _checkTransParam(IloInt size, IloTransitionDistance tdist, IloIntArray tindexes) {
    if (!tdist.getImpl())
      return;
    if (!tindexes.getImpl()) {
      IloAssert(tdist.getSize() == size, "IloNoOverlap: incompatible sizes of input arrays.");
    } else {
      const IloInt tsize = tdist.getSize();
      for (IloInt i = 0; i < size; i++)
        IloAssert(tindexes[i] >= 0 && tindexes[i] < tsize, "IloNoOverlap: index is out of range.");
    }
  }
#endif
};

#if defined(ILO_SAME_SEQUENCE)

IloConstraint _IloSameSequence(const IloEnv env,
                               const IloIntervalSequenceVar seq1,
                               const IloIntervalSequenceVar seq2,
                               const IloIntervalVarArray a1,
                               const IloIntervalVarArray a2,
                               const char* name);

IloConstraint _IloSameCommonSubsequence(const IloEnv env,
                                        const IloIntervalSequenceVar seq1,
                                        const IloIntervalSequenceVar seq2,
                                        const IloIntervalVarArray a1,
                                        const IloIntervalVarArray a2,
                                        const char* name);

 
inline IloConstraint IloSameSequence(const IloEnv env,
                                     const IloIntervalSequenceVar seq1,
                                     const IloIntervalSequenceVar seq2,
                                     const char* name=0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq1.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(seq2.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(seq1.getSize()==seq2.getSize(), "IloSameSequence: sequence variables have different size.");
  return _IloSameSequence(env, seq1, seq2, 0, 0, name);
}

inline IloConstraint IloSameSequence(const IloEnv env,
                                     const IloIntervalSequenceVar seq1,
                                     const IloIntervalSequenceVar seq2,
                                     const IloIntervalVarArray a1,
                                     const IloIntervalVarArray a2,
                                     const char* name=0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq1.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(seq2.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(seq1.getSize()==seq2.getSize(), "IloSameSequence: sequence variables have different size.");
  IloAssert(a1.getImpl(), "IloIntervalVarArray: empty handle.");
  IloAssert(a2.getImpl(), "IloIntervalVarArray: empty handle.");
  IloAssert(seq1.getSize()==a1.getSize(), "IloSameSequence: sequence variable and array of interval variables have different size.");
  IloAssert(seq2.getSize()==a2.getSize(), "IloSameSequence: sequence variable and array of interval variables have different size.");
  IloAssert(IloCheckNoDuplicates(env, a1), "IloSameSequence: found duplicated interval variable in array.");
  IloAssert(IloCheckNoDuplicates(env, a2), "IloSameSequence: found duplicated interval variable in array.");
  IloAssert(IloCheckEqual(env, a1, seq1.getIntervals()), "IloSameSequence: interval variable array different from sequence variable one.");
  IloAssert(IloCheckEqual(env, a2, seq2.getIntervals()), "IloSameSequence: interval variable array different from sequence variable one.");
  return _IloSameSequence(env, seq1, seq2, a1, a2, name);
}

 
inline IloConstraint IloSameCommonSubsequence(const IloEnv env,
                                              const IloIntervalSequenceVar seq1,
                                              const IloIntervalSequenceVar seq2,
                                              const char* name=0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq1.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(seq2.getImpl(), "IloIntervalSequenceVar: empty handle.");
  return _IloSameCommonSubsequence(env, seq1, seq2, 0, 0, name);
}

  
inline IloConstraint IloSameCommonSubsequence(const IloEnv env,
                                              const IloIntervalSequenceVar seq1,
                                              const IloIntervalSequenceVar seq2,
                                              const IloIntervalVarArray a1,
                                              const IloIntervalVarArray a2,
                                              const char* name=0) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(seq1.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(seq2.getImpl(), "IloIntervalSequenceVar: empty handle.");
  IloAssert(a1.getImpl(), "IloIntervalVarArray: empty handle.");
  IloAssert(a2.getImpl(), "IloIntervalVarArray: empty handle.");
  IloAssert(a1.getSize()==a2.getSize(), "IloSameCommonSubsequence: arrays of interval variables have different size.");
  IloAssert(IloCheckNoDuplicates(env, a1), "IloSameCommonSubsequence: found duplicated interval variable in array.");
  IloAssert(IloCheckNoDuplicates(env, a2), "IloSameCommonSubsequence: found duplicated interval variable in array.");
  IloAssert(IloCheckIncluded(env, a1, seq1.getIntervals()), "IloSameCommonSubsequence: interval variable array not included in sequence variable.");
  IloAssert(IloCheckIncluded(env, a2, seq2.getIntervals()), "IloSameCommonSubsequence: interval variable array not included in sequence variable.");
  return _IloSameCommonSubsequence(env, seq1, seq2, a1, a2, name);
}
#endif

class IloCumulFunctionExpr : public IloExtractable {
  ILOEXTRHANDLE(IloCumulFunctionExpr, IloExtractable)
    public:
  
  IloCumulFunctionExpr(const IloEnv env, const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    _ctor(env, name);
  }
  IloBool isElementary() const;
  static IloCumulFunctionExpr Add(const IloCumulFunctionExpr f1,
                                  const IloCumulFunctionExpr f2);
  static IloCumulFunctionExpr Sub(const IloCumulFunctionExpr f1,
                                  const IloCumulFunctionExpr f2);
  static IloCumulFunctionExpr Neg(const IloCumulFunctionExpr f);

  
  IloCumulFunctionExpr& operator+=(const IloCumulFunctionExpr f) {
    IloAssert(getImpl(), "IloCumulFunctionExpr: empty handle.");
    IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
    *this = Add(*this, f);
    return *this;
  }

  
  IloCumulFunctionExpr& operator-=(const IloCumulFunctionExpr f) {
    IloAssert(getImpl(), "IloCumulFunctionExpr: empty handle.");
    IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
    *this = Sub(*this, f);
    return *this;
  }
private:
  friend IloConstraint operator<=(const IloCumulFunctionExpr f, IloInt max);
  friend IloConstraint operator<=(const IloCumulFunctionExpr f, const int max);
  friend IloConstraint operator<=(const IloCumulFunctionExpr f, const IloIntExprArg max);
  friend IloConstraint operator<=(IloInt min, const IloCumulFunctionExpr f);
  friend IloConstraint operator<=(const int min, const IloCumulFunctionExpr f);
  friend IloConstraint operator<=(const IloIntExprArg min, const IloCumulFunctionExpr f);

  friend IloConstraint operator>=(const int max, const IloCumulFunctionExpr f);
  friend IloConstraint operator>=(IloInt max, const IloCumulFunctionExpr f);
  friend IloConstraint operator>=(const IloIntExprArg max, const IloCumulFunctionExpr f);
  friend IloConstraint operator>=(const IloCumulFunctionExpr f, const int min);
  friend IloConstraint operator>=(const IloCumulFunctionExpr f, IloInt min);
  friend IloConstraint operator>=(const IloCumulFunctionExpr f, const IloIntExprArg min);

  friend IloConstraint IloAlwaysIn(const IloEnv env,
                                   const IloCumulFunctionExpr f,
                                   IloInt start, IloInt end,
                                   IloInt vmin, IloInt vmax);
  friend IloConstraint IloAlwaysIn(const IloEnv env,
                                   const IloCumulFunctionExpr f,
                                   const IloIntervalVar a,
                                   IloInt vmin, IloInt vmax);
  friend IloConstraint IloAlwaysEqual(const IloEnv env,
                                      const IloCumulFunctionExpr f,
                                      IloInt start, IloInt end,
                                      IloInt value);
  friend IloConstraint IloAlwaysEqual(const IloEnv env,
                                      const IloCumulFunctionExpr f,
                                      const IloIntervalVar a,
                                      IloInt value);
  friend IloIntExprArg IloHeightAtStart(const IloIntervalVar a,
                                        const IloCumulFunctionExpr f, IloInt absVal);
  friend IloIntExprArg IloHeightAtEnd  (const IloIntervalVar a,
                                        const IloCumulFunctionExpr f, IloInt absVal);
  friend IloIntExprArg IloMin(const IloCumulFunctionExpr f);
  friend IloIntExprArg IloMax(const IloCumulFunctionExpr f);
  friend IloIntExprArg IloMin(const IloCumulFunctionExpr f,
                              IloInt start, IloInt end);
  friend IloIntExprArg IloMax(const IloCumulFunctionExpr f,
                              IloInt start, IloInt end);

  void _ctor(const IloEnv env, const char* name);
  IloConstraint _lowerThan(IloInt max, IloBool fromLT) const;
  IloConstraint _lowerThan(const IloIntExprArg max, IloBool fromLT) const;
  IloConstraint _greaterThan(IloInt min, IloBool fromLT) const;
  IloConstraint _greaterThan(const IloIntExprArg min, IloBool fromLT) const;
  IloConstraint _alwaysIn(const IloEnv env, IloInt start, IloInt end, IloInt vmin, IloInt vmax) const;
  IloConstraint _alwaysIn(const IloEnv env, const IloIntervalVar a, IloInt vmin, IloInt vmax) const;
  IloConstraint _alwaysEqual(const IloEnv env, IloInt start, IloInt end, IloInt value) const;
  IloConstraint _alwaysEqual(const IloEnv env, const IloIntervalVar a, IloInt value) const;
  IloIntExprArg _heightAtStartExpr(const IloEnv env, const IloIntervalVar a, IloInt absVal) const;
  IloIntExprArg _heightAtEndExpr(const IloEnv env, const IloIntervalVar a, IloInt absVal) const;
  IloIntExprArg _min(IloInt start, IloInt end) const;
  IloIntExprArg _max(IloInt start, IloInt end) const;

public:
  static IloBool MatchTypeInfo(IloTypeInfo);
};

inline IloIntExprArg IloHeightAtStart(const IloIntervalVar a,
                                      const IloCumulFunctionExpr f, IloInt absVal=0) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._heightAtStartExpr(a.getEnv(), a, absVal);
}

inline IloIntExprArg IloHeightAtEnd(const IloIntervalVar a,
                                    const IloCumulFunctionExpr f, IloInt absVal=0) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._heightAtEndExpr(a.getEnv(), a, absVal);
}

IloCumulFunctionExpr _IloPulse(const IloEnv env, IloInt start, IloInt end, IloInt v);
IloCumulFunctionExpr _IloPulse(const IloEnv env, const IloIntervalVar a, IloInt v);
IloCumulFunctionExpr _IloPulse(const IloEnv env, const IloIntervalVar a, IloInt vmin, IloInt vmax);
IloCumulFunctionExpr _IloStepAtStart(const IloEnv env, const IloIntervalVar a, IloInt v);
IloCumulFunctionExpr _IloStepAtStart(const IloEnv env, const IloIntervalVar a, IloInt vmin, IloInt vmax);
IloCumulFunctionExpr _IloStepAtEnd(const IloEnv env, const IloIntervalVar a, IloInt v);
IloCumulFunctionExpr _IloStepAtEnd(const IloEnv env, const IloIntervalVar a, IloInt vmin, IloInt vmax);

inline IloCumulFunctionExpr IloStep(const IloEnv env, IloInt t, IloInt v) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(IloIntervalMin <= t, "IloStep: out of range point.");
  IloAssert(t < IloIntervalMax, "IloStep: out of range point.");
  IloAssert(0 <= v, "IloStep: negative value.");
  return _IloPulse(env, t, IloIntervalMax + 1, v);
}

inline IloCumulFunctionExpr IloPulse(const IloEnv env, IloInt start, IloInt end, IloInt v) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloPulse: out of range start.");
  IloAssert(start <= end, "IloPulse: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloPulse: out of range end.");
  IloAssert(0 <= v, "IloPulse: negative value.");
  return _IloPulse(env, start, end, v);
}

inline IloCumulFunctionExpr IloPulse(const IloIntervalVar a, IloInt v) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= v, "IloPulse: negative value.");
  return _IloPulse(a.getEnv(), a, v);
}

inline IloCumulFunctionExpr IloPulse(const IloIntervalVar a, IloInt vmin, IloInt vmax) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= vmin, "IloPulse: negative value.");
  IloAssert(vmin <= vmax, "IloPulse: empty range. ");
  return _IloPulse(a.getEnv(), a, vmin, vmax);
}

inline IloCumulFunctionExpr IloStepAtStart(const IloIntervalVar a, IloInt v) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= v, "IloStepAtStart: negative value.");
  return _IloStepAtStart(a.getEnv(), a, v);
}

inline IloCumulFunctionExpr IloStepAtStart(const IloIntervalVar a, IloInt vmin, IloInt vmax) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= vmin, "IloStepAtStart: negative value.");
  IloAssert(vmin <= vmax, "IloStepAtStart: empty range. ");
  return _IloStepAtStart(a.getEnv(), a, vmin, vmax);
}

inline IloCumulFunctionExpr IloStepAtEnd(const IloIntervalVar a, IloInt v) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= v, "IloStepAtEnd: negative value.");
  return _IloStepAtEnd(a.getEnv(), a, v);
}

inline IloCumulFunctionExpr IloStepAtEnd(const IloIntervalVar a, IloInt vmin, IloInt vmax) {
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= vmin, "IloStepAtEnd: negative value.");
  IloAssert(vmin <= vmax, "IloStepAtEnd: empty range. ");
  return _IloStepAtEnd(a.getEnv(), a, vmin, vmax);
}

inline IloCumulFunctionExpr operator + (const IloCumulFunctionExpr & f1,
                                        const IloCumulFunctionExpr & f2) {
  IloAssert(f1.getImpl() != 0, "IloCumulFunctionExpr: empty handle");
  IloAssert(f2.getImpl() != 0, "IloCumulFunctionExpr: empty handle");
  return IloCumulFunctionExpr::Add(f1, f2);
}

inline IloCumulFunctionExpr operator - (const IloCumulFunctionExpr & f1,
                                        const IloCumulFunctionExpr & f2) {
  IloAssert(f1.getImpl() != 0, "IloCumulFunctionExpr: empty handle");
  IloAssert(f2.getImpl() != 0, "IloCumulFunctionExpr: empty handle");
  return IloCumulFunctionExpr::Sub(f1, f2);
}

inline IloCumulFunctionExpr operator - (const IloCumulFunctionExpr & f) {
  IloAssert(f.getImpl() != 0, "IloCumulFunctionExpr: empty handle");
  return IloCumulFunctionExpr::Neg(f);
}

typedef IloArray<IloCumulFunctionExpr> IloCumulFunctionExprArray;

inline IloConstraint operator<=(const IloCumulFunctionExpr f, IloInt vmax) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._lowerThan(vmax, IloTrue);
}
inline IloConstraint operator<=(const IloCumulFunctionExpr f, const int vmax) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._lowerThan(IloInt(vmax), IloTrue);
}

inline IloConstraint operator<=(const IloCumulFunctionExpr f, const IloIntExprArg vmax) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(vmax.getImpl(), "IloIntExprArg: empty handle.");
  return f._lowerThan(vmax, IloTrue);
}

inline IloConstraint operator<=(IloInt vmin , const IloCumulFunctionExpr f) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._greaterThan(vmin, IloTrue);
}
inline IloConstraint operator<=(const int vmin, const IloCumulFunctionExpr f) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._greaterThan(IloInt(vmin), IloTrue);
}

inline IloConstraint operator<=(const IloIntExprArg vmin, const IloCumulFunctionExpr f) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(vmin.getImpl(), "IloIntExprArg: empty handle.");
  return f._greaterThan(vmin, IloTrue);
}

inline IloConstraint operator>=(IloInt vmax, const IloCumulFunctionExpr f) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._lowerThan(vmax, IloFalse);
}
inline IloConstraint operator>=(const int vmax, const IloCumulFunctionExpr f) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._lowerThan(IloInt(vmax), IloFalse);
}

inline IloConstraint operator>=(const IloIntExprArg vmax, const IloCumulFunctionExpr f) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(vmax.getImpl(), "IloIntExprArg: empty handle.");
  return f._lowerThan(vmax, IloFalse);
}

inline IloConstraint operator>=(const IloCumulFunctionExpr f, IloInt vmin) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._greaterThan(vmin, IloFalse);
}
inline IloConstraint operator>=(const IloCumulFunctionExpr f, const int vmin) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  return f._greaterThan(IloInt(vmin), IloFalse);
}

inline IloConstraint operator>=(const IloCumulFunctionExpr f, const IloIntExprArg vmin) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(vmin.getImpl(), "IloIntExprArg: empty handle.");
  return f._greaterThan(vmin, IloFalse);
}

inline IloConstraint IloAlwaysIn(const IloEnv env,
                                 const IloCumulFunctionExpr f,
                                 IloInt start, IloInt end,
                                 IloInt vmin, IloInt vmax) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloAlwaysIn: out of range start.");
  IloAssert(start <= end, "IloAlwaysIn: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloAlwaysIn: out of range end.");
  IloAssert(0 <= vmin, "IloAlwaysIn: negative value.");
  IloAssert(0 <= vmax, "IloAlwaysIn: negative value. ");
  return f._alwaysIn(env, start, end, vmin, vmax);
}

inline IloConstraint IloAlwaysIn(const IloEnv env,
                                 const IloCumulFunctionExpr f,
                                 const IloIntervalVar a,
                                 IloInt vmin, IloInt vmax) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= vmin, "IloAlwaysIn: negative value.");
  IloAssert(vmin <= vmax, "IloAlwaysIn: empty range. ");
  return f._alwaysIn(env, a, vmin, vmax);
}

inline IloConstraint IloAlwaysEqual(const IloEnv env,
                                    const IloCumulFunctionExpr f,
                                    IloInt start, IloInt end,
                                    IloInt v) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloAlwaysEqual: out of range start.");
  IloAssert(start <= end, "IloAlwaysEqual: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloAlwaysEqual: out of range end.");
  IloAssert(0 <= v, "IloAlwaysEqual: negative value.");
  return f._alwaysEqual(env, start, end, v);
}

inline IloConstraint IloAlwaysEqual(const IloEnv env,
                                    const IloCumulFunctionExpr f,
                                    const IloIntervalVar a,
                                    IloInt v) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= v, "IloAlwaysEqual: negative value.");
  return f._alwaysEqual(env, a, v);
}

inline IloIntExprArg IloMin(const IloCumulFunctionExpr f,
                            IloInt start, IloInt end) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloMin: out of range start.");
  IloAssert(start <= end, "IloMin: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloMin: out of range end.");
  return f._min(start, end);
}
inline IloIntExprArg IloMax(const IloCumulFunctionExpr f,
                            IloInt start, IloInt end) {
  IloAssert(f.getImpl(), "IloCumulFunctionExpr: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloMax: out of range start.");
  IloAssert(start <= end, "IloMax: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloMax: out of range end.");
  return f._max(start, end);
}
inline IloIntExprArg IloMin(const IloCumulFunctionExpr f) {
  return IloMin(f, IloIntervalMin, IloIntervalMax);
}
inline IloIntExprArg IloMax(const IloCumulFunctionExpr f) {
  return IloMax(f, IloIntervalMin, IloIntervalMax);
}

class IloStateFunction : public IloExtractable {
  ILOEXTRHANDLE(IloStateFunction, IloExtractable)
    public:
  
  IloStateFunction(const IloEnv env,
                   const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    _ctor(env, name);
  }
    
  IloStateFunction(const IloEnv env,
                   const IloTransitionDistance tdist,
                   const char* name =0) {
    IloAssert(env.getImpl(), "IloEnv: empty handle.");
    IloAssert(tdist.getImpl(), "IloTransitionDistance: empty handle.");
    _ctor(env, tdist, name);
  }
private:
  friend IloConstraint IloAlwaysNoState(const IloEnv env,
                                        const IloStateFunction f,
                                        IloInt start, IloInt end);
  friend IloConstraint IloAlwaysNoState(const IloEnv env,
                                        const IloStateFunction f,
                                        const IloIntervalVar a);
  friend IloConstraint IloAlwaysIn(const IloEnv env,
                                   const IloStateFunction f,
                                   IloInt start, IloInt end,
                                   IloInt min, IloInt max);
  friend IloConstraint IloAlwaysIn(const IloEnv env,
                                   const IloStateFunction f,
                                   const IloIntervalVar a,
                                   IloInt min, IloInt max);
  friend IloConstraint IloAlwaysConstant(const IloEnv env,
                                         const IloStateFunction f,
                                         IloInt start, IloInt end,
                                         IloBool startAlign,
                                         IloBool endAlign);
  friend IloConstraint IloAlwaysConstant(const IloEnv env,
                                         const IloStateFunction f,
                                         const IloIntervalVar a,
                                         IloBool startAlign,
                                         IloBool endAlign);
  friend IloConstraint IloAlwaysEqual(const IloEnv env,
                                      const IloStateFunction f,
                                      IloInt start, IloInt end,
                                      IloInt state,
                                      IloBool startAlign,
                                      IloBool endAlign);
  friend IloConstraint IloAlwaysEqual(const IloEnv env,
                                      const IloStateFunction f,
                                      const IloIntervalVar a,
                                      IloInt state,
                                      IloBool startAlign,
                                      IloBool endAlign);
  void _ctor(const IloEnv env,
             const char* name);
  void _ctor(const IloEnv env,
             const IloTransitionDistance tdist,
             const char* name);
  IloConstraint _alwaysNoState(const IloEnv env,
                               IloInt start, IloInt end,
                               const char* name) const;
  IloConstraint _alwaysNoState(const IloEnv env,
                               const IloIntervalVar a,
                               const char* name) const;
  IloConstraint _always(const IloEnv env,
                        IloInt start, IloInt end,
                        IloBool range, IloBool min, IloBool max,
                        IloBool align, IloBool startAlign, IloBool endAlign,
                        const char* name) const;
  IloConstraint _always(const IloEnv env,
                        const IloIntervalVar a,
                        IloBool range, IloBool min, IloBool max,
                        IloBool align, IloBool startAlign, IloBool endAlign,
                        const char* name) const;
};

typedef IloArray<IloStateFunction> IloStateFunctionArray;

inline IloConstraint IloAlwaysNoState(const IloEnv env,
                                      const IloStateFunction f,
                                      IloInt start, IloInt end) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloAlwaysNoState: out of range start.");
  IloAssert(start <= end, "IloAlwaysNoState: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloAlwaysNoState: out of range end.");
  return f._alwaysNoState(env, start, end, 0);
}

inline IloConstraint IloAlwaysNoState(const IloEnv env,
                                      const IloStateFunction f,
                                      const IloIntervalVar a) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return f._alwaysNoState(env, a, 0);
}

inline IloConstraint IloAlwaysIn(const IloEnv env,
                                 const IloStateFunction f,
                                 IloInt start, IloInt end,
                                 IloInt vmin, IloInt vmax) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloAlwaysIn: out of range start.");
  IloAssert(start <= end, "IloAlwaysIn: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloAlwaysIn: out of range end.");
  return f._always(env, start, end,
                   IloTrue, vmin, vmax,
                   IloFalse, IloFalse, IloFalse,
                   0);
}

inline IloConstraint IloAlwaysIn(const IloEnv env,
                                 const IloStateFunction f,
                                 const IloIntervalVar a,
                                 IloInt vmin, IloInt vmax) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= vmin, "IloAlwaysIn: negative value.");
  IloAssert(vmin <= vmax, "IloAlwaysIn: empty range. ");
  return f._always(env,a,
                   IloTrue, vmin, vmax,
                   IloFalse, IloFalse, IloFalse,
                   0);
}

inline IloConstraint IloAlwaysConstant(const IloEnv env,
                                       const IloStateFunction f,
                                       IloInt start, IloInt end,
                                       IloBool startAlign =IloFalse,
                                       IloBool endAlign =IloFalse) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloAlwaysConstant: out of range start.");
  IloAssert(start <= end, "IloAlwaysConstant: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloAlwaysConstant: out of range end.");
  return f._always(env, start, end,
                   IloFalse, 0, 0,
                   IloTrue, startAlign, endAlign,
                   0);
}

inline IloConstraint IloAlwaysConstant(const IloEnv env,
                                       const IloStateFunction f,
                                       const IloIntervalVar a,
                                       IloBool startAlign =IloFalse,
                                       IloBool endAlign =IloFalse) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  return f._always(env, a,
                   IloFalse, 0, 0,
                   IloTrue, startAlign, endAlign,
                   0);
}

inline IloConstraint IloAlwaysEqual(const IloEnv env,
                                    const IloStateFunction f,
                                    IloInt start, IloInt end,
                                    IloInt v,
                                    IloBool startAlign =IloFalse,
                                    IloBool endAlign =IloFalse) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(IloIntervalMin <= start, "IloAlwaysEqual: out of range start.");
  IloAssert(start <= end, "IloAlwaysEqual: empty interval.");
  IloAssert(end <= IloIntervalMax, "IloAlwaysEqual: out of range end.");
  IloAssert(0 <= v, "IloAlwaysEqual: negative value.");
  return f._always(env, start, end,
                   IloTrue, v, v,
                   IloTrue, startAlign, endAlign,
                   0);
}

inline IloConstraint IloAlwaysEqual(const IloEnv env,
                                    const IloStateFunction f,
                                    const IloIntervalVar a,
                                    IloInt v,
                                    IloBool startAlign =IloFalse,
                                    IloBool endAlign =IloFalse) {
  IloAssert(env.getImpl(), "IloEnv: empty handle.");
  IloAssert(f.getImpl(), "IloStateFunction: empty handle.");
  IloAssert(a.getImpl(), "IloIntervalVar: empty handle.");
  IloAssert(0 <= v, "IloAlwaysEqual: negative value.");
  return f._always(env, a,
                   IloTrue, v, v,
                   IloTrue, startAlign, endAlign,
                   0);
}

#define IloStateFunctionExprArgI IloStateFunctionExprI

class IloStateFunctionExpr : public IloExtractable {
  ILOEXTRHANDLE(IloStateFunctionExpr, IloExtractable)
    public:
  
  static IloBool MatchTypeInfo(IloTypeInfo);
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
