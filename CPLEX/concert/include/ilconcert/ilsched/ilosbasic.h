// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilsched/ilosbasic.h
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

#ifndef __CONCERT_ilosbasicH
#define __CONCERT_ilosbasicH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloalg.h>
#include <ilconcert/iloextractable.h>

class IloSchedulerI;

#define ILOS_BITSET_MASK(bit) (((IloUInt)1) << bit)

inline void IloBitSet(IloUInt& s, const IloUInt bit) {
	s |= ILOS_BITSET_MASK(bit); }

inline void IloBitSet(IloUInt& s, const IloUInt bit1, const IloUInt bit2) {
	s |= (ILOS_BITSET_MASK(bit1) | ILOS_BITSET_MASK(bit2)); }

inline void IloBitSet(IloUInt& s, const IloUInt bit1, const IloUInt bit2, const IloUInt bit3) {
	s |= (ILOS_BITSET_MASK(bit1) | ILOS_BITSET_MASK(bit2) | ILOS_BITSET_MASK(bit3)); }

inline void IloBitUnset(IloUInt& s, const IloUInt bit) {
	s &= ~ILOS_BITSET_MASK(bit); }

inline void IloBitUnset(IloUInt& s, const IloUInt bit1, const IloUInt bit2) {
	s &= ~(ILOS_BITSET_MASK(bit1) | ILOS_BITSET_MASK(bit2)); }

inline IloBool IloBitTest(const IloUInt s, const IloUInt bit) {
	return s & ILOS_BITSET_MASK(bit); }

inline IloBool IloBitTest(const IloUInt s, const IloUInt bit1, const IloUInt bit2) {
	return s & (ILOS_BITSET_MASK(bit1) | ILOS_BITSET_MASK(bit2)); }

inline IloBool IloBitTest(const IloUInt s, const IloUInt bit1, const IloUInt bit2, const IloUInt bit3) {
	return s & (ILOS_BITSET_MASK(bit1) | ILOS_BITSET_MASK(bit2) | ILOS_BITSET_MASK(bit3)); }

inline IloBool IloBitTest(const IloUInt s, const IloUInt bit1, const IloUInt bit2, const IloUInt bit3, const IloUInt bit4) {
	return s & (ILOS_BITSET_MASK(bit1) | ILOS_BITSET_MASK(bit2) | ILOS_BITSET_MASK(bit3) | ILOS_BITSET_MASK(bit4)); }

inline IloBool IloBitTest(const IloUInt s, const IloUInt bit1, const IloUInt bit2, const IloUInt bit3, const IloUInt bit4, const IloUInt bit5) {
	return s & (ILOS_BITSET_MASK(bit1) | ILOS_BITSET_MASK(bit2) | ILOS_BITSET_MASK(bit3) | ILOS_BITSET_MASK(bit4) | ILOS_BITSET_MASK(bit5)); }

inline IloBool IloBitTestBoth(const IloUInt s, const IloUInt bit1, const IloUInt bit2) {
	
	if (bit2 > bit1) {
		const IloUInt diff = bit2 - bit1;
		return ((s & ILOS_BITSET_MASK(bit1)) << diff) & s;
	} else {
		const IloUInt diff = bit1 - bit2;
		return ((s & ILOS_BITSET_MASK(bit2)) << diff) & s;
	}
}  

class IloPrecedenceI;

class IloSched {
public:
	enum PrecedenceTypeBits {
		BitSource       = 0,  
		BitAtBefore     = 1,  
		BitTarget       = 2,  
		BitDelayNumExpr = 3   
	};

	enum PrecedenceTypeMasks {
		MaskTarget       = ILOS_BITSET_MASK(BitTarget),
		MaskAtBefore     = ILOS_BITSET_MASK(BitAtBefore),
		MaskSource       = ILOS_BITSET_MASK(BitSource),
		MaskDelayNumExpr = ILOS_BITSET_MASK(BitDelayNumExpr)  
	};

	enum PrecedenceType {
		EndsBeforeEnd     = 0*MaskSource | 0*MaskAtBefore | 0*MaskTarget,
		EndsBeforeStart   = 0*MaskSource | 0*MaskAtBefore | 1*MaskTarget,
		EndsAtEnd         = 0*MaskSource | 1*MaskAtBefore | 0*MaskTarget,
		EndsAtStart       = 0*MaskSource | 1*MaskAtBefore | 1*MaskTarget,
		StartsBeforeEnd   = 1*MaskSource | 0*MaskAtBefore | 0*MaskTarget,
		StartsBeforeStart = 1*MaskSource | 0*MaskAtBefore | 1*MaskTarget,
		StartsAtEnd       = 1*MaskSource | 1*MaskAtBefore | 0*MaskTarget,
		StartsAtStart     = 1*MaskSource | 1*MaskAtBefore | 1*MaskTarget,
		PrecedenceTypeMax = StartsAtStart   
	};

	static PrecedenceType getType( const IloPrecedenceI* );

	enum IntegralRoundingMode {
		IntegralRoundUpward   = 0,
		IntegralRoundDownward = 1,
		IntegralRoundOutward  = 2,
		IntegralRoundInward   = 3
	};

	enum StatusRelationType {
		Implies,
		ImpliesNot,
		OrWith,
		EquivalentTo,
		OppositeOf
	};

	enum Range {
		Start,
		End,
		Length,
		Size
	};

	enum Shape {
		
		
		
		
		
		IncreaseOnInterval     = 0,  
		IncreaseAtStart        = 2,  
		IncreaseAtEnd          = 3,  
		IncreaseContinuously   = 4,  
		DecreaseOnInterval     = 8,  
		DecreaseAtStart        = 10, 
		DecreaseAtEnd          = 11, 
		DecreaseContinuously   = 12  
	};

	static IloBool IsDecreaseShape  (IloSched::Shape s) { return (0 != (s & (IloUInt)8)); }
	static IloBool IsContinuousShape(IloSched::Shape s) { return (0 != (s & (IloUInt)4)); }
	static IloBool IsOnIntervalShape(IloSched::Shape s) { return (0 == (s & (IloUInt)2)); }
	static IloBool IsFromStartShape (IloSched::Shape s) { return (3 != (s & (IloUInt)3)); }
	static IloBool IsFromEndShape   (IloSched::Shape s) { return (3 == (s & (IloUInt)3)); }
	static IloBool IsToEndShape     (IloSched::Shape s) { return (0 == (s & (IloUInt)2)); }  

	enum DemandIteratorFilter {
		AllDemands              = 0,
		PossibleDemands          = 1,
		DirectPredecessors       = 3,
		DirectSuccessors         = 4,
		Unranked                 = 5,
		Predecessors             = 6,
		Successors               = 7,
		PossiblePrevious         = 8,
		PossibleNext             = 9,
		PredSucc                 = 10,
		StrictDirectPredecessors = 11,
		StrictDirectSuccessors   = 12,
		StrictPredecessors       = 13,
		StrictSuccessors         = 14,
		Simultaneous             = 15
	};

	
	enum SegmentFunctionEvaluationType {
		EvaluateCumulNumberOfSegments = 0,
		EvaluateCumulSegmentStart     = 2,
		EvaluateCumulSegmentEnd       = 4,
		EvaluateCumulSegmentValue     = 6,
		EvaluateCumulValue            = 8,
		EvaluateStateNumberOfSegments = 1,
		EvaluateStateSegmentStart     = 3,
		EvaluateStateSegmentEnd       = 5,
		EvaluateStateSegmentValue     = 7,
		EvaluateStateValue            = 9
	};
};

#define ILOS_RTTI_NOCOPY(_iloclass)                                     \
	_iloclass& operator=(const _iloclass&);                               \
	_iloclass(const _iloclass&);

#define ILOS_RTTI_DECL_NOCOPY(_iloclass)                                \
	ILORTTIDECL                                                           \
	ILOS_RTTI_NOCOPY(_iloclass)

#define ILOS_CPEXTR_DECL(_iloclass, _ilobaseclass)                      \
	ILOEXTRDECL                                                           \
	ILOS_RTTI_NOCOPY(_iloclass)                                           \
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;

#define ILO_BEGIN_VISIT         v->beginVisit(this)
#define ILO_END_VISIT           v->endVisit(this)
#define ILO_VISIT(sub)          v->visitChildren( this, (sub) )
#define ILO_VISIT_SUB(extr,sub) v->visitChildren( (extr), (sub) )

#define ILOSEXTRACTEXTENSION(_ex, _obj)			\
	if (_ex->getName() != 0)				\
	_obj->setName(_ex->getName());			\
	if (_ex->getObject() != 0)				\
	_obj->setObject(_ex->getObject())

#define ILOSSETEXTRACTABLE(_obj, _ex) _obj->setExtractable((IloAny)_ex)

typedef IloNum (*IloEvalCP)(const IloAlgorithm alg,
							const IloExtractableI* ext);

#define ILOSCHANGE_MAX  3

class IloSchedChange: public IloChange {
	ILOCHANGEDECL
public:
	enum IloChangeMode {
		IloMonotonicChange,       
		IloNonMonotonicChange,    
		IloDestructiveChange      
	};
private:
	IloChangeMode    _changeMode;                   
	IloExtractableI* _extractables[ILOSCHANGE_MAX]; 
	IloBool          _added[ILOSCHANGE_MAX];        
	
	
	
public:
	IloSchedChange(IloChangeMode changeMode = IloDestructiveChange);
	IloSchedChange(IloBool added, IloExtractableI* extractable,
		IloChangeMode changeMode = IloDestructiveChange);
	IloSchedChange(IloBool added, 
		IloExtractableI* extractable1, 
		IloExtractableI* extractable2,
		IloChangeMode changeMode = IloDestructiveChange);
	IloSchedChange(IloBool added, 
		IloExtractableI* extractable1,
		IloExtractableI* extractable2,
		IloExtractableI* extractable3,
		IloChangeMode changeMode = IloDestructiveChange);
	IloSchedChange(IloBool added1, IloExtractableI* extractable1, 
		IloBool added2, IloExtractableI* extractable2,
		IloChangeMode changeMode = IloDestructiveChange);

	IloChangeMode getChangeMode() const { return _changeMode; }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
