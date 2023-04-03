// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilobox.h
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

#ifndef __CONCERT_iloboxH
#define __CONCERT_iloboxH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>

class IloNotOverlapConstraintI;
class IloContainmentConstraintI;
class IloOverlapInDimensionConstraintI;
class IloNotOverlapInDimensionConstraintI;
class IloPrecedenceInDimensionConstraintI;

class IloBoxI : public IloConstraintI {
  ILOEXTRDECL
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
  IloInt _dimensions;
  IloIntVarArray _origin;
  IloIntArray _size;
  IloNotOverlapConstraintI* _notOverlapConstraint;
  IloBoxI(IloEnvI* m,
          IloInt dimensions,
	  const IloIntVarArray origin,
          const IloIntArray size,
          const char* name = 0);
  IloInt getDimensions() const { return _dimensions; }
  IloIntVarArray getOriginArray() const { return _origin; }
  IloIntVar getOrigin(IloInt dimension) const { return _origin[dimension]; }
  IloIntArray getSizeArray() const {return _size; }
  IloInt getSize(IloInt dimension) const {return _size[dimension]; }
  IloContainmentConstraintI* contains(IloBoxI* box);
  IloOverlapInDimensionConstraintI*
    overlapInDimension(IloBoxI* box1, IloBoxI* box2, IloInt dimension);
  IloNotOverlapInDimensionConstraintI*
    notOverlapInDimension(IloBoxI* box1, IloBoxI* box2, IloInt dimension);
  IloPrecedenceInDimensionConstraintI*
    precedenceInDimension(IloBoxI* box1, IloBoxI* box2, IloInt dimension);
  IloNotOverlapConstraintI* getNotOverlapConstraint();
  virtual ~IloBoxI();

  ILOEXTROTHERDECL
};

class ILO_EXPORTED IloBox : public IloConstraint {
  ILOEXTRHANDLE(IloBox,IloConstraint)
public:

  IloBox(const IloEnv env, IloInt dimensions,
         const IloIntVarArray origin, const IloIntArray size);

  inline IloInt getDimensions();

  inline IloIntVar getOrigin(IloInt dimension);

  inline IloInt getSize(IloInt dimension);

  inline IloConstraint contains(IloBox box);

  inline IloConstraint overlapInDimension(IloBox box1,
					  IloBox box2,
					  IloInt dimension);

  inline IloConstraint notOverlapInDimension(IloBox box1,
					     IloBox box2,
					     IloInt dimension);

  inline IloConstraint precedenceInDimension(IloBox box1,
					     IloBox box2,
					     IloInt dimension);

  inline IloConstraint getNotOverlapConstraint();
};

IloInt IloBox::getDimensions() {
  return getImpl()->getDimensions();
}

IloIntVar IloBox::getOrigin(IloInt dimension) {
  return getImpl()->getOrigin(dimension);
}

IloInt IloBox::getSize(IloInt dimension) {
  return getImpl()->getSize(dimension);
}

class IloNotOverlapConstraintI : public IloConstraintI {
  ILOEXTRDECL
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
  IloBoxI* _box;
  IloNotOverlapConstraintI(IloEnvI* m, IloBoxI* box, const char* name = 0);
  virtual ~IloNotOverlapConstraintI();

  IloBox getBox() const {return _box;}

  ILOEXTROTHERDECL
};

class ILO_EXPORTED IloNotOverlapConstraint : public IloConstraint {
  ILOEXTRHANDLE(IloNotOverlapConstraint,IloConstraint)
};

IloConstraint IloBox::getNotOverlapConstraint() {
  return getImpl()->getNotOverlapConstraint();
}

class IloContainmentConstraintI : public IloConstraintI {
  ILOEXTRDECL
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
  IloBoxI* _containingBox;
  IloBoxI* _containedBox;
  IloContainmentConstraintI(IloEnvI* m,
			    IloBoxI* containingBox,
			    IloBoxI* containedBox,
			    const char* name = 0);
  virtual ~IloContainmentConstraintI();

  IloBox getContainingBox() const {return _containingBox;}
  IloBox getContainedBox() const {return _containedBox;}

  ILOEXTROTHERDECL
};

class ILO_EXPORTED IloContainmentConstraint : public IloConstraint {
  ILOEXTRHANDLE(IloContainmentConstraint,IloConstraint)
};

IloConstraint IloBox::contains(IloBox box) {
  return getImpl()->contains(box.getImpl());
}

class IloOverlapInDimensionConstraintI : public IloConstraintI {
  ILOEXTRDECL
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
  IloBoxI* _containingBox;
  IloBoxI* _box1;
  IloBoxI* _box2;
  IloInt _dimension;
  IloOverlapInDimensionConstraintI(IloEnvI* m,
				   IloBoxI* containingBox,
				   IloBoxI* box1,
				   IloBoxI* box2,
				   IloInt dimension,
				   const char* name = 0 );
  virtual ~IloOverlapInDimensionConstraintI();

  IloBox getContainingBox() const {return _containingBox;}
  IloBox getBox1() const {return _box1;}
  IloBox getBox2() const {return _box2;}
  IloInt getDimension() const {return _dimension; }

  ILOEXTROTHERDECL
};

class ILO_EXPORTED IloOverlapInDimensionConstraint : public IloConstraint {
  ILOEXTRHANDLE(IloOverlapInDimensionConstraint,IloConstraint)
};

IloConstraint IloBox::overlapInDimension(IloBox box1,
					 IloBox box2,
					 IloInt dimension) {
  return getImpl()->overlapInDimension(box1.getImpl(),
				       box2.getImpl(),
				       dimension);
}

class IloNotOverlapInDimensionConstraintI : public IloConstraintI {
  ILOEXTRDECL
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
  IloBoxI* _containingBox;
  IloBoxI* _box1;
  IloBoxI* _box2;
  IloInt _dimension;
  IloNotOverlapInDimensionConstraintI(IloEnvI* m,
				      IloBoxI* containingBox,
				      IloBoxI* box1,
				      IloBoxI* box2,
				      IloInt dimension,
				      const char* name = 0);
  virtual ~IloNotOverlapInDimensionConstraintI();

  IloBox getContainingBox() const {return _containingBox;}
  IloBox getBox1() const {return _box1;}
  IloBox getBox2() const {return _box2;}
  IloInt getDimension() const { return _dimension;}

  ILOEXTROTHERDECL
};

class ILO_EXPORTED IloNotOverlapInDimensionConstraint : public IloConstraint {
  ILOEXTRHANDLE(IloNotOverlapInDimensionConstraint,IloConstraint)
};

IloConstraint IloBox::notOverlapInDimension(IloBox box1,
					    IloBox box2,
					    IloInt dimension) {
  return getImpl()->notOverlapInDimension(box1.getImpl(),
					  box2.getImpl(),
					  dimension);
}

class IloPrecedenceInDimensionConstraintI : public IloConstraintI {
  ILOEXTRDECL
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
public:
  IloBoxI* _containingBox;
  IloBoxI* _box1;
  IloBoxI* _box2;
  IloInt _dimension;
  IloPrecedenceInDimensionConstraintI(IloEnvI* m,
				      IloBoxI* containingBox,
				      IloBoxI* box1,
				      IloBoxI* box2,
				      IloInt dimension,
				      const char* name = 0);
  virtual ~IloPrecedenceInDimensionConstraintI();

  IloBox getContainingBox() const {return _containingBox;}
  IloBox getBox1() const {return _box1;}
  IloBox getBox2() const {return _box2;}
  IloInt getDimension() const { return _dimension;}

  ILOEXTROTHERDECL
};

class ILO_EXPORTED IloPrecedenceInDimensionConstraint : public IloConstraint {
  ILOEXTRHANDLE(IloPrecedenceInDimensionConstraint,IloConstraint)
};

IloConstraint IloBox::precedenceInDimension(IloBox box1,
					    IloBox box2,
					    IloInt dimension) {
  return getImpl()->precedenceInDimension(box1.getImpl(),
					  box2.getImpl(),
					  dimension);
}

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
