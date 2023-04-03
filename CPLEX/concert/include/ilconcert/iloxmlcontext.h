// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloxmlcontext.h
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

#ifndef __XML_iloxmlcontextH
#define __XML_iloxmlcontextH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/ilxml/iloxmlhash.h>
#include <ilconcert/iloreader.h>
#include <ilconcert/ilxml/ilotag.h>
#include <ilconcert/ilowriter.h>

class IloXmlInfo;

#include <ilconcert/iloenv.h>
#include <ilconcert/ilosolution.h>
#include <ilconcert/iloany.h>
#include <ilconcert/ilohash.h>

#ifdef ILO_RS6000
void* operator new (size_t sz, const IloEnv& env);
void* operator new[] (size_t s, const IloEnv& env);
#endif

class IloXmlAttributeDef;
class IloXmlVersionDef;

enum  {
	NoUnknown = 1,
	EvenUnknown = -1
};

class IloTagHolder {
private :
	char _name[256];
	IloEnv _env;
public:

	void setName(const char* name){
		if (name!=0) {
			if (strlen(name) > 256){
				throw new  IloXmlStrLen();
			}
			IloStrcpy(_name, (char*)name);
		}
	}

	~IloTagHolder() { }
	IloTagHolder(IloEnv env,const char* name = 0): _env(env) {
		setName(name);
	}
	const char* getName() const { return _name; }
};

typedef IloEnvHashTable<IloTagHolder,IloXmlInfo*> ContextHash;

class IloXmlContextI;

class ILO_EXPORTED IloXmlContext  {
private:
	IloXmlContextI* _impl;

public:
	
	IloXmlContextI* getImpl() const;

	
	int getWritePrecision() const;
	
	void setWritePrecision(int writePrecision) const;

	
	const char* getWriteError() const;

	
	IloInt getParentIdReadError() const;

	
	IloInt getChildIdReadError() const;

	
	const char* getParentTagReadError() const;

	
	const char* getChildTagReadError() const;

	
	IloAnyArray getTagListReadError() const;

	
	IloIntArray getIdListReadError() const;

	IloInt getWriteMode() const;

	
	IloBool setWriteMode(IloInt mode) const;

	
	IloBool readExtractable(IloXmlReader reader, IloXmlElement* element) const{
		return readRtti(reader, element);
	}

	
	IloBool readRtti(IloXmlReader reader, IloXmlElement* element) const;

	
	void end();

	
	IloXmlContext(IloEnv env, const char* name = 0);

	
	IloXmlContext(IloXmlContextI* impl=0) : _impl(impl) {}

	
	void registerXML(IloTypeIndex index, IloXmlInfo* xmlinfo) const;

	
	void registerXMLArray(IloXmlInfo* xmlinfo) const;

	
	IloBool writeRtti(const IloRtti* it, IloXmlWriter writer, IloXmlElement* masterElement) const;

	
	IloBool writeExtractable(const IloExtractableI* it, IloXmlWriter writer, IloXmlElement* masterElement) const{
		return writeRtti((const IloRtti*)it, writer, masterElement);
	}

	IloBool writeExtractable(const IloExtractable it, IloXmlWriter writer, IloXmlElement* masterElement) const{
		return writeRtti((IloRtti*)it.getImpl(), writer, masterElement);
	}

	
	IloBool writeModel(const IloModel model, const char* fileName) const;

	
	IloBool writeModelAndSolution(
		const IloModel model,
		const char* modelFileName,
		const IloSolution solution,
		const char* solutionFileName) const;
	
	IloBool writeSolution(const IloSolution solution, const char* fileName) const;

	
	void writeSolutionValue(const IloExtractable it, const IloSolution solution, IloXmlWriter writer) const;

	
	IloBool readModel(IloModel model, const char* fileName) const;

	
	IloBool readModel(IloModel model, istream& file) const;

	
	IloBool readModelAndSolution(
		IloModel model,
		const char* modelFileName,
		IloSolution solution,
		const char* solutionFileName) const;

	
	IloBool readSolution(IloSolution solution, const char* fileName) const;

	
	IloBool readSolution(IloSolution solution, istream& file) const;

	
	IloBool readSolutionValue(IloSolution solution, IloXmlElement* root, IloXmlReader reader) const;

};

class IloXmlContextI : public IloPlugIn {
public: 
private:
	void setVersion(IloSolution solution, IloXmlWriter writer, IloXmlElement* element);
	void setVersion(IloModel model, IloXmlWriter writer, IloXmlElement* element);
	const char* getName() const{ return _name; }
	void setName(const char* name){ 
		if (_name!=0) {
			getEnv()->free((char *)_name, 1+strlen(_name) );
			_name = 0;
		}
		if (name!=0) {
			IloInt length = strlen(name);
			_name = (char*)getEnv()->alloc(length+1);
			IloStrncpy((char *)_name, length+1, name, length+1);
		}
		else
			_name = 0;
	}

private:
	IloIntArray _listId;
	IloInt _writeMode;
	IloAnyArray _Rtti2Function;
	IloAnyArray _TagError;
	IloIntArray _IdError;
	char* _WriteError;
	int _writePrecision;
	ContextHash _Tag2Function;
	ContextHash _Tag2FunctionArray;

	IloXmlIdManager _idManager;
	IloXmlObjectHandler _voidHandler;
	char* _name;

private:

	void importIdManager(IloXmlWriter writer);
	void exportIdManager(IloXmlWriter writer);
	void importObjectHandler(IloXmlReader reader);
	void exportObjectHandler(IloXmlReader reader);

	void addReadError(IloInt id, const char* tag);
	void setWriteError(const IloRtti* extr);

	IloAnyArray getRtti2Function();

	void checkBigEnough(IloAnyArray& array, IloInt index);
	void registerTag(IloXmlInfo* xmlinfo, const char* tag);
	void registerTypeIndex(IloTypeIndex index, IloXmlInfo* xmlinfo, const char* tag);

	void registerTypeIndexArray(IloXmlInfo* xmlinfo, const char* tag);
	void registerAll();
	IloBool addTagFunction(const char* name, IloXmlInfo* xmlinfo);
	IloBool addTagFunctionArray(const char* name, IloXmlInfo* xmlinfo);
	IloXmlInfo* getFunctFromHash(IloTagHolder tag);
	IloXmlInfo* getFunctFromHashArray(IloTagHolder tag);
	IloXmlInfo* getFunctFromRttiArray(IloAnyArray& array, IloInt _rtti);
	IloBool checkXmlVersion(IloXmlReader reader, IloXmlElement* root);
	IloBool checkSolutionVersusModel(IloXmlReader reader, IloXmlElement* element);
	IloBool readMetaModel(IloXmlElement* root, IloXmlReader reader);
	IloBool readMetaModel(IloModel model, IloXmlReader reader);
	IloBool readMetaSolution(IloSolution solution, IloXmlReader reader);

public:
	int getWritePrecision();
	void setWritePrecision(int writePrecision);
	const char* getWriteError();
	IloInt getParentIdReadError();
	IloInt getChildIdReadError();
	const char* getParentTagReadError();
	const char* getChildTagReadError();
	IloAnyArray getTagListReadError();
	IloIntArray getIdListReadError();
	IloInt getWriteMode();
	IloBool setWriteMode(IloInt mode);
	IloBool readRtti(IloXmlReader reader, IloXmlElement* element){
		IloInt id = 0;
		
		
		if (getRttiFromXml(reader, element) == 0 && reader.getIntAttribute(element, IloXmlAttributeDef::Id,id)){
			return IloFalse;
		}
		return IloTrue;
	}
	void* getRttiFromXml(IloXmlReader reader, IloXmlElement* element);

	~IloXmlContextI();
	IloXmlContextI(IloEnv env, const char* name = 0);

	void registerXML(IloTypeIndex index, IloXmlInfo* xmlinfo);
	void registerXMLArray(IloXmlInfo* xmlinfo);

	IloBool writeRtti(const IloRtti* it, IloXmlWriter writer, IloXmlElement* masterElement){
		if (writeRttiToXml(it, writer, masterElement) != 0){
			return IloTrue;
		}
		return IloFalse;
	}
	IloXmlElement* writeRttiToXml(const IloRtti* it, IloXmlWriter writer, IloXmlElement* masterElement);
	IloBool writeExtractable(const IloExtractable it, IloXmlWriter writer, IloXmlElement* masterElement){
		return writeRtti((IloRtti*)it.getImpl(), writer, masterElement);
	}

	IloBool writeModel(const IloModel model, const char* fileName);
	IloBool writeModelAndSolution(
		const IloModel model,
		const char* modelFileName,
		const IloSolution solution,
		const char* solutionFileName);
	IloBool writeSolution(const IloSolution solution, const char* fileName);
	void writeSolutionValue(const IloExtractable it, const IloSolution solution, IloXmlWriter writer);

	IloBool readModel(IloModel model, const char* fileName);
	IloBool readModel(IloModel model, istream& file);

	IloBool readModelAndSolution(
		IloModel model,
		const char* modelFileName,
		IloSolution solution,
		const char* solutionFileName);

	IloBool readSolution(IloSolution solution, const char* fileName);
	IloBool readSolution(IloSolution solution, istream& file);

	IloBool readSolutionValue(IloSolution solution, IloXmlElement* root, IloXmlReader reader);
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
