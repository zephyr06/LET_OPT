// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/iloi18n.h
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

#ifndef __XML_iloi18nH
#define __XML_iloi18nH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilxml/ilomacros.h>

typedef IloXmlUChar  IloXmlUTF8;
typedef IloXmlUShort IloXmlUTF16;
typedef IloXmlUInt   IloXmlUTF32;

class IloXmlI18n
{
public:
    static IloXmlUTF16* Utf8ToUtf16(IloXmlUInt& outputSize,
				IloXmlUInt  inputSize,
				const IloXmlUTF8* input);
    static IloXmlUTF8* Utf16ToUtf8(IloXmlUInt& outputSize,
			       IloXmlUInt  inputSize,
			       const IloXmlUTF16* input);

    static IloXmlUTF16* EBCDICToUtf16(IloXmlUInt& outputSize,
				      IloXmlUInt  inputSize,
				      const char* input);
    static char* Utf16ToEBCDIC(IloXmlUInt& outputSize,
					IloXmlUInt  inputSize,
					const IloXmlUTF16* input);

};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
