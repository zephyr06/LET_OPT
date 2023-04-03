// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloreader.h
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

#ifndef __XML_iloreaderH
#define __XML_iloreaderH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloenv.h>
#include <ilconcert/ilosolution.h>
#include <ilconcert/iloany.h>
#include <ilconcert/iloset.h>
#include <ilconcert/ilxml/iloxmlexception.h>
#include <ilconcert/ilxml/iloxmlhash.h>

#ifdef ILO_RS6000
void* operator new (size_t sz, const IloEnv& env);
void* operator new[] (size_t s, const IloEnv& env);
#endif

#include <ilconcert/ilxml/iloxml.h>
#include <ilconcert/ilxml/iloparser.h>
#include <ilconcert/ilxml/ilolist.h>
#include <ilconcert/ilxml/ilomemory.h>
#include <ilconcert/ilxml/ilopool.h>

#if defined(IL_STD)
IL_STDUSE
#endif 

class IloXmlReaderI;
class IloXmlContextI;

class ILO_EXPORTED IloXmlReader {
private:
	IloXmlReaderI* _impl;

public:

	const IloXmlReaderI* getImpl();

	void end();

	IloXmlObjectHandler& getObjectHandler();

	
	IloBool isSerialized(IloInt id);

	
	IloIntArray* getSerialized();

	
	IloIntArray* getSolutionSerialized();

	void addLink2Obj(IloInt id, void* object);

	
	IloAny getObjectById(IloInt id);

	
	IloXmlReader(IloEnv env, const char* fileName=0);

	
	IloXmlReader(IloXmlReaderI* impl): _impl(impl) {};

	
	void deleteAllocatedMemory(char* pointer);

	
	void deleteAllocatedMemory(const char* pointer);

	
	void setfileName(const char* fileName);

	
	IloXmlElement* getRoot();

	
	IloEnv getEnv();

	
	IloEnvI* getEnvImpl();

	
	IloBool getIntAttribute(IloXmlElement* element, const char* attribute, IloInt &value);

	
	IloBool getNumAttribute(IloXmlElement* element, const char* attribute, IloNum &value);

	
	IloInt string2Int(const char* str);

	
	IloNum string2Num(const char* str);

	
	IloIntArray string2IntArray(const char* str);

	
	IloNumArray string2NumArray(const char* str);

	
	IloIntSet string2IntSet(const char* str);

	
	IloIntRange string2IntRange(IloXmlElement* element);

	
	IloNumSet string2NumSet(const char* str);

	
	IloXmlElement* getFirstSubElement(IloXmlElement* element);

	
	const char* readAttribute(IloXmlElement* element, const char* attribute);

	
	const char* readComment(IloXmlElement* element);

	
	const char* readText(IloXmlElement* element);

	
	const char* readData(IloXmlElement* element);

	
	const char* readCData(IloXmlElement* element);

	
	IloInt getChildrenCardinal(IloXmlElement* element);

	
	IloXmlElement* findElement(IloXmlElement* root, const char* tag, const char* attribute, const char* value);

	
	IloXmlElement* findElementByTag(IloXmlElement* element, const char* tag);

	
	IloBool openDocument();
	IloBool openStream(istream& file);

	
	IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloInt Xml_Id);

	
	IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloRtti* exprI);

	
	IloBool checkTypeOfObjectById(IloTypeInfo type, IloRtti* exprI);

	
	IloBool checkTypeOfObjectById(IloTypeInfo type, IloInt Xml_Id);
};

class ILO_EXPORTED IloReaderError : public IloException {
public:
	IloReaderError(const char* message);
	virtual ~IloReaderError();
};

class IloXmlReaderI : public IloEnvObjectI {
private:
	char* _fileName;
	IloXmlDocument* _document;
	IloXmlElement* _root;

	IloXmlObjectHandler _voidHandler;

private:
	IloBool checkStream(istream& file);
	IloBool checkIfSolutionObject(IloInt id);

public:
	~IloXmlReaderI();

	void addLink2Obj(IloInt id, void* object);
	IloAny getObjectById(IloInt id);

	IloXmlObjectHandler& getObjectHandler() {
		return _voidHandler;
	}

	IloBool isSerialized(IloInt id);
	IloIntArray* getSerialized();
	IloIntArray* getSolutionSerialized();

	IloXmlReaderI(IloEnv env, const char* fileName=0);
	void deleteAllocatedMemory(char* pointer);
	void deleteAllocatedMemory(const char* pointer);
	void setfileName(const char* fileName);
	IloXmlElement* getRoot();

	IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloInt Xml_Id);
	IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloRtti* exprI);
	IloBool checkTypeOfObjectById(IloTypeInfo type, IloRtti* exprI);
	IloBool checkTypeOfObjectById(IloTypeInfo type, IloInt Xml_Id);

	IloBool getIntAttribute(IloXmlElement* element, const char* attribute, IloInt &value);
	IloBool getNumAttribute(IloXmlElement* element, const char* attribute, IloNum &value);
	IloInt string2Int(const char* string);
	IloNum string2Num(const char* string);
	IloIntArray string2IntArray(const char* string);
	IloNumArray string2NumArray(const char* string);
	IloIntSet string2IntSet(const char* string);
	IloNumSet string2NumSet(const char* string);
	IloIntRange string2IntRange(IloXmlElement* element);
	IloXmlElement* getFirstSubElement(IloXmlElement* element);
	const char* readAttribute(IloXmlElement* element, const char* attribute);
	const char* readComment(IloXmlElement* element);
	const char* readText(IloXmlElement* element);
	const char* readData(IloXmlElement* element);
	const char* readCData(IloXmlElement* element);
	IloInt getChildrenCardinal(IloXmlElement* element);
	IloXmlElement* findElement(IloXmlElement* root, const char* tag, const char* attribute, const char* value);
	IloXmlElement* findElementByTag(IloXmlElement* element, const char* tag);
	IloBool openDocument();
	IloBool openStream(istream& file);
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
