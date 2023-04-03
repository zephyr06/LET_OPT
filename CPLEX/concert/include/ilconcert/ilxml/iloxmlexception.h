// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/iloxmlexception.h
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

#ifndef __XML_iloxmlexceptionH
#define __XML_iloxmlexceptionH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/ilosys.h>

class IloXmlNotExtractable : public IloException {
public:
	IloXmlNotExtractable()
		: IloException("The object is not an IloExtractable"){}
};

class IloXmlStrLen : public IloException {
public:
	IloXmlStrLen()
		: IloException("XML are limited to 256 characters"){}
};

class IloXmlCheckVersion : public IloException {
public:
	IloXmlCheckVersion()
		: IloException("The XML has a bad version"){}
};

class IloXmlUnknownClass : public IloException {
public:
	IloXmlUnknownClass()
		: IloException("The Class is unknown for solution"){}
};

class IloXmlTagExistence : public IloException {
public:
	IloXmlTagExistence()
		: IloException("The Tag does not exist in this xml element.") {}
};

class IloXmlwriteSolution : public IloException {
public:
	IloXmlwriteSolution()
		: IloException("Bad method called.") {}
};

class IloXmlContextOpenStream : public IloException {
public:
	IloXmlContextOpenStream()
		: IloException("Problem with the XML stream.") {}
};

class IloXmlContextOpenDocument : public IloException {
public:
	IloXmlContextOpenDocument()
		: IloException("Impossible to open the XML document.") {}
};

class IloXmlContextNoRoot : public IloException {
public:
	IloXmlContextNoRoot()
		: IloException("Impossible to catch the Root in the XML document.") {}
};

class IloXmlBadTypeInXml : public IloException {
public:
	IloXmlBadTypeInXml()
		: IloException("Problem in the xml: the object is composed of bad types.") {}
};

class IloXmlReadNoRef : public IloException {
public:
	IloXmlReadNoRef()
		: IloException("No reference in the XML for the Solution reading.") {}
};

class IloXmlReadRefNotInMemory : public IloException {
public:
	IloXmlReadRefNotInMemory()
		: IloException("The Extractable in reference has not been yet created for the solution.") {}
};

class IloXmlWriteConstraint : public IloException {
public:
	IloXmlWriteConstraint()
		: IloException("ERROR: Try to write a IloConstraint.") {}
};

class IloXmlReadConstraint : public IloException {
public:
	IloXmlReadConstraint()
		: IloException("ERROR: Try to read a IloConstraint from the XML file.") {}
};

class IloXmlWriteExtractable : public IloException {
public:
	IloXmlWriteExtractable()
		: IloException("ERROR: Try to write a IloExtractable.") {}
};

class IloXmlReadExtractable : public IloException {
public:
	IloXmlReadExtractable()
		: IloException("ERROR: Try to read a IloExtractable from the XML file.") {}
};

class IloXmlregisterTypeIndexArray : public IloException {
public:
	IloXmlregisterTypeIndexArray()
		: IloException("Problem in the register of the xml classes for Array.") {}
};

class IloXmlregisterTypeIndex : public IloException {
public:
	IloXmlregisterTypeIndex()
		: IloException("Problem in the register of the xml classes.") {}
};

class IloXmlBadNumVarSolution : public IloException {
public:
	IloXmlBadNumVarSolution()
		: IloException("Value or Min or Max is undefined for IloNumVar Solution.") {}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
