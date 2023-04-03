// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilowriter.h
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

#ifndef __XML_ilowriterH
#define __XML_ilowriterH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/ilosolution.h>
#include <ilconcert/iloset.h>
#include <ilconcert/iloany.h>
#include <ilconcert/ilxml/iloxmlexception.h>
#include <ilconcert/ilxml/iloxmlhash.h>

#ifdef ILO_RS6000
void* operator new (size_t sz, const IloEnv& env);
void* operator new[] (size_t s, const IloEnv& env);
#endif

#include <ilconcert/ilxml/iloxml.h>
#include <ilconcert/ilxml/ilolist.h>
#include <ilconcert/ilxml/ilomemory.h>
#include <ilconcert/ilxml/ilopool.h>

#if defined(IL_STD)
IL_STDUSE
#endif 

const IloInt IloNoRoundingMode = -1;

class IloXmlWriterI;
class IloXmlContextI;

class ILO_EXPORTED IloXmlWriter {
private:
	IloXmlWriterI* _impl;
public:

	void end();

	IloXmlIdManager& getIdManager();

	const IloXmlWriterI* getImpl();

	
	IloXmlWriter(IloEnv env, const char* rootTag, const char* fileName=0);

	
	IloXmlWriter(IloXmlWriterI* impl): _impl(impl) {};

	
	IloBool isSerialized(IloInt id);

	
	IloIntArray* getSerialized();

	
	IloIntArray* getSolutionSerialized();

	
	IloBool isSolutionSerialized(IloInt id);

	
	void deleteAllocatedMemory(char* pointer);

	void setNumPrecision(const IloInt num);
	IloInt getNumPrecision();

	
	IloInt string2Int(const char* str);

	
	void deleteAllocatedMemory(const char* pointer);

	
	void setfileName(const char* fileName);

	
	const char* getfileName();

	
	IloXmlElement* getRoot();

	
	IloEnv getEnv();

	
	IloEnvI* getEnvImpl();

	
	const char* Int2String(const IloInt number);

	
	const char* Num2String(const IloNum number);

	
	const char* IntArray2String(const IloIntArray intArray);

	
	const char* NumArray2String(const IloNumArray numArray);

	
	const char* IntSet2String(const IloIntSet intSet);

	
	const char* NumSet2String(const IloNumSet numSet);

	
	void addElement(IloXmlElement* element);

	
	void addSubElement(IloXmlElement* element, IloXmlElement* subElement);

	
	void addAttribute(IloXmlElement* element, const char* attribute, const char* value);

	
	IloXmlElement* createElement(const char* element);

	
	void addComment(IloXmlElement* element,const char* comment);

	
	void addText(IloXmlElement* element, const char* text);

	
	void addCData(IloXmlElement* element, const char* CData);

	
	IloBool writeDocument();
};

class ILO_EXPORTED IloWriterError : public IloException {
public:
	IloWriterError(const char* message);
	virtual ~IloWriterError();
};

class IloXmlWriterI : public IloEnvObjectI  {
private:
	int _numPrecision;
	char* _fileName;
	IloXmlDocument* _document;
	IloXmlElement* _root;
	IloXmlIdManager _idManager;

private:
	char* precision(double r);
	IloBool checkIfSolutionObject(IloInt id);

public:
	~IloXmlWriterI();

	IloXmlIdManager& getIdManager() {
		return _idManager;
	}

	IloXmlWriterI(IloEnv env, const char* rootTag, const char* fileName=0);

	void deleteAllocatedMemory(char* pointer);

	void setNumPrecision(const IloInt num);
	IloInt getNumPrecision();

	IloIntArray* getSerialized();
	IloIntArray* getSolutionSerialized();

	IloBool isSolutionSerialized(IloInt id);
	IloBool isSerialized(IloInt id);

	IloInt string2Int(const char* string);

	void deleteAllocatedMemory(const char* pointer);

	void setfileName(const char* fileName);

	const char* getfileName();

	IloXmlElement* getRoot();

	const char* Int2String(const IloInt number);
	const char* Num2String(const IloNum number);
	const char* IntArray2String(const IloIntArray intArray);
	const char* NumArray2String(const IloNumArray numArray);

	const char* IntSet2String(const IloIntSet intSet);
	const char* NumSet2String(const IloNumSet numSet);

	void addElement(IloXmlElement* element);

	void addSubElement(IloXmlElement* element, IloXmlElement* subElement);

	void addAttribute(IloXmlElement* element, const char* attribute, const char* value);

	IloXmlElement* createElement(const char* element);

	void addComment(IloXmlElement* element,const char* comment);

	void addText(IloXmlElement* element, const char* text);

	void addCData(IloXmlElement* element, const char* CData);

	IloBool writeDocument();
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
