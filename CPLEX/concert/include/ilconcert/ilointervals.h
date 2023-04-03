// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilointervals.h
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

#ifndef __CONCERT_IloIntervalsH
#define __CONCERT_IloIntervalsH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloparam.h>
#include <ilconcert/ilointervalsi.h>

class IloIntervalList : public IloParameter {
	ILOHANDLEMININOIMPL(IloIntervalList, IloIntervalListI, IloParameter);
public:

  IloIntervalList(const IloEnv env,
                  IloNum min =-IloInfinity,
                  IloNum max =+IloInfinity,
                  const char* name = 0);

  IloIntervalList(const IloEnv env,
                  const IloNumArray times,
                  const IloNumArray types,
                  const char* name = 0);

	IloIntervalList copy() const;

  IloIntervalList makeCopy() const {
		return copy(); }

  void keepOpen(IloBool val =IloTrue) const;

  IloBool isKeptOpen() const;

  IloNum getDefinitionIntervalMin() const;

  IloNum getDefinitionIntervalMax() const;

  void addInterval(IloNum start, IloNum end, IloNum type = 0L) const;
  void addIntervalOnDuration(IloNum start, IloNum duration, IloNum type = 0L) const;

  void addPeriodicInterval(IloNum start,
                           IloNum duration,
                           IloNum period,
                           IloNum end,
                           IloNum type = 0L) const;

  void removeInterval(IloNum start, IloNum end) const;

  void removeIntervalOnDuration(IloNum start, IloNum duration) const;

  void removePeriodicInterval(IloNum start,
                              IloNum duration,
                              IloNum period,
                              IloNum end) const;

  void empty() const;

  IloBool isEmpty() const;

  void setPeriodic(const IloIntervalList intervals, IloNum x0,
                   IloNum n = IloInfinity) const;

  void shift(IloNum dx) const;

  void dilate(IloNum k) const;

  void setUnion(const IloIntervalList intervals) const;

  void setDifference(const IloIntervalList intervals) const;

  IloBool contains(const IloIntervalList intervals) const;
  void end() const;
};

ILOSTD(ostream)& operator<< (ILOSTD(ostream)& out, const IloIntervalList& l);

inline IloBool
operator== (const IloIntervalList intervals1,
            const IloIntervalList intervals2) {
  return intervals1.getImpl()->isSameAs(intervals2.getImpl());
}
inline IloBool
operator!= (const IloIntervalList intervals1,
            const IloIntervalList intervals2) {
  return !intervals1.getImpl()->isSameAs(intervals2.getImpl());
}

IloIntervalList
IloUnion (const IloIntervalList intervals1,
          const IloIntervalList intervals2);

IloIntervalList
IloDifference (const IloIntervalList intervals1,
               const IloIntervalList intervals2);

class IloIntervalListCursor {

private:
  IloIntervalI* _current;
  IloIntervalListI* _ilist;

public:
  IloIntervalListCursor();

  IloIntervalListCursor(const IloIntervalList);

  IloIntervalListCursor(const IloIntervalList, IloNum x);

  IloIntervalListCursor(const IloIntervalListCursor&);

  void operator=(const IloIntervalListCursor&);
  ~IloIntervalListCursor();

  IloBool  ok()     const;

  void operator++();

  void operator--();

  void seek(IloNum);

  IloNum getStart() const;

  IloNum getEnd() const;

  IloNum getType() const;
};

inline
IloIntervalList::IloIntervalList(const IloEnv env,
                                 IloNum min,
                                 IloNum max,
                                 const char* name)
  : IloParameter(new (env)
      IloIntervalListI(env.getImpl(), min, max, name))
{}

#if !defined(ILO_HP11) || !defined(ILO64)
inline IloIntervalList::IloIntervalList(const IloEnv env,
                                        const IloNumArray times,
                                        const IloNumArray types,
                                        const char* name)
  : IloParameter(new (env)
		      IloIntervalListI(env.getImpl(), times, types, name))
{}
#endif

inline void
IloIntervalList::keepOpen(IloBool val) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->keepOpen(val);
}

inline IloBool
IloIntervalList::isKeptOpen() const {
  IloAssert(getImpl() != 0, "Using empty handle IloResourceParam");
  return getImpl()->isKeptOpen();
}

inline IloNum
IloIntervalList::getDefinitionIntervalMin() const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  return getImpl()->getDefinitionIntervalMin(); }

inline IloNum
IloIntervalList::getDefinitionIntervalMax() const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  return getImpl()->getDefinitionIntervalMax(); }

inline void
IloIntervalList::addInterval(IloNum start,
                             IloNum end,
                             IloNum type) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->addInterval(start, end, type);
}

inline void
IloIntervalList::addIntervalOnDuration(IloNum start,
                                       IloNum duration,
                                       IloNum type) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->addIntervalOnDuration(start, duration, type);
}

inline void
IloIntervalList::addPeriodicInterval(IloNum start,
                                     IloNum duration,
                                     IloNum period,
                                     IloNum end,
                                     IloNum type) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->addPeriodicInterval(start, duration, period, end, type);
}

inline void
IloIntervalList::removeInterval(IloNum start, IloNum end) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->removeInterval(start, end);
}

inline void
IloIntervalList::removeIntervalOnDuration(IloNum start, IloNum duration) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->removeIntervalOnDuration(start, duration);
}

inline void
IloIntervalList::removePeriodicInterval(IloNum start,
                                        IloNum duration,
                                        IloNum period,
                                        IloNum end) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->removePeriodicInterval(start, duration, period, end);
}

inline void
IloIntervalList::empty() const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->reset();
}

inline void
IloIntervalList::end() const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  delete getImpl();
}

inline IloBool
IloIntervalList::isEmpty() const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  return (getImpl()->isEmpty());
}

inline void
IloIntervalList::setPeriodic(const IloIntervalList intervals,
                             IloNum x0,
                             IloNum n) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->setPeriodic(intervals.getImpl(), x0, IloTrunc(n));
}

inline void
IloIntervalList::shift(IloNum dx) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->shift(dx);
}

inline void
IloIntervalList::dilate(IloNum k) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->dilate(k);
}

inline void
IloIntervalList::setUnion(const IloIntervalList intervals) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->setUnion(intervals.getImpl());
}

inline void
IloIntervalList::setDifference(const IloIntervalList intervals) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  getImpl()->setDifference(intervals.getImpl());
}

inline IloBool
IloIntervalList::contains(const IloIntervalList intervals) const {
  IloAssert(getImpl() != 0, "Using empty handle interval list");
  return getImpl()->contains(intervals.getImpl());
}

inline IloIntervalList
IloIntervalList::copy() const {
  IloAssert(getImpl() != 0, "Using empty handle interval list" );
  return (IloIntervalListI*)( getImpl()->makeCopy() );
}

inline void
IloIntervalListCursor::operator=(const IloIntervalListCursor& csor) {
  _current = csor._current;
  _ilist   = csor._ilist;
}

inline
IloIntervalListCursor::IloIntervalListCursor()
  : _current (0L),
    _ilist   (0L)
{}

inline IloBool
IloIntervalListCursor::ok() const {
  return _current->getStart() != IloInfinity;
}

inline void
IloIntervalListCursor::operator++() {
  IloAssert(_current != 0, "Using empty handle IloIntervalListCursor");
  _current = (IloIntervalI*)_current->getNext();
}

inline void
IloIntervalListCursor::operator--() {
  IloAssert(_current != 0, "Using empty handle IloIntervalListCursor");
  _current = (IloIntervalI*)_current->getPrev();
}

inline IloNum
IloIntervalListCursor::getStart() const {
  IloAssert(_current != 0, "Using empty handle IloIntervalListCursor");
  return _current->getStart();
}

inline IloNum
IloIntervalListCursor::getEnd() const {
  IloAssert(_current != 0, "Using empty handle IloIntervalListCursor");
  return _current->getEnd();
}

inline IloNum
IloIntervalListCursor::getType() const {
  IloAssert(_current != 0, "Using empty handle IloIntervalListCursor");
  return _current->getType();
}

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
