// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/ilolist.h
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

#ifndef __XML_ilolistH
#define __XML_ilolistH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilxml/iloalloc.h>

#define IloXmlListNoE

class ILO_EXPORTED IloXml_List;
class ILO_EXPORTED IloXml_LIterator
{
public:
    IloXml_LIterator(const IloXml_List&);
    
    inline IloXmlBoolean isListAlive() const     { return (_l != 0); }
    inline IloXmlBoolean hasMoreElements() const { return (_c != 0); }
    IloXml_LIterator& operator++();
    inline IloXml_LIterator& operator++(int)     { return ++(*this); }
    void d();
    void e();
    void r(IloXmlAny);
protected:
    IloXmlAny     nextElement();
    const IloXml_List*	_l;
private:
    IloXmlAny		_c;
};

class ILO_EXPORTED IloXml_SLIterator
: public IloXml_LIterator
{
public:
    IloXml_SLIterator(const IloXml_List& list);
    ~IloXml_SLIterator();
    IloXml_SLIterator*	_n;
};

typedef void (*IloXmlMapListFunction)(IloXmlAny, IloXmlAny);
typedef IloXmlAny (*IloXmlMapUntilListFunction)(IloXmlAny v, IloXmlAny a, IloXmlAny nf);

class ILO_EXPORTED IloXml_List
{
public:
    IloXml_List() : _first(0), _last(0), _i(0), _length(0) {}
    ~IloXml_List();
    ILOXML_MLK_DECL();
    
    class ILO_EXPORTED Cell {
	friend class IloXml_List;
    public:
	Cell(IloXmlAny value = 0, Cell* next = 0) : _value(value), _next(next) {}
	~Cell();
	ILOXML_MLK_DECL();
	
	inline	IloXmlAny  getValue()   const    { return _value; }
	inline	void   setValue(IloXmlAny value) { _value = value; }
	inline	Cell*  getNext()     const   { return _next; }
	inline	void   setNext(Cell* next)   { _next = next; }
	inline	IloXmlUInt length() const        { return getLength(); }
	inline	void   map(IloXmlMapListFunction f, IloXmlAny a = 0) { apply(f, a); }
	IloXmlUInt         getLength() const;
	Cell*          getLast() const;
	Cell*          copy() const;
	Cell*          remove(IloXmlAny v);
	void           setValue(IloXmlUInt pos, IloXmlAny v);
	IloXmlAny          operator[](IloXmlUInt pos) const;
	const Cell*    find(IloXmlAny v) const;
	IloXmlAny          mapUntil(IloXmlMapUntilListFunction,
				IloXmlAny = 0,
				IloXmlAny = 0) const;
	void           apply(IloXmlMapListFunction, IloXmlAny a = 0);
    private:
	IloXmlAny _value;
	Cell* _next;
    };
    inline Cell*   getFirst()  const { return _first; }
    inline Cell*   getLast()   const { return _last; }
    inline void    map(IloXmlMapListFunction f, IloXmlAny a = 0) { apply(f, a); }
    inline void    apply(IloXmlMapListFunction f, IloXmlAny a = 0)
                     { if (_first) _first->apply(f, a); }
    inline IloXmlUInt  getLength() const { return _length; }
    inline IloXmlUInt  length()  const { return getLength(); }
    inline void    setValue(IloXmlUInt pos, IloXmlAny v)
                     { if(_first) _first->setValue(pos,v); }
    inline void    empty()
                     { e(); delete _first; _first = _last = 0; _length = 0; }
    void           insert(Cell*, IloXmlInt position = 0);
    void           append(Cell*);
    Cell*          removeCell(IloXmlAny value);
    IloXmlAny          mapUntil(IloXmlMapUntilListFunction,
			    IloXmlAny = 0,
			    IloXmlAny = 0) const;
    void           swap(IloXmlAny value1, IloXmlAny value2);
    void           push(IloXmlAny value);
    void           pop(IloXmlAny value);
    void           ai(IloXml_SLIterator*);
    void           ri(IloXml_SLIterator*);
protected:
    void             i(IloXmlAny value, IloXmlInt position = 0);   
    void             a(IloXmlAny value);			   
    IloXmlBoolean        r(IloXmlAny value);			   
    inline IloXmlBoolean f(IloXmlAny v) const			   
	               { return (IloXmlBoolean)(_first && _first->find(v)); }
    inline IloXmlAny     get(IloXmlUInt pos) const		   
	               { return (_first) ? (*_first)[pos] : (IloXmlAny)0; }
    IloXml_List* cp() const;			   
private:
    void d();
    void e();
    void n(Cell*);
    Cell*		_first;
    Cell*		_last;
    IloXml_SLIterator*	_i;
    IloXmlUInt              _length;
};

#define ILOXML_DEF_LIST(NAME, TYPE)						\
NAME: public IloXml_List { public: NAME() : IloXml_List() {}			\
inline void insert(TYPE value, IloXmlInt pos = 0) { i((IloXmlAny)value, pos); }	\
inline void append(TYPE value) { a((IloXmlAny)value); }			\
inline IloXmlBoolean remove(TYPE value) { return r((IloXmlAny)value); }		\
inline TYPE getValue(IloXmlUInt pos) const					\
	{ return ILOXML_UNSAFECAST(TYPE, get(pos));  }			\
inline TYPE operator[](IloXmlUInt pos)  const { return getValue(pos); }	\
inline IloXmlBoolean find(TYPE v) const { return f((IloXmlAny)v); }		\
inline NAME* copy() const { return (NAME*)cp(); }			\
}
#define ILOXML_DEF_LISTI(NAME, LNAME, TYPE)					\
NAME: public IloXml_LIterator { public: NAME(const LNAME& l)		\
    : IloXml_LIterator(l) {}						\
inline TYPE getNextElement()						\
	{ return ILOXML_UNSAFECAST(TYPE, nextElement()); } }
#define ILOXML_DEF_SLISTI(NAME, LNAME, TYPE)				\
NAME: public IloXml_SLIterator { public: NAME(const LNAME& l)		\
    : IloXml_SLIterator(l) {}						\
inline TYPE getNextElement()						\
	{ return ILOXML_UNSAFECAST(TYPE, nextElement()); } }

#define IloXmlListOf(NAME) IloXml##NAME##List
#define IloXmlListIteratorOf(NAME) IloXml##NAME##LIter
#define IloXmlSafeListIteratorOf(NAME) IloXml##NAME##SLIter
#define IloXmlDefineExpListOf(NAME, TYPE, EXP)				\
class EXP ILOXML_DEF_LIST(IloXmlListOf(NAME), TYPE);				\
class EXP ILOXML_DEF_LISTI(IloXmlListIteratorOf(NAME), IloXmlListOf(NAME), TYPE);	\
class EXP ILOXML_DEF_SLISTI(IloXmlSafeListIteratorOf(NAME), IloXmlListOf(NAME),	\
			TYPE)
#define IloXmlDefineListOf(NAME, TYPE) IloXmlDefineExpListOf(NAME, TYPE, IloXmlListNoE)

class ILO_EXPORTED ILOXML_DEF_LIST(IloXmlList, IloXmlAny);
class ILO_EXPORTED ILOXML_DEF_LISTI(IloXmlListIterator, IloXmlList, IloXmlAny);
class ILO_EXPORTED ILOXML_DEF_SLISTI(IloXmlSafeListIterator, IloXmlList, IloXmlAny);

#define IloXmlMapListCell(list, type, arg, exp) {	\
    const IloXml_List::Cell* _l = (list);		\
    while (_l) {				\
	arg = (type)(_l->getValue());		\
	_l  = _l->getNext();			\
	exp; } }

#define IloXmlMapList(list, type, arg, exp) {		\
    const IloXml_List::Cell* _l = (list)->getFirst();	\
    while (_l) {					\
	arg = (type)(_l->getValue());			\
	_l  = _l->getNext();				\
	exp; } }

class ILO_EXPORTED IloXml_AList;
class ILO_EXPORTED IloXml_ALIterator
{
public:
    IloXml_ALIterator(const IloXml_AList&);
    
    inline IloXmlBoolean isListAlive() const	{ return (_l != 0); }
    inline IloXmlBoolean hasMoreElements() const 	{ return (_c != 0); }
    inline IloXml_ALIterator& operator++(int)       { return ++(*this); }
    IloXml_ALIterator& operator++();
    void d();
    void e();
    void r(IloXmlAny);
protected:
    IloXmlAny nextKey();
    IloXmlAny nextElement();
    IloXmlAny nextPair(IloXmlAny&);
    const IloXml_AList*	_l;
private:
    IloXmlAny		_c;
};

class ILO_EXPORTED IloXml_SALIterator
: public IloXml_ALIterator
{
public:
    IloXml_SALIterator(const IloXml_AList& alist);
    ~IloXml_SALIterator();
    IloXml_SALIterator* _n;
};

typedef void (*IloXmlMapAListFunction)(IloXmlAny key, IloXmlAny value, IloXmlAny);
typedef IloXmlAny (*IloXmlMapUntilAListFunction)(IloXmlAny k, IloXmlAny v, IloXmlAny, IloXmlAny);
class ILO_EXPORTED IloXml_AList
{
public:
    class ILO_EXPORTED Cell {
    public:
	Cell(IloXmlAny key, IloXmlAny value, Cell* next = 0)
	: _key(key), _value(value), _next(next) {}
	~Cell();
	ILOXML_MLK_DECL();
	
	inline	IloXmlAny getKey()     const { return _key; }
	inline	void  setKey(IloXmlAny key) { _key = key; }
	inline	IloXmlAny getValue()       const { return _value;  }
	inline	void  setValue(IloXmlAny value) { _value = value; }
	inline	Cell* getNext()     const { return _next; }
	inline	void  setNext(Cell* next) { _next = next; }
	const Cell* find(IloXmlAny key, IloXmlAny* value = 0) const;
	IloXmlAny mapUntil(IloXmlMapUntilAListFunction, IloXmlAny = 0, IloXmlAny = 0) const;
    private:
	IloXmlAny	_key;
	IloXmlAny	_value;
	Cell*	_next;
    };
    IloXml_AList() : _first((Cell*)0), _i(0), _length(0) {}
    ~IloXml_AList();
    ILOXML_MLK_DECL();
    
    inline Cell*  getFirst() const { return _first; }
    inline IloXmlUInt length() const { return getLength(); } 
    inline IloXmlUInt getLength() const { return _length; }
    inline void   empty() { e(); delete _first; _first = 0; _length = 0; }
    IloXmlAny	  mapUntil(IloXmlMapUntilAListFunction,
			   IloXmlAny = 0,
			   IloXmlAny = 0) const;
    void          apply(IloXmlMapAListFunction f, IloXmlAny a = 0);
    void          ai(IloXml_SALIterator*);
    void          ri(IloXml_SALIterator*);
protected:
    void      i(IloXmlAny key, IloXmlAny value, IloXmlInt position = 0);	
    void      a(IloXmlAny key, IloXmlAny value);			
    IloXmlBoolean r(IloXmlAny key, IloXmlAny newvalue);			
    IloXmlBoolean rm(IloXmlAny key);					
    IloXmlBoolean rm(IloXmlAny key, IloXmlAny value);			
    Cell*     gfc(IloXmlAny key, Cell** = 0) const;			
    Cell*     gc(IloXmlAny key, IloXmlAny value, Cell** = 0) const;	
    IloXmlAny     g(IloXmlAny key) const;				
    IloXmlBoolean f(IloXmlAny key, IloXmlAny* value = 0) const;		
private:
    void d();
    void e();
    void n(Cell*);
    Cell*		_first;
    IloXml_SALIterator*	_i;
    IloXmlUInt              _length;
};

#define ILOXML_DEF_ALIST(NAME, KEYTYPE, VALTYPE)				\
NAME: public IloXml_AList { public:						\
    NAME() : IloXml_AList() {}						\
inline void insert(KEYTYPE k, VALTYPE v, IloXmlInt p = 0)			\
	{ i((IloXmlAny)k, (IloXmlAny)v, p); }					\
inline void append(KEYTYPE k, VALTYPE v) { a(k, v); }			\
inline IloXmlBoolean replace(KEYTYPE k, VALTYPE v) { return r(k, v); }	\
inline IloXmlBoolean remove(KEYTYPE k) { return rm(k); }			\
inline IloXmlBoolean remove(KEYTYPE k, VALTYPE v) { return rm(k, v); }	\
inline Cell* getFirstCell(KEYTYPE k, Cell** c=0) const {return gfc(k, c);}\
inline Cell* getCell(KEYTYPE k, IloXmlAny v, Cell** c = 0) const		\
	{ return gc(k, v, c); }						\
inline VALTYPE get(KEYTYPE k) const					\
	{ return ILOXML_UNSAFECAST(VALTYPE, g(k)); }			\
inline IloXmlBoolean find(KEYTYPE k, VALTYPE* v = 0) const			\
	{ return f(k, (IloXmlAny*)v); }					\
inline Cell* getItem(KEYTYPE k, Cell** c=0) const {return gfc(k, c);}	\
}
#define ILOXML_DEF_ALISTI(NAME, LNAME, KEYTYPE, VALTYPE)			\
NAME : public IloXml_ALIterator { public:					\
    NAME(const LNAME& l) : IloXml_ALIterator(l) {}				\
inline KEYTYPE getNextKey()						\
	{ return ILOXML_UNSAFECAST(KEYTYPE, nextKey()); }			\
inline VALTYPE getNextElement()						\
	{ return ILOXML_UNSAFECAST(VALTYPE, nextElement()); }		\
inline KEYTYPE getNextPair(VALTYPE& v)					\
	{ return ILOXML_UNSAFECAST(KEYTYPE, nextPair((IloXmlAny&)v)); } }
#define ILOXML_DEF_SALISTI(NAME, LNAME, KEYTYPE, VALTYPE)			\
NAME : public IloXml_SALIterator { public:					\
    NAME(const LNAME& l) : IloXml_SALIterator(l) {}				\
inline KEYTYPE getNextKey()						\
	{ return ILOXML_UNSAFECAST(KEYTYPE, nextKey()); }			\
inline VALTYPE getNextElement()						\
	{ return ILOXML_UNSAFECAST(VALTYPE, nextElement()); }		\
inline KEYTYPE getNextPair(VALTYPE& v)					\
	{ return ILOXML_UNSAFECAST(KEYTYPE, nextPair((IloXmlAny&)v)); } }

#define IloXmlAListOf(NAME) IloXml##NAME##AList
#define IloXmlAListIteratorOf(NAME) IloXml##NAME##ALIter
#define IloXmlSafeAListIteratorOf(NAME) IloXml##NAME##SALIter
#define IloXmlDefineExpAListOf(NAME, KEYTYPE, VALTYPE, EXP)			\
class EXP ILOXML_DEF_ALIST(IloXmlAListOf(NAME), KEYTYPE, VALTYPE);		\
class EXP ILOXML_DEF_ALISTI(IloXmlAListIteratorOf(NAME), IloXmlAListOf(NAME),	\
			 KEYTYPE, VALTYPE);				\
class EXP ILOXML_DEF_SALISTI(IloXmlSafeAListIteratorOf(NAME), IloXmlAListOf(NAME),	\
			 KEYTYPE, VALTYPE)
#define IloXmlDefineAListOf(NAME, KEYTYPE, VALTYPE) \
IloXmlDefineExpAListOf(NAME, KEYTYPE, VALTYPE, IloXmlListNoE)

class ILO_EXPORTED ILOXML_DEF_ALIST(IloXmlAList, IloXmlAny, IloXmlAny);
class ILO_EXPORTED ILOXML_DEF_ALISTI(IloXmlAListIterator,
				  IloXmlAList, IloXmlAny, IloXmlAny);
class ILO_EXPORTED ILOXML_DEF_SALISTI(IloXmlSafeAListIterator,
				   IloXmlAList, IloXmlAny, IloXmlAny);

#define IloXmlMapAList(list, typeKey, argKey, typeValue, argValue, exp) { \
    const IloXmlAList::Cell* _p = (list)->getFirst();		      \
    while (_p) {						      \
	argKey	 = ILOXML_UNSAFECAST(typeKey, _p->getKey());	      \
	argValue = ILOXML_UNSAFECAST(typeValue, _p->getValue());	      \
	_p	 = _p->getNext(); exp } }

#define IloXmlMapAListKey(list, typeKey, argKey, exp) {	\
    const IloXmlAList::Cell* _p = (list)->getFirst();	\
    while (_p) {					\
	argKey = ILOXML_UNSAFECAST(typeKey, _p->getKey());	\
	_p     = _p->getNext(); exp } }

#define IloXmlMapAListValue(list, typeValue, argValue, exp) {	\
    const IloXmlAList::Cell* _p = (list)->getFirst();		\
    while (_p) {						\
	argValue = ILOXML_UNSAFECAST(typeValue, _p->getValue());	\
	_p	 = _p->getNext(); exp    } }

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
