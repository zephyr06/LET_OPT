// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilomodeli.h
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

#ifndef __CONCERT_ilomodeliH
#define __CONCERT_ilomodeliH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloextractable.h>

class IloModelI : public IloExtractableI {
  ILOEXTRDECL
  friend class IloEnvI;

  IloExtractableList _list;
  virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
protected:
  virtual void atRemove(IloExtractableI* user=0, IloAny info = 0) ILO_OVERRIDE;
public:
  IloModelI(IloEnvI* m, const char* name);
  virtual ~IloModelI();

  const IloExtractableList& getList() const   { return (_list); }

  
  class Iterator : public IloLinkedList<IloExtractableI>::Iterator {
  public:
    Iterator(const IloModelI* g);
  };

  IloExtractableI*          add(IloExtractableI* obj);
  const IloExtractableArray& add(const IloExtractableArray& array);

  void remove(IloExtractableI* obj);
  void remove(const IloExtractableArray obj);

  ILOEXTROTHERDECL
	  	virtual IloBool isModel() ILO_OVERRIDE {
		return IloTrue;
	}
};

class IloAddToModel : public IloChange {
  ILOCHANGEDECL
private:
  IloModelI* _mdl;
  IloExtractableArray _added;
  IloBool _endArray;
public:
  virtual ~IloAddToModel();
  IloAddToModel(IloModelI* mdl, const IloExtractable obj)
    :_mdl(mdl), _added(mdl->getEnv(), 1, obj), _endArray(IloTrue) {}
  IloAddToModel(IloModelI* mdl, const IloExtractableArray obj)
    :_mdl(mdl), _added(obj), _endArray(IloFalse) {}
  IloModelI* getModelI() const {return _mdl;}
  const IloExtractableArray& getAdded() const {return _added;}
  virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
	  return getModelI();
  }
};

class IloAddOneToModel : public IloChange {
  ILOCHANGEDECL
private:
  IloModelI*       _mdl;
  IloExtractableI* _added;
public:
  virtual ~IloAddOneToModel();
  IloAddOneToModel(IloModelI* mdl, IloExtractableI* obj)
    : _mdl(mdl), _added(obj) {}
  IloModelI* getModelI() const {return _mdl;}
  IloExtractableI* getAdded() const {return _added;}
  virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
	  return getModelI();
  }
};

class IloRemoveFromModel : public IloChange {
  ILOCHANGEDECL
  IloModelI*          _mdl;
  IloExtractableArray _removed;
  IloBool _endArray;
public:
  virtual ~IloRemoveFromModel();
  IloRemoveFromModel(IloModelI* mdl, const IloExtractable obj)
    : _mdl(mdl), _removed(mdl->getEnv(), 1, obj), _endArray(IloTrue) {}
  IloRemoveFromModel(IloModelI* mdl, const IloExtractableArray obj)
    : _mdl(mdl), _removed(obj), _endArray(IloFalse) {}
  IloModelI* getModelI() const {return _mdl;}
  const IloExtractableArray& getRemoved() const {return _removed;}
  virtual const IloExtractableI* getChangedExtractable() const ILO_OVERRIDE {
	  return getModelI();
  }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

