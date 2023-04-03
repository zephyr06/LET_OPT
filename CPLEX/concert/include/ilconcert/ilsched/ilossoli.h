// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilsched/ilossoli.h
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

#ifndef __CONCERT_ilossoliH
#define __CONCERT_ilossoliH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosolution.h>
#include <ilconcert/ilsched/ilosstructi.h>
#include <ilconcert/ilsched/ilostimei.h>
#include <ilconcert/ilsched/ilossequencei.h>
#include <ilconcert/ilsched/ilosatomi.h>

class IloSavedIntervalI: public IloSolutionElementI {
  friend class IloSolutionIntervalOperations;
 public:
  IloUInt      _restoreFields;
  IloUInt      _status;
  IloInt       _startMin;
  IloInt       _startMax;
  IloInt       _endMin;
  IloInt       _endMax;
  IloInt       _lengthMin;
  IloInt       _lengthMax;
  IloInt       _sizeMin;
  IloInt       _sizeMax;

 public:
  IloSavedIntervalI(IloMemoryManager m, IloIntervalVarI* ti, IloUInt restore);
  virtual ~IloSavedIntervalI() {}  

  
  IloIntervalVarI* getInterval() const { return (IloIntervalVarI*)getExtractable().getImpl(); }
  IloBool isPresent()   const { return IloBitTest(_status, IloIntervalVarI::BitPresent); }
  IloBool isAbsent()    const { return IloBitTest(_status, IloIntervalVarI::BitAbsent); }
  IloInt getStart()     const;
  IloInt getStartMin()  const { return _startMin; }
  IloInt getStartMax()  const { return _startMax; }
  IloInt getEnd()       const;
  IloInt getEndMin()    const { return _endMin; }
  IloInt getEndMax()    const { return _endMax; }
  IloInt getLength()    const;
  IloInt getLengthMin() const { return _lengthMin; }
  IloInt getLengthMax() const { return _lengthMax; }
  IloInt getSize()      const;
  IloInt getSizeMin()   const { return _sizeMin; }
  IloInt getSizeMax()   const { return _sizeMax; }

  
  void setPresent()  { _status = ILOS_BITSET_MASK(IloIntervalVarI::BitPresent); }
  void setAbsent()   { _status = ILOS_BITSET_MASK(IloIntervalVarI::BitAbsent); }
  void setOptional() { _status = 0; }
  void setStart    (IloInt x) { _startMin  = x; _startMax  = x; }
  void setStartMin (IloInt x) { _startMin  = x; }
  void setStartMax (IloInt x) { _startMax  = x; }
  void setEnd      (IloInt x) { _endMin    = x; _endMax    = x; }
  void setEndMin   (IloInt x) { _endMin    = x; }
  void setEndMax   (IloInt x) { _endMax    = x; }
  void setLength   (IloInt x) { _lengthMin = x; _lengthMax = x; }
  void setLengthMin(IloInt x) { _lengthMin = x; }
  void setLengthMax(IloInt x) { _lengthMax = x; }
  void setSize     (IloInt x) { _sizeMin   = x; _sizeMax   = x; }
  void setSizeMin  (IloInt x) { _sizeMin   = x; }
  void setSizeMax  (IloInt x) { _sizeMax   = x; }

  
  IloBool isToBeRestored(IloUInt flag) const { return (_restoreFields & flag); }
  IloUInt getRestoreFields() const { return (_restoreFields & IloSolution::RestoreAll); }
  void setRestoreFields(IloUInt fields) { 
    _restoreFields &= (~IloSolution::RestoreAll);
    _restoreFields |= fields; }
  void mark  () { _restoreFields |= (IloSolution::RestoreAll+1); }
  void unmark() { _restoreFields &= ~(IloSolution::RestoreAll+1); }
  IloBool isMarked() const { return 0!=(_restoreFields & (IloSolution::RestoreAll+1)); }

  
  virtual void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
  virtual IloSolutionElementI* makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
  virtual IloBool isEquivalent(const IloSolutionElementI *element) const ILO_OVERRIDE;
  virtual void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;

 private:
  void assignValues(IloSavedIntervalI *a, const IloSavedIntervalI *b) const;
};

class IloSavedPrecGraphI {
public:
  enum ArcType {
    
    Before   = ILOS_BITSET_MASK(0),
    Previous = ILOS_BITSET_MASK(1)
  };
  
  class ArcI;
  typedef IlosListI<ArcI> ArcListI;

  class NodeI {
  private:
    ArcListI         _incoming;
    ArcListI         _outgoing;
    IloExtractableI* _extr; 
    IloAny           _tmp;  
    IloUInt          _restoreFields;
  public:
    NodeI(IloMemoryManagerI* m, IloExtractableI* extr);
    void init(IloMemoryManagerI* m);
    
    
    NodeI* init(IloMemoryManagerI* m, IloExtractableI* extr, IloUInt nbIncoming, IloUInt nbOutgoing);
    virtual ~NodeI(){}
    IloExtractableI* getExtractable() const { return _extr; }
    IloBool isToBeRestored(IloUInt flag) const { return (_restoreFields & flag); }
    IloUInt getRestoreFields()           const { return _restoreFields; }
    void setRestoreFields(IloUInt fields) { _restoreFields = fields; }
    void mark() { _restoreFields |= ILOS_BITSET_MASK(31); }
    void unmark() { _restoreFields &= ~ILOS_BITSET_MASK(31); }
    IloBool isMarked() const { return  (_restoreFields & ILOS_BITSET_MASK(31)); }
    void markInStack() { _restoreFields |= ILOS_BITSET_MASK(30); }
    void unmarkInStack() { _restoreFields &= ~ILOS_BITSET_MASK(30); }
    IloBool isMarkedInStack() const { return  (_restoreFields & ILOS_BITSET_MASK(30)); }
    void markRelinkStart() { _restoreFields |= ILOS_BITSET_MASK(29); }
    IloBool isMarkedRelinkStart() const { return (_restoreFields & ILOS_BITSET_MASK(29)); }
    void markRelinkEnd() { _restoreFields |= ILOS_BITSET_MASK(28); }
    IloBool isMarkedRelinkEnd() const { return (_restoreFields & ILOS_BITSET_MASK(28)); }
    
    void unmarkRelinkFlags() { _restoreFields &= ~(ILOS_BITSET_MASK(28) | ILOS_BITSET_MASK(29) | ILOS_BITSET_MASK(30)); }
    void addIncoming(IloMemoryManagerI* m, ArcI* arc) { _incoming.add(m, arc); }
    void addOutgoing(IloMemoryManagerI* m, ArcI* arc) { _outgoing.add(m, arc); }
    void removeIncoming(ArcI* arc) { _incoming.remove(arc); }
    void removeOutgoing(ArcI* arc) { _outgoing.remove(arc); }
    void removeAllIncoming() { _incoming.clear(); }
    void removeAllOutgoing() { _outgoing.clear(); }
    const ArcListI& getIncoming() const { return _incoming; }
    const ArcListI& getOutgoing() const { return _outgoing; }
    IloUInt getNbIncoming() const { return _incoming.getSize(); }
    IloUInt getNbOutgoing() const { return _outgoing.getSize(); }
    void setTmp(IloAny tmp) { _tmp = tmp; }
    IloAny getTmp() const { return _tmp; }
    void end(IloMemoryManagerI* m);
  };
  
  class ArcI {
  public:
    enum Type {
      FromStart    = ILOS_BITSET_MASK(0),
      ToEnd        = ILOS_BITSET_MASK(1),
      Previous     = ILOS_BITSET_MASK(2),
      Relink       = ILOS_BITSET_MASK(3),
      DoNotRestore = ILOS_BITSET_MASK(4)
    };
  private:
    NodeI*  _source;
    NodeI*  _target;
    IloUInt _type;  
  public:
    ArcI(NodeI* source, NodeI* target, IloUInt type);
    ArcI* init(NodeI* source, NodeI* target, IloUInt type);
    virtual ~ArcI(){}
    void setNonRestorable() { IloBitSet(_type, DoNotRestore); }
    NodeI*  getSource() const { return _source; }
    NodeI*  getTarget() const { return _target; }
    IloUInt getType()   const { return _type; }
    IloBool isToBeRestored() const { return 0 == IloBitTest(_type, 4); }
    IloBool isFromStart() const { return IloBitTest(_type, 0); }
    IloBool isToEnd()     const { return IloBitTest(_type, 1); }
    IloBool isPrevious()  const { return IloBitTest(_type, 2); }
    IloBool isRelink()    const { return IloBitTest(_type, 3); }
  };

  typedef IloEnvHashTable<IloInt, NodeI*> NodeHashTableI;
  typedef IlosListI<NodeI>            NodeListI;

  friend class NodeIterator;
  class NodeIterator {
  private:
    NodeListI::Iterator _ite;
  public:
    NodeIterator(IloSavedPrecGraphI* pg)
      :_ite(pg->_nodeList){}
    ~NodeIterator(){}
    NodeI* operator*() { return *_ite; }
    void operator++() { ++_ite; }
    IloBool ok() const { return _ite.ok(); }
  };

  
  
  class AllocationBlockI {
   private:
    void*  _memory;
    IloInt _size; 
   public:
    AllocationBlockI(void* memory, IloInt size): _memory(memory), _size(size) {}
    void free(IloMemoryManagerI* m) { m->free(_memory, _size); }
  };
  typedef IlosListI<AllocationBlockI> AllocationListI;

private:
  friend class NodeIterator;
  IloMemoryManagerI* _m;
  NodeHashTableI     _nodes; 
  NodeListI          _nodeList; 
  NodeListI          _markedNodes;
  NodeListI          _dirtyNodes;
  NodeListI          _nodeStack;
  NodeListI          _recycledNodes;
  ArcListI           _recycledArcs;
  AllocationListI    _allocations;

private:
  void allocateNodes(IloInt minNumber);
  void allocateArcs(IloInt minNumber);
  
  
  NodeI* newNode(IloExtractableI* extr, IloUInt nbIncoming = 0, IloUInt nbOutgoing = 0);
  ArcI*  newArc(NodeI* source, NodeI* target, IloUInt type) {
    if (0 == _recycledArcs.getSize())
      allocateArcs(1);
    return _recycledArcs.pop()->init(source, target, type);
  }
  void removeArc(ArcI* arc);
  void recycle(NodeI* node){ assert(node->getNbIncoming() == 0); assert(node->getNbOutgoing() == 0); _recycledNodes.add(_m, node); }
  void recycleNodes();
  void recycle(ArcI* arc)  { _recycledArcs.add(_m, arc); }
public:
  IloSavedPrecGraphI(IloMemoryManagerI* m);
  ~IloSavedPrecGraphI(){}
  void setRestoreFields(IloUInt fields);
  IloBool isEmpty() const { return (0 == _nodes.getSize()); }
  IloBool hasNode(IloExtractableI* e) const { return _nodes.containsKey(e->getId()); }
  NodeI* makeNode(IloExtractableI* e, IloUInt nbIncoming = 0, IloUInt nbOutgoing = 0);
  NodeI* getNode(IloExtractableI* e) const;
  NodeI* getOrMakeNode(IloExtractableI* e);
  void markNode(IloExtractableI* e);
  void relinkFrom(NodeI* node, IloBool fromStart);
  void relink();
  const NodeHashTableI& getNodes() const { return _nodes; }
  IloBool existArc(NodeI* source, NodeI* target, IloUInt type) const;
  void addArc(NodeI* source, NodeI* target, IloUInt type);
  void removeAllIncoming(IloExtractableI* e);
  void removeAllOutgoing(IloExtractableI* e);
  void removeAllArcs();
  void clear();
  IloExtractableI* getNext(IloExtractableI* x)     const;
  IloExtractableI* getPrevious(IloExtractableI* x) const;
  void assignValues(IloSavedPrecGraphI*);
  void end();
  IloBool display(ILOSTD(ostream) &s) const;
};

class IloSavedScheduleI: public IloSolutionElementI {
  friend class IloSolutionScheduleOperations;
 public:
  IloUInt              _restoreFields;
  IloSavedPrecGraphI*  _pg;
public:
  IloSavedScheduleI(IloMemoryManager m, IloSchedGlobalExtractableI* s, IloUInt restore);
  virtual ~IloSavedScheduleI();

  
  void addArc(IloIntervalVarI* source, IloIntervalVarI* target, IloUInt type);  
  void removeAllIncoming(IloIntervalVarI* ti);
  void removeAllOutgoing(IloIntervalVarI* ti);
  void removeAllPrecedences();
  void mark(IloIntervalVarI* ti);
  void relink();
  IloSavedPrecGraphI* getSavedPG() const { return _pg; }
  IloBool hasEmptyPG() const { return (0==_pg) || _pg->isEmpty(); }

  
  IloBool isToBeRestored(IloUInt flag) const { return (_restoreFields & flag); }
  IloUInt getRestoreFields()           const { return _restoreFields; }
  IloUInt getRestoreFields(IloIntervalVarI* ti) const;
  void setRestoreFields(IloUInt fields);
  void setRestoreFields(IloIntervalVarI* ti, IloUInt fields);

  
  virtual void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
  virtual IloSolutionElementI* makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
  virtual IloBool isEquivalent(const IloSolutionElementI* element) const ILO_OVERRIDE;  
  virtual void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;
};

class IloSavedSequenceI: public IloSolutionElementI {
  friend class IloSolutionSequenceOperations;
 public:
  IloUInt              _restoreFields;
  IloSavedPrecGraphI*  _pg;
  IloIntervalVarI* _first;
  IloIntervalVarI* _last;

 public:
  IloSavedSequenceI(IloMemoryManager m, IloIntervalSequenceVarI* s, IloUInt restore);
  virtual ~IloSavedSequenceI();

  
  void addArc(IloIntervalVarI* source, IloIntervalVarI* target, IloBool immediate);  
  void removeAllIncoming(IloIntervalVarI* ti);
  void removeAllOutgoing(IloIntervalVarI* ti);
  void removeAllPrecedences();
  void mark(IloIntervalVarI* ti);
  void relink();
  IloSavedPrecGraphI* getSavedPG() const { return _pg; }
  IloBool hasEmptyPG() const { return (0==_pg) || _pg->isEmpty(); }
  void setFirst(IloIntervalVarI* x) { _first = x; }
  void setLast (IloIntervalVarI* x) { _last  = x; }
  IloIntervalVarI* getFirst() const { return _first; }
  IloIntervalVarI* getLast() const  { return _last; }
  IloIntervalVarI* getNext(IloIntervalVarI* x) const;
  IloIntervalVarI* getPrevious(IloIntervalVarI* x) const;

  
  IloBool isToBeRestored(IloUInt flag) const { return (_restoreFields & flag); }
  IloUInt getRestoreFields()           const { return _restoreFields; }
  IloUInt getRestoreFields(IloIntervalVarI* ti) const;
  void setRestoreFields(IloUInt fields);
  void setRestoreFields(IloIntervalVarI* ti, IloUInt fields);

  
  virtual void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
  virtual IloSolutionElementI* makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
  virtual IloBool isEquivalent(const IloSolutionElementI* element) const ILO_OVERRIDE;
  
  virtual void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;
};

class IloSavedAtomI: public IloSolutionElementI {
 private:
  IloUInt     _restoreFields;
  IloInt      _lmin;
  IloInt      _lmax;

 public:
  IloSavedAtomI(IloMemoryManager m, IloExtractableI* atom, IloUInt restore);
  virtual ~IloSavedAtomI() {}  

  
  IloCumulAtomI* getCumulAtom() const { return (IloCumulAtomI*)getExtractable().getImpl(); }
  IloInt getLevelMin()  const { return _lmin; }
  IloInt getLevelMax()  const { return _lmax; }

  
  void setLevelMin(IloInt x) { _lmin = x; }
  void setLevelMax(IloInt x) { _lmax = x; }

  
  IloBool isToBeRestored(IloUInt flag) const { return (_restoreFields & flag); }
  IloUInt getRestoreFields()           const { return _restoreFields; }
  void setRestoreFields(IloUInt fields) { _restoreFields = fields; }

  
  virtual void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
  virtual IloSolutionElementI* makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
  virtual IloBool isEquivalent(const IloSolutionElementI *element) const ILO_OVERRIDE;
  virtual void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;

 private:
  void assignValues(IloSavedAtomI *a, const IloSavedAtomI *b) const;
};

class IloSavedStateFunctionI: public IloSolutionElementI {
private:
  friend class IloSolutionStateFunctionOperations;
protected:
  class SegmentI {
  friend class Cursor;
  private:
    IloInt _start;
    IloInt _end;
    IloInt _state; 
  public:
    SegmentI(IloInt start, IloInt end, IloInt state)
      :_start(start), _end(end), _state(state){}
    virtual ~SegmentI(){}
    SegmentI* init(IloInt start, IloInt end, IloInt state);
    IloInt getStart() const { return _start; }
    IloInt getEnd()   const { return _end; }
    IloInt getState() const { return _state; }
  };
  typedef IlosListI<SegmentI> SegmentListI;

 public:
  IloSavedStateFunctionI(IloMemoryManager m, IloStateFunctionI* ti, IloUInt restore);
  virtual ~IloSavedStateFunctionI();
  IloStateFunctionI* getStateFunction() const { return (IloStateFunctionI*)getExtractable().getImpl(); }
  void clear();
  void addSegment(IloInt start, IloInt end, IloInt state);
  void markFixed() { _isFixed = IloTrue; }
  IloBool isFixed() const { return _isFixed; }
  IloUInt getNbSegments() const { return _segments.getSize(); }

  class Cursor;
  friend class Cursor;
  class Cursor {
  private:
    SegmentI** _curr;
    SegmentI** _end;
  public:
    Cursor(const IloSavedStateFunctionI*);
    ~Cursor(){}
    IloInt getStart() const { return (*_curr)->getStart(); }
    IloInt getEnd()   const { return (*_curr)->getEnd(); }
    IloInt getState() const { return (*_curr)->getState(); }
    IloBool ok() const   { return (_curr != _end); }
    Cursor& operator++() { _curr++; return *this; }
  };

  
  IloBool isToBeRestored(IloUInt flag) const { return (_restoreFields & flag); }
  IloUInt getRestoreFields() const { return _restoreFields; }
  void setRestoreFields(IloUInt fields) { _restoreFields = fields; }

  
  virtual void copy(const IloSolutionElementI *e) ILO_OVERRIDE;
  virtual IloSolutionElementI* makeClone(IloMemoryManagerI* env) const ILO_OVERRIDE;
  virtual IloBool isEquivalent(const IloSolutionElementI* e) const ILO_OVERRIDE;
  virtual void display(ILOSTD(ostream) &s) const ILO_OVERRIDE;

 private:
  IloMemoryManagerI* _m;
  IloUInt            _restoreFields;
  SegmentListI       _segments;
  SegmentListI       _recycledSegments;
  IloBool            _isFixed;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

