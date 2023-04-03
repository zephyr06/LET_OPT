// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloskiplist.h
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

#ifndef __CONCERT_IloSkipListH
#define __CONCERT_IloSkipListH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloparami.h>

class IloSkipListNodeI {

  friend class IloSkipListI;
  friend class IloIntervalI;
  friend class IloFunctionSegmentI;
  friend class IloIntervalListI;
  friend class IloSegmentedFunctionI;
  friend class IloAnySetSegmentI;
  friend class IloNumToAnySetStepFunctionI;

 private:
  IloInt             _level;
  IloNum             _start;
  IloSkipListNodeI** _forward;
  IloSkipListNodeI*  _prev;

 public:
  IloSkipListNodeI();
  IloSkipListNodeI(IloEnvI* env, IloInt level);  
#ifdef ILO_SEGF_GENERIC_ALLOC
  IloSkipListNodeI(IloMemoryManagerI* env, IloInt level);  
#endif
  ~IloSkipListNodeI() {}

#ifndef ILO_SEGF_GENERIC_ALLOC
  void end(IloEnvI* env);
#else
  void end(IloMemoryManagerI* env);
#endif

  IloInt getLevel() const { return _level; }
  IloNum getStart() const { return _start; }
  IloSkipListNodeI* getNext() const { return _forward[0]; }
  IloSkipListNodeI* getPrev() const { return _prev; }
  IloBool isHeader() const { return _start == - IloInfinity; }
  IloBool isTail() const { return _start == IloInfinity; } 
  void setStart(IloNum start) { _start = start; }
  IloNum getXLeft() const { return _start; }
#if defined(ILO_MSVC)
  IloNum getXRight() const;
#else
  IloNum getXRight() const { return _forward[0]->_start; }
#endif
};

class IloRandomI;

#define ILOSKIPLISTMAXLEVEL 20

class IloSkipListI : public IloParameterI {
  friend class IloIntervalListI;
  friend class IloSegmentedFunctionI;
  friend class IloNumToAnySetStepFunctionI;

private:
#ifdef ILO_SEGF_GENERIC_ALLOC
  IloMemoryManagerI* _env;
#else
  IloEnvI*           _env;
#endif
  IloInt             _level;
  IloSkipListNodeI*  _header;
  IloSkipListNodeI*  _tail;
  IloSkipListNodeI*  _recycle;
  IloSkipListNodeI** _slice0;
  IloSkipListNodeI** _slice1;
  IloSkipListNodeI** _sliceEnd;
  IloSkipListNodeI** _slice0R;
  IloInt             _randomBits;
  IloInt             _randomsLeft;
  IloInt             _BitsInRandom;
  IloInt             _MaxLevel;
  IloInt             _RandomType;
  IloInt             _RandomMask;
  IloRandomI*        _random;

public:
  IloSkipListI(IloEnvI* env, const char* name =0);  
#ifdef  ILO_SEGF_GENERIC_ALLOC
  IloSkipListI(IloMemoryManagerI* env, IloRandomI* random, const char* name =0);  
#endif
  virtual ~IloSkipListI();  

  void init(IloInt maxLevel =ILOSKIPLISTMAXLEVEL);
  void reset();
  IloBool removeItem(IloNum start);
  IloSkipListNodeI* search       (IloNum start);
  IloSkipListNodeI* insert       (IloNum start);
  IloSkipListNodeI* insertFirst  (IloNum start);
  IloSkipListNodeI* insertLast   (IloNum start);
  IloSkipListNodeI* insertSlice0 (IloNum start);
  IloSkipListNodeI* searchSlice0 (IloNum start);
  IloSkipListNodeI* searchSlice1 (IloNum start);
  void removeFirst   ();
  void removeLast    ();
  void removeSlice0R ();
  void nextSlice0    ();
  void nextSlice1    ();
  void remove01      ();
  IloSkipListNodeI* getHeader() const { return _header; }
  IloSkipListNodeI* getTail() const { return _tail; }
  IloInt getLevel() const { return _level; }
#ifndef ILO_SEGF_GENERIC_ALLOC
  IloBool check() const;
#else
  IloBool check(ILOSTD(ostream)& out) const;
#endif
  void setMaxLevel(IloInt maxLevel) {
    if (maxLevel < _MaxLevel)
      _MaxLevel = maxLevel;
  }
  void setRandomMode16() {
    _RandomType = 4;
    _RandomMask = 15;
  }
  void setRandomMode8() {
    _RandomType = 3;
    _RandomMask = 7;
  }
  void setRandomMode4() {
    _RandomType = 2;
    _RandomMask = 3;
  }
  void setRandomMode2() {
    _RandomType = 1;
    _RandomMask = 1;
  }

protected:
  typedef enum { rawNode=0, headNode, tailNode } NodeType;
  virtual IloSkipListNodeI* allocateNode(NodeType type,
                                         IloInt level);
  virtual void destroyNode(IloSkipListNodeI* node);
  IloSkipListNodeI* destroyAndGetNext(IloSkipListNodeI* node);

  IloBool isDestroyed() const { return (_env==0); }
  void setDestroyed() { _env = 0; }
  void destroyAll();  

private:
  IloInt randomLevel();
  void recycle(IloSkipListNodeI* p);
  IloSkipListNodeI* newNode();
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
