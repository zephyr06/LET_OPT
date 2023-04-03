// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloxmlabstract.h
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

#ifndef __XML_iloxmlabstractH
#define __XML_iloxmlabstractH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloreader.h>
#include <ilconcert/iloexpressioni.h>
#include <ilconcert/ilopathi.h>
#include <ilconcert/iloseti.h>
#include <ilconcert/ilodiffi.h>
#include <ilconcert/ilosymbol.h>
#include <ilconcert/ilowriter.h>
#include <ilconcert/ilxml/ilotag.h>

#ifdef ILO_RS6000
void* operator new (size_t sz, const IloEnv& env);
void* operator new[] (size_t s, const IloEnv& env);
#endif

#define IloDistributeVI IloDistributeI

#include <ilconcert/iloxmlcontext.h>

class IloXmlElement;
class IloConstraintI;
class IloIntExprI;
class IloNumExprI;

template <class XMLINFO, class EXPR> IloBool ITERATE(IloXmlReader reader, IloXmlElement* element, XMLINFO* info, EXPR* exprI){
	for (IloXmlListIteratorOf(IloXmlNodeI) i(element->getChildren());
		i.hasMoreElements();) {
			IloXmlNodeI* node = i.getNextElement();
			if (node->getType() == IloXmlNodeI::Element) {
				IloXmlElement* subElement = (IloXmlElement*)node;
				if (!info->checkSubElement(reader, subElement, exprI)){
					return IloFalse;
				}
			}
	}
	return IloTrue;
}

template <class XMLINFO, class EXPR> IloRtti* CREATE_ITERATOR_TERM(IloXmlReader reader, IloXmlElement* element, XMLINFO* info, EXPR* exprI){
	exprI = new (reader.getEnv()) EXPR(reader.getEnvImpl(),
		reader.readAttribute(element, IloXmlAttributeDef::Name));
	if (ITERATE(reader, element, info, exprI)){
		return (IloRtti*)exprI;
	}
	return 0;
}

template <class XMLINFO, class EXPR> IloExtractableArray* CREATE_ITERATOR_TERM_FOR_ARRAY(IloXmlReader reader, IloXmlElement* element, XMLINFO* info, EXPR* exprI){
	exprI = new (reader.getEnv()) EXPR(reader.getEnv(),0);
	ITERATE(reader, element, info, exprI);
	return (IloExtractableArray*)exprI;
}

class IloXmlInfo {
protected:
	char* _version;
	IloXmlContextI* _context;
public:

	IloEnvI* getEnv() const { return _context->getEnv(); }

	virtual ~IloXmlInfo();
	
public:
	IloXmlInfo(IloXmlContextI* context, const char* version=0);

	
public:
	IloXmlInfo(){}

	
public:
	static const char* getTagName();

	
public:
	virtual const char* getTag();

	
public:
	virtual IloXmlElement* getTagElement(IloXmlWriter writer, const IloRtti* exprI);

	
	IloXmlContextI* getContext() { return _context; }

protected:
	
	const char* getVersion();

	
	void setVersion(const char* version);

	
public:
	IloBool checkExprExistence(IloXmlReader reader,
		IloXmlElement* element,
		const char* attribute,
		IloInt& id);
	
public:
	IloBool getRefInChild(IloXmlReader reader,
		IloXmlElement* element,
		IloInt& id);

	
public:
	IloBool checkAttExistence(IloXmlReader reader,
		IloXmlElement* element,
		const char* attribute);

	
public:
	IloBool writeRtti(IloXmlWriter writer, IloXmlElement* element, const IloRtti* rtti, const char* attribute = 0);

public:
	
	IloBool writeExtractable(IloXmlWriter writer, IloXmlElement* element, const IloExtractable extractable, const char* attribute = 0){
		return writeRtti(writer, element, extractable.getImpl(), attribute);
	}

	
public:
	void writeSolutionValue(IloXmlWriter writer, const IloSolution solution, IloXmlElement* element, const IloRtti* rtti, const char* attribute);

	
public:
	virtual IloBool writeXml(IloXmlWriter writer, const IloExtractableI* exprI, IloXmlElement* masterElement);

	
public:
	virtual IloBool write(IloXmlWriter writer, const IloRtti* exprI, IloXmlElement* masterElement);

	
public:
	virtual IloBool writeRef(IloXmlWriter writer, const IloRtti* exprI, IloXmlElement* masterElement);

	
public:
	virtual IloBool writeXmlRef(IloXmlWriter writer, const IloExtractableI* exprI, IloXmlElement* masterElement);

public:
	
	IloBool readExtractable(IloXmlReader reader, IloXmlElement* element){
		return readRtti(reader, element);
	}

	
public:
	IloBool readRtti(IloXmlReader reader, IloXmlElement* element);

	
protected:
	virtual IloRtti* read(IloXmlReader reader, IloXmlElement* element);

	
protected:
	virtual IloExtractableI* readXml(IloXmlReader reader, IloXmlElement* element);

	
public:
	virtual IloExtractableI* readFromXml(IloXmlReader reader, IloXmlElement* element);

	
public:
	virtual IloRtti* readFrom(IloXmlReader reader, IloXmlElement* element);

protected:
	virtual IloBool checkSubElement(IloXmlReader reader, IloXmlElement* subElement, IloRtti* exprI);

protected:
	
	virtual IloBool checkSubElement(IloXmlReader reader, IloXmlElement* subElement, IloExtractableI* exprI);

	
protected:
	virtual IloExtractableArray* readXmlArray(IloXmlReader reader, IloXmlElement* element);

	
public:
	virtual IloExtractableArray* readArrayFromXml(IloXmlReader reader, IloXmlElement* element);

	
public:
	IloXmlElement* setCommonArrayXml(IloXmlWriter writer,
		const IloExtractableArray* extractable);

protected:
	virtual IloBool checkSubElement(IloXmlReader reader,
		IloXmlElement* subElement,
		IloExtractableArray* varArray);

	
public:
	virtual int write(IloXmlWriter writer, const IloExtractableArray* extractable, IloXmlElement* masterElement);

	
public:
	IloXmlElement* setCommonXml(IloXmlWriter writer, const IloRtti* exprI);

	
public:
	IloXmlElement* setCommonValueXml(IloXmlWriter writer, const IloRtti* exprI);

	
public:
	void setXml(IloXmlWriter writer, IloXmlElement* element, const IloRtti* exprI);

	
protected:
	IloXmlElement* setNumArray(IloXmlWriter writer, const IloNumArray Array);

	
protected:
	IloXmlElement* setIntSet(IloXmlWriter writer, const IloIntSet Array);

	
protected:
	IloXmlElement* setNumSet(IloXmlWriter writer, const IloNumSet Array);

	IloXmlElement* setIntRange(IloXmlWriter writer, const IloIntRange Array);

	
protected:
	IloXmlElement* setIntArray(IloXmlWriter writer, const IloIntArray Array);

	
protected:
	IloXmlElement* setBoolArray(IloXmlWriter writer, const IloBoolArray Array);

	
protected:
	IloNumVar::Type getVarType(IloXmlReader reader, IloXmlElement* element);

	
protected:
	IloBool getNumValArray(IloXmlReader reader, IloXmlElement* element, IloNumArray& numArray);

	
protected:
	IloBool getIntValArray(IloXmlReader reader, IloXmlElement* element, IloIntArray& intArray);

	
public:
	virtual IloBool readSolution(IloXmlReader reader, IloSolution solution, IloXmlElement* element);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloIntVarArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloBoolVarArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloIntExprArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloNumExprArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloIntSetVarArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloNumVarArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloRangeArray array);

protected:

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloConstraintArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloSemiContVarArray array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloSOS1Array array);

	
protected:
	IloBool writeVarArray(IloXmlWriter writer,
		IloXmlElement* element,
		IloSOS2Array array);

	
public:
	virtual void writeSolution(IloXmlWriter writer,
		const IloSolution solution,
		const IloExtractable extractable);
};

class IloSerializeError : public IloException {
public:
	IloSerializeError(const char* message);
	virtual ~IloSerializeError();
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
