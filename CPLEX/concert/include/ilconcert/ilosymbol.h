// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilosymbol.h
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

#ifndef __CONCERT_ilosymbolH
#define __CONCERT_ilosymbolH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloany.h>
#include <ilconcert/iloanyexpri.h>
#include <ilconcert/iloadvcollection.h>

class IloAnyCollection;

#ifndef ILO_WINDOWS
#include <locale.h>
#endif

typedef IloDefaultArrayI IloSymbolArrayI;

class IloSymbolI : public IloOplBaseObjectI {
	ILORTTIDECL
		friend class IloSymbolTable;
private:
	char* _str;
	IloSymbolI(IloEnvI* env, const char* str);
public:
	virtual ~IloSymbolI();
	const char* getString() const {return _str;}
	void display(ILOSTD(ostream)& out) const;
	virtual IloBool isSymbol() const ILO_OVERRIDE { return IloTrue; }
	virtual IloRttiEnvObjectI* makeOplClone(IloEnvI* env) const ILO_OVERRIDE { return env->makeSymbol(this->getString()); }
};

class ILO_EXPORTED IloSymbol {
private:
	IloSymbolI* _impl;
public:
	IloSymbol():_impl(0) {}
	
	IloSymbol(IloSymbolI* impl):_impl(impl) {}

	
	IloSymbolI* getImpl() const { return _impl; }

	
	const char* getString() const {
		IloAssert (getImpl(), "accessing IloSymbol through 0 handle");
		return (_impl->getString());
	}
};

inline ILOSTD(ostream)& operator<< (ILOSTD(ostream)& str, const IloSymbol& s) {
	IloAssert (s.getImpl(), "accessing IloSymbol through 0 handle");
	s.getImpl()->display(str);
	return str;
}

class IloSymbolExprI : public IloAnyExprI {
	ILOEXTRDECL
public:
	IloSymbolExprI(IloEnvI* env, const char* name=0) : IloAnyExprI(env, name) {}
	virtual ~IloSymbolExprI(){}
};

typedef IloSymbolExprI IloSymbolExprArgI;

class IloSymbolConstI : public IloSymbolExprI {
	ILOEXTRDECL
private:
	IloSymbolI* _value;
public:
	IloSymbolConstI(IloEnvI* env, IloSymbolI* value, const char* name=0); 
	virtual ~IloSymbolConstI();
	IloSymbolI* getValue() const {return _value;}
	virtual void visitSubExtractables(IloExtractableVisitor* v) ILO_OVERRIDE;
	ILOEXTROTHERDECL
};

class IloTupleSchemaI;
class IloSchemaExprI : public IloAnyExprI {
	ILOEXTRDECL
protected: 
	IloTupleSchemaI const *_schema;
public:
	IloSchemaExprI(IloEnvI* env, const char* name=0) : IloAnyExprI(env, name), _schema(0) {}
	virtual ~IloSchemaExprI(){}
	virtual void setSchema(IloTupleSchemaI const *schema) { _schema = schema; }
	IloTupleSchemaI const *getSchema() const { return _schema; }
	IloBool hasSchema() const { 
		return 0 != _schema; 
	}
};

class IloTupleSetExprI : public IloSchemaExprI {
	ILOEXTRDECL
public:
	IloTupleSetExprI(IloEnvI* env, const char* name=0);
	virtual ~IloTupleSetExprI(){}
};

#define IloTupleSetExprArgI IloTupleSetExprI

class IloTupleSetReferencesPropertyI : public IloNamedPropertyI {
	ILORTTIDECL
		static const char* TupleSetReferences;
	IloIntArray _idx;
	IloArray<IloDiscreteDataCollectionI*> _references;

public:
	IloTupleSetReferencesPropertyI(IloMemoryManager env, IloIntArray idx, IloArray<IloDiscreteDataCollectionI*> references)
		: IloNamedPropertyI(env), _idx(idx.copy()), _references(references.copy()){};
	~IloTupleSetReferencesPropertyI(){
		_idx.end();
		_references.end();
	}
	IloNamedPropertyI * makeClone(IloMemoryManager env) const ILO_OVERRIDE {
		return new (env) IloTupleSetReferencesPropertyI(env, _idx, _references);
	}
	const char * getName() const ILO_OVERRIDE { return IloTupleSetReferencesPropertyI::TupleSetReferences; }
	IloIntArray getReferenceIndexes() const{ return _idx;}
	IloArray<IloDiscreteDataCollectionI*> getReferences() const{ return _references;}
};

class ILO_EXPORTED IloTupleSetExprArg : public IloExtractable {
	ILOEXTRHANDLE(IloTupleSetExprArg, IloExtractable)
public:
	static void setReferences(IloTupleSetExprI* arg, IloIntArray idx, IloArray<IloDiscreteDataCollectionI*> references){
		IloTestAndRaise(!arg->hasProperty("IloTupleSetReferencesPropertyI::TupleSetReferences"), "references are already set");
		IloTupleSetReferencesPropertyI* prop = new (arg->getMemoryManager()) IloTupleSetReferencesPropertyI(arg->getMemoryManager(), idx, references);
		arg->addProperty(prop);
	}
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif

