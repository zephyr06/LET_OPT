// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/ilopool.h
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
#ifndef __XML_ilopoolH
#define __XML_ilopoolH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilxml/iloiostream.h>
#include <ilconcert/ilxml/ilomemory.h>

class ILO_EXPORTED IloXmlMemoryPool
{
protected:
	IloXmlMemoryPool(int size)
		: _size(size),
		_blocks(0)
	{}
public:
	~IloXmlMemoryPool();
	
	void* alloc(IloXmlUInt size, IloXmlBoolean lock);
	void* reAlloc(void*, IloXmlUInt size, IloXmlBoolean lock);
	void* lock(void*) const;
	void* unLock(void*) const;
	void free();
	IloXmlAny find(void*) const;
	IloXmlAny getIloXmlBlock(void*) const;
	void* getData(IloXmlAny) const;
	void* take(IloXmlAny&, IloXmlUInt size, IloXmlBoolean lock);
	void* release(IloXmlAny);
	void* grow(IloXmlAny, IloXmlUInt size, IloXmlBoolean lock);
	void printInformation(IL_STDPREF ostream&, 
		const char*,
		IloXmlUShort = 0) const;
private:
	int		_size;
	IloXmlAny	_blocks;
};

#define IloXmlPoolOf(name) IloXml##name##Pool
#define IloXmlPoolNoE
#define IloXmlDeclareExpPoolOf(NAME, TYPE, EXPORT)				\
class EXPORT IloXmlPoolOf(NAME) : public IloXmlMemoryPool {			\
public: static TYPE* Alloc(IloXmlUInt n, IloXmlBoolean lock = IloXmlFalse) {	\
	return ILOXML_UNSAFECAST(TYPE*, _Pool.alloc(n*sizeof(TYPE), lock)); }	\
	static TYPE* AllocIloXmlBlock(IloXmlAny& block, IloXmlUInt n) {			\
	return ILOXML_UNSAFECAST(TYPE*, _Pool.take(block, n*sizeof(TYPE), IloXmlTrue));}\
	static IloXmlAny GetIloXmlBlock(TYPE* data) { return _Pool.getIloXmlBlock(data); }	\
	static TYPE* GetData(IloXmlAny block) {					\
	return ILOXML_UNSAFECAST(TYPE*, _Pool.getData(block)); }			\
	static TYPE* ReAlloc(TYPE* d, IloXmlUInt n, IloXmlBoolean l = IloXmlFalse) {	\
	return ILOXML_UNSAFECAST(TYPE*, _Pool.reAlloc((IloXmlAny)d, n*sizeof(TYPE),l));}\
	static TYPE* ReAllocIloXmlBlock(IloXmlAny& block, IloXmlUInt n) {			\
	return block								\
	? ILOXML_UNSAFECAST(TYPE*, _Pool.grow(block, n*sizeof(TYPE), IloXmlFalse))	\
	: AllocIloXmlBlock(block, n); }						\
	static TYPE* Lock(TYPE* data)   {					\
	return ILOXML_UNSAFECAST(TYPE*, _Pool.lock(data)); }			\
	static TYPE* UnLock(TYPE* data) {					\
	return ILOXML_UNSAFECAST(TYPE*, _Pool.unLock(data)); }			\
	static TYPE* ReleaseIloXmlBlock(IloXmlAny block)					\
{ return block ? ILOXML_UNSAFECAST(TYPE*, _Pool.release(block)) : 0; }	\
	static void PrintInformation(IL_STDPREF ostream& os, IloXmlUShort l=0)      \
{ _Pool.printInformation(os, #NAME, l); }	 		        \
private: IloXmlPoolOf(NAME)() : IloXmlMemoryPool(sizeof(TYPE)) {}		\
	static IloXmlPoolOf(NAME) _Pool; }
#define IloXmlDeclarePoolOf(N, T) IloXmlDeclareExpPoolOf(N, T, IloXmlPoolNoE)

#define IloXmlDefinePoolOf(NAME) IloXmlPoolOf(NAME) IloXmlPoolOf(NAME)::_Pool

IloXmlDeclareExpPoolOf(Char,    char,   ILO_EXPORTED);
IloXmlDeclareExpPoolOf(Pointer, IloXmlAny,  ILO_EXPORTED);
IloXmlDeclareExpPoolOf(IloXmlUInt,  IloXmlUInt, ILO_EXPORTED);

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
