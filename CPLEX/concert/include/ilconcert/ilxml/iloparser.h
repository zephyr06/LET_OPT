// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/iloparser.h
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

#ifndef __XML_iloparserH
#define __XML_iloparserH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilxml/iloxml.h>
#include <ilconcert/ilxml/iloi18n.h>

#if defined(IL_STD)
#include <cctype>
#include <iostream>
#include <cstring>
#else  
#include <ctype.h>
#include <iostream.h>
#include <string.h>
#endif 

class IloXmlParser
{
	class Marker;
public:
	IloXmlParser(IL_STDPREF istream& s, IloXmlDocumentI* document);
	~IloXmlParser();
	
	enum Error {
		XML_NO_ERROR,
		INVALID_DOCUMENT,
		EMPTY_DOCUMENT,
		EARLY_END_OF_DOCUMENT,
		END_OF_DOCUMENT_EXPECTED,
		END_OF_DOCUMENT_UNEXPECTED,
		INVALID_VERSION_INFO,
		INVALID_SD_DECLARATION,
		INVALID_EXTERNALID,
		INVALID_DECLARATION_END,
		INVALID_COMMENT_CHARS,
		RESERVED_PI_TARGET_NAME,
		INVALID_CDATA,
		INVALID_ENCODING_NAME,
		INVALID_ENCODING_DECLARATION,
		INVALID_TAG_NAME,
		UNEXPECTED_TAG_SYMBOL,
		END_TAG_EXPECTED,
		END_TAG_NOT_MATCHED,
		INVALID_TAG_END,
		EQUAL_SIGN_EXPECTED,
		INVALID_ATTRIBUTE_VALUE,
		INVALID_REFERENCE_DECLARATION,
		INVALID_CHARREF_DECLARATION,
		INVALID_DOCTYPE_NAME,
		UNRECOGNIZED_TAG
	};
	IloXmlUTF16* getCurrent() const { return _current; }
private:
	void reset();
	IloXmlBoolean	parseS();
	IloXmlBoolean 	parseName(char*& name);
	IloXmlBoolean 	parseAttValue(char*& value);
	IloXmlBoolean 	parseCharData();
	IloXmlBoolean 	parseComment();
	IloXmlBoolean 	parsePI();
	
	IloXmlBoolean 	parseCDSect();
	void 	parseProlog();
	
	IloXmlBoolean 	parseXMLDecl();
	
	IloXmlBoolean 	parseVersionInfo();
	IloXmlBoolean 	parseEq();
	IloXmlBoolean 	parseVersionNum();
	IloXmlBoolean 	parseMisc();
	IloXmlBoolean 	parseDoctypedecl();
	void 	parseSDDecl();
	void 	parseExternalID();
	IloXmlBoolean 	parseElement(IloXmlElementI*& element);
	IloXmlBoolean 	parseTagStart(IloXmlElementI*& node, IloXmlBoolean& isEmpty);
	IloXmlBoolean 	parseTagEnd(IloXmlElementI* node);
	IloXmlBoolean 	parseAttribute(IloXmlElementI* node);
	IloXmlBoolean 	parseContent();
	IloXmlBoolean 	parseReference(IloXmlCharRefI*& reference);
	IloXmlBoolean 	parseEncodingDecl();
	IloXmlBoolean 	parseChar(int c);
	IloXmlBoolean 	parseString(const char* string);
	IloXmlBoolean 	parseIString(const char* string);
	IloXmlUTF16 	nextChar();
	void	previousChar();
	IloXmlBoolean 	parseAttribute(const char* name);
	IloXmlBoolean 	fillBuffer(IL_STDPREF istream& stream);

	
	void 	addNode(IloXmlNodeI*);
	void 	addText(Marker&);
	void 	addText(const char*);
	static IloXmlUTF16 GetChar(IloXmlCharRefI*);

	IloXmlDocumentI*	_document;
	IloXmlAny		_bufferIloXmlBlock;
	IloXmlUTF16*		_buffer;
	IloXmlUInt		_bufferSize;
	IloXmlUTF16*		_current;
	IloXmlElementI*	_currentElement;

	class Marker
	{
	public:
		Marker(const IloXmlParser* parser, IloXmlUTF16* marker)
			: _parser(parser),
			_marker(marker)
		{
		}
		char* getIloXmlBlock(const IloXmlUTF16* marker, IloXmlUInt skipAtEnd = 0) const;
		IloXmlBoolean operator!=(const Marker& ref) const
		{ return (_marker != ref._marker); }
		IloXmlUTF16* getCurrent() const { return _marker; }
		void reset(IloXmlUTF16* marker) { _marker = marker; }
	private:
		const IloXmlParser* _parser;
		IloXmlUTF16*	   _marker;
	};
	void 	goTo(const Marker& marker) { _current = marker.getCurrent(); }
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
