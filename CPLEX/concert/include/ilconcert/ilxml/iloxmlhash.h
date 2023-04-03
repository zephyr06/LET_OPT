// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilxml/iloxmlhash.h
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

#ifndef __XML_iloxmlhashH
#define __XML_iloxmlhashH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloenv.h>
#include <ilconcert/iloany.h>
#include <ilconcert/ilohash.h>

typedef IloAddressHashTable<IloInt> IdHash;

class IloXmlIdTable : public IdHash {
 public:
  IloXmlIdTable(IloEnv env, IloInt size=IloDefaultHashSize): IdHash(env, size){}
  ~IloXmlIdTable(){}
};

 
class IloXmlObjectHandler {
private:
    IloAnyArray _link2obj;
    IloEnv _env;

    IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloRtti* exprI);
    IloBool checkTypeOfObjectById(IloTypeInfo type, IloRtti* exprI);

public:
    ~IloXmlObjectHandler();

    IloAnyArray getObjects(){
	return _link2obj;
    }

 
    IloXmlObjectHandler(IloEnv);

 
    void addLink2Obj(IloInt id, const void* object);

 
    void clearLink2Obj();

 
    IloAny getObjectById(IloInt id);

 
    IloBool isValidId(IloInt id);

 
  IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloInt Xml_Id);

 
  IloBool checkTypeOfObjectById(IloTypeInfo type, IloInt Xml_Id);

};

 
class IloXmlIdManager {
private:
    IloXmlIdTable _idHash;
    IloInt _maxId;
    IloEnv _env;

    IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloRtti* exprI);
    IloBool checkTypeOfObjectById(IloTypeInfo type, IloRtti* exprI);

public:
 IloXmlIdTable& getHash(){
    return _idHash;
 }

 
    IloXmlIdManager(IloEnv env);

 
    IloEnv getEnv() { return _env; }

 
    IloBool isValidId(IloInt id){
	if (_maxId >= id){
            return IloTrue;
        }
        return IloFalse;
    }

 
    void clear();

    IloInt hasId(const void* object, IloBool& firstTime);

    IloInt getId(const void* object, IloBool& firstTime);

 
    IloInt getMaxId();

 
    IloInt getId(const void *p) {
       IloBool dummy;
       return getId(p, dummy);
    }

 
    IloBool hasId(const void *p) {
       IloBool dummy;
       return hasId(p, dummy) >= 0;
    }

    const void* getObjectById(IloInt id);

 
  IloBool checkRttiOfObjectById(IloTypeIndex RTTI, IloInt Xml_Id);

 
  IloBool checkTypeOfObjectById(IloTypeInfo type, IloInt Xml_Id);

    ~IloXmlIdManager();
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
