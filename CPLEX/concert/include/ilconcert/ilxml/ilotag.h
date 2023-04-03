// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/ilotag.h
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
#ifndef __XML_ilotagH
#define __XML_ilotagH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

class IloXmlTagDef{
public:
	static const char* None;
	static const char* Unknown;
	static const char* IloXmlModel;
	static const char* IloXmlModeler;
	static const char* IloXmlSolution;
	static const char* IloXmlObject;

	static const char* IloXmlRoot;

	
	static const char* IloXmlIntSum;
	static const char* IloXmlNumSum;
	static const char* IloXmlIntOpposite;
	static const char* IloXmlNumOpposite;

	static const char* IloXmlIntNeq;
	static const char* IloXmlNumNeq;
	static const char* IloXmlIntNeqCst;
	static const char* IloXmlNumNeqCst;

	static const char* IloXmlAllDiff;
	static const char* IloXmlAllMinDistance;
	static const char* IloXmlAnd;
	static const char* IloXmlArcCos;
	static const char* IloXmlArcSin;
	static const char* IloXmlArcTan;
	static const char* IloXmlBool;
	static const char* IloXmlBoolArray;
	static const char* IloXmlBoolIntAbstraction;
	static const char* IloXmlBoolVar;
	static const char* IloXmlBoolVarArray;
	static const char* IloXmlBox;
	static const char* IloXmlPack;
	static const char* IloXmlConstraint;
	static const char* IloXmlConstConstraint;
	static const char* IloXmlConstraintArray;
	static const char* IloXmlConstraintIndVar;
	static const char* IloXmlContainmentConstraint;
	static const char* IloXmlConversion;
	static const char* IloXmlCos;
	static const char* IloXmlDistribute;
	static const char* IloXmlDistributeV;
	static const char* IloXmlExponent;
	static const char* IloXmlExprPieceWiseLinear;
	static const char* IloXmlExtractable;
	static const char* IloXmlFloat;
	static const char* IloXmlIfThen;
	static const char* IloXmlImply;
	static const char* IloXmlInt;
	static const char* IloXmlIntAbs;
	static const char* IloXmlIntAbstraction;
	static const char* IloXmlIntAllNullIntersect;
	static const char* IloXmlIntArray;
	static const char* IloXmlIntArrayEqUnion;
	static const char* IloXmlIntArrayElement;
	static const char* IloXmlIntArrayMember;
	static const char* IloXmlIntModulo;
	static const char* IloXmlIntModuloCst;
	static const char* IloXmlIntDiv;
	static const char* IloXmlIntDivCst;
	static const char* IloXmlIntEqIntersection;
	static const char* IloXmlIntEqPartition;
	static const char* IloXmlIntEqUnion;
	static const char* IloXmlIntExpr;
	static const char* IloXmlIntQuadExpr;
	static const char* IloXmlIntQuadExprTerm;
	static const char* IloXmlIntExprArray;
	static const char* IloXmlIntExprArrayElement;
	static const char* IloXmlIntLinTerm;
	static const char* IloXmlIntScalProd;
	static const char* IloXmlIntMax;
	static const char* IloXmlIntMaxArray;
	static const char* IloXmlIntMaxCst;
	static const char* IloXmlIntMember;

	static const char* IloXmlIntMemberCst;
	static const char* IloXmlIntMin;
	static const char* IloXmlIntMinArray;
	static const char* IloXmlIntMinCst;
	static const char* IloXmlIntNotMember;
	static const char* IloXmlIntNotMemberCst;
	static const char* IloXmlIntNullIntersect;
	static const char* IloXmlIntPartition1;
	static const char* IloXmlIntPartition2;
	static const char* IloXmlIntRecDivCst;
	static const char* IloXmlIntSetCardVar;
	static const char* IloXmlIntSetNeq;
	static const char* IloXmlIntSetNeqCst;
	static const char* IloXmlIntSetEq;
	static const char* IloXmlIntSetEqCst;
	static const char* IloXmlIntSetVar;
	static const char* IloXmlIntSetVarArray;
	static const char* IloXmlIntSquare;
	static const char* IloXmlIntSubset;
	static const char* IloXmlIntSubsetCst;
	static const char* IloXmlIntSubsetEq;
	static const char* IloXmlNotSubsetEq;
	static const char* IloXmlIntSubsetEqCst;
	static const char* IloXmlIntTableElementConstraint2;
	static const char* IloXmlIntTableSetConstraint;
	static const char* IloXmlIntTimes;
	static const char* IloXmlIntTimesCst;
	static const char* IloXmlIntTupleSet;
	static const char* IloXmlIntVar;
	static const char* IloXmlIntEnumeratedVar;
	static const char* IloXmlIntVarArray;
	static const char* IloXmlInverse;
	static const char* IloXmlSetInverse;
	static const char* IloXmlStrong;
	static const char* IloXmlLog;
	static const char* IloXmlNot;
	static const char* IloXmlNotOverlapConstraint;
	static const char* IloXmlNotOverlapInDimensionConstraint;
	static const char* IloXmlNum;
	static const char* IloXmlNumAbs;
	static const char* IloXmlNumArrayMember;
	static const char* IloXmlNumDiv;
	static const char* IloXmlNumDivCst;
	static const char* IloXmlNumExpr;
	static const char* IloXmlNumQuadExpr;
	static const char* IloXmlNumQuadExprTerm;
	static const char* IloXmlNumExprArray;
	static const char* IloXmlNumArrayElement;
	static const char* IloXmlNumExprArrayElement;
	static const char* IloXmlNumLinTerm;
	static const char* IloXmlNumEq;
	static const char* IloXmlNumGe;
	static const char* IloXmlNumLe;
	static const char* IloXmlNumGT;
	static const char* IloXmlNumLT;

	static const char* IloXmlNumCeil;
	static const char* IloXmlNumFloor;
	static const char* IloXmlNumTrunc;
	static const char* IloXmlDistToInt;
	static const char* IloXmlNumFract;
	static const char* IloXmlNumRound;
	static const char* IloXmlNumToInt;
	static const char* IloXmlCount;
	static const char* IloXmlStandardDeviation;

	static const char* IloXmlIntEq;
	static const char* IloXmlIntGe;
	static const char* IloXmlIntLe;
	static const char* IloXmlNumGeCst;
	static const char* IloXmlNumLeCst;
	static const char* IloXmlIntGeCst;
	static const char* IloXmlIntLeCst;
	static const char* IloXmlIntGtCst;
	static const char* IloXmlIntLtCst;
	static const char* IloXmlNumEqCst;
	static const char* IloXmlIntEqCst;

	static const char* IloXmlNumScalProd;
	static const char* IloXmlNumMax;
	static const char* IloXmlNumMaxArray;
	static const char* IloXmlNumMaxCst;
	static const char* IloXmlNumMin;
	static const char* IloXmlNumMinArray;
	static const char* IloXmlNumMinCst;
	static const char* IloXmlNumPower;
	static const char* IloXmlNumPowerCst;
	static const char* IloXmlNumRecDivCst;
	static const char* IloXmlNumRecPowerCst;
	static const char* IloXmlNumSquare;
	static const char* IloXmlNumTableElementConstraint2;
	static const char* IloXmlNumTableSetConstraint;
	static const char* IloXmlNumTimes;
	static const char* IloXmlNumTimesCst;
	static const char* IloXmlNumTupleSet;
	static const char* IloXmlLinearNumVar;
	static const char* IloXmlLinearIntVar;
	static const char* IloXmlNumVar;
	static const char* IloXmlNumEnumeratedVar;
	static const char* IloXmlNumVarArray;
	static const char* IloXmlObjective;
	static const char* IloXmlOr;
	static const char* IloXmlOverlapInDimensionConstraint;
	static const char* IloXmlPrecedenceInDimensionConstraint;
	static const char* IloXmlRange;
	static const char* IloXmlRangeArray;
	static const char* IloXmlSemiContVar;
	static const char* IloXmlSemiContVarArray;
	static const char* IloXmlSequence;
	static const char* IloXmlSin;
	static const char* IloXmlSOS1;
	static const char* IloXmlSOS1Array;
	static const char* IloXmlSOS2;
	static const char* IloXmlSOS2Array;
	static const char* IloXmlTan;

	
	static const char* IloXmlNumArray;
	static const char* IloXmlRONumArray;
	static const char* IloXmlRONumSet;
	static const char* IloXmlIntSet;
	static const char* IloXmlNumSet;
	static const char* IloXmlIntRange;
	
	static const char* XmlTerm;
	static const char* XmlLinearPart;
	static const char* XmlNonLinearPart;
	static const char* XmlConstantPart;
	static const char* XmlDomain;
	static const char* XmlIntDomain;
	static const char* XmlBoolDomain;
	static const char* XmlIntEnumeratedDomain;
	static const char* XmlFloatDomain;
	static const char* XmlFloatEnumeratedDomain;

	static const char* LeftExpr;
	static const char* RightExpr;
	static const char* Constraint;
	static const char* AVars;
	static const char* Var ;
	static const char* Set ;
	static const char* SetVar ;
	static const char* VarArray;
	static const char* Array;
	static const char* Elements;
	static const char* Values;
	static const char* ValueArray;
	static const char* Vals;
	static const char* Array1;
	static const char* Array2;
	static const char* Vars;
	static const char* Cards;
	static const char* Box;
	static const char* Load;
	static const char* Where;
	static const char* Weight;
	static const char* Used;
	static const char* UsedSet;
	static const char* ContainingBox;
	static const char* ContainedBox;
	static const char* Expr;
	static const char* ExprArray;
	static const char* Indexer;
	static const char* Expr1;
	static const char* Expr2;
	static const char* Expr3;
	static const char* Var1;
	static const char* Var2;
	static const char* Var3;
	static const char* XVar;
	static const char* YVar;
	static const char* Origin;
	static const char* Box1;
	static const char* Box2;
	static const char* PossibleSet;
	static const char* RequiredSet;

	static const char* Condition;
	static const char* Then;
	static const char* Else;
	static const char* Owner;
};

class IloXmlVersionDef{
public:
	static const char* IloModel;
};

class IloXmlAttributeDef {
public:
	static const char* None;
	
	static const char* Partial;
	static const char* AbstractValue;
	static const char* Arity;
	static const char* Bd;
	static const char* ScLb;
	static const char* Coef;
	static const char* Compatible;
	static const char* Cst;
	static const char* Copy;
	static const char* Dimension;
	static const char* ExprArray;
	static const char* firstSlope;
	static const char* Id;
	static const char* IdRef;
	static const char* Ref;
	static const char* K;
	static const char* lastSlope;
	static const char* LowerBound;
	static const char* Name;
	static const char* NbMax;
	static const char* NbMin;
	static const char* Sense;
	static const char* SeqWidth;
	static const char* Set;
	static const char* Size;
	static const char* Type;
	static const char* ObjectType;
	static const char* UpperBound;
	static const char* useSlope;
	static const char* Slot;
	static const char* Value;
	static const char* Version;
	static const char* SolutionValue;
	static const char* SolutionUpperBound;
	static const char* SolutionLowerBound;
	static const char* ThenValue;
	static const char* ElseValue;

	static const char* Min;
	static const char* Max;
};

class IloXmlAttributeValue{
public:
	static const char* Boolean;
	static const char* Float;
	static const char* Int;
	static const char* LowerBound;
	static const char* Maximize;
	static const char* Minimize;
	static const char* No;
	static const char* None;
	static const char* Point;
	static const char* PossibleSet;
	static const char* RequiredSet;
	static const char* SolutionPossibleSet;
	static const char* SolutionRequiredSet;  
	static const char* Slope;
	static const char* UpperBound;
	static const char* Yes;
	static const char* NumConst;
	static const char* IntConst;
	static const char* NumVar;
	static const char* IntVar;

};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

