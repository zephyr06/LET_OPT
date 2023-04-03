// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloevaluator.h
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
#ifndef __CONCERT_iloevaluatorH
#define __CONCERT_iloevaluatorH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/ilomapi.h>

class IloModelEvaluator {
private:
	IloModelEvaluatorI* _impl;
public:
	IloModelEvaluator():_impl(0) {}
	IloModelEvaluator(IloModelEvaluatorI* impl):_impl(impl) {}
	IloModelEvaluatorI* getImpl() const { return _impl; }
	IloModelEvaluator(IloEnv env);
	IloInt evaluate(IloIntExprArg exp) const;
	IloNum evaluate(IloNumExprArg exp) const;
	IloNumExprArg evaluateToNumExpr(IloNumExprArg exp) const;
	
	class Unbound : public IloException {
		IloExtractableI* _exp;
	public:
		Unbound(IloExtractableI* exp);
		IloExtractable getExpr() const;
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
		virtual const char* getMessage() const ILO_OVERRIDE;
	};
public:
	enum ErrorStatus {
		None,
		OverflowOccurred
	};
};

class IloModelEvaluatorSolutionGetterBaseI : public IloRttiEnvObjectI {
	ILORTTIDECL
		IloAlgorithmI* _alg;
public:
	IloModelEvaluatorSolutionGetterBaseI(IloEnvI* env, IloAlgorithm alg) : IloRttiEnvObjectI(env), _alg(alg.getImpl()) {}
	IloAlgorithmI* getAlgorithmI() const { return _alg; }
	IloAlgorithm getAlgorithm() const { return _alg; }
	enum IntervalField {
		BitPresent,
		BitAbsent,
		Start,
		End,
		Length,
		Size
	};	
	enum SequenceDirection {
		LeftToRight,
		RightToLeft
	};
	enum FunctionField {
		NumberOfSegments,
		SegmentStart,
		SegmentEnd,
		SegmentValue,
		FunctionValue,
		_Internal_Concert
	};
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
