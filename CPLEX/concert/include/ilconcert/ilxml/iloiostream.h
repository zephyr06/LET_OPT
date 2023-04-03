// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/iloiostream.h
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

#ifndef __XML_iloiostreamH
#define __XML_iloiostreamH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/ilxml/ilomacros.h>

#if defined(IL_STD)
#if defined(_MSC_VER)
#include <wchar.h>
#endif
#include <iostream>
#if (defined(WINDOWS) || defined(PM)) && !defined(MAINWIN)
#define IloXmlInputStreamMode  (std::ios::in|std::ios::binary)
#define IloXmlOutputStreamMode (std::ios::out|std::ios::binary)
#define IloXmlBinaryInputStreamMode  (std::ios::in|std::ios::binary)
#define IloXmlBinaryOutputStreamMode (std::ios::out|std::ios::binary)
#else  
#define IloXmlInputStreamMode  (std::ios::in)
#define IloXmlOutputStreamMode (std::ios::out)
#define IloXmlBinaryInputStreamMode  IloXmlInputStreamMode
#define IloXmlBinaryOutputStreamMode IloXmlOutputStreamMode
#endif 
#else  
#  if defined(__SUNPRO_CC_COMPAT) && (__SUNPRO_CC_COMPAT >= 5)
#     include <iostream.h>
#  else
class istream;
class ostream;
#  endif
#if (defined(WINDOWS) || defined(PM)) && !defined(MAINWIN)
#define IloXmlInputStreamMode  (ios::in|ios::binary)
#define IloXmlOutputStreamMode (ios::out|ios::binary)
#define IloXmlBinaryInputStreamMode  (ios::in|ios::binary)
#define IloXmlBinaryOutputStreamMode (ios::out|ios::binary)
#else  
#define IloXmlInputStreamMode  (ios::in)
#define IloXmlOutputStreamMode (ios::out)
#define IloXmlBinaryInputStreamMode  IloXmlInputStreamMode
#define IloXmlBinaryOutputStreamMode IloXmlOutputStreamMode
#endif 
#endif 

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
