// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/iloarray.h
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

#ifndef __XML_iloarrayH
#define __XML_iloarrayH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilxml/ilomemory.h>

#if defined(IL_STD)
#include <new>
#else  
#include <new.h>

#endif 

#define IloXmlInternalArrayDeclareHead(Name) 				\
	Name { 									\
	public:									\
	Name(IloXmlUInt startLength = 4) : _array(0), _maxLength(0), _length(0)	\
		{ setMaxLength(startLength); }					\
		Name(const Name& a):_array(0),_maxLength(0),_length(0) {(*this)=a;}

#define IloXmlInternalArrayDeclareMainMembers(Type,Name)   		\
	public:								\
	~Name();							\
	Name&   operator=(const Name&);				\
	void    setMaxLength(IloXmlUInt nlen, IloXmlBoolean exact=IloXmlTrue);	\
	void    setData(Type*, IloXmlUInt, IloXmlUInt=(IloXmlUInt) -1);		\
	void    erase(IloXmlUInt from=0, IloXmlUInt to=(IloXmlUInt)-1);		\
	void    insert(const Type* src, IloXmlUInt len, IloXmlUInt rank);

#define IloXmlInternalArrayDeclareMembers(Type,Name) 			\
	private:								\
	Type*   _array;							\
	IloXmlUInt _maxLength;							\
	IloXmlUInt _length;							\
	public:									\
	 						    	\
	inline IloXmlUInt getMaxLength() const { return _maxLength; }		\
	inline IloXmlUInt getLength() const { return _length; }			\
	inline Type* getArray() const { return _array; }			\
	inline Type& operator[](IloXmlUInt rank) { return _array[rank]; }		\
	inline Type  operator[](IloXmlUInt rank) const { return _array[rank]; }	\
	inline Type& top() { return _array[_length-1]; }			\
	  							\
	inline Type* exportData() 						\
	{ Type* res=_array;_array=0;setData(0,0); return res;}		\
	inline void grow() { setMaxLength(_maxLength*2, IloXmlFalse); }		\
	inline void compact() { setMaxLength(_length, IloXmlTrue); }		\
	inline void insert(const Name& src, IloXmlUInt rank) 			\
		{ insert((const Type*)src.getArray(),src.getLength(),rank); }	\
		inline void append(const Name& src) 					\
	{ insert((const Type*)src.getArray(),src.getLength(),_length);} \
	inline void append(const Type* src, IloXmlUInt l)				\
	{ insert(src, l, _length); }					\
	inline void add (const Type value) 					\
	{ insert(&value, (IloXmlUInt)1, _length); }				\
	inline void insert(IloXmlUInt rank, const Type value) 			\
	{ insert(&value,(IloXmlUInt)1, rank);}				\
	inline void remove(IloXmlUInt rank) { erase(rank, rank+1); }		\
	inline void clear() { erase(); }					\
	inline void empty() { erase(); }					\
	inline void pop() { if (_length > 0) erase(_length - 1, _length); }	\
	inline void push(const Type& value) 					\
	{ insert(&value, (IloXmlUInt)1, _length); }

#define IloXmlDeclareArrayOf(Type,Name)	 		\
	IloXmlInternalArrayDeclareHead(Name)			\
	IloXmlInternalArrayDeclareMainMembers(Type,Name)	\
	IloXmlInternalArrayDeclareMembers(Type,Name) 		\
}

#define IloXmlDeclareArrayWithoutEquality(Type,Name) 	\
	IloXmlDeclareArrayOf(Type,Name)

#define IloXmlImplementArrayOf(T,N) 				\
	N::~N() 							\
{ 								\
	if (_array) { 						\
	for (IloXmlUInt i=0; i<_length;i++)				\
	_array[i].~T();					\
	IloXmlFree(_array); 					\
	}								\
}								\
	void N::setData(T* a, IloXmlUInt l, IloXmlUInt m) 			\
{ 								\
	this->~N();    						\
	_length = l;						\
	_maxLength = (m==(IloXmlUInt)-1)?l:m;				\
	_array=a; 							\
}								\
	N& N::operator=(const N& src)	 				\
{ 								\
	this->~N();							\
	_maxLength   = src._maxLength;				\
	_length      = src._length;					\
	if (_length > 0) 						\
	_array = ILOXML_MALLOC(T, _maxLength);			\
	else { _array=0; _maxLength=0; }				\
	for (IloXmlUInt i=0; i<_length;i++)				\
	new (_array + i) T(src._array[i]);			\
	return *this;						\
}								\
	void N::setMaxLength(IloXmlUInt nlen, IloXmlBoolean exact)		\
{ 								\
	if (nlen < _length) return;					\
	if (!exact) { 						\
	if (nlen < 4) nlen=4;					\
	IloXmlUInt tlen=4;						\
	while (tlen < nlen) tlen*=2;				\
	nlen=tlen;						\
	}								\
	if (nlen == _maxLength) return;				\
	if (nlen==0 && _array) {					\
	for (IloXmlUInt i=0; i<_length;i++)				\
	_array[i].~T();					\
	IloXmlFree(_array);						\
	_array=0;						\
	} else if (_array) {					\
	T* op = _array;						\
	_array = ILOXML_MALLOC(T, nlen);				\
	for (IloXmlUInt i=0; i<_length;i++) {			\
	new (_array + i) T(op[i]);				\
	op[i].~T();						\
	} 							\
	IloXmlFree(op);						\
	} else 							\
	_array = ILOXML_MALLOC(T,nlen);				\
	_maxLength = nlen;						\
}								\
	void N::erase (IloXmlUInt from, IloXmlUInt to)		 		\
{ 								\
	if (to > _length || to < from) 				\
	to = _length; 						\
	if (to < _length)						\
	for (IloXmlUInt i=0; i < _length - to;i++)			\
	_array[from+i]=_array[to+i];			\
	for (IloXmlUInt i = _length - (to - from); i < _length; i++)	\
	_array[from+i].~T();					\
	_length -= to - from;					\
	if (_maxLength >= _length*4)				\
	setMaxLength(_length, IloXmlFalse);				\
}								\
	void N::insert(const T* src, IloXmlUInt len, IloXmlUInt at)		\
{ 								\
	if (len==0) return;						\
	if (at > _length)						\
	at=_length;						\
	if (_length + len > _maxLength)				\
	setMaxLength(_length + len, IloXmlFalse);			\
	if (_length > at)						\
	for (IloXmlUInt i=_length-1; i >= at; i--)			\
	new (_array+i+len) T (_array[i]);			\
	IloXmlUInt i;							\
	for (i=at; i<_length;i++)					\
	_array[i]=src[i-at];					\
	for (i=_length; i < _length+len;i++)			\
	new (_array+i) T (src[i-at]);				\
	_length+=len;						\
}								\

#define IloXmlImplementAmorphicArrayOf(T,N) 			\
	N::~N() { if (_array) IloXmlFree(_array); }				\
	void N::setData(T* a, IloXmlUInt l, IloXmlUInt m) 			\
{ 								\
	if (_array) 						\
	IloXmlFree(_array); 					\
	_length = l;						\
	_maxLength = (m==(IloXmlUInt)-1)?l:m;				\
	_length = _maxLength = l; 					\
	_array=a; 							\
}								\
	N& N::operator=(const N& src)	 				\
{ 								\
	if (_array) 						\
	IloXmlFree(_array); 					\
	_maxLength   = src._maxLength;				\
	_length      = src._length;					\
	if (_length > 0) 						\
	_array = ILOXML_MALLOC(T, _maxLength);			\
	else { _array=0; _maxLength=0; }				\
	IloXmlMemMove(_array, src._array, _length * sizeof(T));		\
	return *this;						\
}								\
	void N::setMaxLength(IloXmlUInt nlen, IloXmlBoolean exact)	 	\
{ 								\
	if (nlen < _length) return;					\
	if (!exact) { 						\
	if (nlen < 4) nlen=4;					\
	IloXmlUInt tlen=4;						\
	while (tlen < nlen) tlen*=2;				\
	nlen=tlen;						\
	}								\
	if (nlen == _maxLength) return;				\
	if (nlen==0 && _array) { 					\
	IloXmlFree(_array);						\
	_array=0;						\
	} else if (_array)						\
	_array = ILOXML_REALLOC(T, _array, nlen);			\
	else 							\
	_array = ILOXML_MALLOC(T, nlen);				\
	_maxLength = nlen;						\
}								\
	void N::erase(IloXmlUInt from, IloXmlUInt to)			 	\
{ 								\
	if (to > _length || to < from) 				\
	to = _length; 						\
	if (to < _length)						\
	IloXmlMemMove(_array + from, 				\
	_array + to, 				\
	(_length - to) * sizeof(T));			\
	_length -= to - from;					\
	if (_maxLength >= _length*4)				\
	setMaxLength(_length, IloXmlFalse);				\
}								\
	void N::insert(const T* src, IloXmlUInt len, IloXmlUInt at)		\
{ 								\
	if (len==0) return;						\
	if (at > _length)						\
	at = _length;						\
	if (_length + len > _maxLength)				\
	setMaxLength(_length + len, IloXmlFalse);			\
	if (_length > at)						\
	IloXmlMemMove(_array + at + len,				\
	_array + at, 				\
	(_length - at) * sizeof(T));			\
	IloXmlMemMove(_array + at, src, len * sizeof(T));		\
	_length+=len;						\
}

class ILO_EXPORTED IloXmlInternalArrayDeclareHead(IloXmlArray)

	IloXmlInternalArrayDeclareMainMembers(IloXmlAny,IloXmlArray)

	IloXmlInternalArrayDeclareMembers(IloXmlAny,IloXmlArray)

	IloXmlUInt getIndex(const IloXmlAny&) const;
inline void remove(const IloXmlAny& value) { 
	IloXmlInt ndx = getIndex(value);	
	if (ndx != -1) remove((IloXmlUInt)ndx);
}
};

#define IloXmlDeclareAndImplementArrayOfPointerTo(Type,Name)		\
	IloXmlInternalArrayDeclareHead(Name)					\
	inline ~Name() {  if (_array) IloXmlFree(_array); }				\
	inline void setData(Type** a, IloXmlUInt l, IloXmlUInt m=(IloXmlUInt)-1) 		\
{ 									\
	void* tmp=this; IloXmlArray* tmp2=(IloXmlArray*)tmp;			\
	tmp2->setData((IloXmlAny*)a, l, m);					\
}									\
	inline Name& operator=(const Name& src)	 				\
{ 									\
	void* tmp=this; IloXmlArray* tmp2=(IloXmlArray*)tmp;			\
	const void* tmp3=&src; const IloXmlArray* tmp4=(const IloXmlArray*)tmp3;	\
	(*tmp2) = (*tmp4);							\
	return *this;							\
}									\
	inline void setMaxLength(IloXmlUInt nlen, IloXmlBoolean exact=IloXmlTrue)		\
{ 									\
	void* tmp=this; IloXmlArray* tmp2=(IloXmlArray*)tmp;			\
	tmp2->setMaxLength(nlen, exact);					\
}									\
	inline void erase (IloXmlUInt from=0, IloXmlUInt to=(IloXmlUInt)-1)			\
{ 									\
	void* tmp=this; IloXmlArray* tmp2=(IloXmlArray*)tmp;			\
	tmp2->erase(from,to);						\
}									\
	inline void insert(const Type** src, IloXmlUInt len, IloXmlUInt at)		\
{ 									\
	void* tmp=this; IloXmlArray* tmp2=(IloXmlArray*)tmp;			\
	tmp2->insert((const IloXmlAny*)src, len, at);				\
}									\
	inline IloXmlUInt getIndex(const Type * & value) const 			\
{ 									\
	void* tmp=(Name*) this; IloXmlArray* tmp2=(IloXmlArray*)tmp;		\
	return tmp2->getIndex((IloXmlAny) value);				\
}									\
	inline void remove(const Type*& value) { 				\
	IloXmlInt ndx = getIndex(value);					\
	if (ndx != -1) remove((IloXmlUInt)ndx);				\
}									\
	IloXmlInternalArrayDeclareMembers(Type*,Name) 				\
}

class ILO_EXPORTED IloXmlBitArray
{
public:
	IloXmlBitArray(IloXmlUInt length, IloXmlBoolean initialState = IloXmlFalse);
	IloXmlBitArray(const IloXmlBitArray&);
	virtual ~IloXmlBitArray();
	
	IloXmlUInt getLength() const { return _length; }
	void    setLength(IloXmlUInt newLength, IloXmlBoolean newState = IloXmlFalse);
	IloXmlBoolean isSet(IloXmlUInt rank) const;
	void set(IloXmlUInt rank, IloXmlBoolean flag);
	void invert(IloXmlUInt rank);
	void set(IloXmlUInt rank)   { set(rank, IloXmlTrue);  }
	void unset(IloXmlUInt rank) { set(rank, IloXmlFalse); }
	IloXmlBoolean operator[](IloXmlUInt rank) const { return isSet(rank); }
	void operator+=(IloXmlUInt rank) { set(rank); }
	void operator-=(IloXmlUInt rank) { unset(rank); }
private:
	IloXmlUInt		_length;
	unsigned char*	_bits;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
